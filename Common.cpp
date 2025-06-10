/*
File Name : Common.cpp
Author: 이시행
Purpose: 에러 처리 함수 정의
Create date : 2025-05-21
Modified date : 2025-05-21
*/

#include "Common.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	char* msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	char* msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
}

// 소켓 함수 오류 출력
void err_display(int errcode)
{
	char* msgbuf = strerror(errcode);
	printf("[오류] %s\n", msgbuf);
}