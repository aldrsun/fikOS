#ifndef SERIAL_H
#define SERIAL_H

void serial_init();

// TODO : this is a mess
void serial_print_string(char *str);
void serial_print_byte(char c);
void serial_print_hex_64(unsigned long long value);
void serial_print_hex(unsigned int value);
void serial_print_dec(unsigned int value);

#endif
