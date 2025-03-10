#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

pid_t mainProcessValue;

void signal_handler(int sig) {
    // Empty handler to wake up sigsuspend
}

pid_t child1() {
    pid_t process = fork();
    if (process == -1)
        exit(EXIT_FAILURE);
    if (process > 0)
        return process;

    printf("Child1 ready!\n");
    kill(mainProcessValue, SIGUSR1); 

    exit(EXIT_SUCCESS);
}

pid_t child2() {
    pid_t process = fork();
    if (process == -1)
        exit(EXIT_FAILURE);
    if (process > 0)
        return process; 

    printf("Child2 ready!\n");
    kill(mainProcessValue, SIGUSR1);

    exit(EXIT_SUCCESS);
}

int main() {
    mainProcessValue = getpid(); 

    // We define a sigaction structure named sa, which specifies how the process should handle SIGUSR1 signals.
    struct sigaction sa;
    sa.sa_handler = signal_handler; // Sets signal_handler as the function to handle SIGUSR1
    sigemptyset(&sa.sa_mask); // Clears the signal mask, meaning no signals are blocked during the execution of signal_handler.
    sa.sa_flags = 0; //  No special flags are used.
    sigaction(SIGUSR1, &sa, NULL); // Registers sa as the new signal handler for SIGUSR1.

    pid_t c1 = child1();
    pid_t c2 = child2();

    // Creates two signal sets:
    // - mask (which will contain SIGUSR1)
    // - oldmask (to store the previous signal mask)
    // mask contains SIGUSR1 (which we blocked).
    // oldmask contains the previous signal mask (which did not block SIGUSR1).
    sigset_t mask, oldmask;
    sigemptyset(&mask); // Initializes mask to be empty.
    sigaddset(&mask, SIGUSR1); //  Adds SIGUSR1 to mask, meaning we want to block it. -> Ensures that the signals aren't accidentally handled before we call sigsuspend()   We only want to process signals when we are explicitly waiting for them
    // Blocks SIGUSR1, preventing it from being handled until we’re ready. 
    // Saves the previous signal mask in oldmask, so we can restore it later.
    sigprocmask(SIG_BLOCK, &mask, &oldmask); // Blocks SIGUSR1, so it cannot be delivered yet. Saves the old signal mask (the state before blocking SIGUSR1) in oldmask.

    // Temporarily unblocks SIGUSR1 and pauses execution until a signal arrives. -> When SIGUSR1 arrives from child1(), 
    // the process wakes up and executes signal_handler(), then resumes execution after sigsuspend().
    // First wait (for first child signal)
    sigsuspend(&oldmask); // -> &oldmask IMPORTANT -> Temporarily restores the signal mask to oldmask (which means SIGUSR1 is no longer blocked).
    // sigsuspend(&mask); // Wrong! Still blocks SIGUSR1 -> Then SIGUSR1 would still be blocked, and sigsuspend() would never return because it would never receive SIGUSR1!
    printf("Child1 signal received!\n");

    // The process is suspended again until another SIGUSR1 arrives.
    // Second wait (for second child signal)
    sigsuspend(&oldmask);
    printf("Child2 signal received!\n");

    // Restore original signal mask
    // Restores the original signal mask (the one before we blocked SIGUSR1).
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    printf("Both children ready! Proceeding...\n");

    // Wait for first child
    int status1;
    pid_t finished1 = waitpid(c1, &status1, 0);
    if (WIFEXITED(status1)) {
        printf("Child1 (PID %d) exited with status %d\n", finished1, WEXITSTATUS(status1));
    }

    // Wait for second child
    int status2;
    pid_t finished2 = waitpid(c2, &status2, 0);
    if (WIFEXITED(status2)) {
        printf("Child2 (PID %d) exited with status %d\n", finished2, WEXITSTATUS(status2));
    }

    printf("Done\n");

    return 0;
}
