// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xbg.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XBg_CfgInitialize(XBg *InstancePtr, XBg_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_bus_BaseAddress = ConfigPtr->Control_bus_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XBg_Start(XBg *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL) & 0x80;
    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL, Data | 0x01);
}

u32 XBg_IsDone(XBg *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XBg_IsIdle(XBg *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XBg_IsReady(XBg *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XBg_EnableAutoRestart(XBg *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL, 0x80);
}

void XBg_DisableAutoRestart(XBg *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_AP_CTRL, 0);
}

void XBg_InterruptGlobalEnable(XBg *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_GIE, 1);
}

void XBg_InterruptGlobalDisable(XBg *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_GIE, 0);
}

void XBg_InterruptEnable(XBg *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_IER);
    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_IER, Register | Mask);
}

void XBg_InterruptDisable(XBg *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_IER);
    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_IER, Register & (~Mask));
}

void XBg_InterruptClear(XBg *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBg_WriteReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_ISR, Mask);
}

u32 XBg_InterruptGetEnabled(XBg *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_IER);
}

u32 XBg_InterruptGetStatus(XBg *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XBg_ReadReg(InstancePtr->Control_bus_BaseAddress, XBG_CONTROL_BUS_ADDR_ISR);
}

