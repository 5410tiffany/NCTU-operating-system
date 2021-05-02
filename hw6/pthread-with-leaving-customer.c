#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
# define CUTTING 200 //barber cutting time (ms)
# define MAX_CUST 30 //max. customer #
void * barber_function(void *idp);
void * customer_function(void *idp);
void service_customer();
void * make_customer_function();
void input(const char* file_name);
    
/* Mutex*/
pthread_mutex_t srvCust;

/* Semaphores */
sem_t barber_ready; 
sem_t customer_ready;
sem_t modifySeats;

/* Inputs */
int chair_cnt;
int total_custs;
int arrtime[MAX_CUST];
int available_seats;

/* Output */
int no_served_custs = 0;
int served_cust = 0; //served customer
time_t waiting_time_sum;


int main(){
    
    input("input.txt"); // read input file
    srand(time(NULL)); /* Initialization, should only be called once. */
    pthread_t barber_1; //Barber thread
    pthread_t customer_maker; //thread that creates customers
    int tmp;

    /* Initialize the mutex */
    pthread_mutex_init(&srvCust, NULL);

    /* Initialize semaphores */
    sem_init(&customer_ready, 0, 0);
    sem_init(&barber_ready, 0, 0);
    sem_init(&modifySeats, 0, 1);
    
    available_seats = chair_cnt; 
    
    /* Create barber thread*/
    tmp = pthread_create(&barber_1, NULL, (void *)barber_function, NULL);  

    /* Create customer_maker thread*/
    tmp = pthread_create(&customer_maker, NULL, (void *)make_customer_function, NULL);  
     
   /* Wait for threads to finish */
    pthread_join(barber_1, NULL);
    pthread_join(customer_maker, NULL);
        
    printf("\n------------------------------------------------\n");
    printf("Serviced clients: %d\n", served_cust);
    printf("Unserviced clients: %d\n", no_served_custs);    	
}


void * barber_function(void *idp)
{    
    while(served_cust != (total_custs - no_served_custs))
    {
        /* Lock semaphore "customer_ready" - try to get a customer or sleep if there is none */
        sem_wait(&customer_ready);
        /* Lock semaphore "modifySeats" - try to get access to seats */
        sem_wait(&modifySeats);
        /* Increment by 1 the available seats */
        available_seats++;
        printf("Barber services a client, available seats: %d\n", available_seats);
        /* Unlock semaphore "modifySeats" */
        sem_post(&modifySeats);
        /*Unlock semaphore "barber_ready" - set barber ready to serve" */
        sem_post(&barber_ready);        
        /* Lock mutex "srvCust - protect service from the same barber from other threads */
        pthread_mutex_lock(&srvCust);
        /* serve customer */
        usleep(CUTTING * 1000);  
        /* Unlock mutex "srvCust - finished service */
        pthread_mutex_unlock(&srvCust);
        
        served_cust++; 

    }
    pthread_exit(NULL);    
}

void * customer_function(void *idp)
{  
    struct timeval start, stop;
    /* Lock semaphore "modifySeats" */
    sem_wait(&modifySeats); 
    /* If there is available seat*/
    if(available_seats >= 1)
    {
        /* Occupy a seat*/
        available_seats--;
//         printf("Client[pid = %lu] is waiting, available seats: %d\n", pthread_self(), available_seats);
        printf("Client takes a seat, available seats: %d\n", available_seats);
        /* Start waiting-time counter */   
        gettimeofday(&start, NULL);
        /* Unlock semaphore "customer_ready" - set the customer ready to be served */
        sem_post(&customer_ready);
        /* Unlock semaphore "modifySeats" */
        sem_post(&modifySeats);         
        /* Lock semaphore "barber_ready" - wait for barber to get ready */
        sem_wait(&barber_ready); 
        /* Stop waiting-time counter */ 
        gettimeofday(&stop, NULL);
        double sec = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        /* Assigning the time spent to global variable (ms)*/
        waiting_time_sum += 1000 * sec;
//         printf("Customer[pid = %lu] is being served. \n", pthread_self());        
    }
    else
    {
        /* Unlock semaphore "modifySeats" */
        sem_post(&modifySeats);
        no_served_custs++;
//         printf("Customer[pid = %lu] is left. \n", pthread_self());
    }
        
    pthread_exit(NULL);
}


void * make_customer_function(){
    int tmp;   
    int curcust = 0;

    while(curcust < total_custs)
    {
        /* Declare and create a customer thread */
        pthread_t customer_thread;
        tmp = pthread_create(&customer_thread, NULL, (void *)customer_function, NULL);  
        /* Sleep for 100ms before creating another customer */
        usleep(1000 * arrtime[curcust]);
        /* Increment the counter */
        curcust++;
            

    }
}

void input(const char* file_name){
      FILE* file = fopen (file_name, "r");
      int i = 0;
      fscanf (file, "%d", &chair_cnt); //# of seats
      fscanf (file, "%d", &total_custs); // # of customers 
      
      while (!feof (file))
        {  
          fscanf (file, "%d", &arrtime[i]);
//           printf ("%d ", arrtime[i]);
          i++;
        }
      fclose (file); 
}
