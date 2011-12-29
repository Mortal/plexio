#ifndef PLEXIO_H
#define PLEXIO_H

#define BUFSIZE (4096)

#define MY_SOCK_PATH "socket"
#define LISTEN_BACKLOG 50

void handle_error(const char * msg);

extern int guest_in, guest_out;

#endif
