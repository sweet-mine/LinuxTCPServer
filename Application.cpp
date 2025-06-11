/*
File Name : Application.cpp
Author: 이시행
Purpose: 통신 Applications 정의
Create date : 2025-05-21
Modified date : 2025-06-12
Modification : 유저 리스트 중복 체크 및 추가 기능 수정
*/

#include "Application.h"

// 클라이언트 등록하는 이벤트
void HandleAcceptEvent(int epollfd, SOCKET listen_sock) {
    int retval = 0;
    char buf[BUFSIZE + 1];
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        err_display("accept()");
        return;
    }

    SOCKETINFO* sockinfo = RegisterEvent(epollfd, client_sock, EPOLLIN, ""); // 클라이언트 등록
    retval = recv(sockinfo->sock, buf, 21, 0); // ID 수신
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        return;
    }
    else if (retval == 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        return;
    }

    string id(buf);

    // 유저리스트에 ID 등록 시도 후 true(정상), false(중복)임을 클라이언트에게 송신
    bool flag = false;
    if (!pushUserList(id, sockinfo)) {
        flag = false;
    }
    else flag = true;

    retval = send(sockinfo->sock, (char*)&flag, 1, 0);
    if (retval == SOCKET_ERROR) {
        err_display("send()");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        return;
    }

    //ID 등록 실패시 종료
    if (flag == false) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        return;
    }

    sockinfo->id = id;

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("\n[메세지 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d, ID=%s\n", addr, ntohs(clientaddr.sin_port), id.c_str());

    // 현재 유저 리스트 출력
    if (flag == true) {
        int usersize = userList.size();
        printf("%d\n", usersize);
        retval = send(sockinfo->sock, (char*)&usersize, sizeof(int), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
            close(sockinfo->sock); delete sockinfo;
            return;
        }
        for (auto& [id, user] : userList) {
            retval = send(sockinfo->sock, id.c_str(), 21, 0);
            if (retval == SOCKET_ERROR) {
                err_display("send()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
                close(sockinfo->sock); delete sockinfo;
                return;
            }
        }
    }

    // 채팅 메시지 복원
    if (flag == true) {
        int chatsize = chatList.size();
        send(sockinfo->sock, (char*)&chatsize, sizeof(int), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
            close(sockinfo->sock); delete sockinfo;
            return;
        }
        for (MSGTYPE msg : chatList) {
            send(sockinfo->sock, (char*)&msg, sizeof(MSGTYPE), 0);
            if (retval == SOCKET_ERROR) {
                err_display("send()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
                close(sockinfo->sock); delete sockinfo;
                return;
            }
        }
    }

    int flags = fcntl(client_sock, F_GETFL);
    fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);
}


// 송수신 이벤트
void HandleClientEvent(int epollfd, struct epoll_event& ev) {
    SOCKETINFO* ptr = (SOCKETINFO*)ev.data.ptr;
    int retval;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
	bool is_group;
    uint32_t msg_length;
    getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    if (ev.events & EPOLLIN) {
        // 헤더 수신
        if (ptr->hsendbytes != 4) {
            // 헤더 받기
            retval = recv(ptr->sock, (char*)&(ptr->header) + ptr->hsendbytes, 4 - ptr->hsendbytes, 0);
            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                close(ptr->sock); delete ptr;
            }
            else if (retval == 0) {
                printf("[메세지 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d, ID=%s\n",
                    addr, ntohs(clientaddr.sin_port), ptr->id.c_str());
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                userList.erase(ptr->id);
                close(ptr->sock); delete ptr;
            }
            else {
                ptr->hsendbytes += retval;
                printf("헤더 전송\n");
            }
        }
        else {
            // 헤더 해석 및 저장
            parse_header(ptr->header, is_group, msg_length);
            ptr->recvbytes = msg_length;

            printf("group = %d, length = %d\n", is_group, msg_length);

            // 메시지 수신
            retval = recv(ptr->sock, (char*)&(ptr->msg) + ptr->sendbytes,
                ptr->recvbytes - ptr->sendbytes, 0);
            if (retval == SOCKET_ERROR && errno != EAGAIN) {
                err_display("recv()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                close(ptr->sock); delete ptr;
            }
            else if (retval == 0) {
                printf("[메세지 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d, ID=%s\n",
                    addr, ntohs(clientaddr.sin_port), ptr->id.c_str());
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                userList.erase(ptr->id);
                close(ptr->sock); delete ptr;
            }
            ptr->sendbytes += retval;

            if (ptr->recvbytes == ptr->sendbytes) { // 메시지 수신 완료
                ptr->recvbytes = ptr->sendbytes = 0;
                ptr->hsendbytes = 0;

                if (is_group) { // 그룹 메시지인 경우
                    printf("%s to 그룹 : %s\n", ptr->msg.src, ptr->msg.data);
                    pushChatList(ptr->msg); // 그룹 메시지 기록 저장
                    broadcast(epollfd, ptr, ptr->header, ptr->msg, retval);
                }

                else { // 1:1 메시지인 경우
                    printf("%s to %s : %s\n", ptr->msg.src, ptr->msg.dst, ptr->msg.data);
                    unicast(epollfd, ptr->header, ptr->msg, retval);

                }
                memset(ptr->msg.data, 0, sizeof(ptr->msg.data)); // 메세지 버퍼 초기화
            }
        }
    }

    else if (ev.events & EPOLLOUT) {
        if (ptr->hsendbytes != 4) {
            // 헤더 보내기
            retval = send(ptr->sock, (char*)&(ptr->header) + ptr->hsendbytes, 4 - ptr->hsendbytes, 0);
            if (retval == SOCKET_ERROR) {
                err_display("send()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                close(ptr->sock); delete ptr;
            }
            else {
                ptr->hsendbytes += retval;
            }
        }
        
        else {
            // 데이터 보내기
            retval = send(ptr->sock, (char*)&(ptr->msg) + ptr->sendbytes,
                ptr->recvbytes - ptr->sendbytes, 0);
            if (retval == SOCKET_ERROR) {
                err_display("send()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                close(ptr->sock); delete ptr;
            }
            else {
                ptr->sendbytes += retval;
                if (ptr->recvbytes == ptr->sendbytes) {
                    ptr->recvbytes = ptr->sendbytes = 0;
					ptr->hsendbytes = 0;
                    memset(ptr->msg.data, 0, sizeof(ptr->msg.data));
                    ModifyEvent(epollfd, ev, EPOLLIN);
                }
            }
        }
    }
}
