#ifndef CHILD_H
#define CHILD_H

#include <sys/types.h>

pid_t fork_child(int argc, char ** argv);
void register_sigchld_handler();
extern int got_sigchld;

#endif
