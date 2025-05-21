#include "Common.h"
#include "SocketEvent.h"

// ���� �̺�Ʈ ��� �Լ�
void RegisterEvent(int epollfd, SOCKET sock, uint32_t events)
{
	SOCKETINFO* ptr = new SOCKETINFO;
	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;

	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = ptr;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		perror("epoll_ctl()");
		close(sock);
		exit(1);
	}
}

// ���� �̺�Ʈ ���� �Լ�
void ModifyEvent(int epollfd, struct epoll_event ev, uint32_t events)
{
	ev.events = events;
	SOCKETINFO* ptr = (SOCKETINFO*)ev.data.ptr;
	if (epoll_ctl(epollfd, EPOLL_CTL_MOD, ptr->sock, &ev) < 0) {
		perror("epoll_ctl()");
		close(ptr->sock);
		exit(1);
	}
}