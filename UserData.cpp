/*
File Name : UserData.cpp
Author: 이시행
Purpose: 사용자 정보 관리 함수 정의
Create date : 2025-05-21
Modified date : 2025-06-12
Modification : chatList 형식 변경에 의한 함수 인자 변경
*/

#include "UserData.h"

map<string, SOCKETINFO*> userList;
deque<MSGTYPE> chatList;

bool pushUserList(string id, SOCKETINFO* sock) {
	auto result1 = userList.insert({ id, sock });
	if (result1.second) return true;
	else return false;
}

void pushChatList(MSGTYPE msg) {
	chatList.push_back(msg);
	if (chatList.size() > 20) // 일정 개수 초과시 채팅 목록 유지
		chatList.pop_front();
}