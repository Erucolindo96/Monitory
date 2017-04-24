
#include "Bufory.h"


#define VAL_EMPTY          0
#define VAL_FULL           SIZE
#define VAL_MUTEX          1
#define VAL_MORE_THAN      0

#define VAL_PSHARED        1
#define MIN_ELEM           3


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
/*
    int ilosc_elem_buf;
    sem_wait(&(buffer->buf_full));//sprawdzamy czy bufor pelny - jesli tak to czekamy az sie zwolni
    sem_wait(&(buffer->mutex));//wchodzimy do sekcji krytycznej

    sem_getvalue(&(buffer->buf_empty), &ilosc_elem_buf);//wczytujemy ile jest elementow w buforze(mowi o tym wielkosc semafora empty)

    buffer->value[ilosc_elem_buf] = element; //zapisujemy element

    sem_post(&(buffer->buf_empty));

    sem_post(&(buffer->mutex));//wychodzimy z sekcji krytycznej

    //printf("Koniec procedury add\n");
*/
    //unsigned int empty_1_val, empty_2_val;
    sem_wait(&(buffer->buf_full));
    sem_wait(&(buffer->mutex));

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

    sem_post(&(buffer->buf_empty_1));
    sem_post(&(buffer->buf_empty_2));

    sem_post(&(buffer->mutex));

}

void buf_add_two(buf_t *buffer,char element_1, char element_2)
{
    buf_add_one(buffer, element_1);
    buf_add_one(buffer, element_2);
}



char buf_remove_1_client(buf_t *buffer)
{
/*    int ilosc_elem_buf;
    char element;
    sem_wait(&(buffer->buf_empty));//sprawdzamy czy w buforze cos jest - jesli nie ma, zasypiamy i czekamy az bdzie
    sem_wait(&(buffer->mutex));//wchodzimy do sekcji krytycznej
    //printf("Usuwanie elementu\n");
    sem_getvalue(&(buffer->buf_empty), &ilosc_elem_buf);
    //printf("Wartosc zmiennej 'ilosc_elem_buf': %d", ilosc_elem_buf);
    element = buffer->value[ilosc_elem_buf];

    sem_post(&(buffer->buf_full));//wyjelismy element, wiec oddalamy buf_full od bycia pelnym - wiec inkrementujemy
    sem_post(&(buffer->mutex));//wychodzimy w sekcji krytycznej


   // printf("Koniec procedury remove\n");
    return element;
*/

    char ret_elem;
    int val_empty_1, val_empty_2, more;

    sem_wait(&(buffer->more_than_1));
    sem_wait(&(buffer->buf_empty_1));
    sem_wait(&(buffer->mutex));//jesli przeszlismy przez semafor more_than_2 to znaczy ze na pewno mozemy usuwac element


    ret_elem = buffer->value[buffer->i_del_1];
    buffer->i_del_1 = (++(buffer->i_del_1))%SIZE;

    sem_getvalue( &(buffer->buf_empty_1),&val_empty_1);
    sem_getvalue(&(buffer->buf_empty_2), &val_empty_2);


            printf("Semafory empty: pierwszy: %d, drugi: %d, ", val_empty_1, val_empty_2);

    if(val_empty_2 <= val_empty_1)//jesli drugi klient usunal tyle elementow co my lub wiecej
    {
        sem_post(&(buffer->buf_full));//pozwalamy producentom wpisac
        --(buffer->counter);//zmniejszamy ilosc elementow w buforze
    }
    //dlatego badamy jego wartosc
            printf("counter: %d, ", buffer->counter);

    sem_getvalue(&(buffer->more_than_2), &more);//jezeli sie okaze, ze po usunieciu elementu bdzie za malo miejsca - to musimy rowniez opuscic semafor nr 2

    if(buffer->counter >= MIN_ELEM)
    {
        sem_post(&(buffer->more_than_1));
    }
    else if(more)//jesli sa mniej niz 2 elementy, a semafor drugiego klienta jest podniesiony
    {
        sem_wait(&(buffer->more_than_2));
    }

            sem_getvalue(&(buffer->more_than_1), &more);
            printf("more_than_1: %d\n", more);

    sem_post(&(buffer->mutex));
    return ret_elem;
}


char buf_remove_2_client(buf_t *buffer)
{
/*    int ilosc_elem_buf;
    char element;
    sem_wait(&(buffer->buf_empty));//sprawdzamy czy w buforze cos jest - jesli nie ma, zasypiamy i czekamy az bdzie
    sem_wait(&(buffer->mutex));//wchodzimy do sekcji krytycznej
    //printf("Usuwanie elementu\n");
    sem_getvalue(&(buffer->buf_empty), &ilosc_elem_buf);
    //printf("Wartosc zmiennej 'ilosc_elem_buf': %d", ilosc_elem_buf);
    element = buffer->value[ilosc_elem_buf];

    sem_post(&(buffer->buf_full));//wyjelismy element, wiec oddalamy buf_full od bycia pelnym - wiec inkrementujemy
    sem_post(&(buffer->mutex));//wychodzimy w sekcji krytycznej


   // printf("Koniec procedury remove\n");
    return element;
*/

    char ret_elem;
    int val_empty_1, val_empty_2, more;

    sem_wait(&(buffer->more_than_2));
    sem_wait(&(buffer->buf_empty_2));
    sem_wait(&(buffer->mutex));//jesli przeszlismy przez semafor more_than_2 to znaczy ze na pewno mozemy usuwac element


    ret_elem = buffer->value[buffer->i_del_2];
    buffer->i_del_2 = (++(buffer->i_del_2))%SIZE;

    sem_getvalue( &(buffer->buf_empty_1), &val_empty_1);
    sem_getvalue( &(buffer->buf_empty_2), &val_empty_2);

            printf("Semafory empty: pierwszy: %d, drugi: %d, ", val_empty_1, val_empty_2);
    if(val_empty_1 <= val_empty_2)//jesli pierwszy klient usunal tyle elementow co my lub wiecej
    {
        sem_post(&(buffer->buf_full));//pozwalamy producentom wpisac
        --(buffer->counter);//zmniejszamy ilosc elementow w buforze
    }
            printf("counter: %d, ", buffer->counter);

    sem_getvalue(&(buffer->more_than_1), &more);
    if( (buffer->counter) >= MIN_ELEM)
    {
        sem_post(&(buffer->more_than_2));//opusc, bo jest przynajmniej MIN elementow
    }
    else if(more)//jest za malo elementow, a drugi z semaforów more_than jest podniesiony - nalezy go opuscic
    {
        sem_wait(&(buffer->more_than_1));
    }

            sem_getvalue(&(buffer->more_than_2), &more);
            printf("more_than_2: %d\n", more);

    sem_post(&(buffer->mutex));
    return ret_elem;
}

