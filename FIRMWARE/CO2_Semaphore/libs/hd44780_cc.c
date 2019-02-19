/*
 * hd44780_cc.c
 *
 * Created: 23.01.2019 12:31:00
 *  Author: fly8r
 */
#include "include/hd44780_cc.h"


#if (LCD_SUPPORT_CUSTOM_CHARS_SIZE == 2)
const	char	cc_smb1[8]		PROGMEM = {
	0b00000111,					//   ***
	0b00001111,					//  ****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};

const	char	cc_smb2[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00011111,					// *****
	0b00011111					// *****
};

const	char	cc_smb3[8]		PROGMEM = {
	0b00011100,					// ***
	0b00011110,					// ****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};

const	char	cc_smb4[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};
const	char	cc_smb5[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00001111,					//  ****
	0b00000111					//   ***
};
const	char	cc_smb6[8]		PROGMEM = {
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};
const	char	cc_smb7[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011110,					// ****
	0b00011100					// ***
};
const	char	cc_smb8[8]		PROGMEM = {
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};

//------------------------------ CHAR CONSTRUCTIONS
//------------------------------------[ ccX ccX ccX& ccX ccX ccX& ccX ccX ccX$ ]
const	char	cc_char_0[]			PROGMEM = "\x00\x03\x02&\x04\x07\x06$";
const	char	cc_char_1[]			PROGMEM = "\x03\x02\x20&\x20\xFF\x20$";
const	char	cc_char_2[]			PROGMEM = "\x01\x01\x02&\x04\x05\x05$";
const	char	cc_char_3[]			PROGMEM = "\x01\x01\x02&\x05\x05\x06$";
const	char	cc_char_4[]			PROGMEM = "\x00\x07\x02&\x20\x20\x06$";
const	char	cc_char_5[]			PROGMEM = "\x00\x01\x01&\x05\x05\x06$";
const	char	cc_char_6[]			PROGMEM = "\x00\x01\x01&\x04\x05\x06$";
const	char	cc_char_7[]			PROGMEM = "\x03\x01\x02&\x20\x06\x20$";
const	char	cc_char_8[]			PROGMEM = "\x00\x01\x02&\x04\x05\x06$";
const	char	cc_char_9[]			PROGMEM = "\x00\x01\x02&\x05\x05\x06$";
const	char	cc_char_a[]			PROGMEM = "\x00\x01\x02&\x04\x03\x06$";
const	char	cc_char_b[]			PROGMEM = "\x02\x01\x02&\x06\x05\x06$";
const	char	cc_char_c[]			PROGMEM = "\x00\x03\x03&\x04\x07\x07$";
const	char	cc_char_d[]			PROGMEM = "\x02\x03\x02&\x06\x20\x06$";
const	char	cc_char_e[]			PROGMEM = "\x00\x01\x01&\x04\x05\x05$";
const	char	cc_char_f[]			PROGMEM = "\x00\x01\x03&\x04\x03\x20$";
const	char	cc_char_colon[]		PROGMEM = "\x20\x07\x20&\x20\x07\x20$";
const	char	cc_char_space[]		PROGMEM = "\x20\x20\x20&\x20\x20\x20$";


#elif (LCD_SUPPORT_CUSTOM_CHARS_SIZE == 3)
const	char	cc_smb1[8]		PROGMEM = {
	0b00000011,					//    **
	0b00001111,					//	****
	0b00001111,					//  ****
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};

const	char	cc_smb2[8]		PROGMEM = {
	0b00011000,					// **
	0b00011110,					// ****
	0b00011110,					// ****
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};

const	char	cc_smb3[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};

const	char	cc_smb4[8]		PROGMEM = {
	0b00011111,					// *****
	0b00011110,					// ****
	0b00011110,					// ****
	0b00011000,					// **
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};
const	char	cc_smb5[8]		PROGMEM = {
	0b00011111,					// *****
	0b00001111,					//	****
	0b00001111,					//  ****
	0b00000011,					//    **
	0b00000000,					//
	0b00000000,					//
	0b00000000,					//
	0b00000000					//
};
const	char	cc_smb6[8]		PROGMEM = {
	0b00000011,					//    **
	0b00001111,					//	****
	0b00001111,					//  ****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};
const	char	cc_smb7[8]		PROGMEM = {
	0b00011000,					// **
	0b00011110,					// ****
	0b00011110,					// ****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011111					// *****
};
const	char	cc_smb8[8]		PROGMEM = {
	0b00011000,					// **
	0b00011100,					// ***
	0b00011110,					// ****
	0b00011111,					// *****
	0b00011111,					// *****
	0b00011110,					// ****
	0b00011100,					// ***
	0b00011000					// **
};

//------------------------------ CHAR CONSTRUCTIONS
//------------------------------------[ ccX ccX ccX& ccX ccX ccX& ccX ccX ccX$ ]
const	char	cc_char_0[]			PROGMEM = "\x05\x02\x06&\xFF\x20\xFF&\x04\x02\x03$";
const	char	cc_char_1[]			PROGMEM = "\x00\xFF\xFE&\x20\xFF\x20&\x04\x02\x03$";
const	char	cc_char_2[]			PROGMEM = "\x00\x02\x06&\x05\x02\x03&\x04\x02\x03$";
const	char	cc_char_3[]			PROGMEM = "\x00\x02\x06&\x20\x02\x06&\x04\x02\x03$";
const	char	cc_char_4[]			PROGMEM = "\x05\x20\x06&\x04\x02\xFF&\x20\x20\x03$";
const	char	cc_char_5[]			PROGMEM = "\x05\x02\x01&\x04\x02\x06&\x04\x02\x03$";
const	char	cc_char_6[]			PROGMEM = "\x05\x02\x01&\xFF\x02\x06&\x04\x02\x03$";
const	char	cc_char_7[]			PROGMEM = "\x00\x02\x06&\x20\x05\x03&\x20\x04\x20$";
const	char	cc_char_8[]			PROGMEM = "\x05\x02\x06&\xFF\x02\xFF&\x04\x02\x03$";
const	char	cc_char_9[]			PROGMEM = "\x05\x02\x06&\x04\x02\xFF&\x04\x02\x03$";
const	char	cc_char_a[]			PROGMEM = "\x05\x02\x06&\xFF\x02\xFF&\x04\x20\x03$";
const	char	cc_char_b[]			PROGMEM = "\x05\x02\x06&\xFF\x02\x07&\x04\x02\x03$";
const	char	cc_char_c[]			PROGMEM = "\x05\x02\x01&\xFF\x20\x20&\x04\x02\x03$";
const	char	cc_char_d[]			PROGMEM = "\x06\x02\x06&\xFF\x20\xFF&\x03\x02\x03$";
const	char	cc_char_e[]			PROGMEM = "\x05\x02\x01&\xFF\x02\x20&\x04\x02\x03$";
const	char	cc_char_f[]			PROGMEM = "\x05\x02\x01&\xFF\x02\x20&\x04\x20\x20$";
const	char	cc_char_colon[]		PROGMEM = "\x20\x20\x20&\x20\x02\x20&\x20\x02\x20$";
const	char	cc_char_space[]		PROGMEM = "\x20\x20\x20&\x20\x20\x20&\x20\x20\x20$";

#endif
