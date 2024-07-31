#pragma once

#ifndef ARDUINO

#include <iostream>
#include <fstream>

namespace fs
{
    class SDMMCFS
    {
    public:
        bool begin(const char* mountPoint, bool)
        {
            return true;
        }

        std::ifstream open(const char* fileName)
        {
            return std::ifstream(fileName);
        }

        int cardType()
        {
            return 4;
        }

        unsigned long cardSize()
        {
            return 1024 * 1024 * 32;
        }
    };
}

extern fs::SDMMCFS SD_MMC;

#endif