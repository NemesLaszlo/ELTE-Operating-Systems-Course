#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Summary:
// Parent sets up signal handlers for SIGTERM and SIGUSR1.
// Parent suspends execution using sigsuspend, waiting only for SIGTERM.
// Child waits 3 seconds and sends SIGUSR1 (ignored by parent).
// Child waits another 3 seconds and sends SIGTERM.
// Parent resumes execution upon receiving SIGTERM.
// Parent waits for child to terminate.
// Both processes exit.

// This function is executed when a signal is received.
void handler(int signumber)
{
  printf("Signal with number %i has arrived\n", signumber);
}

int main()
{

  struct sigaction sigact; // Declare a sigaction structure to define how signals are handled.
  sigact.sa_handler = handler;  // Set the handler function (handler) for signals.
  sigemptyset(&sigact.sa_mask); // Initialize sa_mask to block no additional signals while handling a signal.
  sigact.sa_flags = 0; // No special behavior flags are set.

  // Register the handler function for SIGTERM and SIGUSR1 signals.
  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGUSR1, &sigact, NULL);
  // How sigaction works:
  // - The first argument is the signal (SIGTERM or SIGUSR1).
  // - The second argument is the new sigaction struct.
  // - The third argument (NULL) means we don’t store the previous action.

  pid_t child = fork();
  if (child > 0)
  {
    // Initialize a signal set (sigset) and add all signals to it.
    sigset_t sigset;
    sigfillset(&sigset);
    // Remove SIGTERM from the blocked set, meaning SIGTERM is the only signal the process is waiting for.
    sigdelset(&sigset, SIGTERM);
    // Suspend the parent process, waiting for SIGTERM. Other signals are blocked.
    sigsuspend(&sigset);
    // This message appears when SIGTERM is received, resuming the process.
    printf("The program comes back from suspending\n");
    int status;
    wait(&status);
    printf("Parent process ended\n");
  }
  else
  {
    printf("Waits 3 seconds, then send a SIGUSR %i signal (it is not waited for by suspend) - so the suspend continues waiting\n", SIGUSR1);
    sleep(3);
    // Since SIGUSR1 is not unblocked in sigsuspend, the parent ignores it.
    kill(getppid(), SIGUSR1);
    printf("Waits 3 seconds, then send a SIGTERM %i signal (it is waited for by suspend)\n", SIGTERM);
    sleep(3);
    // This wakes up the parent from sigsuspend.
    kill(getppid(), SIGTERM);
    printf("Child process ended\n");
  }
  return 0;
}
