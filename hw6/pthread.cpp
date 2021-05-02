#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#define MAX_CUSTOMERS 30
#define CUTTING_TIME 0.2
using namespace std;


int numChairs;
int numCustomers;
float Bshop[MAX_CUSTOMERS];
int serviced = 0;

void *customer(void *num);
void *barber(void *);
void randwait(int secs);
void input();
void calarr();
// Define the semaphores.

sem_t waitingRoom; // waitingRoom Limits the # of customers allowed to enter the waiting room at one time.
sem_t barberChair; // barberChair ensures mutually exclusive access tothe barber chair.
sem_t barberPillow;// barberPillow is used to allow the barber to sleep until a customer arrives.
sem_t seatBelt; // seatBelt is used to make the customer to wait until the barber is done cutting his/her hair.

// Flag to stop the barber thread when all customers
// have been serviced.
int allDone = 0;

int main(int argc, char *argv[]) {
    pthread_t btid;
    pthread_t tid[MAX_CUSTOMERS];
    int Number[MAX_CUSTOMERS];

    input();
    calarr();
    cout << " the number of customer is " << numCustomers << endl;
    cout << " the number of chair is " << numChairs << endl;


    // Initialize the numbers array.
    for (int i=0; i<MAX_CUSTOMERS; i++) {
    Number[i] = i;
    }

    // Initialize the semaphores with initial values
    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);

    // Create the barber.
    pthread_create(&btid, NULL, barber, NULL);

    // Create the customers.
    for (int i=0; i<numCustomers; i++) {
    pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
    sleep(1);
    }

    // Join each of the threads to wait for them to finish.
    for (int i=0; i<numCustomers; i++) {
    pthread_join(tid[i],NULL);
    sleep(1);
}

// When all of the customers are finished, kill the barber thread.
allDone = 1;
sem_post(&barberPillow); // Wake the barber so he will exit.
pthread_join(btid,NULL);
}


void *customer(void *number) {
    int num = *(int *)number;

    // Leave for the shop and take some random amount of time to arrive.

    sleep(0.001 * Bshop[num]);
    cout << "Customer" << num << "arrived at barber shop at" << 0.001 * Bshop[num] <<endl;

    // Wait for space to open up in the waiting room…
    sem_wait(&waitingRoom);
    printf("Customer %d entering waiting room.\n", num);

    // Wait for the barber chair to become free.
    sem_wait(&barberChair);

    // The chair is free so give up your spot in the
    // waiting room.
    sem_post(&waitingRoom);

    // Wake up the barber…
    printf("Customer %d waking the barber.\n", num);
    sem_post(&barberPillow);

    // Wait for the barber to finish cutting your hair.
    sem_wait(&seatBelt);

    // Give up the chair.
    sem_post(&barberChair);
    printf("Customer %d leaving barber shop.\n", num);
}

void *barber(void *junk) {
    // While there are still customers to be serviced… Our barber is omnicient and can tell if there are
    // customers still on the way to his shop.
    while (!allDone) {

    // Sleep until someone arrives and wakes you..
    printf("The barber is sleeping\n");
    sem_wait(&barberPillow);

    // Skip this stuff at the end…
    if (!allDone) {

    cout << "The barber services a client, available seats: " << endl;
    sleep(CUTTING_TIME);
    serviced += 1;
    printf("The barber has finished cutting hair.\n");

    // Release the customer when done cutting…
    sem_post(&seatBelt);
    }
    else {
    cout << "Serviced clients: " << serviced <<endl;
    }
    }
}

void randwait(int secs) {
int len;

// Generate a random number…
len = (int) ((1 * secs) + 1);
cout << "wait: " << len << endl;
sleep(len);
}

void input(){
    string filename("input.txt");
    ifstream input_file(filename);

    input_file >> numChairs;
    input_file >> numCustomers;
//     cout << chairs << " " << customers << " " << endl;
   
    for (int i=0; i< numCustomers; i++){
        if (i==0){Bshop[i]=0;}
        else{
            input_file >> Bshop[i];
        }
    }
//     cout << endl;
    input_file.close();
}

void calarr(){
    cout << "customer arrives at timestamp: ";
    for (int i=1; i<numCustomers; i++){
        Bshop[i] = (Bshop[i] + Bshop[i-1]);
        cout << Bshop[i] << " ";
    }
}

