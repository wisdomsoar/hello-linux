/*
 * https://www.pacificsimplicity.ca/blog/epoll-devinput-keyboard-press-example/
 * Submitted by admin on Wed, 01/11/2017 - 08:41
 * Epoll /dev/input keyboard press example
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <linux/input.h>

#define MAXEVENTS 10

static const char *const evval[3] = {
        "RELEASED",
        "PRESSED ",
        "REPEATED"
};

static int make_socket_non_blocking(int sfd)
{
        int flags, s;

        flags = fcntl(sfd, F_GETFL, 0);
        if (flags == -1) {
                perror("fcntl");
                return -1;
        }

        flags |= O_NONBLOCK;
        s = fcntl(sfd, F_SETFL, flags);
        if (s == -1) {
                perror("fcntl");
                return -1;
        }

        return 0;
}

int main(int argc, char *argv[])
{
        int sfd, s;
        int efd;
        struct epoll_event event;
        struct epoll_event *events;
        char *dev_path;
        int done = 0;

        if (argc != 2) {
                fprintf(stderr, "Usage: %s [devPath]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        dev_path = argv[1];
        sfd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
        if (sfd == -1) {
                fprintf(stderr, "open_port: Unable to open %s\n", dev_path);
                return (-1);
        }
        make_socket_non_blocking(sfd);

        char name[256] = "Unknown";
        ioctl(sfd, EVIOCGNAME(sizeof(name)), name);
        printf("Reading from device : %s (%s)\n", dev_path, name);

        efd = epoll_create1(0);
        if (efd == -1) {
                perror("epoll_create");
                abort();
        }

        event.data.fd = sfd;
        event.events = EPOLLIN | EPOLLET;
        s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
        if (s == -1) {
                perror("epoll_ctl");
                abort();
        }

        /* Buffer where events are returned */
        events = calloc(MAXEVENTS, sizeof event);

        while (1) {
                int n, i;

                n = epoll_wait(efd, events, MAXEVENTS, -1);
                for (i = 0; i < n; i++) {
                        if ((events[i].events & EPOLLERR) ||
                            (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                                       
                                fprintf(stderr, "epoll error\n");
                                close(events[i].data.fd);
                                continue;
                        }

                        else if (sfd == events[i].data.fd) {
                               
                                while (1) {
                                        int len = 0;
                                        struct input_event key_press[64];
                                        if ((len = read(sfd, &key_press, sizeof key_press)) != -1) {
                                                printf("len: %d\n",len);
                                                printf("Type %d Code %d Val %s \n", key_press[1].type, key_press[1].code, evval[key_press[1].value]);
                                               
                                        }
                                        break;

                                }
                                continue;
                        } else {
                                int done = 0;

                                if (done) {
                                        printf("Closed on descriptor %d\n",
                                               events[i].data.fd);

                                        close(events[i].data.fd);
                                }
                        }
                }
        }

        free(events);

        close(sfd);
        return EXIT_SUCCESS;
}
