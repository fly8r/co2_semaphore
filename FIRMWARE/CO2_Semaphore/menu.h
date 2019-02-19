/*
 * menu.h
 *
 * Created: 04.02.2019 15:16:08
 *  Author: fly8r
 */
#ifndef MENU_H_
#define MENU_H_

#include <stdio.h>
#include <avr/pgmspace.h>
#include "global_vars.h"
#include "strings.h"


/************************************************************************/
/* External Functions                                                   */
/************************************************************************/
void MENU_Change(menu_item_t *new_menu);
char *MENU_GetMenuTextByShift(int8_t shift);
char *MENU_GetChildMenuSymbolByShift(int8_t shift);
void MENU_Start(void);

#endif /* MENU_H_ */