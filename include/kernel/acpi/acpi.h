#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <multiboot2.h>
#include <timer/timer.h>
#include <serial.h>

typedef struct rsdp_descriptor {
  char     signature[8]; // "RSD PTR "
  uint8_t  checksum;
  char     oem_id[6];
  uint8_t  revision;
  uint32_t rsdt_address; // Physical address of the RSDT
} __attribute__((packed)) rsdp_descriptor_t;
typedef rsdp_descriptor_t rsdp_t;
// ACPI 2.0+ Extended RSDP Structure
typedef struct rsdp_descriptor_20 {
  rsdp_descriptor_t first_part; // The first 20 bytes

  uint32_t length;        // Size of entire RSDP structure
  uint64_t xsdt_address;  // Physical address of the XSDT (Preferred!)
  uint8_t  extended_checksum;
  uint8_t  reserved[3];
} __attribute__((packed)) rsdp_descriptor_20_t;

typedef struct acpi_sdt_header {
  char     signature[4];
  uint32_t length;
  uint8_t  revision;
  uint8_t  checksum;
  char     oem_id[6];
  char     oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

typedef struct {
  acpi_sdt_header_t header; // "FACP"

  uint32_t firmware_ctrl;    // Physical address of FACS
  uint32_t dsdt;             // Physical address of DSDT

  uint8_t  reserved;         // reserved (was int_model in ACPI 1.0)
  uint8_t  preferred_pm_profile;
  uint16_t sci_int;
  uint32_t smi_cmd;
  uint8_t  acpi_enable;
  uint8_t  acpi_disable;
  uint8_t  s4bios_req;
  uint8_t  pstate_cnt;

  uint32_t pm1a_evt_blk;
  uint32_t pm1b_evt_blk;
  uint32_t pm1a_cnt_blk;
  uint32_t pm1b_cnt_blk;
  uint32_t pm2_cnt_blk;
  uint32_t pm_tmr_blk;
  uint32_t gpe0_blk;
  uint32_t gpe1_blk;

  uint8_t  pm1_evt_len;
  uint8_t  pm1_cnt_len;
  uint8_t  pm2_cnt_len;
  uint8_t  pm_tmr_len;
  uint8_t  gpe0_blk_len;
  uint8_t  gpe1_blk_len;
  uint8_t  gpe1_base;
  uint8_t  cst_cnt;

  uint16_t p_lvl2_lat;
  uint16_t p_lvl3_lat;
  uint16_t flush_size;
  uint16_t flush_stride;
  uint8_t  duty_offset;
  uint8_t  duty_width;
  uint8_t  day_alarm;
  uint8_t  mon_alarm;
  uint8_t  century;

  uint16_t iapc_boot_arch;
  uint8_t  reserved2;
  uint32_t flags;

  // 64-bit addresses for ACPI 2.0+
  struct {
    uint8_t  address_space_id;
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  access_size;
    uint64_t address;
  } __attribute__((packed)) reset_reg;

  uint8_t  reset_value;
  uint8_t  reserved3[3];

  uint64_t x_firmware_ctrl;
  uint64_t x_dsdt;

  // Extended power management blocks
  struct {
    uint8_t  address_space_id;
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  access_size;
    uint64_t address;
  } __attribute__((packed)) x_pm1a_evt_blk,
    x_pm1b_evt_blk,
    x_pm1a_cnt_blk,
    x_pm1b_cnt_blk,
    x_pm2_cnt_blk,
    x_pm_tmr_blk,
    x_gpe0_blk,
    x_gpe1_blk;

  // ... ACPI 6.0 has even more fields ...
} __attribute__((packed)) fadt_t;
typedef struct {
  acpi_sdt_header_t header; // signature = "RSDT"
  // Flexible array member for the variable number of table pointers
  uint32_t table_ptrs[];    // Array of physical addresses to other ACPI SDTs
} __attribute__((packed)) rsdt_t;
// Extended System Description Table (64-bit pointers)
typedef struct {
  acpi_sdt_header_t header; // signature = "XSDT"
  // Flexible array member for the variable number of table pointers
  uint64_t table_ptrs[];    // Array of physical addresses to other ACPI SDTs
} __attribute__((packed)) xsdt_t;

// You would define a struct like this to hold the results:
typedef struct {
  uint16_t smi_cmd;
  uint8_t  acpi_enable;
  uint16_t pm1a_cnt;
} pm_regs_t;

uint8_t acpi_checksum(void* data, size_t length);

int validate_rsdp(rsdp_t* rsdp);

acpi_sdt_header_t* find_root_table(rsdp_t* rsdp);

fadt_t* find_fadt(acpi_sdt_header_t* root_table);

void extract_pm_registers(fadt_t* fadt /*, pm_regs_t* regs */);

void acpi_shutdown(/*pm_regs_t* regs*/);

void acpi_1_0_init(struct multiboot_tag_old_acpi *old_acpi_tag);

void acpi_2_0plus_init(struct multiboot_tag_new_acpi *new_acpi_tag);

#endif
