#ifndef BUFORY_H_INCLUDED
#define BUFORY_H_INCLUDED

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>

#define SIZE 10 //rozmiar bufora - musi byc przynajmniej 3

typedef struct buf_t
{
    char value[SIZE];
    sem_t mutex;
    sem_t buf_empty_1, buf_empty_2;
    sem_t buf_full, buf_full_2;
    sem_t more_than_1, more_than_2;
    int i_add, i_del_1, i_del_2;
    int counter;

}buf_t;

//inicjuje odpowiednio semafory
void buf_init_sem(buf_t *allocated_buffer);


//wersje operacji dodawania i usuwania sa stworzone dla 1 i 2 producenta/klienta

//Dodaje element do bufora, wykorzystujac semafory
void buf_add_one(buf_t *buffer,char element);

void buf_add_two(buf_t *buffer,char element_1, char element_2);

//usuwa element z kolejki bufora i zwraca go
char buf_remove_1_client(buf_t *buffer);

char buf_remove_2_client(buf_t *buffer);

//void print_buffer(buf_t *buffer);

#endif // BUFORY_H_INCLUDED
