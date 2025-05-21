#pragma once
void HandleAcceptEvent(int epollfd, SOCKET listen_sock);
void HandleClientEvent(int epollfd, struct epoll_event& ev);