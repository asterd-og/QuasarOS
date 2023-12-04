#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>

#define PIT_BaseFreq 100

void PIT_Init();