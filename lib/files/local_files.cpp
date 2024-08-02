#ifndef ARDUINO

#include <iostream>
#include <fstream>

#include <files.h>

class NativeWriteStream : public IWriteStream {
    public:
        NativeWriteStream(const char* fileName) {
            _stream = new std::ofstream(fileName);
        }

        virtual void write(char data) {
            // if (!_stream->is_open()) 
            //     return;

            char buffer[1] { data };
            _stream->write(buffer, 1);
        }

        ~NativeWriteStream() {
            _stream->flush();
            _stream->close();
            delete _stream;
        }

    private:
        std::ofstream* _stream;
};

class NativeReadStream : public IReadStream {
    public:
        NativeReadStream(const char* fileName) {
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
        }

        ~NativeReadStream() {
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

        inline virtual IReadStream* OpenRead(const char* fileName) const {
            auto fullPath =  _root + "/" + fileName;
            auto stream = new NativeReadStream(fullPath.c_str());
            auto result = stream->IsOpen() ? stream : NULL;
            if (!stream->IsOpen()) {
                delete stream;
            }
            return result;
        }

        inline virtual IWriteStream* OpenWrite(const char* fileName) const {
            auto fullPath =  _root + "/" + fileName;
            auto stream = new NativeWriteStream(fullPath.c_str());

            // if (!stream->IsOpen()) {
            //     delete stream;
            //     return NULL;
            // }
            return stream;
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