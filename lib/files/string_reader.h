#pragma once

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <logs.h>
    #include <cassert>
#endif

#include <string>
#include <stdexcept>

#include "file_stream.h"

class StringReader {
    public:
        StringReader(IReadStream* stream) : _stream(stream)
        { }

        bool ReadInt16(int16_t& value)
        {
            uint8_t i = 0;
            char num[16]; 
            char c = (char) _stream->read();

            if (c == '\n') return false;

            while (c>='0' && c <= '9') {
                assert( i < 15);
                num[i++] = c;
                c = (char) _stream->read();
            }

            num[i] = '\0';

            if (c != ';' && c != ',' && c != '\n') {
                log_e("ReadInt16 - Error: %c %i [%s]\n", c, c, num);
                return false;
            }

            try
            {
                value = std::stoi(num);
                return true;
            } catch (std::invalid_argument) {
                log_e("ReadInt16 - invalid_argument: [%c] [%s]\n", c, num);
            } catch (std::out_of_range) {
                log_e("ReadInt16 - out_of_range: [%c] [%s]\n", c, num);
            }
            return false;
        }

        void ReadUntil(char terminator, char *str)
        {
            uint8_t i = 0;
            char c = (char)_stream->read();

            while (c != terminator && c != '\n'){
                assert(i < 29);
                str[i++] = c;
                c = (char) _stream->read();
            }
            str[i] = '\0';
        }

        virtual ~StringReader() {};

    private:
        IReadStream* _stream;
};