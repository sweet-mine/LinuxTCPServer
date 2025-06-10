/*
File Name : UserData.h
Author: 이시행
Purpose: 사용자 정보 저장 형식 및 함수 선언
Create date : 2025-05-21
Modified date : 2025-05-22
*/

#pragma once
#include "Common.h"
#include "SocketEvent.h"

#define CHAT_LIST_MAX 20 // 채팅 최대 저장 수

using chatList = queue<string>; // 채팅 리스트

struct GROUPINFO { // 그룹 정보
	map<string, SOCKETINFO*> userList;
	queue<string> chatList;
};

extern map<string, SOCKETINFO*> userList; // 유저 리스트(id, socketinfo)
extern map<string, GROUPINFO> grouplist; // 그룹 리스트(id, groupinfo)

struct MSGTYPE { // 메세지 형식
	string dst; // 수신자 id
	string src; // 송신자 id
	char data[512];
};

// 유저 추가 및 중복 체크 (false = 중복)
bool pushUserList(string id, SOCKETINFO* sock);

// 채팅 목록 CHAT_LIST_MAX만큼 저장
void pushChatList(chatList list, string str);
