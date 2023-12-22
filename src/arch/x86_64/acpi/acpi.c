#include <arch/x86_64/acpi/acpi.h>
#include <libc/printf.h>
#include <mm/pmm.h>

#define MAX(A, B) ({ \
    __auto_type MAX_a = A; \
    __auto_type MAX_b = B; \
    MAX_a > MAX_b ? MAX_a : MAX_b; \
})

// Note: We only check for version 1 (RSDP/RSDT) for now
// Later I can implement XSDP/XSDT

volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

int acpi_strcmp(const char* x, const char* y, int len) {
    for (int i = 0; i < len; i++) {
        if (x[i] != y[i]) return 1;
    }
    return 0;
}

void* acpi_find_madt(void* rsdt_root) {
    acpi_rsdt* rsdt = (acpi_rsdt*)rsdt_root;
    u64 entries = (rsdt->sdt.len - sizeof(rsdt->sdt)) / 4;

    printf("Found %ld RSDT entries.\n", entries);
    acpi_sdt* sdt;

    for (u64 i = 0; i < entries; i++) {
        sdt = (acpi_sdt*)((u64)rsdt->table[i]);
        if (!memcmp(sdt->signature, "APIC", 4)) {
            printf("Found MADT.\n");
            return (void*)sdt;
        }
    }

    printf("Couldn't find MADT!\n");
    return NULL;
}

void acpi_madt_parse(void* madt_addr) {
    acpi_madt* madt = (acpi_madt*)madt_addr;
    u64 off = 0;

    int cpu_count = 0;

    while (true) {
        if (off > madt->len - sizeof(madt)) {
            break;
        }

        acpi_madt_entry* madt_entry = (acpi_madt_entry*)(madt->data + off);
        switch (madt_entry->type) {
            case 0:
                printf("Found CPU %d.\n", cpu_count);
                cpu_count++;
                break;
            case 1:
                printf("Found IOAPIC, address = %x.\n", ((madt_ioapic_entry*)madt_entry)->apic_addr);
                break;
            case 5:
                printf("Found LAPIC, address = %lx.\n", ((madt_lapic_addr_entry*)madt_entry)->phys_lapic);
                break;
        }

        off += madt_entry->len;
    }
}

void acpi_init() {
    acpi_rsdp* rsdp = (acpi_rsdp*)rsdp_request.response->address;
    if (rsdp->revision == 0) {
        printf("ACPI Version 1 is used.\n");
        if (acpi_strcmp(rsdp->signature, "RSD PTR", 7)) {
            printf("Invalid signature! sig: '%s'\n", rsdp->signature);
            return;
        }
        printf("RSDP is valid, located at PHYSICAL ADDR: %x.\n", rsdp->address);
        // Begin parsing RSDT to find MADT
        void* madt = acpi_find_madt((void*)(u64)rsdp->address);
        printf("MADT Located at %lx.\n", (u64)madt);
        acpi_madt_parse(madt);
    } else {
        if (rsdp->revision != 2) {
            printf("Invalid ACPI RSDP!\n");
            return;
        }
        printf("ACPI Version 2 is used. Can't parse that yet! :P\n");
        return;
    }
}