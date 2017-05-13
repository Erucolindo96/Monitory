#include <QCoreApplication>
#include<thread>
#include"MonitorBuffer.hpp"
#include<iostream>
#include<unistd.h>
const int BUF_SIZE = 10;

MonitorBuffer<int> my_buffer(BUF_SIZE);


void producerOneCode()
{
    std::cout<<"Producent 1 gotowy!"<<" "<<std::endl;
    for(int i = 0; i<10;++i)
    {
        my_buffer.pushOneElement(i);
        std::cerr<<"P1: Dodano element "<<i<<" "<<std::endl;
        sleep(2);
    }
}

void producerDoubleCode()
{
    std::cerr<<"Producent 2 gotowy!"<<std::endl;
    for(int i = 10; i<20;i+=2 )
    {
        my_buffer.pushTwoElements(i, i+1);
        std::cerr<<"P2: Dodano elementy "<<i<<" oraz "<<i+1<<" "<<std::endl;
        sleep(1);
    }
}

void consumentFirstCode()
{
    int ret;
    std::cout<<"Konsument 1 gotowy!"<<" "<<std::endl;
    for(int i = 0; i<17;++i)
    {
        ret = my_buffer.popConsumentFirst();
        std::cerr<<"K1: Zdjeto element "<<ret<<" "<<std::endl;
        sleep(2);
    }
}

void consumentSecondCode()
{
    int ret;
    std::cout<<"Konsument 2 gotowy!"<<" "<<std::endl;
    for(int i = 0; i<17;++i)
    {
        ret = my_buffer.popConsumentSecond();
        std::cerr<<"K2: Zdjeto element "<<ret<<" "<<std::endl;
        sleep(2);
    }
}


int main(int argc, char *argv[])
{


    std::thread producer_one(producerOneCode), producer_double(producerDoubleCode);
    sleep(1);
    std::thread consument_first(consumentFirstCode), consument_second(consumentSecondCode);


    producer_one.join();
    producer_double.join();
    consument_first.join();
    consument_second.join();

    my_buffer.printBuffer();

    std::cout<<"Wszystkie watki wykonaly swoja prace"<<std::endl;

    return 0;
}

