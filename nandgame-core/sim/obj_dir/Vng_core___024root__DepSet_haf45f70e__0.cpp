// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vng_core.h for the primary calling header

#include "Vng_core__pch.h"
#include "Vng_core__Syms.h"
#include "Vng_core___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__ico(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG

void Vng_core___024root___eval_triggers__ico(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_triggers__ico\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered.setBit(0U, (IData)(vlSelfRef.__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vng_core___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__act(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG

void Vng_core___024root___eval_triggers__act(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_triggers__act\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered.setBit(0U, ((IData)(vlSelfRef.clk) 
                                          & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))));
    vlSelfRef.__VactTriggered.setBit(1U, ((~ (IData)(vlSelfRef.rst)) 
                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__rst__0)));
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vng_core___024root___dump_triggers__act(vlSelf);
    }
#endif
}
