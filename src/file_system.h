#pragma once

#ifdef ARDUINO
#include <StreamUtils.h>
    #ifdef SD_CS_PIN
    #include "SD.h"
    #else
    #include "SD_MMC.h"
    #define SD SD_MMC
    #endif
#else
#include <_StreamUtils.h>
#include "_SD_MMC.h"
#define SD SD_MMC
#endif

class stream
{
public:
    stream(const char* fileName) 
    : _file(SD.open(fileName)), _stream(_file, 2000)
    { }

    inline virtual int read() {
        return _stream.read();
    }

    inline bool init() {
        #ifdef ARDUINO
        return true;
        #else
        return true;
        #endif
    }

    inline bool is_open() {
        #ifdef ARDUINO
        return true;
        #else
        return _file.is_open();
        #endif
    }

    ~stream() {
        _file.close();
    }
private:
    #ifdef ARDUINO
    fs::File _file;
    #else
    std::ifstream _file;
    #endif
    ReadBufferingStream _stream;
};

class FileSystem
{
public:
    FileSystem(const char* root) : _root(root)
    { }

    stream open(const char* fileName){
        auto fullPath =  _root + "/" + fileName;
        return stream(fullPath.c_str());
    }
private:
    const std::string _root;
};