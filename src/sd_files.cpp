#ifdef ARDUINO

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "FS.h"
//#include "SD.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <StreamUtils.h>

#include <draw.h>

#include "../conf.h"

#include "files.h"

#ifdef SD_CS_PIN
#define SD SD_MMC
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

// bool init_file_system() 
// {
//     if (!SD.begin(SD_CS_PIN)) {
//         log_e("Card Mount Failed");
//        //Serial.println();
//         return false;
//     }
//     uint8_t cardType = SD.cardType();

//     if (cardType == CARD_NONE) {
//         log_e("No SD card attached");
//        //Serial.println("No SD card attached");
//         header_msg("No SD card attached");
//         return false;
//     }

//     log_i("SD Card Type:");
//     //Serial.print("SD Card Type: ");
//     if (cardType == CARD_MMC) {
//         //Serial.println("MMC");
//         log_i("MMC");
//     } else if (cardType == CARD_SD) {
//         //Serial.println("SDSC");
//         log_i("SDSC");
//     } else if (cardType == CARD_SDHC) {
//         //Serial.println("SDHC");
//         log_i("SDHC");
//     } else {
//         //Serial.println("UNKNOWN");
//         log_i("UNKNOWN");
//     }

//     uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//    // Serial.printf("SD Card Size: %lluMB\n", cardSize);   
//     //Serial.println("initialisation done.");
//     log_i("SD Card Size: %lluMB\ninitialisation done.\n", cardSize);
//     return true;
// }

bool init_file_system() 
{
    #ifdef SD_DAT1
    if (!SD.setPins(SD_SCLK, SD_CMND, SD_DAT0, SD_DAT1, SD_DAT2, SD_DAT3)) {
        log_e("Unable to set pins");
        return false;
    }
    #else
    if (!SD.setPins(SD_SCLK_PIN, SD_MOSI_PIN, SD_MISO_PIN)) {
        log_e("Unable to set pins");
        return false;
    }
    #endif

    if (!SD.begin("/", true)) {
        log_e("Card Mount Failed");
        return false;
    }

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
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    log_i("SD Card Size: %lluMB\ninitialisation done.\n", cardSize);

    return true;
}

IFileSystem* get_file_system(const char* root) {
    return new ArduinoFileStreamFactory(root);
}

#endif