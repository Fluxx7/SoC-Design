#include "Vng_core.h"
#include "verilated.h"
#include "debugger.hpp"
#include "../obj_dir/Vng_core___024root.h"
#include "rom.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vng_core* core = new Vng_core;
    std::string rom_file;
    if (argc < 2) {
        std::cout << "Provide target file:" << std::endl;
        std::cin >> rom_file;
    } else {
        rom_file.assign(argv[1]);
    }

    ROM rom = ROM(rom_file);

    uint64_t time = 0;
    const int MAX_CYCLES = 100;

    // Apply reset
    core->rst = 0;
    core->eval();
    core->rst = 1;
    core->eval();  // simulate positive edge
    time += 1;
    int16_t pc = 0;
    for (int cycle = 0; cycle < MAX_CYCLES; ++cycle) {

        uint16_t inst = rom.next_line();
        
        core->instruction = inst;

        // Clock low
        core->clk = 0;
        core->eval();
        time++;

        // Clock high (posedge)
        core->clk = 1;
        core->eval();
        time++;

        
        // Print output for debug
        int16_t a_val = core->rootp->ng_core__DOT__a_reg;
        int16_t d_val = core->rootp->ng_core__DOT__d_reg;
        int16_t am_val = core->rootp->ng_core__DOT__a_mem_reg;
        int16_t curr_pc = pc;
        // Get next PC
        pc = core->addr;
        std::println("Cycle {}", cycle);
        if (rom.debug) {
            uint16_t comp_linenum;
            std::string comp_code;
            std::tie(comp_linenum, comp_code) = rom.debug_info(); 
            std::println("Line {2:d}: '{1}' -> {0:016b}", inst, comp_code, comp_linenum);
        } else {
            std::println("Instruction = {:016b}", inst);
        }
        std::println("PC = {:d}, A = {:d} (0x{:04x}), D = {:d} (0x{:04x}), *A = {:d} (0x{:04x}), next PC = {}\n", 
                curr_pc, a_val, static_cast<uint16_t>(a_val), d_val, static_cast<uint16_t>(d_val), am_val, static_cast<uint16_t>(am_val), pc);
        if (rom.next_addr(pc)) break;
    }
    std::cout << "Simulation completed" << std::endl;
    delete core;
    return 0;
}
