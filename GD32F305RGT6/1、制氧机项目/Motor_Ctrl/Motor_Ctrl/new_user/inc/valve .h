#ifndef VALVE_H
#define VALVE_H

#include "main.h"

/* 定义阀门状态标签 */
#define VALVE_FORWARD   0x01   // 正向
#define VALVE_REVERSE   0x02   // 反向
#define VALVE_STOP      0x00   // 停止

/* 定义阀门编号 */
#define VALVE1          1      // 阀门1
#define VALVE2          2      // 阀门2


/* 阀门编号标签 */
#define BALANCE_VALVE1   1   // 平衡阀门1
#define BALANCE_VALVE2   2   // 平衡阀门2 



void valve_config(void);
void xxod(uint8_t valve_id, uint8_t state);
void single(uint8_t valve_id, uint8_t state);
void release(uint8_t state);
void valve_monitor(void);
void valve_test(void);

#endif
