// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vng_core.h for the primary calling header

#ifndef VERILATED_VNG_CORE___024ROOT_H_
#define VERILATED_VNG_CORE___024ROOT_H_  // guard

#include "verilated.h"


class Vng_core__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vng_core___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(rst,0,0);
    CData/*2:0*/ ng_core__DOT__write;
    CData/*0:0*/ ng_core__DOT__control__DOT__handle__DOT__lt;
    CData/*0:0*/ ng_core__DOT__control__DOT__handle__DOT__eq;
    CData/*0:0*/ ng_core__DOT__control__DOT__handle__DOT__gt;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__rst__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(instruction,15,0);
    VL_OUT16(addr,15,0);
    SData/*15:0*/ ng_core__DOT__a_reg;
    SData/*15:0*/ ng_core__DOT__d_reg;
    SData/*15:0*/ ng_core__DOT__a_mem_reg;
    SData/*15:0*/ ng_core__DOT__result;
    VlWide<2048>/*65535:0*/ ng_core__DOT__mem__DOT__mem_array;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle_out;
    IData/*31:0*/ __VactIterCount;
    VL_OUT64(reflect,47,0);
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<2> __VactTriggered;
    VlTriggerVec<2> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vng_core__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vng_core___024root(Vng_core__Syms* symsp, const char* v__name);
    ~Vng_core___024root();
    VL_UNCOPYABLE(Vng_core___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
