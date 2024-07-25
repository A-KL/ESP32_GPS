#ifdef ARDUINO

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "FS.h"
#include "SPI.h"
#include <StreamUtils.h>
#include <draw.h>
#include "files.h"

#ifdef SD_CS_PIN
#include "SD.h"
#else
#include "SD_MMC.h"
#define SD_MMC SD
#endif

class ArduinoReadFileStream : public IReadStream {
    public:
        ArduinoReadFileStream(const char* fileName) 
        {
            _file = SD.open(fileName);
           _stream  = new ReadBufferingStream {_file, 2000};
        }

        inline virtual int read() {
            return _stream->read();
        };

        ~ArduinoReadFileStream() {
            _file.close();
            delete _stream;
        }

    private:
        ReadBufferingStream* _stream;
        fs::File _file;
};

class ArduinoFileStreamFactory : public IFileSystem {
    public:
        ArduinoFileStreamFactory(const char* root = NULL) : _root(root)
        {}

        virtual IReadStream* Open(const char* fileName) const
        {
            auto fullPath =  _root + "/" + fileName;
            return new ArduinoReadFileStream(fullPath.c_str());
        }
    private:
        std::string _root;
};

bool init_file_system() 
{
    #ifndef SD_CS_PIN
    if (!SD.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN)) {
        log_e("Unable to set pins");
        return false;
    }
    #endif

    log_i("Mounting SD MMC");

    #ifdef SD_CS_PIN
    if (!SD.begin(SD_CS_PIN, SPI, 40000000)) {
        log_e("Card Mount Failed");
        return false;
    }
    #else
    if (!SD_MMC.begin("/sdcard", true)) {
        log_e("Card Mount Failed");
        return false;
    }
    #endif

    log_i("Card Mounted");

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        log_e("No SD card attached");
        return false;
    }

    log_i("SD Card Type:");
    
    if (cardType == CARD_MMC) {
        log_i("MMC");
    } else if (cardType == CARD_SD) {
        log_i("SDSC");
    } else if (cardType == CARD_SDHC) {
        log_i("SDHC");
    } else {
        log_i("UNKNOWN");
    }
    
    auto cardSize = SD.cardSize() / 1024.0 / 1024.0 / 1024.0;
    log_i("SD Card Size: %.2f GB\n", cardSize);

    return true;
}

IFileSystem* get_file_system(const char* root) {
    return new ArduinoFileStreamFactory(root);
}

#endif