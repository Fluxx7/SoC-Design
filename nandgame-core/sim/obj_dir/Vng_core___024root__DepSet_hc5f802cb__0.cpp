// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vng_core.h for the primary calling header

#include "Vng_core__pch.h"
#include "Vng_core___024root.h"

void Vng_core___024root___ico_sequent__TOP__0(Vng_core___024root* vlSelf);

void Vng_core___024root___eval_ico(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_ico\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vng_core___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vng_core___024root___ico_sequent__TOP__0(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___ico_sequent__TOP__0\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
    ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2 = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 = 0;
    // Body
    ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2 
        = ((0x1000U & (IData)(vlSelfRef.instruction))
            ? (IData)(vlSelfRef.ng_core__DOT__a_mem_reg)
            : (IData)(vlSelfRef.ng_core__DOT__a_reg));
    if ((0x40U & (IData)(vlSelfRef.instruction))) {
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y 
            = vlSelfRef.ng_core__DOT__d_reg;
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 
            = ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
    } else {
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y 
            = ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 
            = vlSelfRef.ng_core__DOT__d_reg;
    }
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x 
        = ((0x80U & (IData)(vlSelfRef.instruction))
            ? 0U : (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0));
    if ((0x400U & (IData)(vlSelfRef.instruction))) {
        if ((0x400U & (IData)(vlSelfRef.instruction))) {
            vlSelfRef.ng_core__DOT__control__DOT__handle_out 
                = (0xffffU & ((0x200U & (IData)(vlSelfRef.instruction))
                               ? ((0x100U & (IData)(vlSelfRef.instruction))
                                   ? ((IData)(0xffffU) 
                                      + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))
                                   : ((IData)(1U) + 
                                      ((~ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)) 
                                       + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))))
                               : ((0x100U & (IData)(vlSelfRef.instruction))
                                   ? ((IData)(1U) + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))
                                   : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                      + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))));
        }
    } else {
        vlSelfRef.ng_core__DOT__control__DOT__handle_out 
            = (0xffffU & ((0x200U & (IData)(vlSelfRef.instruction))
                           ? ((0x100U & (IData)(vlSelfRef.instruction))
                               ? ((0x80U & (IData)(vlSelfRef.instruction))
                                   ? 0xffffU : (~ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0)))
                               : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  ^ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))
                           : ((0x100U & (IData)(vlSelfRef.instruction))
                               ? ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  | (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y))
                               : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  & (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))));
    }
    if ((0x8000U & (IData)(vlSelfRef.instruction))) {
        vlSelfRef.ng_core__DOT__write = (7U & ((IData)(vlSelfRef.instruction) 
                                               >> 3U));
        vlSelfRef.ng_core__DOT__result = vlSelfRef.ng_core__DOT__control__DOT__handle_out;
    } else {
        vlSelfRef.ng_core__DOT__write = 4U;
        vlSelfRef.ng_core__DOT__result = vlSelfRef.instruction;
    }
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__lt 
        = (1U & ((IData)(vlSelfRef.ng_core__DOT__control__DOT__handle_out) 
                 >> 0xfU));
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__eq 
        = (0U == (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle_out));
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__gt 
        = (1U & ((~ (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__lt)) 
                 & (~ (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__eq))));
}

void Vng_core___024root___eval_triggers__ico(Vng_core___024root* vlSelf);

bool Vng_core___024root___eval_phase__ico(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_phase__ico\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vng_core___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vng_core___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vng_core___024root___eval_act(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_act\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vng_core___024root___nba_sequent__TOP__0(Vng_core___024root* vlSelf);
void Vng_core___024root___nba_sequent__TOP__1(Vng_core___024root* vlSelf);

void Vng_core___024root___eval_nba(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_nba\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vng_core___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vng_core___024root___nba_sequent__TOP__1(vlSelf);
    }
}

VL_INLINE_OPT void Vng_core___024root___nba_sequent__TOP__0(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___nba_sequent__TOP__0\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.addr = ((IData)(vlSelfRef.rst) ? (0xffffU 
                                                & ((((IData)(vlSelfRef.instruction) 
                                                     >> 0xfU) 
                                                    & ((((IData)(vlSelfRef.instruction) 
                                                         >> 2U) 
                                                        & (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__lt)) 
                                                       | ((((IData)(vlSelfRef.instruction) 
                                                            >> 1U) 
                                                           & (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__eq)) 
                                                          | ((IData)(vlSelfRef.instruction) 
                                                             & (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__gt)))))
                                                    ? (IData)(vlSelfRef.ng_core__DOT__a_reg)
                                                    : 
                                                   ((IData)(1U) 
                                                    + (IData)(vlSelfRef.addr))))
                       : 0U);
}

VL_INLINE_OPT void Vng_core___024root___nba_sequent__TOP__1(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___nba_sequent__TOP__1\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
    ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2 = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y = 0;
    SData/*15:0*/ ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0;
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 = 0;
    // Body
    if ((2U & (IData)(vlSelfRef.ng_core__DOT__write))) {
        vlSelfRef.ng_core__DOT__d_reg = vlSelfRef.ng_core__DOT__result;
    }
    vlSelfRef.ng_core__DOT__a_mem_reg = (0xffffU & 
                                         (((0U == (0x1fU 
                                                   & VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U)))
                                            ? 0U : 
                                           (vlSelfRef.ng_core__DOT__mem__DOT__mem_array[
                                            (((IData)(0xfU) 
                                              + (0xffffU 
                                                 & VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U))) 
                                             >> 5U)] 
                                            << ((IData)(0x20U) 
                                                - (0x1fU 
                                                   & VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U))))) 
                                          | (vlSelfRef.ng_core__DOT__mem__DOT__mem_array[
                                             (0x7ffU 
                                              & (VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U) 
                                                 >> 5U))] 
                                             >> (0x1fU 
                                                 & VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U)))));
    if ((1U & (IData)(vlSelfRef.ng_core__DOT__write))) {
        VL_ASSIGNSEL_WI(65536,16,(0xffffU & VL_SHIFTL_III(16,32,32, (IData)(vlSelfRef.ng_core__DOT__a_reg), 4U)), vlSelfRef.ng_core__DOT__mem__DOT__mem_array, vlSelfRef.ng_core__DOT__result);
    }
    if ((4U & (IData)(vlSelfRef.ng_core__DOT__write))) {
        vlSelfRef.ng_core__DOT__a_reg = vlSelfRef.ng_core__DOT__result;
    }
    ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2 
        = ((0x1000U & (IData)(vlSelfRef.instruction))
            ? (IData)(vlSelfRef.ng_core__DOT__a_mem_reg)
            : (IData)(vlSelfRef.ng_core__DOT__a_reg));
    if ((0x40U & (IData)(vlSelfRef.instruction))) {
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y 
            = vlSelfRef.ng_core__DOT__d_reg;
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 
            = ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
    } else {
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y 
            = ng_core__DOT__control__DOT__handle__DOT____Vcellinp__alu__reg2;
        ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0 
            = vlSelfRef.ng_core__DOT__d_reg;
    }
    ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x 
        = ((0x80U & (IData)(vlSelfRef.instruction))
            ? 0U : (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0));
    if ((0x400U & (IData)(vlSelfRef.instruction))) {
        if ((0x400U & (IData)(vlSelfRef.instruction))) {
            vlSelfRef.ng_core__DOT__control__DOT__handle_out 
                = (0xffffU & ((0x200U & (IData)(vlSelfRef.instruction))
                               ? ((0x100U & (IData)(vlSelfRef.instruction))
                                   ? ((IData)(0xffffU) 
                                      + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))
                                   : ((IData)(1U) + 
                                      ((~ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)) 
                                       + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))))
                               : ((0x100U & (IData)(vlSelfRef.instruction))
                                   ? ((IData)(1U) + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x))
                                   : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                      + (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))));
        }
    } else {
        vlSelfRef.ng_core__DOT__control__DOT__handle_out 
            = (0xffffU & ((0x200U & (IData)(vlSelfRef.instruction))
                           ? ((0x100U & (IData)(vlSelfRef.instruction))
                               ? ((0x80U & (IData)(vlSelfRef.instruction))
                                   ? 0xffffU : (~ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT____VdfgRegularize_h948b49f6_0_0)))
                               : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  ^ (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))
                           : ((0x100U & (IData)(vlSelfRef.instruction))
                               ? ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  | (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y))
                               : ((IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__x) 
                                  & (IData)(ng_core__DOT__control__DOT__handle__DOT__alu__DOT__y)))));
    }
    vlSelfRef.ng_core__DOT__result = ((0x8000U & (IData)(vlSelfRef.instruction))
                                       ? (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle_out)
                                       : (IData)(vlSelfRef.instruction));
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__lt 
        = (1U & ((IData)(vlSelfRef.ng_core__DOT__control__DOT__handle_out) 
                 >> 0xfU));
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__eq 
        = (0U == (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle_out));
    vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__gt 
        = (1U & ((~ (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__lt)) 
                 & (~ (IData)(vlSelfRef.ng_core__DOT__control__DOT__handle__DOT__eq))));
}

void Vng_core___024root___eval_triggers__act(Vng_core___024root* vlSelf);

bool Vng_core___024root___eval_phase__act(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_phase__act\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<2> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vng_core___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vng_core___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vng_core___024root___eval_phase__nba(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_phase__nba\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vng_core___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__ico(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__nba(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vng_core___024root___dump_triggers__act(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG

void Vng_core___024root___eval(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY(((0x64U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vng_core___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../rtl/../rtl/ng_core.sv", 1, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vng_core___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vng_core___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../rtl/../rtl/ng_core.sv", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vng_core___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../rtl/../rtl/ng_core.sv", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vng_core___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vng_core___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vng_core___024root___eval_debug_assertions(Vng_core___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vng_core___024root___eval_debug_assertions\n"); );
    Vng_core__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.rst & 0xfeU)))) {
        Verilated::overWidthError("rst");}
}
#endif  // VL_DEBUG
