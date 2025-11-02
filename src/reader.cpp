#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
class Reader {
    std::ifstream file;
public:
    Reader(std::string filename) {
        file=std::ifstream(filename,std::ios::binary);
    }
    bool eof() {
        return file.eof();
    }
    bool good() {
        return file.good();
    }
    template<typename T>
    T read_into(T& num) {
        file.read(reinterpret_cast<char*>(&num),sizeof(num));
        return num;
    }
    std::string read_bytes(int bytes) {
        std::string out(bytes,0);
        file.read(out.data(),bytes);
        return out;
    }
};
