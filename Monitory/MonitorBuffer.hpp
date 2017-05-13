#ifndef MONITORBUFFER_HPP
#define MONITORBUFFER_HPP

#include<vector>
#include"monitor.h"

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

    void pushOneElement(const T &element)
    {
        enter();
        if(buffer_.size() >= MAX_ITEMS_ )
        {
            //wait(canPush);
            wait(canPushOne);//czekaj dopoki konsumenci nie zwolnia elementu
        }
        buffer_.push_back(element);

        if(buffer_.size() >= MIN_ELEM_)
        {
            signal(canPop);

        }

        leave();
    }

    void pushTwoElements(const T &elem_1, const T &elem_2)
    {
        enter();

        if(buffer_.size() >= MAX_ITEMS_ - 1)//jesli w buforze jest albo tylko jedno miejsce, albo nie ma go wcale
        {
           // wait(canPush);
            wait(canPushTwo);//cos jest nie tak z producentami, pozwalaja dodawac za duzo elementow
        }
        buffer_.push_back(elem_1);
        buffer_.push_back(elem_2);

        if(buffer_.size() >= MIN_ELEM_)
        {
            signal(canPop);

        }

        leave();
    }

    T popConsumentFirst()
    {
        T returned_elem;
        enter();

        if(buffer_.size() < MIN_ELEM_  )//jesli jest zbyt malo elementow w buforze
        {
            wait(canPop);//czekamy
            //teraz mamy 3 lub 4 elementy
            if(buffer_.size() == MIN_ELEM_)
            {
                returned_elem = situationMIN_ELEM_forFirst();
            }
            else
            {
                returned_elem = situationNormalForFirst();
            }

        }
        else if(buffer_.size() == MIN_ELEM_)//wtedy nie mozna usunac elementu
        {
            returned_elem = situationMIN_ELEM_forFirst();
        }
        else //jest wiecej niz MIN_ELEM - mozna swobodnie usuwac
        {
            returned_elem = situationNormalForFirst();
        }

        if(!wasFirstConsumentPop && !wasSecondConsumentPop)//jesli obydwie flagi konsumentów == false, to element został usunięty, a nie tylko przeczytany
        {//wiec mozemy zasygnalizowac mozliwosc dodawania

            if(buffer_.size() < MAX_ITEMS_ - 2)//jesli sa chociaz dwa wolne miejsca
                signal(canPushTwo);

            if(buffer_.size() < MAX_ITEMS_ - 1 )//jesli jest chociaz jedno wolne miejsce
                signal(canPushOne);
        }


/*
        if(buffer_.size() < MAX_ITEMS_)
            signal(canPush);

        if(buffer_.size() < MAX_ITEMS_ - 1)//jesli sa chociaz dwa wolne miejsca
            signal(canPushTwo);

        if(buffer_.size() < MAX_ITEMS_)//jesli jest chociaz jedno wolne miejsce
            signal(canPushOne);
*/
        leave();
        return returned_elem;

    }

    T popConsumentSecond()
    {
        T returned_elem;
        enter();

        if(buffer_.size() < MIN_ELEM_  )//jesli jest zbyt malo elementow w buforze
        {
            wait(canPop);//czekamy
            //teraz mamy 3 lub 4 elementy
            if(buffer_.size() == MIN_ELEM_)
            {
                returned_elem = situationMIN_ELEM_forSecond();
            }
            else
            {
                returned_elem = situationNormalForSecond();
            }

        }
        else if(buffer_.size() == MIN_ELEM_)//wtedy nie mozna usunac elementu
        {
            returned_elem = situationMIN_ELEM_forSecond();
        }
        else //jest wiecej niz MIN_ELEM - mozna swobodnie usuwac
        {
            returned_elem = situationNormalForSecond();
        }


        if(!wasFirstConsumentPop && !wasSecondConsumentPop)//jesli obydwie flagi konsumentów == false, to element został usunięty, a nie tylko przeczytany
        {//wiec mozemy zasygnalizowac mozliwosc dodawania

            if(buffer_.size() < MAX_ITEMS_ - 1)//jesli sa chociaz dwa wolne miejsca
                signal(canPushTwo);

            if(buffer_.size() < MAX_ITEMS_)//jesli jest chociaz jedno wolne miejsce
                signal(canPushOne);
        }
/*        if(buffer_.size() < MAX_ITEMS_)
            signal(canPush);

        if(buffer_.size() < MAX_ITEMS_ - 1)//jesli sa chociaz dwa wolne miejsca
            signal(canPushTwo);

        if(buffer_.size() < MAX_ITEMS_)//jesli jest chociaz jedno wolne miejsce
            signal(canPushOne);
*/
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


private:

    MonitorBuffer() = delete;

    int getWaitingCount(const Condition &other)
    {
        return other.waitingCount;
    }
    T PopAndDelete()
    {
        auto first_elem  = buffer_.begin();
        T temp = *first_elem;
        buffer_.erase(first_elem);
        return temp;
    }

    T situationNormalForFirst()
    {
        T ret;
        if(wasSecondConsumentPop)
        {
            ret = PopAndDelete();
            wasSecondConsumentPop = false;
        }
        else
        {
            ret = buffer_.front();
            wasFirstConsumentPop = true;
        }
        return ret;
    }

    T situationNormalForSecond()
    {
        T ret;
        if(wasFirstConsumentPop)
        {
            ret = PopAndDelete();
            wasFirstConsumentPop = false;
        }
        else
        {
            ret = buffer_.front();
            wasSecondConsumentPop = true;
        }
        return ret;
    }


    T situationMIN_ELEM_forFirst()
    {
        T ret;
        if(wasSecondConsumentPop)
        {
            wait(canPop);//zaczekaj az ktos doda element, potem usun
            ret = PopAndDelete();
            wasSecondConsumentPop = false;
        }
        else//drugi konsument nie odczytal elementu - czytamy bez usuwania
        {
            ret = buffer_.front();
            wasFirstConsumentPop = true;
        }
        return ret;
    }

    T situationMIN_ELEM_forSecond()
    {
        T ret;
        if(wasFirstConsumentPop)//nie mozemy trwale usunac elementu, bo w buforze bylo by ich za malo
        {
            wait(canPop);//zaczekaj az ktos doda element, potem usun
            ret = PopAndDelete();
            wasFirstConsumentPop = false;
        }
        else//drugi konsument nie odczytal elementu - czytamy bez usuwania
        {
            ret = buffer_.front();
            wasSecondConsumentPop = true;
        }
        return ret;
    }

    mutable Condition canPushOne, canPushTwo, canPush;
    mutable Condition canPop;//, canDeleteFirstElem;

    std::vector<T> buffer_;
    //T elementToPop;

    mutable bool wasFirstConsumentPop, wasSecondConsumentPop;//mowi czy drugi konsument usunał ten element

    const unsigned int MAX_ITEMS_;
    const unsigned int MIN_ELEM_ = 3;//minimalna ilość elementów - gdy w buforze jest więcej elementów to można je wyciągać z bufora





};

#endif // MONITORBUFFER_HPP
