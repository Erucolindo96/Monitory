
#include "Bufory.h"


#define VAL_EMPTY 0
#define VAL_FULL 100
#define VAL_MUTEX 1

#define VAL_PSHARED 0
void buf_init_sem(buf_t *allocated_buffer)
{
    int res;



    printf("Poczatek inicjalizacji bufora\n");

    res = sem_init(&(allocated_buffer->buf_empty), VAL_PSHARED, VAL_EMPTY);
   // printf("Wartosc zmiennej res: %d\n", res);
    if(res == 0 )
        printf("Zainicjowany semafor pusty\n");

    res = sem_init(&(allocated_buffer->buf_full), VAL_PSHARED, VAL_FULL);
    if(res == 0)
        printf("Zainicjowany semafor pelny\n");

    res = sem_init(&(allocated_buffer->mutex), VAL_PSHARED, VAL_MUTEX);
    if(res == 0)
        printf("Zainicjowany semafor mutex\n");

}

void buf_add(buf_t *buffer, char element)
{

    int ilosc_elem_buf;
    sem_wait(&(buffer->buf_full));//sprawdzamy czy bufor pelny - jesli tak to czekamy az sie zwolni
    sem_wait(&(buffer->mutex));//wchodzimy do sekcji krytycznej

    sem_getvalue(&(buffer->buf_empty), &ilosc_elem_buf);//wczytujemy ile jest elementow w buforze(mowi o tym wielkosc semafora empty)

    buffer->value[ilosc_elem_buf] = element; //zapisujemy element

    sem_post(&(buffer->buf_empty));

    sem_post(&(buffer->mutex));//wychodzimy z sekcji krytycznej

    //printf("Koniec procedury add\n");
}


char buf_remove(buf_t *buffer)
{
    int ilosc_elem_buf;
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


}
