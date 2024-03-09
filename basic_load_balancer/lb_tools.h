#ifndef LB_TOOLS_H
#define LB_TOOLS_H

#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

int random_port_bind(int socket, struct sockaddr_in *socket_address);
int accept_client_http(int http_socket);
void proccess_browser_request(int from_socket, int to_socket, int end_condition);

#endif
