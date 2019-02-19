/*
 * tools.h
 *
 * Created: 25.01.2019 15:55:34
 *  Author: fly8r
 */
#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdio.h>
#include <avr/pgmspace.h>

/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
uint8_t crc8(uint8_t crc, uint8_t data);
char * hex_to_ascii(uint8_t number, char * buffer);
char * hex_to_ascii16(uint16_t number, char * buffer);
char * utoa_cycle_sub8(uint8_t value, char *buffer, uint8_t _trim_zero, uint8_t max_value_length);
char * utoa_cycle_sub16(uint16_t value, char *buffer, uint8_t max_value_length);
uint8_t bcd2dec(uint8_t val);
uint8_t dec2bcd(uint8_t val);

#endif /* TOOLS_H_ */