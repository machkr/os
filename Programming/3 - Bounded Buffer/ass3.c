
/*------------------------------*
 *  Program: Bounded Buffer (3) *
 *  Name: Matthew Kramer        *
 *  Username: kramerm           *
 *  Date: 30 Oct 2016           *
 *  U#: 20891900                *
 *------------------------------*/

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define _REENTRANT
#define BUFFER_SIZE 15
#define SHMKEY ((key_t) 7890)

// Function prototype declarations
void* produce(void *arg);
void* consume(void *arg);

// Structs for shared memory buffer and shared data buffer
typedef struct { char* value; } sbuffer;
typedef struct { int value; } scounter;

// 3 semaphores
sem_t empty;
sem_t full;
sem_t critical;

// Variables for beginning and end, file-reading
int beg = 0;
int end = 0;
char newChar;
FILE* infile;

// Initialization of structs
sbuffer *buffer;
scounter *counter;

/*----------------------------------------------------------------------*
 * MAIN()
 *----------------------------------------------------------------------*/

int main(void)
{
    // Open input file -- "mytest.dat"
    infile = fopen("mytest.dat", "r");

    int i;
    int shmid;                  // Shared Memory ID
    pthread_t producer[1];      // PID for Thread 1
    pthread_t consumer[1];      // PID for Thread 2
    pthread_attr_t attr;        // Pointer Array for Attributes

    char *shmadd;
    shmadd = (char *) 0;

    // Shared memory segment creation with error-checking
    if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror ("shmget");
        return 1;
    }

    // Shared memory segment connection with error-checking
    if ((buffer = (sbuffer *) shmat (shmid, shmadd, 0)) == (sbuffer *) -1)
    {
        perror ("shmat");
        return 0;
    }

    // Character buffer array
    char buffer_array[15];
    buffer->value = buffer_array;

    // Allocation of memory for counter struct, initialization of value
    counter = (scounter *) malloc(sizeof(scounter));
    counter->value = 0 ;

    // Initialization of semaphores
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&critical, 0, 1);

    // Flush output buffer
    fflush(stdout);

    // Independent thread scheduling
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); 

    // Thread creation
    pthread_create(&producer[0], &attr, produce, 0);
    pthread_create(&consumer[0], &attr, consume, 0);

    // Wait until threads are finished
    pthread_join(producer[0], 0);
    pthread_join(consumer[0], 0);

    // Destroy semaphores...
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&critical);

    // ...and release shared memory
    if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
    {
        perror ("shmctl");
        return -1;
    }

    // Close file
    fclose(infile);

    // Output results
    printf("Parent Counter: %d\n\n", counter->value);
    for(i = 0; i < 75; i++) { printf("#"); }
    printf("\n\t\t\t     End of Simulation\n");
    for(i = 0; i < 75; i++) { printf("#"); }
    printf("\n\n");

    return 0;
}

// Producer function
void* produce(void *arg)
{
    bool done = false;

    while(!done)
    {
        sem_wait(&empty);
        sem_wait(&critical);
        end++;

        // CRITICAL SECTION

        if(fscanf(infile,"%c", &newChar) != EOF)
        {
            // Modulo 15 to maintain a circular buffer
            buffer->value[(end) % 15] = newChar;
            printf("Produced: %c\n", newChar);
        }
        else
        {
            buffer->value[(end) % 15] = '*';
            done = true;
        }

        sem_post(&critical);
        sem_post(&full);
    }
}



// Consumer function
void* consume(void *arg)
{
    bool done = false;
    char value;

    while(!done)
    {
        sem_wait(&full);
        sem_wait(&critical);
        beg++;
    
        // Necessary sleep
        sleep(1);
    
        //CRITICAL SECTION

        // Modulo 15 to maintain a circular buffer
        if((value = buffer->value[(beg) % 15]) != '*')
        {
            printf("Consumed: %c\n", value);
            counter->value++;
        }
        else
        {
            done = true;
        }

        sem_post(&critical);
        sem_post(&empty);
    }
}
