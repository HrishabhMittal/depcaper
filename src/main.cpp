#include "pcap.cpp"
#include <cstdint>
#include <ios>
#include <iostream>
#include <ostream>

int main(int argc, char** argv) {
    if (argc==1) {
        std::cerr<<"Usage: "<<argv[0]<<" <pcap file> <flags>\n";
        std::cerr<<"Flags:\n";
        std::cerr<<"  -v: verbose\n";
        return 1;
    }
    Reader r(argv[1]);
    if (!r.good()) {
        std::cerr<<"Error opening file "<<argv[1]<<"\n";
        return 1;
    }
    bool verbose=false;
    if (argc>=3) {
        std::string flag(argv[2]);
        if (flag=="-v") {
            verbose=true;
        }
    }
    PCAPHeader header(r);
    if (verbose) std::cout<<header;
    while (!r.eof()) {
        PacketRecord record(r);
        if (verbose) std::cout<<record;
        else {
            print_bin_string(std::cout,record.data);
        }
    }
}
