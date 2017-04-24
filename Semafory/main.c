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


void deleteSharedMemoryBuffer(void)
{
    char name[16];
    strcpy(name, MEMORY_NAME);
    shm_unlink(name);
    printf("Pamiec wspoldzielona usunieta\n");
}

void kod_producenta_1(void)
{
    char i;
    for(i='a';i<'a'+25;++i)
    {
        //printf("Producent_1: rozpoczynam dodawanie\n");
        buf_add_one(shared_buffer,i);
        printf("Producent 1: Dodano litere %c\n\n", i);
        sleep(1);
    }
    exit(0);
}
void kod_producenta_2(void)
{
    char i;
    for(i='A';i<'A'+5;++i)
    {
       // printf("Producent_2: rozpoczynam dodawanie\n");
        buf_add_two(shared_buffer,i, i+1);
        printf("Producent 2: Dodano litery %c oraz %c\n\n", i, 'Z'-(i-'A') );
        sleep(2);
    }
    exit(0);
}

void kod_konsumenta_1(void)
{
    char litera;
    int i;
    for(i=0; i<25;++i)
    {
        litera = buf_remove_1_client(shared_buffer);
        printf("Konsument_1: Usunieto litere %c\n\n", litera);
        sleep(2);
    }
    exit(0);
}

void kod_konsumenta_2(void)
{
    char litera;
    int i;
    for(i=0; i<25;++i)
    {
        litera = buf_remove_2_client(shared_buffer);
        printf("Konsument_2: Usunieto litere %c\n\n", litera);
        sleep(5);
    }
    exit(0);

}


int main(void)
{
    //buf_t *shared_buffer;
    //int value;
//    char name[16];

    printf("Tworzenie pamieci wspoldzielonej\n");

    shared_buffer = createSharedMemoryBuffer();
    if(shared_buffer != NULL)
        printf("Przydzielono pamiec dla bufora\n\n");

   // printf("Adres bufora: %d, wartosc 99 elementu: %d\n", (unsigned int)shared_buffer, shared_buffer->value[99]);
    buf_init_sem(shared_buffer);


    if(fork() == 0)//tworzymy proces producenta a potem dzielimy go na dwoch producentow
    {
        if(fork() == 0)//forkujemy i w tego ifa wchodzi dziecko
        {
            printf("Proces producent_1 utworzony\n\n");
            kod_producenta_1();//ten proces ginie pod koniec wykonania tej procedury
        }
        else//a w tego rodzic
        {
            //printf("Proces producent_2 utworzony\n\n");
            //kod_producenta_2();//ten proces ginie pod koniec wykonania tej procedury
            exit(0);
        }

    }

    if(fork()==0)//tworzymy proces konsumenta a potem dzielimy go na dwoch konsumentow
    {
        if(fork()==0)//forkujemy i w tego ifa wchodzi dziecko
        {
            printf("Proces konsument_1 utworzony\n\n");
            kod_konsumenta_1();////ten proces ginie pod koniec wykonania tej procedury
        }
        else//a w tego rodzic
        {
            printf("Proces konsument_2 utworzony\n\n");
            kod_konsumenta_2();//ten proces ginie pod koniec wykonania tej procedury
        }
    }

    sleep(600);//czekamy 2 minuty az procesy sie wykonaja
    printf("Procesy wykonaly swoja prace. Koniec programu demonstacyjnego\n");
    //usuwanie pamieci zaallokowanej
    deleteSharedMemoryBuffer();
    exit(0);







    return 0;
}

