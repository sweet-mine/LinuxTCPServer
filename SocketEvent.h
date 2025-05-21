#pragma once
// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
};

// 소켓 이벤트 등록 함수
void RegisterEvent(int epollfd, SOCKET sock, uint32_t events);
// 소켓 이벤트 수정 함수
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events);

