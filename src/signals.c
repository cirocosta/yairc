#include "yairc/signals.h"

sighandler_t yi_signal(int signo, sighandler_t func)
{
  struct sigaction act, oact;

  act.sa_handler = func;
  // we don't want to interfere with other signals
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if (signo == SIGALRM) {
    // interrupt because we may want to stop a syscall
    act.sa_flags |= SA_INTERRUPT;
  } else {
    // if a syscall is interrupted, restart
    act.sa_flags |= SA_RESTART;
  }

  // change the signal action for the process on receipt of the signal.
  if (sigaction(signo, &act, &oact) < 0)
    return SIG_ERR;

  return oact.sa_handler;
}
