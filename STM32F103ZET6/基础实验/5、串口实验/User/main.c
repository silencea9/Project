/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-19
 * @brief       串口实验 主程序
 * @license     Copyright (c) 2020-2032, 正点原子团队版权所有
 ****************************************************************************************************
 * @attention
 *
 * 实验说明:串口收发实验 STM32F103基础实验
 * 技术论坛:www.yuanzige.com
 * 资料下载:www.openedv.com
 * 视频教程:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "stdio.h"

#define BUFF_SIZE 256
uint8_t led1_state = 0;

int main(void)
{
    uint8_t t;               /* 串口数据发送循环计数器 */
    uint8_t len;             /* 数据长度 */
    uint16_t times = 0;      /* 定时计数器 */
    int my_buff[BUFF_SIZE]; /* 数据缓冲区 */

    sys_stm32_clock_init(9); /* 初始化系统时钟, 72Mhz */  
    delay_init(72);          /* 初始化延迟函数 */
    usart_init(72, 115200);    /* 初始化串口，波特率9600 */
    led_init();              /* 初始化LED */

    while (1)
    {
        if (g_usart_rx_sta & 0x8000) /* 判断是否接收完一组数据 */
        {
            len = g_usart_rx_sta & 0x3fff; /* 获取接收数据的长度,除掉已经装满一个字的最高位 */
            printf("\r\n接收到的数据为:\r\n");

            /* 判断接收到的数据是否为 "01" */
            led1_state = !led1_state;
            LED1(led1_state);

            for (t = 0; t < len; t++)
            {
                USART_UX->DR = g_usart_rx_buf[t];

                while ((USART_UX->SR & 0X40) == 0)
                    ; /* 等待发送完成 */
            }

            printf("\r\n\r\n"); /* 打印换行 */
            g_usart_rx_sta = 0;
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\r\n欢迎进入 STM32串口移植实验 测试程衆\r\n");
                printf("版权所有@ALIENTEK\r\n\r\n\r\n");
            }

            if (times % 200 == 0)
                printf("请输入数据,串口收发中...\r\n");

            if (times % 30 == 0)
                LED0_TOGGLE(); /* 切换LED,让LED闪烁,提示正在运行. */

            delay_ms(10);
        }
    }
}
