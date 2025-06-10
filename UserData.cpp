/*
File Name : UserData.cpp
Author: 이시행
Purpose: 사용자 정보 관리 함수 정의
Create date : 2025-05-21
Modified date : 2025-05-22
*/

#include "UserData.h"

map<string, SOCKETINFO*> userList;
map<string, GROUPINFO> grouplist;

bool pushUserList(string id, SOCKETINFO* sock) {
	auto result1 = userList.insert({ id, sock });
	if (result1.second) return true;
	else return false;
}

void pushChatList(chatList list, string str) {
	list.push(str);
	if (list.size() > 20) // 일정 개수 초과시 채팅 목록 유지
		list.pop();
}