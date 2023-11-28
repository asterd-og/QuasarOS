#pragma once

#include <types.h>

#define COM1 0x3F8

int Serial_Init();
void Serial_Send(char* pStr);
void Serial_Printf(char* pStr, ...);