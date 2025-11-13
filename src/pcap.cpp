#include "reader.cpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
struct PCAPHeader {
    int32_t magic,snaplen;
    int16_t major,minor,linktype,fcs;
    PCAPHeader(Reader& r) {
        r.read_into(magic);
        r.read_into(major);
        r.read_into(minor);
        //ignore
        r.read_into(snaplen);
        r.read_into(snaplen);
        
        r.read_into(snaplen);
        r.read_into(linktype);
        r.read_into(fcs);
    }
};
std::ostream& operator<<(std::ostream& os, const PCAPHeader& header) {
    os<<"Magic: "<<std::hex<<header.magic<<std::dec<<"\n";
    os<<"Version: "<<header.major<<"."<<header.minor<<"\n";
    os<<"Snaplen: "<<header.snaplen<<"\n";
    os<<"Linktype: "<<header.linktype<<"\n";
    os<<"FCS: "<<header.fcs<<"\n";
    return os;
}
void print_bin_string(std::ostream& os, const std::string& data) {
    for(unsigned char c:data) {
        if (std::isprint(c)) {
            os<<c;
        } else {
            os<<".";
        }
    }
}
std::string get_bin_string(const std::string& data) {
    std::string out;
    for(unsigned char c:data) {
        if (std::isprint(c)) {
            out+=c;
        } else {
            out+=".";
        }
    }
    return out;
}
void print_hex_string(std::ostream& os, const std::string& data) {
    os<<std::hex;
    for(unsigned char c:data) {
        os<<std::setw(2)<<std::setfill('0')<<(int)c<<" ";
    }
    os<<std::setfill(' ')<<std::dec;
}

std::string get_hex_string(const std::string& data) {
    std::stringstream os;
    os<<std::hex;
    for(unsigned char c:data) {
        os<<std::setw(2)<<std::setfill('0')<<(int)c<<" ";
    }
    os<<std::setfill(' ')<<std::dec;
    return std::string(os.str());
}
struct PacketRecord {
    int32_t ts_sec,ts_usec,incl_len,orig_len;
    std::string data;
    PacketRecord(Reader& r) {
        r.read_into(ts_sec);
        r.read_into(ts_usec);
        r.read_into(incl_len);
        r.read_into(orig_len);
        data=r.read_bytes(incl_len);
    }
};
std::ostream& operator<<(std::ostream& os, const PacketRecord& record) {
    os<<"Timestamp: "<<record.ts_sec<<"."<<record.ts_usec<<"\n";
    os<<"Included Length: "<<record.incl_len<<"\n";
    os<<"Original Length: "<<record.orig_len<<"\n";
    os<<"Data: ";
    print_bin_string(os,record.data);
    os<<"\nHex Data: ";
    print_hex_string(os,record.data);
    os<<"\n";
    return os;
}
class PCAPFile {
public:
    PCAPHeader header;
    std::vector<PacketRecord> records;
    PCAPFile(Reader& r): header(r) {
        while(!r.eof()) {
            records.push_back(r);
        }
    }
};
