use to save the test code

IO event testing:
    select()  --> fd_set rFds, wFds, eFds;
    poll()    --> struct pollfd
    epoll()   --> Just for Linux, but it is the best funciton to monitor many FD's event.
