#include "Common.h"
#include "SocketEvent.h"

void HandleAcceptEvent(int epollfd, SOCKET listen_sock) {
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);

    SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
    if (client_sock == INVALID_SOCKET) {
        err_display("accept()");
        return;
    }

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
        addr, ntohs(clientaddr.sin_port));

    int flags = fcntl(client_sock, F_GETFL);
    fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);

    RegisterEvent(epollfd, client_sock, EPOLLIN);
}

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
            printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
                addr, ntohs(clientaddr.sin_port));
            epoll_ctl(epollfd, EPOLL_CTL_DEL, ptr->sock, NULL);
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
