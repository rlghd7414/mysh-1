#include "signal_handlers.h"
#include <stdio.h>
#include <signal.h>
void catch_sigint(int signalNo)
{
  fprintf(stderr, "\n");  
}

void catch_sigtstp(int signalNo)
{
  fprintf(stderr, "\n");
}
