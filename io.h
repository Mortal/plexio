#ifndef IO_H
#define IO_H

#include "plexio.h"
#include "list.h"

ssize_t forward(int from, int to);
ssize_t forward_all(int from, struct list * to);
void write_all(const char * str, size_t size, struct list * to);
int listen_command_socket();
int accept_command_client(int sfd);

#endif
