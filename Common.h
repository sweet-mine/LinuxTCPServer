/*
File Name : Common.h
Author: 이시행
Purpose: 기본 라이브러리 및 에러 처리 함수 선언
Create date : 2025-05-21
Modified date : 2025-05-21
*/

#pragma once
#include <sys/types.h> // basic type definitions
#include <sys/socket.h> // socket(), AF_INET, ...
#include <arpa/inet.h> // htons(), htonl(), ...
#include <netdb.h> // gethostbyname(), ...
#include <unistd.h> // close(), ...
#include <fcntl.h> // fcntl(), ...
#include <pthread.h> // pthread_create(), ...
#include <poll.h> // poll()
#include <sys/epoll.h> // epoll()

#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strerror(), ...
#include <errno.h> // errno

#include <vector>
#include <deque>
#include <string>
#include <map>
using namespace std;

// Windows 소켓 코드와 호환성을 위한 정의
typedef int SOCKET;
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1

// 서버 포트, 버퍼 사이즈 정의
#define SERVERPORT 9000
#define BUFSIZE    4096 // 4KB 버퍼

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg);
// 소켓 함수 오류 출력
void err_display(const char* msg);
// 소켓 함수 오류 출력
void err_display(int errcode);

