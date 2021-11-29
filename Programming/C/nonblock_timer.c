
#include <pthread.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

static void add_event(int epoll_fd, int fd, int state)
{
    struct epoll_event ev = {
        .events = state,
        .data.fd = fd,
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    return;
}

static int calc_proc_time(int *start)
{
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    if (start)
        return end.tv_sec * 1000 + end.tv_nsec / 1000000 - *start;
    return end.tv_sec * 1000 + end.tv_nsec / 1000000;
}

static void *worker(void *user)
{
    int epoll_fd = *(int *) user;
    struct epoll_event events[1] = {0};
    while (1) {
        int ms = calc_proc_time(NULL);
        int fire_events = epoll_wait(epoll_fd, events, 1, -1);
        ms = calc_proc_time(&ms);
        if (fire_events > 0) {
            printf("this wait time: %d ms\n", ms);
            uint64_t exp;
            ssize_t size = read(events[0].data.fd, &exp, sizeof(uint64_t));
            if (size != sizeof(uint64_t))
                perror("read error");
        }
    }
    return NULL;
}

int timer_update(int timer_fd, int ms)
{
    struct itimerspec its = {
        .it_interval = {.tv_sec = ms/1000, .tv_nsec = 0},
        .it_value.tv_sec = ms / 1000,
        .it_value.tv_nsec = (ms % 1000) * 1000 * 1000,
    };
    if (timerfd_settime(timer_fd, 0, &its, NULL) < 0)
        return -1;
    printf("timer update: %d\n", ms);
    return 0;
}

int main()
{
    /* create epoll */
    int epoll_fd = epoll_create(1);

    /* create timer */
    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);

    /* add timer fd to epoll monitor event */
    add_event(epoll_fd, timer_fd, EPOLLIN);

    /* create thread to monitor */
    pthread_t tid;
    pthread_create(&tid, NULL, &worker, (void *) &epoll_fd);

    timer_update(timer_fd, 1000);
    
	for (int i = 1; i < 99999; i++)
	{
		sleep(1);
		printf("tick...%d\n", i);
			timer_update(timer_fd, 20 * 1000);
	}
    
	
    return 0;
}

