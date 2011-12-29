#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
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

  /* from bind(2) */
  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  struct sockaddr_un my_addr;
  memset(&my_addr, 0, sizeof(struct sockaddr_un)); /* Clear structure */
  my_addr.sun_family = AF_UNIX;
  strncpy(my_addr.sun_path, MY_SOCK_PATH,
	  sizeof(my_addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *) &my_addr,
	   sizeof(struct sockaddr_un)) == -1)
    handle_error("bind");

  if (listen(sfd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  /* Now we can accept incoming connections one
     at a time using accept(2) */

  struct sockaddr_un peer_addr;
  socklen_t peer_addr_size = sizeof(struct sockaddr_un);
  int cfd = accept(sfd, (struct sockaddr *) &peer_addr,
		   &peer_addr_size);
  if (cfd == -1)
    handle_error("accept");

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
