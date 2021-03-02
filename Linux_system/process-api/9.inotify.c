#include <sys/inotify.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>


static void displayInotifyEvent(struct inotify_event *i)
{
    printf(" WD=%2d; ", i->wd);

    if(i->cookie > 0)
        printf("cookie=%4d; ", i->cookie);

    printf("mask=");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS");
    if (i->mask & IN_ATTRIB) printf("IN_ATTRIB");
    if (i->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS");
    printf("\n");

    if (i->len > 0)
        printf("  name=%s\n", i->name);

}

int main(int argc, char** argv)
{
    int inotifyFd, wd, j;
    char buf[256];
    int numRead;
    char* p;

    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") ==0)
    {
        printf("%s pathname...\n", argv[0]);
        return 0;
    }


    inotifyFd = inotify_init();
    if (inotifyFd == -1)
    {
        printf("init failed\n");
        return -1;
    }

    for (j =1 ; j < argc; j++)
    {
        if (wd == -1)
        {
            printf("wd error..\n");
            return -2;
        }

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    for(;;)
    {
        numRead = read(inotifyFd, buf, 256);
        if (numRead == 0)
        {
            printf("read() from inotify fd return 0!");
            return -3;
        }

        if (numRead == -1)
        {
            printf("read() from inotify fd return -1!");
            return -4;
        }


        for (p=buf; p < buf + numRead;)
        {
            event = (struct inotify_event*)p;
            displayInotifyEvent(event);
            p + sizeof(struct inotify_event) + event->len;
        }
    }

    return 0;
}