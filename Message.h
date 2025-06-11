/*
File Name : Message.h
Author: 이시행
Purpose: 메세지 타입 및 헤더 처리 함수 선언
Create date : 2025-06-12
*/

#pragma once
#include "Common.h"

// 메세지 타입 구조체
#pragma pack(push, 1) // 구조체 정렬 설정
struct MSGTYPE { // 메세지 형식
	char dst[21]; // 수신자 id
	char src[21]; // 송신자 id
	char data[BUFSIZE + 1];
};
#pragma pack(pop)

// 유저(소켓) 정보 저장을 위한 구조체
struct SOCKETINFO
{
	string id;
	SOCKET sock;
	MSGTYPE msg;
	uint32_t header;
	uint32_t sendbytes;
	uint32_t recvbytes;
	uint32_t hsendbytes;
};

// 메시지 헤더 생성 함수
uint32_t make_header(bool is_group, uint32_t msg_length);

// 메시지 헤더 파싱 함수
void parse_header(uint32_t header, bool& is_group, uint32_t& msg_length);