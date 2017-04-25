
#include "Bufory.h"


#define VAL_EMPTY          0
#define VAL_FULL           SIZE
#define VAL_MUTEX          1
#define VAL_MORE_THAN      0
#define VAL_FULL_2         SIZE/2

#define VAL_PSHARED        1
#define MIN_ELEM           3

#define MIN(x,y)  (x > y)?x:y


void buf_init_sem(buf_t *allocated_buffer)
{
    printf("Poczatek inicjalizacji zmiennych\n");
    int res;
//inicjalizacja zmiennych
    allocated_buffer->i_add = 0;
    allocated_buffer->i_del_1 = 0;
    allocated_buffer->i_del_2 = 0;
    allocated_buffer->counter = 0;


    printf("Poczatek inicjalizacji bufora\n");

    res = sem_init(&(allocated_buffer->buf_empty_1), VAL_PSHARED, VAL_EMPTY);
    if(res == 0 )
        printf("Zainicjowany semafor pusty_1\n");

    res = sem_init(&(allocated_buffer->buf_empty_2), VAL_PSHARED, VAL_EMPTY);
    if(res == 0)
        printf("Zainicjowany semafor pusty_2\n");


    res = sem_init(&(allocated_buffer->buf_full), VAL_PSHARED, VAL_FULL);
    if(res == 0)
        printf("Zainicjowany semafor pelny\n");

    res = sem_init(&(allocated_buffer->buf_full_2), VAL_PSHARED, VAL_FULL_2);
    if(res == 0)
        printf("Zainicjowany semafor pelny_2\n");


    res = sem_init(&(allocated_buffer->mutex), VAL_PSHARED, VAL_MUTEX);
    if(res == 0)
        printf("Zainicjowany semafor mutex\n");


    res = sem_init(&(allocated_buffer->more_than_1), VAL_PSHARED, VAL_MORE_THAN);
    if(res == 0)
        printf("Zainicjowany semafor more_than_two\n");


    res = sem_init(&(allocated_buffer->more_than_2), VAL_PSHARED, VAL_MORE_THAN);
    if(res == 0)
        printf("Zainicjowany semafor more_than_two\n");

}

void buf_add_one(buf_t *buffer, char element)
{
    //unsigned int empty_1_val, empty_2_val;
    int val_empty_1, val_empty_2, more, full, full_2;
    sem_wait(&(buffer->buf_full));


    sem_wait(&(buffer->mutex));

    if((buffer->counter)%2 == 0)//jesli jest parzysta ilosc elementow, to po wstawieniu jest juz nieparzysta - czyli opuszczamy tez semafor ful_2
        sem_wait(&(buffer->buf_full_2));

            //sem_getvalue(&(buffer->buf_empty_1),&val_empty_1);
            //sem_getvalue(&(buffer->buf_empty_2), &val_empty_2);
            //sem_getvalue(&(buffer->buf_full), &full);
            //sem_getvalue(&(buffer->buf_full_2), &full_2);

//            printf("Semafory empty: pierwszy: %d, drugi: %d, full %d , full_2 %d\n", val_empty_1, val_empty_2, full, full_2);

    buffer->value[buffer->i_add] = element;
    buffer->i_add = (++(buffer->i_add))%SIZE;
    ++(buffer->counter);

    //sem_getvalue(&(buffer->buf_empty_1), &empty_1_val);
    //sem_getvalue(&(buffer->buf_empty_2), &empty_2_val);

    if(buffer->counter == MIN_ELEM )//jezeli wczesniej bylo mniej niz min elementow, a teraz po dodaniu jest minimalna ilosc, aby mozliwe bylo usuniecie elementu przez klienta, to podnosimy semafor
    {
        sem_post(&(buffer->more_than_1));
        sem_post(&(buffer->more_than_2));
    }

    //if((buffer->counter)%2 == 0)//jesli byla nieparzysta ilosc elementow, a teraz jest parzysta - to prod_2 moze wstawiac elementy, wiec ten sem podnosimy
      //  sem_post(&(buffer->buf_full_2));

    sem_post(&(buffer->buf_empty_1));
    sem_post(&(buffer->buf_empty_2));

    sem_post(&(buffer->mutex));

}

void buf_add_two(buf_t *buffer,char element_1, char element_2)
{
    //buf_add_one(buffer, element_1);
    //buf_add_one(buffer, element_2);

    int val_empty_1, val_empty_2, more, full, full_2;
    sem_wait(&(buffer->buf_full_2));

    //if(&(buffer->counter)%2 == 0)//jesli wstawiamy na parzyste miejsce
      //  sem_wait(&(buffer->buf_full));

    sem_wait(&(buffer->mutex));


    sem_wait(&(buffer->buf_full));//opuszczamy i te, bo wstawiamy dwa elementy
    sem_wait(&(buffer->buf_full));
            //sem_getvalue(&(buffer->buf_empty_1),&val_empty_1);
            //sem_getvalue(&(buffer->buf_empty_2), &val_empty_2);
            //sem_getvalue(&(buffer->buf_full), &full);
            //sem_getvalue(&(buffer->buf_full_2), &full_2);

            //printf("Semafory empty: pierwszy: %d, drugi: %d, full %d , full_2, %d, add_i(po zwiekszeniu): %d\n", val_empty_1, val_empty_2, full, full_2, buffer->i_add+2);

    //zapis 1 elementu
    buffer->value[buffer->i_add]   = element_1;
    buffer->i_add = (++(buffer->i_add))%SIZE;
    //i 2 elementu
    buffer->value[buffer->i_add] = element_2;
    buffer->i_add = (++(buffer->i_add))%SIZE;

    ++(buffer->counter);
    if(buffer->counter == MIN_ELEM +1)//jezeli wczesniej bylo mniej niz min elementow, a teraz po dodaniu jest minimalna ilosc, aby mozliwe bylo usuniecie elementu przez klienta, to podnosimy semafor
    {
        sem_post(&(buffer->more_than_1));
        sem_post(&(buffer->more_than_2));
    }

    //i jeszcze raz, bo dodalismy dwa elementy
    ++(buffer->counter);
    if(buffer->counter == MIN_ELEM +1)//jezeli wczesniej bylo mniej niz min elementow, a teraz po dodaniu jest minimalna ilosc, aby mozliwe bylo usuniecie elementu przez klienta, to podnosimy semafor
    {
        sem_post(&(buffer->more_than_1));
        sem_post(&(buffer->more_than_2));
    }

    //podnosimy dwa razy, bo wstawilismy dwa elementy
    sem_post(&(buffer->buf_empty_1));
    sem_post(&(buffer->buf_empty_1));

    sem_post(&(buffer->buf_empty_2));
    sem_post(&(buffer->buf_empty_2));

    sem_post(&(buffer->mutex));


}



char buf_remove_1_client(buf_t *buffer)
{
    char ret_elem;
    int val_empty_1, val_empty_2, more, full, full_2;

    sem_wait(&(buffer->more_than_1));
    sem_wait(&(buffer->buf_empty_1));
    sem_wait(&(buffer->mutex));//jesli przeszlismy przez semafor more_than_2 to znaczy ze na pewno mozemy usuwac element


    ret_elem = buffer->value[buffer->i_del_1];
    buffer->i_del_1 = (++(buffer->i_del_1))%SIZE;

    sem_getvalue(&(buffer->buf_empty_1),&val_empty_1);
    sem_getvalue(&(buffer->buf_empty_2), &val_empty_2);
            //sem_getvalue(&(buffer->buf_full), &full);
            //sem_getvalue(&(buffer->buf_full_2), &full_2);

           // printf("Semafory empty: pierwszy: %d, drugi: %d, full %d , full_2 %d", val_empty_1, val_empty_2, full, full_2);

    if(val_empty_2 <= val_empty_1)//jesli drugi klient usunal tyle elementow co my lub wiecej
    {
        sem_post(&(buffer->buf_full));//pozwalamy producentom wpisac
        --(buffer->counter);//zmniejszamy ilosc elementow w buforze

        if((buffer->counter%2)==0)//jezeli usuwamy element, i teraz jest parzysta ilosc elementow, to znaczy ze wczesniej byla nieparzysta
            sem_post(&(buffer->buf_full_2));//wiec nalezy podniesc rowniez ten semafor
    }
    //dlatego badamy jego wartosc
         //   printf("counter: %d, ", buffer->counter);

    sem_getvalue(&(buffer->more_than_2), &more);//jezeli sie okaze, ze po usunieciu elementu bdzie za malo miejsca - to musimy rowniez opuscic semafor nr 2

    if(buffer->counter > MIN_ELEM)
    {
        sem_post(&(buffer->more_than_1));
    }
    else if(more)//jesli sa mniej niz 4 elementy, a semafor drugiego klienta jest podniesiony
    {
        sem_wait(&(buffer->more_than_2));
    }

      //      sem_getvalue(&(buffer->more_than_1), &more);
       //     printf("more_than_1: %d\n", more);



    sem_post(&(buffer->mutex));
    return ret_elem;
}


char buf_remove_2_client(buf_t *buffer)
{

    char ret_elem;
    int val_empty_1, val_empty_2, more, full, full_2;

    sem_wait(&(buffer->more_than_2));
    sem_wait(&(buffer->buf_empty_2));
    sem_wait(&(buffer->mutex));//jesli przeszlismy przez semafor more_than_2 to znaczy ze na pewno mozemy usuwac element


    ret_elem = buffer->value[buffer->i_del_2];
    buffer->i_del_2 = (++(buffer->i_del_2))%SIZE;

    sem_getvalue( &(buffer->buf_empty_1), &val_empty_1);
    sem_getvalue( &(buffer->buf_empty_2), &val_empty_2);
    if(val_empty_1 <= val_empty_2)//jesli pierwszy klient usunal tyle elementow co my lub wiecej
    {
        sem_post(&(buffer->buf_full));//pozwalamy producentom wpisac
        --(buffer->counter);//zmniejszamy ilosc elementow w buforze

        if((buffer->counter%2)==0)//jezeli usuwamy element, i teraz jest parzysta ilosc elementow, to znaczy ze wczesniej byla nieparzysta
            sem_post(&(buffer->buf_full_2));//wiec nalezy podniesc rowniez ten semafor
    }
            //printf("counter: %d, ", buffer->counter);

    sem_getvalue(&(buffer->more_than_1), &more);
    if( (buffer->counter) > MIN_ELEM)
    {
        sem_post(&(buffer->more_than_2));//opusc, bo jest przynajmniej MIN elementow
    }
    else if(more)//jest za malo elementow by pozwolic na usuwanie, a drugi z semaforów more_than jest podniesiony - nalezy go opuscic
    {
        sem_wait(&(buffer->more_than_1));
    }


    sem_post(&(buffer->mutex));
    return ret_elem;
}
/*
void print_buffer(buf_t *buffer)
{
    char buffer_val[SIZE];
    int i;
    sem_wait(&(buffer->mutex));
    for(i = 0;i < SIZE;++i)
    {
        buffer_val[SIZE-i-1] = buffer->value[ ((buffer->i_add) - i)%SIZE ] ;
    }
    sem_post(&(buffer->mutex));

    printf("Zawartosc bufora:");
    for(i = 0;i<SIZE;++i)
    {
        printf("%c, ",buffer_val[i]);
    }
    printf("\n\n");


}*/
