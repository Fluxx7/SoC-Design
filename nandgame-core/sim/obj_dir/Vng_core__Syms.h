// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VNG_CORE__SYMS_H_
#define VERILATED_VNG_CORE__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vng_core.h"

// INCLUDE MODULE CLASSES
#include "Vng_core___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vng_core__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vng_core* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vng_core___024root             TOP;

    // CONSTRUCTORS
    Vng_core__Syms(VerilatedContext* contextp, const char* namep, Vng_core* modelp);
    ~Vng_core__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
