#ifndef PLEXIO_H
#define PLEXIO_H

#define BUFSIZE (4096)

#define LISTEN_BACKLOG 50

void handle_error(const char * msg);

// File descriptors of target program input and output
extern int guest_in, guest_out;
// Path to UNIX socket to listen on
extern char * socketpath;

#endif
