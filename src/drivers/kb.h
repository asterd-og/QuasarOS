#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>
#include <drivers/kbMap.h>

extern char KB_CurrentChar;

void KB_Init();
char KB_GetChar();