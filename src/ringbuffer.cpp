#include "ringbuffer.h"
Ringbuffer::Ringbuffer(int size)
{
    arr_size = size;
}

Ringbuffer::~Ringbuffer()
{
}

void Ringbuffer::add(int x)
{
    if(buffer.size() == arr_size){
        buffer.at(cur) = x;
        cur += 1;
        if(cur == arr_size){
            cur = 0;
        }
    }
    else{
        buffer.push_back(x);
    }
}
