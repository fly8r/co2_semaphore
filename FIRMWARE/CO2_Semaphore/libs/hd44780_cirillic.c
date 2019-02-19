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
		case 'À': chr = 'A'; break;
		case 'à': chr = 'a'; break;
		case 'Á': chr = 0xA0; break;
		case 'á': chr = 0xB2; break;
		case 'Â': chr = 'B'; break;
		case 'â': chr = 0xB3; break;
		case 'Ã': chr = 0xA1; break;
		case 'ã': chr = 0xB4; break;
		case 'Ä': chr = 0xE0; break;
		case 'ä': chr = 0xE3; break;
		case 'Å': chr = 'E'; break;
		case 'å': chr = 'e'; break;
		case '¨': chr = 0xA2; break;
		case '¸': chr = 0xB5; break;
		case 'Æ': chr = 0xA3; break;
		case 'æ': chr = 0xB6; break;
		case 'Ç': chr = 0x33; break;
		case 'ç': chr = 0xB7; break;
		case 'È': chr = 0xA5; break;
		case 'è': chr = 0xB8; break;
		case 'É': chr = 0xA6; break;
		case 'é': chr = 0xB9; break;
		case 'Ê': chr = 'K'; break;
		case 'ê': chr = 0xBA; break;
		case 'Ë': chr = 0xA7; break;
		case 'ë': chr = 0xBB; break;
		case 'Ì': chr = 'M'; break;
		case 'ì': chr = 0xBC; break;
		case 'Í': chr = 'H'; break;
		case 'í': chr = 0xBD; break;
		case 'Î': chr = 'O'; break;
		case 'î': chr = 'o'; break;
		case 'Ï': chr = 0xA8; break;
		case 'ï': chr = 0xBE; break;
		case 'Ð': chr = 'P'; break;
		case 'ð': chr = 'p'; break;
		case 'Ñ': chr = 'C'; break;
		case 'ñ': chr = 'c'; break;
		case 'Ò': chr = 'T'; break;
		case 'ò': chr = 0xBF; break;
		case 'Ó': chr = 0xA9; break;
		case 'ó': chr = 'y'; break;
		case 'Ô': chr = 0xAA; break;
		case 'ô': chr = 0xE4; break;
		case 'Õ': chr = 'X'; break;
		case 'õ': chr = 'x'; break;
		case 'Ö': chr = 0xE1; break;
		case 'ö': chr = 0xE5; break;
		case '×': chr = 0xAB; break;
		case '÷': chr = 0xC0; break;
		case 'Ø': chr = 0xAC; break;
		case 'ø': chr = 0xC1; break;
		case 'Ù': chr = 0xE2; break;
		case 'ù': chr = 0xE6; break;
		case 'Ú': chr = 0xAD; break;
		case 'ú': chr = 0xC2; break;
		case 'Û': chr = 0xAE; break;
		case 'û': chr = 0xC3; break;
		case 'Ü': chr = 0xC4; break;
		case 'ü': chr = 0xC4; break;
		case 'Ý': chr = 0xAF; break;
		case 'ý': chr = 0xC5; break;
		case 'Þ': chr = 0xB0; break;
		case 'þ': chr = 0xC6; break;
		case 'ß': chr = 0xB1; break;
		case 'ÿ': chr = 0xC7; break;
		default: break;
	}
	return chr;
}
