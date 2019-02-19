/*
 * hd44780_cc.h
 *
 * Created: 23.01.2019 12:30:25
 *  Author: fly8r
 */
#ifndef HD44780_CC_H_
#define HD44780_CC_H_

#include <stdio.h>
#include <avr/pgmspace.h>

#include "../../fsms/include/lcd.h"

extern const	char
		cc_smb1[8],
		cc_smb2[8],
		cc_smb3[8],
		cc_smb4[8],
		cc_smb5[8],
		cc_smb6[8],
		cc_smb7[8],
		cc_smb8[8];

extern const	char
		cc_char_0[],
		cc_char_1[],
		cc_char_2[],
		cc_char_3[],
		cc_char_4[],
		cc_char_5[],
		cc_char_6[],
		cc_char_7[],
		cc_char_8[],
		cc_char_9[],
		cc_char_a[],
		cc_char_b[],
		cc_char_c[],
		cc_char_d[],
		cc_char_e[],
		cc_char_f[],
		cc_char_colon[],
		cc_char_space[];

#endif /* HD44780_CC_H_ */
