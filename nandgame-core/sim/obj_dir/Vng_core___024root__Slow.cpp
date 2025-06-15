// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vng_core.h for the primary calling header

#include "Vng_core__pch.h"
#include "Vng_core__Syms.h"
#include "Vng_core___024root.h"

void Vng_core___024root___ctor_var_reset(Vng_core___024root* vlSelf);

Vng_core___024root::Vng_core___024root(Vng_core__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vng_core___024root___ctor_var_reset(this);
}

void Vng_core___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vng_core___024root::~Vng_core___024root() {
}
