#include "lb_tools.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define HTTP_PORT_FILENAME "http_port"
#define SERVER_PORT_FILENAME "server_port"
#define BUFFER_SIZE 1024
#define NUM_OF_SERVERS 3
#define BROWSER 1

int main()
{

  srand(time(NULL));

  // create server socket & address
  struct sockaddr_in server_address;
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  int server_port_number = random_port_bind(server_socket, &server_address);
  write_port_to_file(server_port_number, SERVER_PORT_FILENAME);
  listen(server_socket, NUM_OF_SERVERS);

  // create http socket and address
  struct sockaddr_in http_address;
  int http_socket = socket(AF_INET, SOCK_STREAM, 0);
  http_address.sin_family = AF_INET;
  http_address.sin_addr.s_addr = INADDR_ANY;
  int http_port_number = random_port_bind(http_socket, &http_address);
  write_port_to_file(http_port_number, HTTP_PORT_FILENAME);
  listen(http_socket, BROWSER);

  int i;
  int accepted_server_sockets[NUM_OF_SERVERS];
  for (i = 0; i < NUM_OF_SERVERS; i++) {
    accepted_server_sockets[i] = accept(server_socket, NULL, NULL);  
  }

  int http_connection;
  int current_server_ind = 0;
  while (true) {
    current_server_ind %= NUM_OF_SERVERS;
    http_connection = accept_client_http(http_socket);
    proccess_browser_request(http_connection, accepted_server_sockets[current_server_ind], 1);
    proccess_browser_request(accepted_server_sockets[current_server_ind], http_connection, 2);
    current_server_ind++;
  }

  return 0;
}
