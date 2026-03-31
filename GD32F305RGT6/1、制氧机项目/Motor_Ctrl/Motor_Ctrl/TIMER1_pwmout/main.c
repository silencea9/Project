#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>

// 串口0引脚定义
#define USART0_TX_PIN    GPIO_PIN_6
#define USART0_RX_PIN    GPIO_PIN_7
#define USART0_GPIO_PORT GPIOB
#define USART0_GPIO_RCU  RCU_GPIOB
#define USART0_RCU       RCU_USART0

// 串口配置参数
#define USART0_BAUDRATE  115200  // 波特率
#define USART0_WORDLEN   USART_WL_8BIT  // 8位数据位 
#define USART0_STOPBITS  USART_STB_1BIT // 1位停止位
#define USART0_PARITY    USART_PM_NONE  // 无校验


void gpio_config(void);
void timer_config(void);


/*!
    \brief      配置 MCU 的 GPIO 端口
    \param[in]  none
    \param[out] none
    \retval     none
    \note       本函数主要完成以下操作：
                - 开启 AFIO 时钟并进行 SWJ 引脚重映射
                - 初始化 GPIOA15 为推挽输出并置高
                - 初始化 GPIOC6 为推挽输出并置高
                - 初始化 GPIOC7 为推挽输出并置低
                - 初始化 GPIOD2 为推挽输出并置高
*/
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    gpio_bit_set(GPIOA, GPIO_PIN_15);    
    
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_bit_set(GPIOC, GPIO_PIN_6);    
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_bit_reset(GPIOC, GPIO_PIN_7);    
    
    rcu_periph_clock_enable(RCU_GPIOD);
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_bit_set(GPIOD, GPIO_PIN_2);    
}


void pwm_init(void)
{
    //使能时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_periph_clock_enable(RCU_AF); // 使能复用功能时钟

    //配置PA8为复用推挽输出（TIMER0_CH0）
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    //定时器基础参数配置
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;

    //初始化结构体变量
    timer_struct_para_init(&timer_initpara);
    timer_channel_output_struct_para_init(&timer_ocinitpara);

    //配置定时器基础参数
    //假设系统时钟为120MHz，预分频系数120-1，得到1MHz计数器时钟
    timer_initpara.prescaler = 120 - 1; // PSC = 119
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 1000 - 1; // CAR = 999, 产生1kHz PWM
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    //配置定时器输出通道参数
    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH; // 输出高有效
    timer_ocinitpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);

    //设置PWM模式与占空比
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    
    //设置占空比为25%（比较值CH0CV = 250）
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 450);

    //使能高级定时器的主输出（MOE）
    timer_primary_output_config(TIMER0, ENABLE);

    //使能定时器
    timer_enable(TIMER0);
}

/**
  * @brief  USART0初始化函数
  * @param  无
  * @retval 无
  */
void usart0_init(void)
{
    //开启时钟
    rcu_periph_clock_enable(USART0_GPIO_RCU);  // GPIOB时钟
    rcu_periph_clock_enable(USART0_RCU);       // USART0时钟
		rcu_periph_clock_enable(RCU_AF);           // 必须开启AFIO时钟进行重映射
    
    // 配置GPIO引脚重映射：USART0重映射到PB6/PB7
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);	
	
    //配置GPIO引脚
    // PB6 - USART0_TX (复用推挽输出)
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART0_TX_PIN);
    // PB7 - USART0_RX (浮空输入)
    gpio_init(USART0_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART0_RX_PIN);
    
    //配置USART0参数
    usart_deinit(USART0);  // 复位USART0
    usart_baudrate_set(USART0, USART0_BAUDRATE);  // 设置波特率
    usart_word_length_set(USART0, USART_WL_8BIT);  // 设置数据位长度
    usart_stop_bit_set(USART0, USART0_STOPBITS);  // 设置停止位
    usart_parity_config(USART0, USART0_PARITY);  // 设置校验位
		usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
		usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);  // 使能发送 
		usart_receive_config(USART0, USART_RECEIVE_ENABLE);    // 使能接收
    usart_enable(USART0);  // 使能USART0
    
    //使能接收中断（如果需要中断方式接收）
    usart_interrupt_enable(USART0, USART_INT_RBNE);  // 接收缓冲区非空中断
    nvic_irq_enable(USART0_IRQn, 0, 0);  // 使能USART0中断
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	systick_config();
	
	delay_1ms(500);

	gpio_config();
    
  pwm_init();

	usart0_init();
	printf("hello\r\n");
	
    while (1)
	{
		delay_1ms(10000);
		printf("hello world!");
		
	}
}



/*!
    \brief      重定向 C 标准库 printf 输出到 USART0
    \param[in]  ch: 要发送的字符
    \param[in]  f: 文件指针 (标准库接口要求)
    \param[out] none
    \retval     已发送的字符
    \note       通过调用 usart_data_transmit() 将字符写入 USART0，
                并等待发送缓冲区空 (USART_FLAG_TBE) 后返回。
*/
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
