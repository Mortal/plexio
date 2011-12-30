#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "io.h"

ssize_t forward(int from, int to) {
  void * buf = malloc(BUFSIZE);
  ssize_t r = read(from, buf, BUFSIZE);
  if (r < 0) {
    free(buf);
    handle_error("forward read");
  }
  if (!r) {
    free(buf);
    return 0;
  }
  write(to, buf, r);
  free(buf);
  return r;
}

ssize_t forward_all(int from, struct list * to) {
  void * buf = malloc(BUFSIZE);
  ssize_t r = read(from, buf, BUFSIZE);
  if (r < 0) {
    free(buf);
    handle_error("forward read");
  }
  if (!r) {
    free(buf);
    return 0;
  }
  write_all(buf, r, to);
  free(buf);
  return r;
}

void write_all(const char * buf, size_t r, struct list * to) {
  for_each_list(to, to_, i, fd) {
    if (fd == -1) continue;
    write(fd, buf, r);
  }
}

int listen_command_socket() {
  /* from bind(2) */
  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  struct sockaddr_un my_addr;
  memset(&my_addr, 0, sizeof(struct sockaddr_un)); /* Clear structure */
  my_addr.sun_family = AF_UNIX;
  strncpy(my_addr.sun_path, socketpath,
	  sizeof(my_addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *) &my_addr,
	   sizeof(struct sockaddr_un)) == -1)
    handle_error("bind");

  if (listen(sfd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  return sfd;
}

int accept_command_client(int sfd) {
  /* Now we can accept incoming connections one
     at a time using accept(2) */

  struct sockaddr_un peer_addr;
  socklen_t peer_addr_size = sizeof(struct sockaddr_un);
  int cfd = accept(sfd, (struct sockaddr *) &peer_addr,
		   &peer_addr_size);
  if (cfd == -1)
    handle_error("accept");

  return cfd;
}
