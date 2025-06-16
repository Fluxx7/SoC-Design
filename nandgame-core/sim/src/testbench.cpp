#include "Vng_core.h"
#include "verilated.h"
#include "ANSI_format.hpp"
#include "../obj_dir/Vng_core___024root.h"
#include "rom.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

std::string reg_print(int16_t old, int16_t curr);

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
    const int CYCLE_PAUSE = 100;

    // Apply reset
    core->rst = 0;
    core->eval();
    core->rst = 1;
    core->eval();  // simulate positive edge
    time += 1;
    int16_t pc = 0;
    uint16_t cycle = 0;
    uint16_t cyloop = 0;
    std::string a_str, d_str, am_str;
    while(1) {
        if (cyloop >= CYCLE_PAUSE) {
            std::cout << "Continue? (type 'cont' or 'y')" << std::endl;
            std::string response;
            std::cin >> response;
            if (!(response == "y" || response == "cont")) {
                break;
            }
            cyloop = 0;
        }

        uint16_t inst = rom.next_line();
        
        core->instruction = inst;
        int16_t a_val_old = core->rootp->ng_core__DOT__a_reg;
        int16_t d_val_old = core->rootp->ng_core__DOT__d_reg;
        int16_t am_val_old = core->rootp->ng_core__DOT__a_mem_reg;

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
        a_str = reg_print(a_val_old, a_val);
        d_str = reg_print(d_val_old, d_val);
        am_str = reg_print(am_val_old, am_val);


        int16_t curr_pc = pc;
        // Get next PC
        pc = core->addr;
        println_ANSI("{:1m}",std::format("Cycle {}", cycle));
        if (rom.debug) {
            uint16_t comp_linenum;
            std::string comp_code;
            std::tie(comp_linenum, comp_code) = rom.debug_info(); 
            std::println("Line {2:d}: '{1}' -> {0:016b}", inst, comp_code, comp_linenum);
        } else {
            std::println("Instruction = {:016b}", inst);
        }
        println_ANSI("PC = {:31m>d} -> {:32m>d}\nA  = {}\nD  = {}\n*A = {}\n", curr_pc, pc, a_str, d_str, am_str);
        if (rom.next_addr(pc)) break;
        cycle++;
        cyloop++;
    }
    std::cout << "Simulation completed" << std::endl;
    delete core;
    return 0;
}

inline std::string reg_print(int16_t old, int16_t curr) {
    if (curr == old) {
        return format_ANSI("{0:36m>d} ({1:36m>#06x})", curr, static_cast<uint16_t>(curr));
    } else {
        return format_ANSI("{0:31m>d} ({1:31m>#06x}) -> {2:1;96m>d} ({3:1;96m>#06x})", old, static_cast<uint16_t>(old), curr, static_cast<uint16_t>(curr));
    }
}

