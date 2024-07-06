#ifndef ARDUINO

#include <iostream>
#include <fstream>

#include <files.h>

class LocalFileStream : public IReadStream {
    public:
        LocalFileStream(const char* fileName) {
            _stream = new std::ifstream(fileName);
        }

        virtual int read() {
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

class LocalFileStreamFactory : public IFileSystem {
    public:
        LocalFileStreamFactory(const char* root = NULL) : _root(root)
        {}

        inline virtual IReadStream* Open(const char* fileName) const {
            auto fullPath =  _root + "/" + fileName;
            return new LocalFileStream(fullPath.c_str());
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