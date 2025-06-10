/*
File Name : SocketEvent.cpp
Author: 이시행
Purpose: epoll 사용을 위한 함수 정의
Create date : 2025-05-21
Modified date : 2025-05-21
*/
#include "SocketEvent.h"

// 소켓 이벤트 등록 후 반환 함수
SOCKETINFO* RegisterEvent(int epollfd, SOCKET sock, uint32_t events, string id)
{
	SOCKETINFO* ptr = new SOCKETINFO;
	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->id = id;
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = ptr;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		perror("epoll_ctl()");
		close(sock);
		exit(1);
	}
	return ptr;
}

// 소켓 이벤트 수정 함수
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events)
{
	ev.events = events;
	SOCKETINFO* ptr = (SOCKETINFO*)ev.data.ptr;
	if (epoll_ctl(epollfd, EPOLL_CTL_MOD, ptr->sock, &ev) < 0) {
		perror("epoll_ctl()");
		close(ptr->sock);
		exit(1);
	}
}