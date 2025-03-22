#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int child_communication_in[2];
int child_communication_out[2];

int rand_num(int max) {
    return rand() % max;
}

pid_t child_1(int pipe_id_rec, int pipe_id_send) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) return pid;

    close(pipe_id_send); // Child 1 does not write to this pipe
    close(child_communication_out[0]); // Only writing to this pipe

    int arrived;
    read(pipe_id_rec, &arrived, sizeof(int)); 
    printf("Child_1 received: %d\n", arrived);

    write(child_communication_out[1], &arrived, sizeof(int));

    srand(time(NULL) ^ getpid()); // Unique seed per process

    for (int i = 0; i < arrived; i++) {
        int random_num = rand_num(1000);
        write(child_communication_out[1], &random_num, sizeof(int));
    }

    close(pipe_id_rec);
    close(child_communication_out[1]);
    exit(0);
}

pid_t child_2(int pipe_id_rec) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) return pid;

    close(child_communication_out[1]); // Only reading from this pipe

    int arrived;
    read(pipe_id_rec, &arrived, sizeof(int)); 
    printf("Child_2 received: %d\n", arrived);

    for (int i = 0; i < arrived; i++) {
        int buffer;
        read(pipe_id_rec, &buffer, sizeof(int));
        printf("Child_2 received random number: %d\n", buffer);
    }

    close(pipe_id_rec);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int console_num = atoi(argv[1]);

    int io_pipes_out[2]; 
    int io_pipes_in[2]; 
    pipe(io_pipes_out);
    pipe(io_pipes_in);
    pipe(child_communication_in);
    pipe(child_communication_out);

    pid_t c1 = child_1(io_pipes_out[0], io_pipes_in[1]); 
    pid_t c2 = child_2(child_communication_out[0]);

    close(io_pipes_out[0]);
    close(io_pipes_in[1]);
    close(child_communication_out[0]);

    write(io_pipes_out[1], &console_num, sizeof(int)); 
    close(io_pipes_out[1]); 

    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);

    close(io_pipes_in[0]);
    close(child_communication_out[1]);
    return 0;
}