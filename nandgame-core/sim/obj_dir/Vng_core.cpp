// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vng_core__pch.h"

//============================================================
// Constructors

Vng_core::Vng_core(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vng_core__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , rst{vlSymsp->TOP.rst}
    , instruction{vlSymsp->TOP.instruction}
    , addr{vlSymsp->TOP.addr}
    , reflect{vlSymsp->TOP.reflect}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vng_core::Vng_core(const char* _vcname__)
    : Vng_core(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vng_core::~Vng_core() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vng_core___024root___eval_debug_assertions(Vng_core___024root* vlSelf);
#endif  // VL_DEBUG
void Vng_core___024root___eval_static(Vng_core___024root* vlSelf);
void Vng_core___024root___eval_initial(Vng_core___024root* vlSelf);
void Vng_core___024root___eval_settle(Vng_core___024root* vlSelf);
void Vng_core___024root___eval(Vng_core___024root* vlSelf);

void Vng_core::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vng_core::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vng_core___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vng_core___024root___eval_static(&(vlSymsp->TOP));
        Vng_core___024root___eval_initial(&(vlSymsp->TOP));
        Vng_core___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vng_core___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vng_core::eventsPending() { return false; }

uint64_t Vng_core::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vng_core::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vng_core___024root___eval_final(Vng_core___024root* vlSelf);

VL_ATTR_COLD void Vng_core::final() {
    Vng_core___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vng_core::hierName() const { return vlSymsp->name(); }
const char* Vng_core::modelName() const { return "Vng_core"; }
unsigned Vng_core::threads() const { return 1; }
void Vng_core::prepareClone() const { contextp()->prepareClone(); }
void Vng_core::atClone() const {
    contextp()->threadPoolpOnClone();
}
