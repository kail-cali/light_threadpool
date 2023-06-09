#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/epoll.h>


#include <semaphore.h>
#include <pthread.h>

#include <fcntl.h>

#define err(str) fprintf(stderr, str)

struct Cluster;
struct Worker;

typedef struct Container{
    int worker_id;
    //char buufer[1024];


} Container;


typedef struct Job{
    struct Job* prev;
    int worker_id;
    void (* handler )(void* args);
    Container* args;

} Job;


typedef struct JobQueue{
    /* FIFO Queue */
    sem_t mutex; // mutex for concuruncy
    pthread_mutex_t rxmutex;
    Job* front;
    Job* rear;
    int len;

} JobQueue;

typedef struct Worker{
    int id;
    pthread_t pthread;
    Job* job // container for new Job

} Worker; 


typedef struct Cluster{
    pthread_t schedule_thread;

    int num_worker;
    Worker** workers;

    int* cluster_pipe;
    int** worker_pipe;
    

    int* schedule_fd;
    int schedule_event_fd;
    struct epoll_event* schedule_events;

    

} Cluster;




/*********************************************************************************/

static int set_event_loop(Cluster* cluster){


    return 0;
}


static int add_event(int epoll_fd, int client_fd, int args){
    struct epoll_event ev;
    ev.events = args;
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) <0){
        
        return -1;
    }

    return 0;
}


static int add_event_non_block(int epoll_fd, int client_fd, int args){

    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags <0){

        return -1;
    }
    if (fcntl(client_fd, F_GETFL, 0)){
        
        return -1;
    }

    struct epoll_event ev;
    ev.events = args;
    ev.data.fd = client_fd;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) <0 ){
        
        return -1;
    }
    


    return 0;
}

static int job_queue_init(Cluster* cluster, JobQueue* job_queue){
    

    cluster -> schedule_fd = (int* )malloc(2* sizeof(int));
    pipe(cluster -> schedule_fd);
    struct epoll_event register_ ;
    register_.events = EPOLLIN | EPOLLET;
    register_.data.fd = cluster -> schedule_fd[0];  // read 
    

    
    if (add_event(cluster->schedule_event_fd, cluster->schedule_fd[0], EPOLLIN | EPOLLET) < 0){
        err("set scheudle event loop failed \n ");
        return -1;
    }
    
    job_queue -> len = 0;
    job_queue -> front = NULL;
    job_queue -> rear = NULL;
    sem_init(&job_queue->mutex, 0, 0);
    pthread_mutex_init(&job_queue->rxmutex, NULL);

    return 0;
}




int main(){
    
        


    return 0;
}



