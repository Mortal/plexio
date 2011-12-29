#include "child.h"
#include "plexio.h"

#include <unistd.h>

void exec_child(int reader, int writer) {
  dup2(reader, 0);
  dup2(writer, 1);
  dup2(writer, 2);
  execlp("/bin/cat", "cat", NULL);
  handle_error("exec");
}

