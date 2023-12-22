#pragma once

#include <types.h>
#include <limine.h>

#define LAPIC_SPURIOUS 0x0f0
#define LAPIC_BASE 0xfee00000

void lapic_init();