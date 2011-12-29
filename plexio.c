#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "plexio.h"
#include "child.h"
#include "io.h"

int guest_in, guest_out;

void handle_error(const char * msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main() {
  /*pid_t child =*/ fork_child();

  int sfd = listen_command_socket();

  int cfd = accept_command_client(sfd);

  int eof = 0;

  while (!eof) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(cfd, &rfds);
    FD_SET(guest_out, &rfds);
    int max = 1+((cfd > guest_out) ? cfd : guest_out);
    int retval = select(max, &rfds, NULL, NULL, NULL);
    if (retval < 0)
      handle_error("select");
    while (retval) {
      if (FD_ISSET(cfd, &rfds)) {
	if (!forward(cfd, guest_in)) {
	  eof = 1;
	  break;
	}
	FD_CLR(cfd, &rfds);
      } else if (FD_ISSET(guest_out, &rfds)) {
	if (!forward(guest_out, cfd)) {
	  eof = 1;
	  break;
	}
	FD_CLR(guest_out, &rfds);
      }
      --retval;
    }
  }
  close(guest_in);
  close(guest_out);
  close(cfd);
  return 0;
}
/* vim:set sw=2 ts=8 sts=2 noet: */
