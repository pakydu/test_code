#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <time.h>


int main(int argc, char** argv)
{
    int numPipes, j, ready, randPipe, numWrites;

    int (*pfds)[2];
    struct pollfd *pollFd;
    
    numPipes = 2;
    pfds = calloc(numPipes, sizeof(int[2]));
    if (pfds == NULL)
        exit(1);

    pollFd = calloc(numPipes, sizeof(struct pollfd));
    
    //Create the number of pipes
    for (j = 0; j < numPipes; j++)
        if (pipe(pfds[j]) == -1)
            exit(3);
    
    numWrites = 2;
    srandom((int)time(NULL));
    for (j = 0; j < numWrites; j++)
    {
        randPipe = random() % numPipes;
        printf("Writeing to fd:%d(read fd:%d)\n", pfds[randPipe][1], pfds[randPipe][0]);
        if (write(pfds[randPipe][1], "a", 1) == -1)
            exit(4);
    }

    for (j = 0; j < numPipes; j++)
    {
        pollFd[j].fd = pfds[j][0];
        pollFd[j].events = POLLIN;
    }

    ready = poll(pollFd, numPipes, -1);
    if (ready == -1)
        exit(5);

    printf("poll return : %d\n", ready);

    //check the data
    for (j = 0; j < numPipes; j++)
        if (pollFd[j].revents & POLLIN)
            printf("Reading: %d %d\n", j, pollFd[j].fd);
}