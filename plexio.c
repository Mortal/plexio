#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/select.h>

#include "plexio.h"
#include "child.h"
#include "io.h"
#include "list.h"

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

  struct list * rfds_l = new_list();
  list_insert(rfds_l, cfd);
  list_insert(rfds_l, guest_out);

  while (!eof) {
    fd_set rfds;
    int max = 0;
    FD_ZERO(&rfds);
    for_each_list(rfds_l, l_, i, el) {
      if (el == -1) continue;
      FD_SET(el, &rfds);
      if (el > max) max = el;
    }
    int retval = select(max+1, &rfds, NULL, NULL, NULL);
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
