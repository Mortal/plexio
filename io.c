#include <stdlib.h>
#include <unistd.h>
#include "io.h"

int forward(int from, int to) {
  void * buf = malloc(BUFSIZE);
  ssize_t r = read(from, buf, BUFSIZE);
  if (r < 0)
    handle_error("forward read");
  if (!r)
    return 0;
  write(to, buf, r);
  return r;
}
