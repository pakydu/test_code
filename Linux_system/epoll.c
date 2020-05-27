#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#define MAX_BUF 1000
#define MAX_EVENTS 5


int main(int argc, char ** argv)
{
    int epfd, ready, fd, s,j, numOpenFds;
    struct epoll_event ev;
    struct epoll_event evlist[MAX_EVENTS];

    char buf[MAX_BUF];

    if (argc < 2 )
        exit(1);

    epfd = epoll_create(argc -1);
    if (epfd == -1)
        exit(3);

    for (j = 1; j < argc; j++)
    {
        fd = open(argv[j], O_RDONLY);
        if (fd == -1)
            exit(4);

        printf("Opened \"%s\" on fd %d.\n", argv[j], fd);

        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
            exit(5);
        
    }

    numOpenFds = argc -1;

    while (numOpenFds > 0)
    {
        printf("About to epoll_wait()\n");
        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if (ready == -1)
        {
            if (errno == EINTR)
                continue;
            else
            {
                exit(6);
            }
            
        }
        printf("Ready: %d\n", ready);

        for (j = 0; j < ready; j++)
        {
            printf("fd=%d, event: %s%s%s\n", evlist[j].data.fd, 
            (evlist[j].events & EPOLLIN) ? "EPOLLIN" : "", 
            (evlist[j].events & EPOLLHUP) ? "EPOLLHUP" : "", 
            (evlist[j].events & EPOLLERR) ? "EPOLLERR" : ""            );

            if (evlist[j].events & EPOLLIN)
            {
                memset(buf, 0, sizeof(buf));
                s  = read(evlist[j].data.fd, buf, MAX_BUF);
                if (s == -1)
                    exit(7);
                printf("read %d bytes: %s", s,buf);
            }
            else if (evlist[j].events & (EPOLLHUP | EPOLLERR))
            {
                printf(" closing fd %d\n", evlist[j].data.fd);
                if (close(evlist[j].data.fd) == -1)
                    exit(8);
                numOpenFds--;
            }
        }
    }
    printf("All file closed; bye\n");
}