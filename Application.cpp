/*
File Name : Application.cpp
Author: 이시행
Purpose: 통신 Applications 정의
Create date : 2025-05-21
Modified date : 2025-05-22
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

    SOCKETINFO* sockinfo = RegisterEvent(epollfd, client_sock, EPOLLIN, 0); // 클라이언트 등록
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
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        flag = false;
    }
    else flag = true;

    retval = send(sockinfo->sock, (char*)&flag, 1, 0);
    if (retval == SOCKET_ERROR) {
        err_display("send()");
        epoll_ctl(epollfd, EPOLL_CTL_DEL, sockinfo->sock, NULL);
        close(sockinfo->sock); delete sockinfo;
        userList.erase(id);
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
    getpeername(ptr->sock, (struct sockaddr*)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    if (ev.events & EPOLLIN) {
        int retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
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
            ptr->recvbytes = retval;
            ptr->buf[retval] = '\0';
            printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), ptr->buf);

            if (ptr->recvbytes > ptr->sendbytes) {
                ModifyEvent(epollfd, ev, EPOLLOUT);
            }
        }
    }
    else if (ev.events & EPOLLOUT) {
        int retval = send(ptr->sock, ptr->buf + ptr->sendbytes,
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
                ModifyEvent(epollfd, ev, EPOLLIN);
            }
        }
    }
}
