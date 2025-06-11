/*
File Name : SocketEvent.cpp
Author: 이시행
Purpose: epoll 사용을 위한 함수 정의
Create date : 2025-05-21
Modified date : 2025-06-12
Modification : broadcast, unicast 기능 추가
*/
#include "SocketEvent.h"

// 소켓 이벤트 등록 후 반환 함수
SOCKETINFO* RegisterEvent(int epollfd, SOCKET sock, uint32_t events, string id)
{
	SOCKETINFO* ptr = new SOCKETINFO;
	ptr->sock = sock;
	ptr->id = id;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->hsendbytes = 0;
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

// UserList에 있는 모든 사용자에게 브로드캐스트(EPOLLOUT)
void broadcast(int epollfd, SOCKETINFO* sender, uint32_t header, MSGTYPE msg, uint32_t recvbytes) {
	for (auto& [id, user] : userList) {
		if (user == sender) continue;  // 자신 제외
		user->header = header;
		user->recvbytes = recvbytes;
		user->msg = msg;

		struct epoll_event ev;
		ev.events = EPOLLOUT;
		ev.data.ptr = user;
		epoll_ctl(epollfd, EPOLL_CTL_MOD, user->sock, &ev);
	}
}

// UserList에 있는 특정 사용자에게 전송(EPOLLOUT)
void unicast(int epollfd, uint32_t header, MSGTYPE msg, uint32_t recvbytes) {
	auto it = userList.find(string(msg.dst));
	if(it == userList.end()) {
		printf("Error: User not found for unicast.\n");
		return;
	}
	SOCKETINFO* reciever = it->second;
	reciever->header = header;
	reciever->recvbytes = recvbytes;
	reciever->msg = msg;
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.ptr = reciever;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, reciever->sock, &ev);
}