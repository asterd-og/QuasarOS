#pragma once

#include <types.h>
#include <limine.h>

typedef struct {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 address;
} acpi_rsdp;

typedef struct {
    char signature[4];
    u32 len;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[6];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} acpi_sdt;

typedef struct {
    acpi_sdt sdt;
    u32 table[];
} acpi_rsdt;

// MADT STRUCTURES

typedef struct {
    /* Same as the sdt */
    char signature[4];
    u32 len;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[6];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;

    /* MADT Specs */
    u32 lapic_address;
    u32 flags;

    char data[];
} acpi_madt;

typedef struct {
    u8 type;
    u8 len;
} acpi_madt_entry;

typedef struct {
    u8 cpu_id;
    u8 apic_id;
    u32 flags;
} madt_cpu_lapic_entry;

typedef struct {
    u8 apic_id;
    u8 resv;
    u32 apic_addr;
    u32 sys_int_base;
} madt_ioapic_entry;

typedef struct {
    u16 resv;
    u64 phys_lapic;
} madt_lapic_addr_entry;

void acpi_init();