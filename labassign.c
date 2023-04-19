#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX 3
#define MSG 100

int num_child = 0;
pid_t child_pid[MAX];    
int pipe_fd[MAX][2];     


void sigint_handler(int sig) {
    printf("\n\n  Ctrl+c Detected!Exit The Program.\n");
    exit(0);
}


void parent_process() {
    for (int i=0; i< MAX; i++) { 
        sleep(1);
        fflush(stdin);
        printf("\nEnter message to send to child %d: ", i+1);
        char message[MSG];
        if (fgets(message, sizeof(message), stdin) == NULL) {
            printf("\nError reading input.");
            exit(1);
        }
        printf("Parent writing to child %d\n", i+1);
        write(pipe_fd[i][1], message, sizeof(message));
    }
}

void child_process(int id) {
    printf("Child %d reading from pipe\n", id+1);

    char message[MSG];
    read(pipe_fd[id][0], message, sizeof(message));
    printf("Child %d received message: %s", id+1, message);
}

int main() {
    printf("ctrl+c to exit the program\n");
    signal(SIGINT, sigint_handler);
    

    for (int i = 0; i < MAX; i++) {
        if (pipe(pipe_fd[i]) == -1) {
            printf("Error creating pipe for child %d\n", i+1);
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error forking child %d\n", i+1);
            exit(1);
        } else if (pid == 0) {
            child_process(num_child); 
            exit(0);
        } else {
            child_pid[num_child] = pid; 
            num_child++;
            if (num_child == MAX) {
                parent_process();
                pause();
            }
        }  
    }
    
    for (int i = 0; i < num_child; i++) {
        waitpid(child_pid[i], NULL, 0);
    }

    
    return 0;
    
}
