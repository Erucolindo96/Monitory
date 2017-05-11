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
    MonitorBuffer(const int MAX_ITEMS):MAX_ITEMS_(MAX_ITEMS)
    {}

    void pushOneElement(const T &element)
    {
        enter();
        if(buffer_.size() == MAX_ITEMS_)
        {
            wait(canPushOne);//czekaj dopoki konsumenci nie zwolnia elementu
        }
        buffer_.push_back(element);

        if(buffer_.size() >= MIN_ELEM_)
        {
            signal(canPop);
            signal(canPopFirst);
            signal(canPopSecond);
        }

        leave();
    }

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
            signal(canPop);
            signal(canPopFirst);
            signal(canPopSecond);

        }

        leave();
    }

    T popConsumentFirst()
    {
        T returned_elem;
        enter();

        if(buffer_.size() < MIN_ELEM_)//jesli jest mniejsza niz MIN - to gdy element zostanie dodany, to albo będzie wiecej niz MIN_ELEm
        {                           //albo ==MIN_ELEM, ale bedziemy pierwszymi ktorzy wyjmuja
            wait(canPop);
            if(wasOtherConsumentPop)//mozemy usunac element
            {
                returned_elem = PopAndDelete();
                wasOtherConsumentPop = false;
            }
            else//nie mozemy trwale usunac, ale mozemy pobrac
            {
                returned_elem = buffer_.front();
                wasOtherConsumentPop = true;
            }

        }
        else if(buffer_.size() == MIN_ELEM_)//jest minimalna liczba elementow - jeden moze usuwac
        {
            if(wasOtherConsumentPop)//musimy poczekac az ktorys producent wstawi i  usunac element
            {
                wait(canPop);
                returned_elem = PopAndDelete();
                wasOtherConsumentPop = false;
            }
            else//nie mozemy trwale usunac, ale mozemy pobrac
            {
                returned_elem = buffer_.front();
                wasOtherConsumentPop = true;
            }

        }
        else//jest wiecej niz MIN_ELEM
        {
            if(wasOtherConsumentPop)//mozemy usunac element
            {
                returned_elem = PopAndDelete();
                wasOtherConsumentPop = false;
            }
            else//nie mozemy trwale usunac, ale mozemy pobrac
            {
                returned_elem = buffer_.front();
                wasOtherConsumentPop = true;
            }
        }

        if(buffer_.size() < MIN_ELEM_ - 1)//jesli sa dwa wolne miejsca
            signal(canPushTwo);

        signal(canPushOne);

        leave();
        return returned_elem;
    }

    T popConsumentSecond()
    {
        return popConsumentFirst();
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
        return T;
    }

    Condition canPushOne, canPushTwo, canPush;
    Condition canPopFirst, canPopSecond, canPop;

    std::vector<T> buffer_;
    T elementToPop;

    bool wasOtherConsumentPop;//mowi czy drugi konsument usunał ten element

    const int MAX_ITEMS_;
    const int MIN_ELEM_ = 3;//minimalna ilość elementów - gdy w buforze jest więcej elementów to można je wyciągać z bufora





};

#endif // MONITORBUFFER_HPP
