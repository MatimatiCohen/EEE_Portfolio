#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "lb_tools.h"
#include "utils.h"

#define MIN_PORT 1025
#define MAX_PORT 63999
#define INIT_BUFFER_SIZE 16

int random_port_bind(int socket, struct sockaddr_in *socket_address)
{
  while (true) {
    int random_port_number = create_random_port_number(MIN_PORT, MAX_PORT);
    socket_address->sin_port = htons(random_port_number);
    if (bind(socket, (struct sockaddr *)socket_address, sizeof(*socket_address)) == 0) {
      return random_port_number;
    }
  }
}

int accept_client_http(int http_socket)
{
  int accepted_http_socket = accept(http_socket, NULL, NULL);
  return accepted_http_socket;
}

void proccess_browser_request(int from_socket, int to_socket, int end_condition)
{
  int msg_len = 0;
  char *http_msg = (char *)malloc(INIT_BUFFER_SIZE * sizeof(char));
  assert(http_msg != NULL);
  while (true) {
    int bytes_recv = recv(from_socket, &(http_msg[msg_len]), (int)INIT_BUFFER_SIZE, 0);
    msg_len += bytes_recv;
    if (count_substr_in_msg(http_msg, msg_len, "\r\n\r\n") >= end_condition) {
      break;
    }
    http_msg = realloc(http_msg, msg_len + INIT_BUFFER_SIZE);
  }
  send(to_socket, http_msg, msg_len, 0);
  if (http_msg != NULL) {
    free(http_msg);
  }
}
