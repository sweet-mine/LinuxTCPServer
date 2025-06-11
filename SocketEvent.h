/*
File Name : SocketEvent.h
Author: 이시행
Purpose: epoll 사용을 위한 구조체 및 함수 선언
Create date : 2025-05-21
Modified date : 2025-06-12
Mocification : broadcast, unicast 함수 추가
*/

#pragma once
#include "Common.h"
#include "UserData.h"
#include "Message.h"

// 소켓 이벤트 등록 함수
SOCKETINFO* RegisterEvent(int epollfd, SOCKET sock, uint32_t events, string id);
// 소켓 이벤트 수정 함수
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events);
// UserList에 있는 모든 사용자에게 브로드캐스트(EPOLLOUT)
void broadcast(int epollfd, SOCKETINFO* sender, uint32_t header, MSGTYPE msg, uint32_t recvbytes);
// UserList에 있는 특정 사용자에게 전송(EPOLLOUT)
void unicast(int epollfd, uint32_t header, MSGTYPE msg, uint32_t recvbytes);

