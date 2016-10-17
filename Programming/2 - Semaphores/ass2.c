
/*------------------------------*
 *  Program: Semaphores (2)     *
 *  Name: Matthew Kramer        *
 *  Username: kramerm           *
 *  Date: 16 Oct 2016           *
 *  U#: 20891900                *
 *------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* change the key number */
#define SHMKEY ((key_t) 7890)
#define SEMKEY ((key_t) 400L)   //Was 4500
#define NSEMS 1 // Number of semaphores

typedef struct { int value; } shared_mem;

shared_mem *total;

int sem_id;

typedef union
{
    int val;
    struct semid_ds *buf;
    ushort *array;
} semunion;

//Semaphore buffer structures
static struct sembuf owait = {0, -1, 0};
static struct sembuf osignal = {0, 1, 0};

//Semaphore buffer pointers
struct sembuf *wait_buffer = &owait;
struct sembuf *signal_buffer = &osignal;

// Semaphore wait function
int semwait()
{
    int status;
    status = semop(sem_id, wait_buffer, 1);
    return status;
}

// Semaphore signal function
int semsignal()
{
    int status;
    status = semop(sem_id, signal_buffer, 1);
    return status;
}

/*----------------------------------------------------------------------*
 * This function increases the value of shared variable "total"         *
 * by one all the way to 100000                                         *
 *----------------------------------------------------------------------*/

void process1()
{
    int k = 0;

    // CRITICAL SECTION

    while (k < 100000)
    {
        semwait();
        if(total->value < 600000) total->value = total->value + 1;
        semsignal();
        k++;
    }

    printf ("\nTotal from Process 1: %d\n", total->value);
}


/*----------------------------------------------------------------------*
 * This function increases the vlaue of shared memory variable "total"  *
 * by one all the way to 200000                                         *
 *----------------------------------------------------------------------*/

void process2()
{
    int k = 0;

    // CRITICAL SECTION

    while (k < 200000)
    {
        semwait();
        if(total->value < 600000) total->value = total->value + 1;
        semsignal();
        k++;
    }

    printf ("\nTotal from Process 2: %d\n", total->value);
}


/*----------------------------------------------------------------------*
 * This function increases the vlaue of shared memory variable "total"  *
 * by one all the way to 300000                                         *
 *----------------------------------------------------------------------*/

void process3()
{
    int k = 0;

    // CRITICAL SECTION

    while (k < 300000)
    {
        semwait();
        if(total->value < 600000) total->value = total->value + 1;
        semsignal();
        k++;
    }

    printf ("\nTotal from Process 3: %d\n", total->value);
}

/*----------------------------------------------------------------------*
 * MAIN()
 *----------------------------------------------------------------------*/

int main(void)
{
    int shmid;
    int pid1;
    int pid2;
    int pid3;
    int ID;
    int status;

    char *shmadd;
    shmadd = (char *) 0;

/* Variables for Semaphores */
    int numsem = 0;
    int val1;
    int val2;
    semunion semctl_arg;
    semctl_arg.val = 1;

/* Semaphore Creation */
    sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666);
    if(sem_id < 0) printf("Error when creating semaphore.\n");

/* Semaphore Initialization*/
    val2 = semctl(sem_id, numsem, SETVAL, semctl_arg);
    val1 = semctl(sem_id, numsem, GETVAL, semctl_arg);
    if (val1 < 1) printf("Error detected with SETVAL.\n");

/* Create and connect to a shared memory segment */

    if ((shmid = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    if ((total = (shared_mem *) shmat(shmid, shmadd, 0)) == (shared_mem*) - 1)
    {
        perror("shmat");
        exit(0);
    }

    total->value = 0;

/* 1st Child */
    if ((pid1 = fork()) == 0)                               //First child is created
    {
        process1();                                         //First child process starts
    }

/* 2nd Child */
    if ((pid1 != 0) && (pid2 = fork()) == 0)                //Second child is created
    {
        process2();                                         //Second child process starts
    }

/* 3rd Child */
    if ((pid1 != 0) && (pid2 != 0) && (pid3 = fork()) == 0) //Third child is created
    {
        process3();                                         //Third child process starts
    }

    while((ID = wait(&status)) != -1)                       //Parent loops until the children finished
    {
        printf("Child with ID#%d has finished.\n", ID);     //Parent prints out the value once child has finished
    }

    if ((pid1 != 0) && (pid2 != 0) && (pid3 != 0))
    {
        if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
        {
            perror("shmctl");
            exit(-1);
        }

        printf("\nEnd of Program.\n\n");                    //Only reached once all three children have finished

/* Semaphore Deallocation*/
        semctl_arg.val = 0;
        status = semctl(sem_id, 0, IPC_RMID, semctl_arg);
        if(status < 0) printf("Error when deallocating semaphore.\n");
    }

    return 0;
}
