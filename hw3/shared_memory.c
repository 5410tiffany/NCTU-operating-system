# include <stdlib.h>
# include <stdio.h>
# include <sys/shm.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/wait.h>
# include <errno.h>
// So we could use other sizes without editing the source.
#ifndef MAX_SEQUENCE
# define MAX_SEQUENCE 10
#endif
// Check that MAX_SEQUENCE is large enough!
#if MAX_SEQUENCE < 2
#error MAX_SEQUENCE must be at least 2f
#endif

typedef struct{
    long fib_sequence[MAX_SEQUENCE];
    int sequence_size;
} shared_data;

int input(){
    int m;
    printf("Enter the number of a Fibonacci Sequence:\n");
    // Always check whether input conversion worked & values are between 0-10
    if ((scanf("%d", &m) != 1) || (m <= 0) || (m > MAX_SEQUENCE)) {
        printf("Please enter a positive integer less than %d\n", MAX_SEQUENCE);
        return -1;
    }
    return m;
};
int main()
{
    int a, b, m, n, i;
    a = 0; b = 1;
    m = input(); //get valid parameters 
    if(m == -1){ //invalid input
        return EXIT_FAILURE;
    }
    
    printf("Parent process-start, pid = %ld\n", (long)getpid());
    /* the identifier for the shared memory segment */
    int segment_id;

    /* the size (in bytes) of the shared memory segment */
    size_t segment_size = sizeof(shared_data);

    /* allocate  a shared memory segment */
    segment_id = shmget(IPC_PRIVATE, segment_size, S_IRUSR | S_IWUSR);

    /* attach the shared memory segment */
    shared_data *shared_memory = shmat(segment_id, NULL, 0);

    printf("Create a shared-memory segment, segment_id = %d\n ", segment_id);

    shared_memory->sequence_size = m;
    printf("%d \n", shared_memory->sequence_size);
    pid_t pid;
    pid = fork();
    
//     start child process
    if (pid == 0){
        printf("Child process-start, pid = %ld\n", (long)getpid());
        shared_memory->fib_sequence[0] = a;
        shared_memory->fib_sequence[1] = b;
        for (i = 2; i < shared_memory->sequence_size; i++){
            n = a+b;
            shared_memory->fib_sequence[i] = n;
            a = b;
            b = n;
        }
        printf("Child process-end, pid = %ld\n", (long)getpid());
    }
    else{//parent process
        
        wait(NULL);
       
        //print sequence from shared memory
        for(i = 0; i < shared_memory->sequence_size; i++) {
            printf("%ld ", shared_memory->fib_sequence[i]);
        }
        printf("\n");
        printf("Parent process-end, pid = %ld\n", (long)getpid());
    }

    /* now detach the shared memory segment */ 
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "Unable to detach\n");
    }

    destroy:
    /* remove the shared memory segment */
    shmctl(segment_id, IPC_RMID, NULL); 

    return 0;
}