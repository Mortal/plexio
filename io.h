#ifndef IO_H
#define IO_H

#include "plexio.h"

ssize_t forward(int from, int to);
int listen_command_socket();

#endif
