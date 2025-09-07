#include <timer/timer.h>
#include <acpi/acpi.h>
#include <string.h>
#include <io.h>
#include <frame_buffer.h>
#include <multiboot2.h>
#include <serial.h>

pm_regs_t registers;

void acpi_1_0_init(struct multiboot_tag_old_acpi *old_acpi_tag) {
  uint8_t *rsdp = old_acpi_tag->rsdp;
  validate_rsdp((rsdp_t*)rsdp);
  acpi_sdt_header_t *root_table = find_root_table((rsdp_t*)rsdp);
  fadt_t *fadt_table = find_fadt(root_table);
  extract_pm_registers(fadt_table);
}

void acpi_2_0plus_init(struct multiboot_tag_new_acpi *new_acpi_tag) {
  uint8_t *rsdp = new_acpi_tag->rsdp;
  validate_rsdp((rsdp_t*)rsdp);
  acpi_sdt_header_t *root_table = find_root_table((rsdp_t*)rsdp);
  fadt_t *fadt_table = find_fadt(root_table);
  extract_pm_registers(fadt_table);
}

int validate_rsdp(rsdp_t* rsdp) {
  if (strncmp(rsdp->signature, "RSD PTR ", 8) != 0) {
    //serial_write_string(0x3F8, "ACPI: Invalid RSDP signature\n");
    return 0;
  }

  if (acpi_checksum(rsdp, sizeof(rsdp_descriptor_t)) != 0) {
    //serial_write_string(0x3F8, "ACPI: RSDP v1.0 checksum invalid\n");
    return 0;
  }

  if (rsdp->revision >= 2) {
    rsdp_descriptor_20_t* rsdp20 = (rsdp_descriptor_20_t*)rsdp;
    if (acpi_checksum(rsdp20, rsdp20->length) != 0) {
      //serial_write_string(0x3F8, "ACPI: RSDP v2.0+ checksum invalid\n");
      return 0;
    }
  }
  //serial_write_string(0x3F8, "ACPI: RSDP validated successfully\n");
  return 1;
}


acpi_sdt_header_t* find_root_table(rsdp_t* rsdp) {
  void* root_table_phys_addr = NULL;
  char* expected_signature;

  if (rsdp->revision >= 2) {
    // Use the 64-bit XSDT address from ACPI 2.0+
    rsdp_descriptor_20_t* rsdp20 = (rsdp_descriptor_20_t*)rsdp;
    root_table_phys_addr = (void*)(unsigned int)rsdp20->xsdt_address;
    expected_signature = "XSDT";
  } else {
    // Use the 32-bit RSDT address from ACPI 1.0
    root_table_phys_addr = (void*)(unsigned int)rsdp->rsdt_address;
    expected_signature = "RSDT";
  }

  //serial_write_string(0x3F8,"ACPI: Root table at phys 0x%p, expecting %s\n");//, root_table_phys_addr, expected_signature);

  // 1. MAP the physical address to a virtual address your kernel can access.
  // This function is specific to your OS's memory management!
  acpi_sdt_header_t* root_table_header = (acpi_sdt_header_t*)root_table_phys_addr;

  // 2. Validate the root table header
  if (strncmp(root_table_header->signature, expected_signature, 4) != 0) {
    //serial_write_string(0x3F8, "ACPI: Root table signature mismatch\n");
    return (acpi_sdt_header_t*)NULL;
  }
  if (acpi_checksum(root_table_header, root_table_header->length) != 0) {
    //serial_write_string(0x3F8,"ACPI: Root table checksum invalid\n");
    return (acpi_sdt_header_t*)NULL;
  }
  //serial_write_string(0x3F8, "ACPI: Found valid %s\n");//, expected_signature);
  return root_table_header;
}

fadt_t* find_fadt(acpi_sdt_header_t* root_table) {
  // Determine if the root table is an RSDT or XSDT and calculate the number of entries.
  size_t num_entries;
  void** table_ptrs; // A generic pointer to the array of pointers

  if (strncmp(root_table->signature, "XSDT", 4) == 0) {
    num_entries = (root_table->length - sizeof(acpi_sdt_header_t)) / sizeof(uint64_t);
    uint64_t* entries = (uint64_t *)((uintptr_t)root_table + sizeof(acpi_sdt_header_t));
    table_ptrs = (void**)entries;
  } else { // "RSDT"
    num_entries = (root_table->length - sizeof(acpi_sdt_header_t)) / sizeof(uint32_t);
    uint32_t* entries = (uint32_t *)((uintptr_t)root_table + sizeof(acpi_sdt_header_t));
    table_ptrs = (void**)entries;
  }

  //serial_write_string(0x3F8, "ACPI: Searching %d entries for FADT...\n");//, num_entries);

  for (size_t i = 0; i < num_entries; i++) {
    // 1. Get the physical address of the next SDT from the array
    // The cast depends on whether we're using XSDT or RSDT!
    uintptr_t table_phys_addr;
    if (strncmp(root_table->signature, "XSDT", 4) == 0) {
      table_phys_addr = (uintptr_t)((uint64_t*)table_ptrs)[i];
    } else {
      table_phys_addr = (uintptr_t)((uint32_t*)table_ptrs)[i];
    }

    // 2. Map the physical address to virtual and get the header
    acpi_sdt_header_t* header = (acpi_sdt_header_t*)table_phys_addr;

    // 3. Check if this is the FADT (signature "FACP")
    if (strncmp(header->signature, "FACP", 4) == 0) {
      // 4. Validate the FADT before trusting it!
      if (acpi_checksum(header, header->length) == 0) {
	//serial_write_string(0x3F8, "ACPI: Found valid FADT\n");
	return (fadt_t*)header; // Safe to cast now
      } else {
	//serial_write_string(0x3F8, "ACPI: Found FADT but checksum invalid\n");
      }
    }
  }
  //serial_write_string(0x3F8, "ACPI: FADT not found in root table\n");
  return (fadt_t*)NULL;
}


void extract_pm_registers(fadt_t* fadt) {
  registers.smi_cmd = fadt->smi_cmd;
  registers.acpi_enable = fadt->acpi_enable;

  // GET THE PM1a CONTROL REGISTER PORT
  // 1. Check the 64-bit Extended Address first (for ACPI 2.0+)
  if (fadt->header.revision >= 2 && fadt->x_pm1a_cnt_blk.address != 0) {
    // Use the 64-bit address. Truncate to 16-bit port number.
    registers.pm1a_cnt = (uint16_t)fadt->x_pm1a_cnt_blk.address;
    //serial_write_string(0x3F8, "ACPI: Using 64-bit X_PM1a_CNT_BLK: 0x%x\n");
    //serial_print_hex(registers.pm1a_cnt);
  }
  // 2. If the 64-bit address wasn't available, use the 32-bit address.
  else if (fadt->pm1a_cnt_blk != 0) {
    registers.pm1a_cnt = (uint16_t)fadt->pm1a_cnt_blk;
    //serial_write_string(0x3F8, "ACPI: Using 32-bit PM1a_CNT_BLK: 0x%x\n");
    //serial_print_hex(registers.pm1a_cnt);
  }
  else {
    //serial_write_string(0x3F8, "ACPI: ERROR! No PM1a control register block found in FADT!\n");
    registers.pm1a_cnt = 0;
    return;
  }

  // Sanity check
  if (fadt->pm1_cnt_len != 2) {
    //serial_write_string(0x3F8, "ACPI: WARNING! PM1_CNT_LEN is %d, expected 2.\n");
    //serial_print_dec(fadt->pm1_cnt_len);
  }
}

#include <serial.h>
uint8_t acpi_shutdown_hook = 0;
void acpi_shutdown() {
  __asm__ volatile("cli");
  if (acpi_shutdown_hook) {
    return;
  }
  acpi_shutdown_hook = 1;
  __asm__ volatile("sti");
  //serial_write_string(0x3F8, "ACPI: Attempting shutdown...\n");

  // 1. Check if ACPI is already enabled. If not, enable it.
  // (This is a simplification. You should really check the SCI_EN bit in the PM1_CNT register)
  if (registers.smi_cmd != 0 && registers.acpi_enable != 0) {
    ////fb_write("enabling acpi\n");
    serial_print_string("ACPI: Enabling ACPI via SMI_CMD 0x%x\n");
    serial_print_dec(registers.smi_cmd);
    outb(registers.smi_cmd, registers.acpi_enable);
    fsleep(50); // Wait ~10ms
  }

  uint16_t shutdown_value = (6 << 10) | (1 << 13); // (SLP_TYP=5) + SLP_EN
  uint16_t shutdown_value2 = (1 << 13); // SLP_EN
  //serial_write_string(0x3F8, "ACPI: Writing 0x%x to PM1a_CNT 0x%x\n");
  serial_print_hex(shutdown_value);
  serial_print_string("\n");
  serial_print_hex(registers.pm1a_cnt);
  serial_print_string("\n");
  //fb_write("Shutdown value: "); fb_print_hex(shutdown_value); //fb_write("\n");
  //fb_write("Address: "); fb_print_hex(regs->pm1a_cnt); //fb_write("\n");

  outw(registers.pm1a_cnt, shutdown_value);
  fsleep(100);
  serial_print_string("First attempt failed\n");
  fsleep(100);
  outw(registers.pm1a_cnt, shutdown_value2);
  fsleep(200);
  //fb_write("shutdown failed\n");
  serial_print_string("ACPI: Shutdown command failed. Hanging.\n");
}

uint8_t acpi_checksum(void* data, size_t length) {
  uint8_t sum = 0;
  uint8_t* ptr = (uint8_t*)data;
  for (size_t i = 0; i < length; i++) {
    sum += ptr[i];
  }
  return sum; // If valid, this will be 0.
}
