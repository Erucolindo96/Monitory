#ifndef MONITORBUFFER_HPP
#define MONITORBUFFER_HPP

#include<vector>
#include"monitor.h"
#include<iostream>
enum Consument
{
FIRST,
SECOND
};


template<class T>
class MonitorBuffer: public Monitor
{
public:
    MonitorBuffer(const unsigned int MAX_ITEMS):MAX_ITEMS_(MAX_ITEMS)
    {}

    /**
     * @brief pushOneElement Wkłada jeden element do bufora
     * @param element Wkładany element, kopiowany i wstawiany do bufora
     */
    void pushOneElement(const T &element)
    {
        enter();
        if(buffer_.size() >= MAX_ITEMS_ )
        {
            wait(canPushOne);//czekaj dopoki konsumenci nie zwolnia elementu
        }
        buffer_.push_back(element);

        if(buffer_.size() >= MIN_ELEM_)
        {
            signal(canPop);//sygnalizuje, ze konsumenci moga czytac(sami stwierdzą, czy moga potem element usunac)
        }

        leave();
    }

    /**
     * @brief pushTwoElements Wkłada 2 elementy do bufora na raz.
     * @param elem_1 Pierwszy element, kopiowany i wstawiany do bufora.
     * @param elem_2 Drugi element, kopiowany i wstawiany do bufora.
     */
    void pushTwoElements(const T &elem_1, const T &elem_2)
    {
        enter();

        if(buffer_.size() >= MAX_ITEMS_ - 1)//jesli w buforze jest albo tylko jedno miejsce, albo nie ma go wcale
        {
            wait(canPushTwo);
        }

        buffer_.push_back(elem_1);
        buffer_.push_back(elem_2);

        if(buffer_.size() >= MIN_ELEM_)
        {
            signal(canPop);//sygnalizuje, ze konsumenci moga czytac(sami stwierdzą, czy moga potem element usunac)

        }

        leave();
    }

    /**
     * @brief popConsumentFirst Czyta jeden element z bufora. Jeśli drugi konsument już przeczytał ten element - to jest trwale usuwany z bufora
     * @return Przeczytany element
     */
    T popConsumentFirst()
    {
        T returned_elem;
        enter();

        if(buffer_.size() < MIN_ELEM_  )//jesli jest zbyt malo elementow w buforze by czytac
        {
            wait(canPop);//czekamy az bedzie mozna czytac

            //teraz mamy 3 lub 4 elementy - rozpatrujemy te dwa przypadki
            if(buffer_.size() == MIN_ELEM_)
            {
                returned_elem = situationMIN_ELEM_forFirst();
            }
            else
            {
                returned_elem = situationNormalForFirst();
            }

        }
        else if(buffer_.size() == MIN_ELEM_)
        {
            returned_elem = situationMIN_ELEM_forFirst();
        }
        else //jest wiecej niz MIN_ELEM - mozna swobodnie czytac
        {
            returned_elem = situationNormalForFirst();
        }

        if(!wasFirstConsumentRead && !wasSecondConsumentRead)
        {//jesli obydwie flagi konsumentów == false, to element został usunięty, a nie tylko przeczytany

            //wiec mozemy zasygnalizowac mozliwosc czytania kolejnego elementu
            signal(canReadNext);

            //wiec mozemy zasygnalizowac mozliwosc dodawania elementow
            if(buffer_.size() < MAX_ITEMS_ - 2)//jesli sa chociaz dwa wolne miejsca
                signal(canPushTwo);

            if(buffer_.size() < MAX_ITEMS_ - 1 )//jesli jest chociaz jedno wolne miejsce
                signal(canPushOne);
        }

        leave();
        return returned_elem;

    }
    /**
     * @brief popConsumentSecond Czyta jeden element z bufora.
     * Jeśli pierwszy konsument już przeczytał ten element - to jest trwale usuwany z bufora
     * @return Przeczytany element
     */
    T popConsumentSecond()
    {
        T returned_elem;
        enter();

        if(buffer_.size() < MIN_ELEM_  )//jesli jest zbyt malo elementow by w ogole czytac
        {
            wait(canPop);//czekamy

            //teraz mamy 3 lub 4 elementy  - ropatrujemy obydwa przypadki
            if(buffer_.size() == MIN_ELEM_)
            {
                returned_elem = situationMIN_ELEM_forSecond();
            }
            else
            {
                returned_elem = situationNormalForSecond();
            }

        }
        else if(buffer_.size() == MIN_ELEM_)
        {
            returned_elem = situationMIN_ELEM_forSecond();
        }
        else //jest wiecej niz MIN_ELEM - mozna swobodnie usuwac
        {
            returned_elem = situationNormalForSecond();
        }


        if(!wasFirstConsumentRead && !wasSecondConsumentRead)
        {//jesli obydwie flagi konsumentów == false, to element został usunięty, a nie tylko przeczytany

            //wiec mozemy zasygnalizowac mozliwosc czytania kolejnego elementu
            signal(canReadNext);

            //wiec mozemy zasygnalizowac mozliwosc dodawania
            if(buffer_.size() < MAX_ITEMS_ - 1)//jesli sa chociaz dwa wolne miejsca
                signal(canPushTwo);

            if(buffer_.size() < MAX_ITEMS_)//jesli jest chociaz jedno wolne miejsce
                signal(canPushOne);
        }

        leave();
        return returned_elem;

    }


    unsigned int size() const
    {
        int ret_val;

        enter();
        ret_val = buffer_.size();
        leave();
        return ret_val;
    }

    void printBuffer()
    {
        enter();

        std::cout<<"Zawartość bufora: ";
        for(auto iter = buffer_.begin(); iter!=buffer_.end();++iter)
        {
            std::cout<<*iter<<" ";
        }
        std::cout<<std::endl;
    }


private:

    MonitorBuffer() = delete;

    int getWaitingCount(const Condition &other)
    {
        return other.waitingCount;
    }

    T readAndDelete()
    {
        auto first_elem  = buffer_.begin();
        T temp = *first_elem;
        buffer_.erase(first_elem);
        return temp;
    }

    T situationNormalForFirst()
    {
        T ret;
        if(wasSecondConsumentRead)//drugi konsumnet przeczytal
        {
            ret = readAndDelete();
            wasSecondConsumentRead = false;//zerujemy flage czytania przez drugiego konsumenta
        }
        else//drugi konsument nie przeczytal - nie mozemy usuwac, tylko czytac
        {
            if(wasFirstConsumentRead)//jesli raz odczytalismy znak, i chcemy czytac jeszcze raz, ale drugi konsument jeszcze nie przeczytał
                wait(canReadNext);//to czekamy az drugi konsument go odczyta, i dopiero wtedy mozemy czytac nastepny

            ret = buffer_.front();
            wasFirstConsumentRead = true;//ustawiamy flage, ze przeczytalismy juz znak
        }
        return ret;
    }

    T situationNormalForSecond()
    {
        T ret;
        if(wasFirstConsumentRead)//pierwszy konsumnet przeczytal - czytamy i usuwamy
        {
            ret = readAndDelete();
            wasFirstConsumentRead = false;//zerujemy flage pierwszego konsumenta
        }
        else//pierwszy konsument nie przeczytal - mozemy tylko czytac
        {
            if(wasSecondConsumentRead)//jesli raz odczytalismy znak, i chcemy czytac jeszcze raz, ale drugi konsument jeszcze nie przeczytał
                wait(canReadNext);//to czekamy az drugi konsument go odczyta, i dopiero wtedy mozemy czytac nastepny

            ret = buffer_.front();
            wasSecondConsumentRead = true;//ustawiamy flage, ze przeczytalismy
        }
        return ret;
    }


    T situationMIN_ELEM_forFirst()
    {
        T ret;
        if(wasSecondConsumentRead)//drugi konsumnet przeczytals, alenie mozemy trwale usunac elementu, bo w buforze bylo by ich za malo
        {
            wait(canPop);//zaczekaj az ktos doda element, potem usun
            ret = readAndDelete();
            wasSecondConsumentRead = false;
        }
        else//drugi konsument nie odczytal elementu - czytamy bez usuwania
        {
            if(wasFirstConsumentRead)
                wait(canReadNext);

            ret = buffer_.front();
            wasFirstConsumentRead = true;
        }
        return ret;
    }

    T situationMIN_ELEM_forSecond()
    {
        T ret;
        if(wasFirstConsumentRead)//drugi konsument przeczytal, nie mozemy trwale usunac elementu, bo w buforze bylo by ich za malo
        {
            wait(canPop);//zaczekaj az ktos doda element, potem usun
            ret = readAndDelete();
            wasFirstConsumentRead = false;

        }
        else//drugi konsument nie odczytal elementu - czytamy bez usuwania
        {
            if(wasSecondConsumentRead)
                wait(canReadNext);

            ret = buffer_.front();
            wasSecondConsumentRead = true;
        }
        return ret;
    }

    mutable Condition canPushOne, canPushTwo;
    mutable Condition canPop, canReadNext;//, canDeleteFirstElem;

    std::vector<T> buffer_;

    mutable bool  wasFirstConsumentRead, wasSecondConsumentRead;//mowi czy drugi konsument usunał ten element

    const unsigned int MAX_ITEMS_;
    const unsigned int MIN_ELEM_ = 3;//minimalna ilość elementów - gdy w buforze jest więcej elementów to można je wyciągać z bufora





};

#endif // MONITORBUFFER_HPP
