#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int signaled=0;
sem_t sem;
 int ok=0;

 void *thread_function(void *arg)
{
    int thread_id = *((int *)arg);
    pthread_mutex_lock(&lock);

    if(thread_id == 3 && ok == 0)
    {
    
            pthread_cond_wait(&cond, &lock);
            info(BEGIN, 2, thread_id);
        
        
    }
    else if(ok == 1 && thread_id == 3){
            info(BEGIN,2,thread_id);
        
    }

    else if(thread_id == 4)
    {
        ok = 1;
        info(BEGIN, 2, thread_id);
        pthread_cond_signal(&cond);
       
    }
    else
        info(BEGIN, 2, thread_id);
    
    if(thread_id ==4){
         pthread_cond_wait(&cond,&lock);
        info(END, 2, thread_id);
    }
    else if(thread_id==3)
    { 
        info(END, 2, thread_id);
        pthread_cond_signal(&cond);
    }
    else 
       info(END, 2, thread_id);

    pthread_mutex_unlock(&lock);

    return NULL;
}


 void *thread_function2_4(void* arg)
{
    int thread_id = *(int*)arg;
    info(BEGIN, 2, thread_id);

    pthread_mutex_lock(&lock);
    // critical section
    info(END, 2, thread_id);
    pthread_mutex_unlock(&lock);

    return NULL;
} 
    void *thread_function5(void* arg)
{
    int thread_id = *(int*)arg;
    info(BEGIN, 5, thread_id);
  
    info(END,5,thread_id);
    return NULL;
    }
    void *thread_function4(void* arg)
{
    int thread_id = *(int*)arg;
    sem_wait(&sem);
    info(BEGIN, 4, thread_id);
    
    info(END,4,thread_id);
    sem_post(&sem);
    return NULL;
    }
int main(){
    init();
    info(BEGIN, 1, 0);
   
    pid_t  pid2, pid3, pid4, pid5, pid6, pid7;

    pid2 = fork(); 

    if (pid2 == 0) { 
        info(BEGIN, 2, 0);
        pthread_t thread_ids[4];
        int thread_args[4] = {1, 2, 3, 4};
        
            pthread_create(&thread_ids[0], NULL, thread_function, &thread_args[0]);
            pthread_create(&thread_ids[1], NULL, thread_function, &thread_args[1]);
            pthread_create(&thread_ids[2], NULL, thread_function, &thread_args[2]);
            pthread_create(&thread_ids[3], NULL, thread_function, &thread_args[3]);
        
        for (int i = 0; i < 4; i++) {
            pthread_join(thread_ids[i], NULL);
        }
        pid6 = fork(); 

        if (pid6 == 0) {
            info(BEGIN, 6, 0); 
            pid7 = fork(); 

            if (pid7 == 0) { 
                info(BEGIN, 7, 0);
                info(END, 7, 0);
                return 0;
            } else { 
                int status;
                waitpid(pid7, &status, 0); // wait for p7 to finish
            }
            info(END,6,0);
            return 0;
        } else { // this is the parent process (p2)
            int status;
            waitpid(pid6, &status, 0); // wait for p6 to finish
        }
        info(END, 2, 0);
        return 0;
    } else { 
        pid3 = fork(); 

        if (pid3 == 0) { 
            info(BEGIN, 3, 0);
            pid4 = fork(); 

            if (pid4 == 0) {
                sem_init(&sem,0,6);
                info(BEGIN, 4, 0);
                pthread_t thread_ids[41];
        int thread_args[41];
        for (int i = 0; i < 41; i++) {
        	thread_args[i]=i+1;
            pthread_create(&thread_ids[i], NULL, thread_function4, &thread_args[i]);
        }
        
        for (int i = 0; i < 41; i++) {
            pthread_join(thread_ids[i], NULL);
        }
                 sem_destroy(&sem);
                info(END,4,0);
                return 0;
            } else { 
                pid5 = fork(); 
                if (pid5 == 0) { 
                    info(BEGIN, 5, 0); 
                    pthread_t thread_ids[4];
        int thread_args[4] = {1, 2, 3, 4};
        for (int i = 0; i < 4; i++) {
            pthread_create(&thread_ids[i], NULL, thread_function5, &thread_args[i]);
        }
        
        for (int i = 0; i < 4; i++) {
            pthread_join(thread_ids[i], NULL);
        }
                    info(END, 5, 0);
                    return 0;
                } else { 
                    int status;
                    waitpid(pid4, &status, 0); // wait for p4 to finish
                    waitpid(pid5, &status, 0); // wait for p5 to finish
                }
            }
            info(END,3,0);
            return 0;
        } else { 
            int status;
            waitpid(pid2, &status, 0); // wait for p2 to finish
            waitpid(pid3, &status, 0); // wait for p3 to finish
        }

        info(END, 1, 0);
        return 0;
    }
}
