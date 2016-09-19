
/*------------------------------*
 *  Program: Shared Memory (1)  *
 *  Name: Matthew Kramer        *
 *  Username: kramerm           *
 *  Date: 09 Sep 2016           *
 *  U#: 20891900                *
 *------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* change the key number */
#define SHMKEY ((key_t) 4500)   //Was 7890

typedef struct { int value; } shared_mem;

shared_mem *total;

/*----------------------------------------------------------------------*
 * This function increases the value of shared variable "total"         *
 * by one all the way to 100000                                         *
 *----------------------------------------------------------------------*/

void process1()
{
    int k = 0;

    while (k < 100000)
    {
        k++;
        total->value = total->value + 1;
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

    while (k < 200000)
    {
        k++;
        total->value = total->value + 1;
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

    while (k < 300000)
    {
        k++;
        total->value = total->value + 1;
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

/* Create and connect to a shared memory segment*/

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
    }

    return 0;
}
