#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Restart target program on exit
int restart;
// Path to UNIX socket to listen on
char * socketpath;
// Number of lines in scrollback buffer
long int scrollback_size;
// Name used to start plexio
char * progname;

static void usage() {
  printf("Usage: %s [-r] [-l scrollback size] <socketpath> <daemon> <daemon args>\n", progname);
}

// Set global parameter variables and update argc and argv of caller.
static void parse_args(int * argc, char *** argv) {
  // Store program name.
  progname = (*argv)[0];
  restart = 0;
  socketpath = 0;
  scrollback_size = 30;

  // Shift program name.
  --*argc, ++*argv;

  // Usage: [-l n] [-r] [--] <socket> <program> [options...]
  while (*argc) {
    char * arg = (*argv)[0];
    if (!strcmp("--", arg)) {
      --*argc, ++*argv;
      break;
    } else if (!strcmp("-l", arg)) {
      --*argc, ++*argv;
      char * endptr;
      scrollback_size = strtol((*argv)[0], &endptr, 0);
      if ((!*(*argv)[0] || *endptr) || (scrollback_size < 0)) {
	printf("Argument to -l must be a non-negative integer\n");
	usage();
	exit(1);
      }
    } else if (!strcmp("-r", arg)) {
      restart = 1-restart;
    } else {
      break;
    }
    // Shift argument.
    --*argc, ++*argv;
  }

  // We require two additional arguments: Socket path and program.
  if (*argc < 2) {
    usage();
    exit(1);
  }

  // Store socket path.
  socketpath = (*argv)[0];
  --*argc, ++*argv;
}

int main(int argc, char ** argv) {
  // Parse arguments
  parse_args(&argc, &argv);

  // Start target program
  pid_t child = fork_child(argc, argv);
  register_sigchld_handler();

  // Open UNIX socket
  int sfd = listen_command_socket();

  // Set when we get EOF from the target program
  int eof = 0;

  // List of program clients
  struct list * rfds_l = new_list();

  // Add stdin to clients
  list_insert(rfds_l, 0);

  // Scrollback buffer
  struct lines * scrollback = scrollback_size ? new_lines(scrollback_size) : NULL;

  while (restart || !eof) {
    if (eof) {
      // This is not the first iteration of the loop; restart program.
      waitpid(child, NULL, 0);
      child = fork_child(argc, argv);
      eof = 0;
    }

    // select(1) file descriptors:
    fd_set rfds;
    FD_ZERO(&rfds);

    // Select new socket clients
    FD_SET(sfd, &rfds);

    // Select the stdout/stderr of the target program
    FD_SET(guest_out, &rfds);

    // The highest file descriptor
    int max = (sfd > guest_out) ? sfd : guest_out;

    // Add each client
    for_each_list(rfds_l, l_, i, el) {
      if (el == -1) continue;
      FD_SET(el, &rfds);
      if (el > max) max = el;
    }

    // Wait for something to happen
    int retval = select(max+1, &rfds, NULL, NULL, NULL);

    if (retval < 0) {
      // select might fail due to SIGCHLD; this is handled by the signal handler
      if (got_sigchld) {
	eof = 1;
	continue;
      }
      // otherwise, report error to user
      handle_error("select");
    }

    // Handle each event
    while (retval > 0) {
      if (FD_ISSET(sfd, &rfds)) {
	// We got a new client

	int cfd = accept_command_client(sfd);
	int j;

	// Send scrollback buffer to new client
	if (scrollback != NULL) {
	  lines_for_each(j, scrollback) {
	    char * line = scrollback->lines[j];
	    write_one(line, strlen(line), cfd);
	  }
	}

	// Add to list of clients
	list_insert(rfds_l, cfd);
	--retval;
      }

      if (FD_ISSET(guest_out, &rfds)) {
	// Target program sent output

	char * buf = read_and_forward_all(guest_out, rfds_l);
	if (buf == NULL) {
	  eof = 1;
	}

	// Go through buffer and add each line to scrollback
	int line_beginning = 0;
	int idx = 0;
	while (buf[idx]) {
	  if (buf[idx] == '\n') {
	    if (scrollback != NULL)
	      lines_insert(scrollback, strndup(&buf[line_beginning], idx+1-line_beginning));
	    line_beginning = idx+1;
	  }
	  ++idx;
	}
	free(buf);
	FD_CLR(guest_out, &rfds);
	--retval;
      }
      // Check if a client sent data to the target program
      for_each_list(rfds_l, l__, j, fd) {
	if (!retval) break;
	if (fd == -1) continue;
	if (!FD_ISSET(fd, &rfds)) continue;
	--retval;
	if (!forward(fd, guest_in)) {
	  // Client disconnected
	  list_find_and_remove(rfds_l, fd);
	  close(fd);
	  break;
	}
	FD_CLR(fd, &rfds);
      }
    }
  }
  // Target program exited and we don't restart it
  close(guest_in);
  close(guest_out);

  // Report program exit to clients and close all sockets
  {
    int status = -1;
    waitpid(child, &status, 0);
#define EXITMESSAGE "Program exited with code %d\n"
    int len = 1 + snprintf(NULL, 0, EXITMESSAGE, status);
    char *msg = (char *) malloc(len * sizeof(char));
    snprintf(msg, len, EXITMESSAGE, status);
    write_all(msg, len, rfds_l);
    free(msg);
    for_each_list(rfds_l, l_, i, fd) {
      if (fd != -1) close(fd);
    }
  }

  free_list(rfds_l);

  if (scrollback) {
    int j;
    lines_for_each(j, scrollback) {
      if (scrollback->lines[j])
	free(scrollback->lines[j]);
    }
    free_lines(scrollback);
  }
  return 0;
}
/* vim:set sw=2 ts=8 sts=2 noet: */
