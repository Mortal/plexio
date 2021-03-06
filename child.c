#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "child.h"
#include "plexio.h"

static void exec_child(int reader, int writer, int argc, char ** argv);

pid_t child_pid;
int got_sigchld;

pid_t fork_child(int argc, char ** argv) {
  int prog_in[2];
  int prog_out[2];
  pipe(prog_in);
  pipe(prog_out);
  pid_t child = fork();
  if (!child) {
    exec_child(prog_in[0], prog_out[1], argc, argv);
    return -1;
  }
  if (child == -1)
    handle_error("fork");
  child_pid = child;
  guest_in = prog_in[1];
  guest_out = prog_out[0];
  return child;
}

/*static*/ void exec_child(int reader, int writer, int argc, char ** argv) {
  dup2(reader, 0);
  dup2(writer, 1);
  dup2(writer, 2);
  char ** argvp = (char **) malloc((argc+1) * sizeof(char *));
  memcpy(argvp, argv, argc * sizeof(char *));
  argvp[argc] = NULL;
  execvp(argv[0], argvp);
  handle_error("exec");
}

static void sigchld_handler(int pid, siginfo_t * info, void * p) {
  (void)(pid||info||p);
  got_sigchld = 1;
}

void register_sigchld_handler() {
  struct sigaction sa;
  sa.sa_sigaction = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
  if (-1 == sigaction(SIGCHLD, &sa, NULL))
    handle_error("sigaction");

  got_sigchld = 0;
}
