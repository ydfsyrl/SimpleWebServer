#ifndef UTILS 
#define UTILS

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
// #include <string.h>
// #include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <time.h>

#include "./timer/lst_timer.h"
#include "./http/http_conn.h"
#include "./timer/tw_timer.h"

class Utils
{
public:
    Utils() {}
    ~Utils() {}

    void init(int timeslot, int timer_mode);

    //对文件描述符设置非阻塞
    int setnonblocking(int fd);

    //将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int             *u_pipefd;
    sort_timer_lst  m_timer_lst;  // 升序计时链表
    tw::time_wheel     m_tw_timer;
    static int              u_epollfd;
    int                          m_TIMESLOT;
    int                          m_timer_mode;
};

void cb_func(client_data *user_data);

namespace tw {
    void cb_func(client_data *user_data);
}

#endif 
