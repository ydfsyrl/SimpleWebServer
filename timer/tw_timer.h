#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <time.h>
#include <netinet/in.h>
#include <stdio.h>

namespace tw{

#define BUFFER_SIZE 64
class tw_timer;
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[ BUFFER_SIZE ];
    tw_timer* timer;
};

class tw_timer
{
public:
    tw_timer( int rot=0, int ts=0 ) 
    : next( NULL ), prev( NULL ), rotation( rot ), time_slot( ts ){}

public:
    int rotation;
    int time_slot;
    void (*cb_func)( client_data* );
    client_data* user_data;
    tw_timer* next;
    tw_timer* prev;
};

class time_wheel
{
public:
    time_wheel() : cur_slot( 0 )
    {
        for( int i = 0; i < N; ++i )
        {
            slots[i] = NULL;
        }
    }
    ~time_wheel()
    {
        for( int i = 0; i < N; ++i )
        {
            tw_timer* tmp = slots[i];
            while( tmp )
            {
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }
    /*创建一个定时器，插入适合的槽中*/
    tw_timer* add_timer( int timeout ) // 15
    {
        if( timeout < 0 )
        {
            return NULL;
        }
        int ticks = 0;
        if( timeout < TI )
        {
            ticks = 1;
        }
        else
        {
            ticks = timeout / TI;  // TI=5, ticks=3
        }
        int rotation = ticks / N;
        int ts = ( cur_slot + ticks ) % N;
        tw_timer* timer = new tw_timer( rotation, ts );
        if( !slots[ts] )
        {
            slots[ts] = timer;
            // printf( "add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot );
        }
        else
        {
            timer->next = slots[ts];
            slots[ts]->prev = timer;
            slots[ts] = timer;
        }
        return timer;
    }

// 延后slotnum个单位
void adjust_timer(tw_timer* timer, int slotnum)
{
    if (!timer)
    {
        return;
    }
    
    int ts = timer->time_slot;
        if( timer == slots[ts] )
        {
            slots[ts] = slots[ts]->next;
            if( slots[ts] )
            {
                slots[ts]->prev = NULL;
            }
        }
        else
        {
            timer->prev->next = timer->next;
            if( timer->next )
            {
                timer->next->prev = timer->prev;
            }
        }
    timer->next = NULL;  // !!!
    timer->prev = NULL;
    timer->rotation += (slotnum+ts)/N; //gai
    ts = (ts+slotnum)%N;  
    timer->time_slot = ts;
    // timer->rotation += slotnum/N;    
    if( !slots[ts] )  // 插入
    {
        slots[ts] = timer;
    }
    else
    {
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
}

    void del_timer( tw_timer* timer )
    {
        if( !timer )
        {
            return;
        }
        int ts = timer->time_slot;
        if( timer == slots[ts] )
        {
            slots[ts] = slots[ts]->next;
            if( slots[ts] )
            {
                slots[ts]->prev = NULL;
            }
            delete timer;
        }
        else
        {
            timer->prev->next = timer->next;
            if( timer->next )
            {
                timer->next->prev = timer->prev;
            }
            delete timer;
        }
    }

    void tick()
    {
        tw_timer* tmp = slots[cur_slot];
        printf( "current slot is %d\n", cur_slot );
        while( tmp )
        {
            printf( "tick the timer once\n" );
            if( tmp->rotation > 0 )
            {
                tmp->rotation--;
                tmp = tmp->next;
            }
            else
            {
                tmp->cb_func( tmp->user_data );
                if( tmp == slots[cur_slot] )
                {
                    printf( "delete header in cur_slot\n" );
                    slots[cur_slot] = tmp->next;
                    delete tmp;
                    if( slots[cur_slot] )
                    {
                        slots[cur_slot]->prev = NULL;
                    }
                    tmp = slots[cur_slot];
                }
                else
                {
                    printf( "delete in cur_slot\n" );
                    tmp->prev->next = tmp->next;
                    if( tmp->next )
                    {
                        tmp->next->prev = tmp->prev;
                    }
                    tw_timer* tmp2 = tmp->next;
                    delete tmp;
                    tmp = tmp2;
                }
            }
        }
        cur_slot = ++cur_slot % N;
    }

private:
    static const int N = 12; // N个槽
    static const int TI = 5;  // 一个槽的时间间隔
    tw_timer* slots[N];
    int cur_slot;
};
}

#endif
