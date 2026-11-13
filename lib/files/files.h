#pragma once

class IReadStream {
    public:
        virtual int read() = 0;

        virtual ~IReadStream() {};
};

class IWriteStream {
    public:
        virtual void write(char data) = 0;

        virtual ~IWriteStream() {};
};

class IFileSystem {
    public:
        virtual IReadStream* OpenRead(const char* fileName) const = 0;
        virtual IWriteStream* OpenWrite(const char* fileName) const = 0;
};

bool init_file_system();

IFileSystem* get_file_system(const char* root = (const char*)0);