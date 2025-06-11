/*
File Name : UserData.h
Author: 이시행
Purpose: 사용자 정보 저장 형식 및 처리 함수 선언
Create date : 2025-05-21
Modified date : 2025-06-12
Modification : chatList 형식 변경
*/

#pragma once
#include "Common.h"
#include "Message.h"

#define CHAT_LIST_MAX 20 // 채팅 최대 저장 수

extern deque<MSGTYPE> chatList; // 채팅 리스트
extern map<string, SOCKETINFO*> userList; // 유저 리스트(id, socketinfo)

// 유저 추가 및 중복 체크 (false = 중복)
bool pushUserList(string id, SOCKETINFO* sock);

// 채팅 저장 CHAT_LIST_MAX까지
void pushChatList(MSGTYPE msg);
