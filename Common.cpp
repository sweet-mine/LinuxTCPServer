#include "Common.h"

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	char* msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	char* msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
}

// ���� �Լ� ���� ���
void err_display(int errcode)
{
	char* msgbuf = strerror(errcode);
	printf("[����] %s\n", msgbuf);
}