#include "lcd.h"


/**
  * @brief  USART0 初始化函数
  * @param  baudrate 串口波特率
  * @retval 无
  */
void screen_init(uint32_t baudrate)
{
    // 打开时钟
    rcu_periph_clock_enable(RCU_GPIOB);  // 使能 GPIOB 时钟
    rcu_periph_clock_enable(RCU_USART0);       // 使能 USART0 时钟
    rcu_periph_clock_enable(RCU_AF);           // 必须使能 AFIO 时钟以进行引脚重映射
    
    // 配置 GPIO 引脚重映射：USART0 重映射到 PB6/PB7
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);   
    
    // 配置 GPIO 引脚
    // PB6 - USART0_TX (复用推挽输出)
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    // PB7 - USART0_RX (浮空输入)
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    
    // 配置 USART0 参数
    usart_deinit(USART0);                          // 复位 USART0
    usart_baudrate_set(USART0, baudrate);   // 设置波特率
    usart_word_length_set(USART0, USART_WL_8BIT);  // 设置数据位长度
    usart_stop_bit_set(USART0, USART_STB_1BIT);   // 设置停止位
    usart_parity_config(USART0, USART_PM_NONE);    // 设置校验位
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE); // 禁用 RTS 硬件流控
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE); // 禁用 CTS 硬件流控
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);      // 使能发送
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);        // 使能接收
    usart_enable(USART0);                                     // 使能 USART0
    
    // 使能接收中断（如果需要中断方式接收）
    usart_interrupt_enable(USART0, USART_INT_RBNE);  // 接收缓冲区非空中断
    nvic_irq_enable(USART0_IRQn, 0, 0);              // 使能 USART0 中断
}

/*!
    \brief      通过 USART0 向屏幕发送一个字节数据
    \param[in]  data: 要发送的 8 位数据
    \param[out] 无
    \retval     无
    \note       本函数会调用 USART0 的数据发送函数，并等待发送缓冲区空 (TBE 标志置位)，
                保证数据成功写入发送缓冲区。
*/
void screen_send_byte(uint8_t data)
{
    // 发送数据到 USART0
    usart_data_transmit(USART0, data);
    // 等待发送缓冲区空
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}


/*!
    \brief      通过 USART0 向屏幕发送字符串
    \param[in]  str: 指向待发送字符串的指针（以 '\0' 结尾）
    \param[out] 无
    \retval     无
    \note       本函数会逐字节调用 usart0_send_byte() 发送字符串，
                直到遇到字符串结束符 '\0'。
*/
void screen_send_string(const char *str)
{
    while(*str)
    {
        screen_send_byte((uint8_t)*str);
        str++;
    }
}
