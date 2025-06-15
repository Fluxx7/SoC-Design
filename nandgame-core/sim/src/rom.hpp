#include <string>
#include <iostream>
#include <vector>
#include <fstream>


class ROM {
    public:
        ROM(const std::string& filename);
        int next_addr(uint16_t);
        uint16_t next_line();
        std::tuple<uint16_t, std::string> debug_info();
        bool debug;

    private:
        struct debug_line {
            uint16_t instruction;
            uint16_t comp_linenum;
            uint16_t true_linenum;
            std::string comp_codeline;
            std::string true_codeline;
        };
        uint16_t pc;
        std::vector<struct debug_line> debug_instructions;
        std::vector<uint16_t> instructions;
};