#ifndef _LCD_H_
#define _LCD_H_

#include "main.h" 

void screen_init(uint32_t baudrate);
void screen_send_byte(uint8_t data);
void screen_send_string(const char *str);

#endif 
