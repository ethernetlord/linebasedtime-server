// LineBasedTime Server - C Header File
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

#ifndef __LINEBASEDTIME_SERVER_H__
#define __LINEBASEDTIME_SERVER_H__

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdint.h>
#include<inttypes.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include<netdb.h>



#define CHANGE_WORKING_DIR_ON_STARTUP
#define DEBUG_SOCK_REUSEADDR



#define PROGRAM_NAME "LineBasedTime Server"
#define PROGRAM_VERSION "1.0"

#define USAGE_STRING "\
 --- " PROGRAM_NAME " v" PROGRAM_VERSION " --- \n\
\n\
Usage - arguments:\n\
\t1. IPv4/v6 address or hostname to listen on\n\
\t2. TCP port number or service name to listen on\n\
"
#define SIGNAL_MESSAGE "[INFO] Signal received; server is going to stop.\n"

#define LISTEN_BACKLOG 10



volatile sig_atomic_t g_continue_running = 1;
// initialized in initialize_globals() and never changed after that!!!
char g_proto_ver_str[4];
char g_err_string[17];



void usage(void);
void crash(const char *, const char *, ...);
void fai_crash(struct addrinfo *, const char *, const char *, ...);
void print_info(const bool, const char *, ...);

void initialize_globals(void);
void set_signal_handlers(void);
void signal_handler(int);

void print_server_socket_info(struct addrinfo *);
int create_server_socket(const char *, const char *);
void close_server_socket(int);

void wait_for_clients(int);
bool serve_client(int);
void send_error_to_client(int);



#endif
