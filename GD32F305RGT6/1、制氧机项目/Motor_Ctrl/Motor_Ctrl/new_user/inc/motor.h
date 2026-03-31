#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "gd32f30x.h"
#include "main.h"

void motor_init(void);
void motor_set(uint16_t duty);
void motor_set_direction(uint8_t dir);
void motor_change_direction(uint8_t dir);

void motor_test(void);

#endif      
