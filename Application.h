/*
File Name : Application.h
Author: 이시행
Purpose: 통신 Applications 함수 선언
Create date : 2025-05-21
Modified date : 2025-05-22
*/
#pragma once
#include "Common.h"
#include "SocketEvent.h"
#include "UserData.h"
#include "Message.h"

void HandleAcceptEvent(int epollfd, SOCKET listen_sock);
void HandleClientEvent(int epollfd, struct epoll_event& ev);