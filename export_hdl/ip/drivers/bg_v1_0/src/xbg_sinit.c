// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.2.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xbg.h"

extern XBg_Config XBg_ConfigTable[];

XBg_Config *XBg_LookupConfig(u16 DeviceId) {
	XBg_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XBG_NUM_INSTANCES; Index++) {
		if (XBg_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XBg_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XBg_Initialize(XBg *InstancePtr, u16 DeviceId) {
	XBg_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XBg_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XBg_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

