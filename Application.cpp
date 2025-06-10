/*
File Name : Application.cpp
Author: 이시행
Purpose: 통신 Applications 정의
Create date : 2025-05-21
Modified date : 2025-06-10
Modification : 유저 리스트 중복 체크 및 추가 기능 수정
*/

#include "Application.h"

// 클라이언트 등록하는 이벤트
void HandleAcceptEvent(int epollfd, SOCKET listen_sock) {
    int retval = 0;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        err_display("accept()");
        return;
    }

    SOCKETINFO* sockinfo = RegisterEvent(epollfd, client_sock, EPOLLIN, ""); // 클라이언트 등록
    retval = recv(sockinfo->sock, sockinfo->buf, 20, 0); // ID 수신
    if (retval == SOCKET_ERROR) {
        err_display("recv()");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        return;
    }

    string id(sockinfo->buf, 20);

    // 유저리스트에 ID 등록 시도 후 true(정상), false(중복)임을 클라이언트에게 송신
    bool flag;
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

    int flags = fcntl(client_sock, F_GETFL);
    fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);
}


// 송수신 이벤트
void HandleClientEvent(int epollfd, struct epoll_event& ev) {
    SOCKETINFO* ptr = (SOCKETINFO*)ev.data.ptr;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    uint32_t header;
    string buf;
    getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    if (ev.events & EPOLLIN) {
        // 헤더 수신
        int retval = recv(ptr->sock, (char*)&header, 32, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
            close(ptr->sock); delete ptr;
        }

        // 헤더 해석
        parse_header(header, ptr->is_group, ptr->is_file, ptr->bytes);

        // 메시지 수신
        retval = recv(ptr->sock, ptr->buf, ptr->bytes, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
            close(ptr->sock); delete ptr;
        }

        else if (retval == 0) {
            printf("[메세지 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
                addr, ntohs(clientaddr.sin_port));
            epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
            userList.erase(ptr->id);
            close(ptr->sock); delete ptr;
        }
        else {
            ptr->bytes = retval;
            ptr->buf[retval] = '\0';
            printf("[TCP/%s:%d ID : %s] %s\n", addr, ntohs(clientaddr.sin_port), ptr->id.c_str(), ptr->buf);
            ModifyEvent(epollfd, ev, EPOLLOUT);
        }
    }

    // TODO : 서버 송신부 고정+가변 처리 및 케이스 나누기
    else if (ev.events & EPOLLOUT) {
        int retval;
        // 모든 유저에게 전송
        for (pair<string, SOCKETINFO*> data : userList) {
            SOCKET sock = data.second->sock;
            retval = send(sock, ptr->buf, ptr->bytes, 0);
            if (retval == SOCKET_ERROR) {
                err_display("send()");
                epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
                close(ptr->sock); delete ptr;
            }
        }
        ModifyEvent(epollfd, ev, EPOLLIN);
    }
}
