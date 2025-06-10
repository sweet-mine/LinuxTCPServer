/*
File Name : LinuxTCPServer.cpp
Author: 이시행
Purpose: main
Create date : 2025-05-21
Modified date : 2025-05-22
*/

#include "Common.h"
#include "Application.h"

int main(int argc, char* argv[])
{
	int retval;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 넌블로킹 소켓으로 전환
	int flags = fcntl(listen_sock, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(listen_sock, F_SETFL, flags);

	// EPoll 인스턴스 생성
	int epollfd = epoll_create(1); /* 전달 인수(> 0)는 무시됨 */
	if (epollfd < 0) {
		perror("epoll_create()");
		exit(1);
	}

	// 소켓 이벤트 등록(1)
	RegisterEvent(epollfd, listen_sock, EPOLLIN, 0);

	// 데이터 통신에 사용할 변수
	struct epoll_event events[FD_SETSIZE];
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;

	while (1) {
		nready = epoll_wait(epollfd, events, FD_SETSIZE, -1);
		if (nready < 0) err_quit("epoll_wait()");

		for (int i = 0; i < nready; i++) {
			SOCKETINFO* ptr = (SOCKETINFO*)events[i].data.ptr;

			if (ptr->sock == listen_sock) {
				HandleAcceptEvent(epollfd, listen_sock);
			}
			else {
				HandleClientEvent(epollfd, events[i]);
			}
			if (--nready <= 0) break;
		}
	}


MAIN_EXIT:
	// EPoll 인스턴스 삭제
	close(epollfd);

	// 소켓 닫기
	close(listen_sock);
	return 0;
}