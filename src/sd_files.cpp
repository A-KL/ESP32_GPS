#ifdef ARDUINO

#include <canvas.h>

#include "files.h"
#include "../conf.h"

#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <StreamUtils.h>

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
        ArduinoFileStreamFactory(const char* root = NULL)
        {}

        virtual IReadStream* Open(const char* fileName) const
        {
            return new ArduinoReadFileStream(fileName);
        }
};

bool init_file_system() {
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("Card Mount Failed");
        return false;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        tft_header_msg("No SD card attached");
        return false;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);   
    Serial.println("initialisation done.");
    return true;
}

IFileSystem* get_file_system(const char* root) {
    return new ArduinoFileStreamFactory(root);
}

#endif