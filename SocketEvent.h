/*
File Name : SocketEvent.h
Author: 이시행
Purpose: epoll 사용을 위한 구조체 및 함수 선언
Create date : 2025-05-21
Modified date : 2025-05-21
*/

#pragma once
#include "Common.h"

// 소켓(유저) 정보 저장을 위한 구조체
struct SOCKETINFO
{
	string id;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
};

// 소켓 이벤트 등록 함수
SOCKETINFO* RegisterEvent(int epollfd, SOCKET sock, uint32_t events, string id);
// 소켓 이벤트 수정 함수
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events);

