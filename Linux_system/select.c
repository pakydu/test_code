#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>



#define FIFO_CLEAR 0X01
#define BUFFER_LEN 20


int main()
{
    int fd, num;
    char rd_ch[BUFFER_LEN];
    fd_set rfds, wfds;

    fd = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    if (fd != -1)
    {
        if (ioctl(fd, FIFO_CLEAR, 0) < 0)
        {
            printf("ioctl command failed.\n");
        }

        struct timeval tOut;
        tOut.tv_sec = 3;
        tOut.tv_usec = 0;

        while (1)
        {
            //every time, we need set the fds, because select() will changed them. 
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_SET(fd, &rfds);
            //FD_SET(fd, &wfds);

            //reset the timeout again.
            tOut.tv_sec = 3;
            tOut.tv_usec = 0;

            int ret = select(fd+1, &rfds, &wfds, NULL, &tOut);
            printf("ret:%d\n", ret);

            if (ret > 0)
            {
                if (FD_ISSET(fd, &rfds))
                    printf("Select monitor: can be read.\n");

                //if (FD_ISSET(fd, &wfds))
                //    printf("Select monitor: can be write.\n");
            }
            else if (ret == 0)//timeout
            {
                printf("Time out...\n");                    
            }
            else
            {
                printf("Error....\n");
            }
            
        }
    }
    else
    {
        printf("Device open failed.\n");
    }
    
    
}
