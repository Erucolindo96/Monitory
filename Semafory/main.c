#include <stdio.h>
#include <stdlib.h>
#include "Bufory.h"

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>


#define MEMORY_NAME "Bufor"


buf_t *shared_buffer;

buf_t* createSharedMemoryBuffer(void)
{
    int result;
    buf_t  *buf;
    char name[16];
    int  fd; // Deskryptor segmentu
    strcpy(name,MEMORY_NAME);
    shm_unlink(name);

    //tworzenie segmentu
    fd = shm_open(name, O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        printf("Dupa, nie przydzielilem pamieci\n");
        return NULL;
    }

    printf("Przydzielono deskryptor: %d\n", fd);

    //ustalenie rozmiaru pamieci
    result = ftruncate(fd, sizeof(buf_t));
    if(result < 0)
    {
        printf("Dupa, nie okreslilem rozmiaru pamieci\n");
        return NULL;
    }

    //zrzutowanie segmentu na adres bufora
    buf = (buf_t*) mmap(0, sizeof(buf_t),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0 );
    if(buf == NULL)
    {
        printf("Dupa, nie zrzutowalem wskaznika na bufor\n");
        return NULL;
    }

    return buf;
}



void kod_producenta()
{
    char litera;
    int val;
    //produkuje 10 liter, od a w gore
    //gdy skonczy produkowac, ginie
    for(litera = 'a';litera < ('a'+10);++litera)
    {
        buf_add(shared_buffer, litera);
        //printf("Wyprodukowano litere %c\n", litera);

        sem_getvalue(&(shared_buffer->buf_empty), &val);
        //printf("Wartosc semafora empty:%d\n", val);

        sleep(1);//czekaj 1 s
    }


    exit(0);
}

void kod_konsumenta()
{
    char litera;
    int i, val;
    for(i = 0;i < 10;++i)
    {
        litera = buf_remove(shared_buffer);
        printf("Skonsumowano litere %c\n", litera);

        sem_getvalue(&(shared_buffer->buf_empty), &val);
        printf("Wartosc semafora empty:%d\n", val);
        sleep(1);
    }
    exit(0);
}


int main(void)
{
    //buf_t *shared_buffer;
    //int value;
    char name[16];

    printf("Tworzenie pamieci wspoldzielonej\n");

    shared_buffer = createSharedMemoryBuffer();
    if(shared_buffer != NULL)
        printf("Przydzielono pamiec dla bufora\n");

   // printf("Adres bufora: %d, wartosc 99 elementu: %d\n", (unsigned int)shared_buffer, shared_buffer->value[99]);
    buf_init_sem(shared_buffer);





    if(fork() == 0)//tworzymy procudenta
    {
        kod_producenta();
    }

    sleep(10);

    if(fork() == 0)//tworzymy konsumenta
    {
        kod_konsumenta();
    }

    //sleep(100);

    //usuwanie pamieci zaallokowanej

    strcpy(name, MEMORY_NAME);
    shm_unlink(name);








    return 0;
}

