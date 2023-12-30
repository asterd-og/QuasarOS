#pragma once

// File operations (read for now)

#include <types.h>
#include <initrd/quasfs.h>

int file_read(char* path, char* buf);