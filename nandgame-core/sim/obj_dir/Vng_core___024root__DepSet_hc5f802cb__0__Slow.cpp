// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vng_core.h for the primary calling header

#include "Vng_core__pch.h"
#include "Vng_core___024root.h"

VL_ATTR_COLD void Vng_core___024root___eval_static(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_static\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    vlSelfRef.__Vtrigprevexpr___TOP__rst__0 = vlSelfRef.rst;
}

VL_ATTR_COLD void Vng_core___024root___eval_initial(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_initial\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vng_core___024root___eval_final(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_final\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__stl(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vng_core___024root___eval_phase__stl(Vng_core___024root* vlSelf);

VL_ATTR_COLD void Vng_core___024root___eval_settle(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_settle\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY(((0x64U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vng_core___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../rtl/../rtl/ng_core.sv", 1, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vng_core___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__stl(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___dump_triggers__stl\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VstlTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

void Vng_core___024root___ico_sequent__TOP__0(Vng_core___024root* vlSelf);

VL_ATTR_COLD void Vng_core___024root___eval_stl(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_stl\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vng_core___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vng_core___024root___eval_triggers__stl(Vng_core___024root* vlSelf);

VL_ATTR_COLD bool Vng_core___024root___eval_phase__stl(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_phase__stl\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vng_core___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vng_core___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__ico(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___dump_triggers__ico\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VicoTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__act(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___dump_triggers__act\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(negedge rst)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__nba(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___dump_triggers__nba\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(negedge rst)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vng_core___024root___ctor_var_reset(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___ctor_var_reset\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->rst = VL_RAND_RESET_I(1);
    vlSelf->instruction = VL_RAND_RESET_I(16);
    vlSelf->addr = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__a_reg = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__d_reg = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__a_mem_reg = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__result = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__write = VL_RAND_RESET_I(3);
    VL_RAND_RESET_W(65536, vlSelf->ng_core__DOT__mem__DOT__mem_array);
    vlSelf->ng_core__DOT__control__DOT__handle_out = VL_RAND_RESET_I(16);
    vlSelf->ng_core__DOT__control__DOT__handle__DOT__lt = VL_RAND_RESET_I(1);
    vlSelf->ng_core__DOT__control__DOT__handle__DOT__eq = VL_RAND_RESET_I(1);
    vlSelf->ng_core__DOT__control__DOT__handle__DOT__gt = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__rst__0 = VL_RAND_RESET_I(1);
}
