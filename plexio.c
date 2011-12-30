#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>

#include "plexio.h"
#include "child.h"
#include "io.h"
#include "list.h"
#include "lines.h"

int guest_in, guest_out;

void handle_error(const char * msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int restart;
char * socketpath;

void parse_args(int * argc, char *** argv) {
  char * progname = (*argv)[0];
  restart = 0;
  socketpath = 0;

  --*argc, ++*argv;

  while (*argc) {
    char * arg = (*argv)[0];
    if (!strcmp("-r", arg)) {
      restart = 1-restart;
      printf("Restarting upon exit\n");
    } else {
      break;
    }
    --*argc, ++*argv;
  }

  if (*argc < 2) {
    printf("Usage: %s [-r] <socketpath> <daemon> <daemon args>\n", progname);
    exit(1);
  }

  socketpath = (*argv)[0];
  --*argc, ++*argv;
}

int main(int argc, char ** argv) {
  parse_args(&argc, &argv);

  pid_t child = fork_child(argc, argv);
  register_sigchld_handler();

  int sfd = listen_command_socket();

  int eof = 0;

  struct list * rfds_l = new_list();

  list_insert(rfds_l, 0);

  struct lines * scrollback = new_lines(30);

  while (restart || !eof) {
    if (eof) {
      waitpid(child, NULL, 0);
      child = fork_child(argc, argv);
      eof = 0;
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);
    FD_SET(guest_out, &rfds);
    int max = (sfd > guest_out) ? sfd : guest_out;
    for_each_list(rfds_l, l_, i, el) {
      if (el == -1) continue;
      FD_SET(el, &rfds);
      if (el > max) max = el;
    }
    int retval = select(max+1, &rfds, NULL, NULL, NULL);
    if (retval < 0) {
      if (got_sigchld) {
	eof = 1;
	continue;
      }
      handle_error("select");
    }
    while (retval > 0) {
      if (FD_ISSET(sfd, &rfds)) {
	int cfd = accept_command_client(sfd);
	int j;
	lines_for_each(j, scrollback) {
	  char * line = scrollback->lines[j];
	  write_one(line, strlen(line), cfd);
	}
	list_insert(rfds_l, cfd);
	--retval;
      }
      if (FD_ISSET(guest_out, &rfds)) {
	char * buf = read_and_forward_all(guest_out, rfds_l);
	if (buf == NULL) {
	  eof = 1;
	}
	int line_beginning = 0;
	int idx = 0;
	while (buf[idx]) {
	  if (buf[idx] == '\n') {
	    lines_insert(scrollback, strndup(&buf[line_beginning], idx+1-line_beginning));
	    line_beginning = idx+1;
	  }
	  ++idx;
	}
	FD_CLR(guest_out, &rfds);
	--retval;
      }
      for_each_list(rfds_l, l__, j, fd) {
	if (!retval) break;
	if (fd == -1) continue;
	if (!FD_ISSET(fd, &rfds)) continue;
	--retval;
	if (!forward(fd, guest_in)) {
	  list_find_and_remove(rfds_l, fd);
	  close(fd);
	  break;
	}
	FD_CLR(fd, &rfds);
      }
    }
  }
  close(guest_in);
  close(guest_out);
  {
    int status = -1;
    waitpid(child, &status, 0);
#define EXITMESSAGE "Program exited with code %d\n"
    int len = 1 + snprintf(NULL, 0, EXITMESSAGE, status);
    char *msg = (char *) malloc(len * sizeof(char));
    snprintf(msg, len, EXITMESSAGE, status);
    write_all(msg, len, rfds_l);
    for_each_list(rfds_l, l_, i, fd) {
      if (fd != -1) close(fd);
    }
  }
  return 0;
}
/* vim:set sw=2 ts=8 sts=2 noet: */
