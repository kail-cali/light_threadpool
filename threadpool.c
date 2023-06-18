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

/*---------------------*/


typedef struct Context {
    int max_epoll_event;
    char* root;
    int num_worker;
    int epoll_timeout;


} Context;


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


typedef struct JobScheduler{
    int num_lock;
    int len;
    
    pthread_mutex_t rxmutex;
    int* bit;


} JobScheduler;


typedef struct Worker{
    int id;
    pthread_t pthread;
    Job* job;// container for new Job
    struct Cluster* cluster;
    

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




int SERVICE_KEEPALIVE;


/*********************************************************************************/

static int set_event_loop(Cluster* cluster, int* event_loop_fd){

    


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


static void push_back(JobQueue* job_queue, Job* new_job){

    pthread_mutex_lock(&job_queue -> rxmutex);
    new_job -> prev = NULL;

    if ( job_queue -> len ==0 ){
        job_queue -> front = new_job;
        job_queue -> rear = new_job;
    }
    else if (job_queue -> len >=1){
        job_queue -> rear -> prev = new_job; // link
        job_queue -> rear = new_job; // change rear pointer to new_job
    }
    job_queue -> len ++ ;

    pthread_mutex_unlock(&job_queue -> rxmutex);
    
}

static struct Job* pop_front(JobQueue* job_queue){
    pthread_mutex_lock(&job_queue -> rxmutex);
    Job* front = job_queue -> front;
    if (job_queue -> len == 1){
        job_queue -> len = 0;
        job_queue -> front = NULL;
        job_queue-> rear  = NULL;
    }
    else if (job_queue -> len >=2){
        job_queue -> front = front -> prev;
        job_queue -> len --;
    }
    pthread_mutex_unlock(&job_queue -> rxmutex);
    return front;
}


static void* worker_handler(Worker* worker){
    char worker_name[16] = {0};
    snprintf(worker_name, 16, "worker#%d", worker->id);
    Cluster* cluster = worker -> cluster;
    int self = worker -> id;
    char stage[2];

    while (SERVICE_KEEPALIVE){
        int status = read(cluster -> worker_pipe[self][0], stage, sizeof(stage));

        if ( status < 0){
            continue;
        }

        void(* handler)(void* );
        void* args;
        Job* new_job = worker->job;
        if(new_job ){
            handler = new_job -> handler;
            args = new_job -> args;

            handler(args);

        }



    }




}





static int pipe_init(Cluster* cluster, int num_worker){
    cluster -> worker_pipe = (int**)malloc(num_worker * sizeof(int* ));
    cluster -> cluster_pipe = (int* )malloc(2 * sizeof(int));

    pipe(cluster -> cluster_pipe);

    cluster -> schedule_event_fd = epoll_create(1024);

    cluster -> schedule_events = (struct epoll_event* )malloc(1024 * sizeof(struct epoll_event));





    return 0;
}

static int worker_init(Cluster* cluster, struct Worker** worker, int id){
    
    *worker = (struct Worker* )malloc(sizeof(struct Worker ));
    if (worker == NULL){
        err("worker_init():: allocate worker thread failed \n");
        return -1;
    }

    (*worker) - > cluster = cluster;
    (*worker) -> id = id;
    pthrad_create(&(*worekr)->pthread, NULL, (void* (*)(void* )) worker_handler, (*worker));
    pthread_detach((*worker)->pthread);
    
    return 0;
}


struct Cluster* cluster_init_with_context(Context* context){
    Cluster* cluster;


    return cluster;
}

struct Cluster* cluster_init(int num_worker){

    Cluster* cluster;
    cluster = (struct Cluster* )malloc(sizeof(struct Cluster));
    
    if (cluster == NULL){

        err("cluster_init():: allocate cluster on memory failed \n");
        return NULL;
    }

    cluster -> num_worker = num_worker;
    





    // worker thread init
    cluster -> workers = (struct Worker** )malloc(num_worker * sizeof(struct Worker* ));
    if (cluster -> workers ==NULL){
        err("cluster_init():: allocate worker thread on memeory failed \n");
        return NULL;
    }
    
    for (int i=0; i < num_worker; i ++){
        
    }

    return cluster;
}



int main(){
   SERVICE_KEEPALIVE = 1;

        


    return 0;
}



