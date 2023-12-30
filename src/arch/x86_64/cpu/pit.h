#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>

#define pit_base_freq 100

extern u64 pit_tick;

void pit_sleep(u64 ms);
void pit_init();