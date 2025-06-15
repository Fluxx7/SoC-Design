#include "rom.hpp"
#include <sstream>
using namespace std;

ROM::ROM(const string& filename){
    vector<uint16_t> rom;
    vector<struct debug_line> d_rom;
    ifstream infile(filename);
    string line;
    this->debug = false;
    /*
    Puts each instruction from the provided file into rom
    Needs to both interact with and adjust for debug info IF the file has it
    I don't know what that interaction will look like or even what the debug info will look like
    We might be cooked

    I want to see the original instruction and what that instruction was broken down to
    I want to be able to see at least part of the memory
    
    */
    std::getline(infile, line);
    if(strcmp(line.c_str(), "<debug>") == 0) {
        this->debug = true;
    } else {
        infile.seekg(0);
        infile.clear();
    }
    while (std::getline(infile, line)) {
        if (this->debug == false) {
            uint16_t inst = bitset<16>(line).to_ulong();
            rom.push_back(inst);
        } else {
            stringstream ss(line);
            string buf;
            struct debug_line inst;
            getline(ss, buf, ' ');
            inst.instruction = bitset<16>(buf).to_ulong();
            getline(ss, buf, ' ');
            inst.comp_linenum = stoi(buf);
            getline(ss, buf, ' ');
            inst.comp_codeline = buf;
            d_rom.push_back(inst);
        }
       
    }
    
    if (this->debug) {
        this->debug_instructions = d_rom;
    } else {
        this->instructions = rom;
    }
    this-> pc = 0;
}

// Sets the address of the next instruction to execute
int ROM::next_addr(uint16_t addr)
{
    size_t comparison_size = debug ? this->debug_instructions.size() : this->instructions.size();
    if (addr >= comparison_size) {
        return 1;
    } 
    this->pc = addr;
    return 0;
}

// Returns the current instruction to be executed
uint16_t ROM::next_line() {
    if (debug) {
        return this->debug_instructions[pc].instruction;
    }
    return this->instructions[pc];
}

tuple<uint16_t, string> ROM::debug_info() {
    if (!debug){
        return {0, ""};
    }
    return {this->debug_instructions[pc].comp_linenum, this->debug_instructions[pc].comp_codeline};
}