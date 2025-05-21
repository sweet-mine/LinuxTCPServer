#pragma once
// ���� ���� ������ ���� ����ü
struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
};

// ���� �̺�Ʈ ��� �Լ�
void RegisterEvent(int epollfd, SOCKET sock, uint32_t events);
// ���� �̺�Ʈ ���� �Լ�
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events);

