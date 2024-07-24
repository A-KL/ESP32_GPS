#ifndef ARDUINO

#include <iostream>
#include <fstream>

#include <files.h>

class LocalFileStream : public IReadStream {
    public:
        LocalFileStream(const char* fileName) {
            _stream = new std::ifstream(fileName);
        }

        inline uint8_t IsOpen() {
            return _stream->is_open();
        }

        virtual int read() {
            if (!_stream->is_open()) 
                return -1;

            char buffer[1];
            _stream->read(buffer, 1);
            return *buffer;
        };

        ~LocalFileStream() {
            _stream->close();
            delete _stream;
        }

    private:
        std::ifstream* _stream;
};
// "../OSM_Extract/maps/netherlands-latest.osm/mymap/+008+104/3_12.fmp"
class LocalFileStreamFactory : public IFileSystem {
    public:
        LocalFileStreamFactory(const char* root = NULL) : _root(root)
        {}

        inline virtual IReadStream* Open(const char* fileName) const {
            auto fullPath =  _root + "/" + fileName;
            auto stream = new LocalFileStream(fullPath.c_str());
            auto result = stream->IsOpen() ? stream : NULL;
            if (!stream->IsOpen()) {
                delete stream;
            }
            return result;
        }
    private:
        std::string _root;
};

bool init_file_system() {
    return true;
}

IFileSystem* get_file_system(const char* root) {
    return new LocalFileStreamFactory(root);
}
#endif