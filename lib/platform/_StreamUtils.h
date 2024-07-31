#pragma once

#ifndef ARDUINO

#include <iostream>
#include <fstream>

class ReadBufferingStream
{
    public:
        explicit ReadBufferingStream(std::ifstream &upstream, size_t capacity) 
            : _upstream(upstream), _capacity(capacity), _buffer(0)
        { }

        inline virtual int read() 
        {
            if (!_upstream.is_open()) 
                return -1;

            _upstream.read(&_buffer, 1);

            return _buffer;
        };
    
    private:
        std::ifstream& _upstream;
        size_t _capacity;
        char _buffer;
};

#endif