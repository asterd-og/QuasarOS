#pragma once

#include <types.h>
#include <arch/x86_64/tables/idt/idt.h>
#include <arch/x86_64/io.h>
#include <drivers/kb_map.h>

void kb_init();
char kb_get_char();