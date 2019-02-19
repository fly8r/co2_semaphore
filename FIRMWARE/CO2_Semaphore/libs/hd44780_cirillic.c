/*
 * hd44780_cirillic.c
 *
 * Created: 23.01.2019 11:58:23
 *  Author: fly8r
 */
#include "include/hd44780_cirillic.h"

char HD44780_CirillicTable(char chr)
{
	switch(chr) {
		case '�': chr = 'A'; break;
		case '�': chr = 'a'; break;
		case '�': chr = 0xA0; break;
		case '�': chr = 0xB2; break;
		case '�': chr = 'B'; break;
		case '�': chr = 0xB3; break;
		case '�': chr = 0xA1; break;
		case '�': chr = 0xB4; break;
		case '�': chr = 0xE0; break;
		case '�': chr = 0xE3; break;
		case '�': chr = 'E'; break;
		case '�': chr = 'e'; break;
		case '�': chr = 0xA2; break;
		case '�': chr = 0xB5; break;
		case '�': chr = 0xA3; break;
		case '�': chr = 0xB6; break;
		case '�': chr = 0x33; break;
		case '�': chr = 0xB7; break;
		case '�': chr = 0xA5; break;
		case '�': chr = 0xB8; break;
		case '�': chr = 0xA6; break;
		case '�': chr = 0xB9; break;
		case '�': chr = 'K'; break;
		case '�': chr = 0xBA; break;
		case '�': chr = 0xA7; break;
		case '�': chr = 0xBB; break;
		case '�': chr = 'M'; break;
		case '�': chr = 0xBC; break;
		case '�': chr = 'H'; break;
		case '�': chr = 0xBD; break;
		case '�': chr = 'O'; break;
		case '�': chr = 'o'; break;
		case '�': chr = 0xA8; break;
		case '�': chr = 0xBE; break;
		case '�': chr = 'P'; break;
		case '�': chr = 'p'; break;
		case '�': chr = 'C'; break;
		case '�': chr = 'c'; break;
		case '�': chr = 'T'; break;
		case '�': chr = 0xBF; break;
		case '�': chr = 0xA9; break;
		case '�': chr = 'y'; break;
		case '�': chr = 0xAA; break;
		case '�': chr = 0xE4; break;
		case '�': chr = 'X'; break;
		case '�': chr = 'x'; break;
		case '�': chr = 0xE1; break;
		case '�': chr = 0xE5; break;
		case '�': chr = 0xAB; break;
		case '�': chr = 0xC0; break;
		case '�': chr = 0xAC; break;
		case '�': chr = 0xC1; break;
		case '�': chr = 0xE2; break;
		case '�': chr = 0xE6; break;
		case '�': chr = 0xAD; break;
		case '�': chr = 0xC2; break;
		case '�': chr = 0xAE; break;
		case '�': chr = 0xC3; break;
		case '�': chr = 0xC4; break;
		case '�': chr = 0xC4; break;
		case '�': chr = 0xAF; break;
		case '�': chr = 0xC5; break;
		case '�': chr = 0xB0; break;
		case '�': chr = 0xC6; break;
		case '�': chr = 0xB1; break;
		case '�': chr = 0xC7; break;
		default: break;
	}
	return chr;
}
