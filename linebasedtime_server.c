// LineBasedTime Server - Main C File
// Open source software licensed under MIT License
// Author: EthernetLord
// - website: https://ethernetlord.eu
// - GitHub: https://github.com/ethernetlord/
// - Twitter: https://twitter.com/ethernetlordcz

/*
MIT License

Copyright (c) 2020 EthernetLord (https://ethernetlord.eu/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include"linebasedtime_server.h"

int main(int argc, char **argv) {
  if(argc != 3)
    usage();


  print_info(false, PROGRAM_NAME " v" PROGRAM_VERSION " initializing...");

  #ifdef CHANGE_WORKING_DIR_ON_STARTUP
  if(chdir("/") < 0)
    crash("chdir", "Error occurred while changing working directory!");
  #endif

  initialize_globals();
  set_signal_handlers();

  int server_sockfd = create_server_socket(argv[1], argv[2]);
  wait_for_clients(server_sockfd);
  close_server_socket(server_sockfd);


  return EXIT_SUCCESS;
}



void usage(void) {
  fputs(USAGE_STRING, stderr);
  exit(EXIT_FAILURE);
}

void crash(const char *perror_msg, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  fputs("[ERROR] ", stderr);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);

  if(perror_msg != NULL)
    perror(perror_msg);

  exit(EXIT_FAILURE);
}

void fai_crash(struct addrinfo *gai_result, const char *perror_msg, const char *format, ...) {
  freeaddrinfo(gai_result);

  // copied code from above to prevent making an extra function
  va_list ap;
  va_start(ap, format);
  fputs("[ERROR] ", stderr);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);

  if(perror_msg != NULL)
    perror(perror_msg);

  exit(EXIT_FAILURE);
}

void print_info(const bool is_debug, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  fputs(is_debug ? "[DEBUG] " : "[INFO] ", stderr);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);
}



void initialize_globals(void) {
  const uint8_t proto_ver = 1;

  sprintf(g_proto_ver_str, "%"PRIu8, proto_ver);
  sprintf(g_err_string, "VER: %s\nERR: Y\n", g_proto_ver_str);
}

void set_signal_handlers(void) {
    sigset_t sig_set;
    sigemptyset(&sig_set);

    struct sigaction sig_act;
    memset(&sig_act, 0, sizeof(sig_act));
    sig_act.sa_handler = signal_handler;
    sig_act.sa_mask = sig_set;

    if(sigaction(SIGINT, &sig_act, NULL) < 0 || sigaction(SIGHUP, &sig_act, NULL) < 0 || sigaction(SIGTERM, &sig_act, NULL) < 0)
      crash("sigaction", "Error occurred while setting signal handler!");
}

void signal_handler(int sig_num) {
  // cannot use print_info() because it's AS-Unsafe
  write(STDERR_FILENO, SIGNAL_MESSAGE, strlen(SIGNAL_MESSAGE));

  g_continue_running = 0;
}



void print_server_socket_info(struct addrinfo *gai_result) {
  char ipaddr_str_buf[INET6_ADDRSTRLEN];
  switch(gai_result->ai_family) {
    case AF_INET:
      if(inet_ntop(AF_INET, &(((struct sockaddr_in *) gai_result->ai_addr)->sin_addr), ipaddr_str_buf, INET6_ADDRSTRLEN) == NULL)
        fai_crash(gai_result, "inet_ntop", "Failed to convert IPv4 to text while trying to print server socket info!");
      print_info(false, "Listening on IPv4 %s:%"PRIu16"...", ipaddr_str_buf, ntohs(((struct sockaddr_in *) gai_result->ai_addr)->sin_port));
      break;

    case AF_INET6:
      if(inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) gai_result->ai_addr)->sin6_addr), ipaddr_str_buf, INET6_ADDRSTRLEN) == NULL)
        fai_crash(gai_result, "inet_ntop", "Failed to convert IPv6 to text while trying to print server socket info!");
      print_info(false, "Listening on IPv6 [%s]:%"PRIu16"...", ipaddr_str_buf, ntohs(((struct sockaddr_in6 *) gai_result->ai_addr)->sin6_port));
      break;

    default:
      fai_crash(gai_result, NULL, "Invalid address family detected while trying to print server socket info!");
  }
}

int create_server_socket(const char *ip_or_hostname, const char *port_or_service) {
  // resolve arguments-passed IP address or hostname and port or service name
  int gai_errcode;
  struct addrinfo gai_hints, *gai_result;
  memset(&gai_hints, 0, sizeof(gai_hints));
  gai_hints.ai_family = AF_UNSPEC;
  gai_hints.ai_socktype = SOCK_STREAM;
  gai_hints.ai_flags = AI_PASSIVE; // will be ignored on Linux

  if((gai_errcode = getaddrinfo(ip_or_hostname, port_or_service, &gai_hints, &gai_result)) != 0 || gai_result == NULL)
    crash(NULL, "Error when resolving listening hostname and port!\ngetaddrinfo: %s", gai_strerror(gai_errcode));



  // prepare the socket itself
  int sockfd;
  if((sockfd = socket(gai_result->ai_family, gai_result->ai_socktype, gai_result->ai_protocol)) < 0)
    fai_crash(gai_result, "socket", "Error occurred while initializing server socket!");

  #ifdef DEBUG_SOCK_REUSEADDR
  const int sock_reuseaddr = 1;
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_reuseaddr, sizeof(sock_reuseaddr)) < 0)
    fai_crash(gai_result, "setsockopt", "Error occurred while setting a server socket option!");
  #endif

  if(bind(sockfd, gai_result->ai_addr, gai_result->ai_addrlen) < 0)
    fai_crash(gai_result, "bind", "Error occurred while binding server socket!");

  if(listen(sockfd, LISTEN_BACKLOG) < 0)
    fai_crash(gai_result, "listen", "Error occurred while marking server socket as listening!");



  // maintenance & return
  print_server_socket_info(gai_result);
  freeaddrinfo(gai_result);

  return sockfd;
}

void close_server_socket(int server_sockfd) {
  if(close(server_sockfd) < 0)
    crash("close", "Error occurred while closing server socket!");

  print_info(false, "Server socket closed, quitting program...");
}



void wait_for_clients(int server_sockfd) {
  int client_sockfd;

  while(g_continue_running) {
    if((client_sockfd = accept(server_sockfd, NULL, NULL)) < 0)
      continue;

    if(!serve_client(client_sockfd))
      send_error_to_client(client_sockfd);

    // no success checking because there is no need to handle it
    close(client_sockfd);
  }
}

bool serve_client(int client_sockfd) {
  time_t unix_time;
  struct tm utc_tm, loc_tm;
  char utc_str[26], loc_str[26];

  if(time(&unix_time) < 0 || gmtime_r(&unix_time, &utc_tm) == NULL || localtime_r(&unix_time, &loc_tm) == NULL || asctime_r(&utc_tm, utc_str) == NULL || asctime_r(&loc_tm, loc_str) == NULL)
    return false;

  char send_str[103];
  sprintf(send_str, "VER: %s\nERR: N\nUNX: %"PRIu64"\nUTC: %sLOC: %s", g_proto_ver_str, (uint64_t) unix_time, utc_str, loc_str);

  // no success checking - same reason as above
  // if this send() failed, the send_error_to_client() would fail too, so true is returned even if this fails
  send(client_sockfd, send_str, strlen(send_str), MSG_DONTWAIT);

  return true;
}

void send_error_to_client(int client_sockfd) {
  // no success checking - same reason as above
  send(client_sockfd, g_err_string, strlen(g_err_string), MSG_DONTWAIT);
}
