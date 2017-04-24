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

#define BUF_SIZE 128

typedef struct buf_t
{
    char value[BUF_SIZE];
    sem_t mutex;
    sem_t buf_empty;
    sem_t buf_full;

}buf_t;

//inicjuje odpowiednio semafory
void buf_init_sem(buf_t *allocated_buffer);

//Dodaje element do bufora, wykorzystujac semafory
void buf_add(buf_t *buffer,char element);

//usuwa ostatnio wstawiony element bufora i zwraca go
char buf_remove(buf_t *buffer);



#endif // BUFORY_H_INCLUDED
