#include "pcap.cpp"
#include "oui/src/elements.cpp"
#include "argparse/include/argparse.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <sstream>
void stateUpdate(int index,std::weak_ptr<Label> packetbarptr,std::weak_ptr<Label> textdisplayptr,std::weak_ptr<Label> hexdisplayptr,PCAPFile& pcap) {
    if (!packetbarptr.expired()) {
        packetbarptr.lock()->text="Packet No. "+std::to_string(index+1);
    }
    if (!textdisplayptr.expired()) {
        textdisplayptr.lock()->text=get_bin_string(pcap.records[index].data);
    }
    if (!hexdisplayptr.expired()) {
        hexdisplayptr.lock()->text=get_hex_string(pcap.records[index].data);
    }
}
void runGui(Reader& r) {
    // ui setup
    Terminal::set_raw_mode();
    Terminal::OpenAltBuffer();
    Terminal::cursorInvisible();
    Application app({},0);
    app.fit_screen();
    auto titlebar = std::make_shared<Label>("De-PCAPer");
    auto packetbar = std::make_shared<Label>("Packet No. 1");
    std::weak_ptr<Label> packetbarptr=packetbar;
    auto mainscreen = std::make_shared<Div>();
    auto textdisplay = std::make_shared<Label>("..................");
    std::weak_ptr<Label> textdisplayptr=textdisplay;
    auto hexdisplay = std::make_shared<Label>("...................");
    std::weak_ptr<Label> hexdisplayptr=hexdisplay;
    mainscreen->add(textdisplay,0.5);
    mainscreen->add(hexdisplay,0.5);
    app.add(titlebar,3);
    app.add(packetbar,5);
    app.add(mainscreen,-1);

    PCAPFile pcap(r);
    bool running=true;
    int index=0;
    stateUpdate(index,packetbarptr,textdisplayptr,hexdisplayptr,pcap);
    while (running) {
        app.render();
        auto c=Input::handleInput();
        if (c=='q') running=false;
        else if (c==InputType::RIGHT) {
            if (index+1<pcap.records.size()) {
                index++;
                stateUpdate(index,packetbarptr,textdisplayptr,hexdisplayptr,pcap);
            }
        } else if (c==InputType::LEFT) {
            if (index-1>=0) {
                index--;
                stateUpdate(index,packetbarptr,textdisplayptr,hexdisplayptr,pcap);
            }
        }
    }
    // PCAPHeader header(r);
    // if (verbose) std::cout<<header;
    // while (!r.eof()) {
    //     PacketRecord record(r);
    //     if (verbose) std::cout<<record;
    //     else {
    //         print_bin_string(std::cout,record.data);
    //     }
    // }
    Terminal::resetColor();
}
int main(int argc, char** argv) {
    
    // argparser
    ArgParser parser(argc,argv);

    parser.addHelp();
    parser.addArg("input",'i',"input file");
    parser.addArg("output",'o',"output file, for no gui mode, ignored in -n not given");
    parser.addFlag('v',"verbose output");
    parser.addFlag('n',"no gui");
    if (!parser.parse()) {
        std::cout << "Error parsing arguments.\n";
        std::cout << parser.getHelpMessage();
        return 1;
    }
    if (parser.Flags().at('h')) {
        std::cout<< parser.getHelpMessage()<<std::endl;
        return 0;
    }
    bool verbose=parser.Flags().at('v');
    bool nogui=parser.Flags().at('n');
    std::string outfile=parser.Args().at("output");
    Reader r(parser.Args().at("input"));
    if (!r.good()) {
        std::cerr<<"Error opening file \'"<<parser.Args().at("input")<<"\'\n";
        return 1;
    }
    if (nogui) {
        std::ofstream file(outfile);
        std::ostream& out = *(!file.good()?&std::cout:&file);
        PCAPFile pcap(r);
        if (verbose) {
            out<<pcap.header<<std::endl;
        }
        for (auto& i:pcap.records) {
            if (verbose) {
                out<<i<<std::endl;
            } else {
                out<<"Data: ";
                print_bin_string(out,i.data);
                out<<"\nHex Data: ";
                print_hex_string(out,i.data);
                out<<std::endl;
            }
        }
        return 0;
    }
    runGui(r);
}
