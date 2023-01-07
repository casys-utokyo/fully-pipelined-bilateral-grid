// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XBG_H
#define XBG_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xbg_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_bus_BaseAddress;
} XBg_Config;
#endif

typedef struct {
    u32 Control_bus_BaseAddress;
    u32 IsReady;
} XBg;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XBg_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XBg_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XBg_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XBg_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XBg_Initialize(XBg *InstancePtr, u16 DeviceId);
XBg_Config* XBg_LookupConfig(u16 DeviceId);
int XBg_CfgInitialize(XBg *InstancePtr, XBg_Config *ConfigPtr);
#else
int XBg_Initialize(XBg *InstancePtr, const char* InstanceName);
int XBg_Release(XBg *InstancePtr);
#endif

void XBg_Start(XBg *InstancePtr);
u32 XBg_IsDone(XBg *InstancePtr);
u32 XBg_IsIdle(XBg *InstancePtr);
u32 XBg_IsReady(XBg *InstancePtr);
void XBg_EnableAutoRestart(XBg *InstancePtr);
void XBg_DisableAutoRestart(XBg *InstancePtr);


void XBg_InterruptGlobalEnable(XBg *InstancePtr);
void XBg_InterruptGlobalDisable(XBg *InstancePtr);
void XBg_InterruptEnable(XBg *InstancePtr, u32 Mask);
void XBg_InterruptDisable(XBg *InstancePtr, u32 Mask);
void XBg_InterruptClear(XBg *InstancePtr, u32 Mask);
u32 XBg_InterruptGetEnabled(XBg *InstancePtr);
u32 XBg_InterruptGetStatus(XBg *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
