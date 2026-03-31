
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "main.h"

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);

#endif 
