#include <unistd.h>
#include "child.h"
#include "plexio.h"

void exec_child(int reader, int writer);

pid_t fork_child() {
  int prog_in[2];
  int prog_out[2];
  pipe(prog_in);
  pipe(prog_out);
  pid_t child = fork();
  if (!child) {
    exec_child(prog_in[0], prog_out[1]);
    return -1;
  }
  if (child == -1)
    handle_error("fork");
  guest_in = prog_in[1];
  guest_out = prog_out[0];
  return child;
}

void exec_child(int reader, int writer) {
  dup2(reader, 0);
  dup2(writer, 1);
  dup2(writer, 2);
  execlp("/bin/cat", "cat", NULL);
  handle_error("exec");
}
