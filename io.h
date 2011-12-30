#ifndef IO_H
#define IO_H

#include "plexio.h"
#include "list.h"

ssize_t forward(int from, int to);
char * read_and_forward_all(int from, struct list * to);
void write_one(const char * str, size_t size, int to);
void write_all(const char * str, size_t size, struct list * to);
int listen_command_socket();
int accept_command_client(int sfd);

#endif
