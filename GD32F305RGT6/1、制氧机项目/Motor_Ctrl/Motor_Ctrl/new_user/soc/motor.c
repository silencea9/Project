#include "motor.h"

/*!
    \brief      初始化 TIMER0 的 PWM 输出功能
    \param[in]  无
    \param[out] 无
    \retval     无
    \note       本函数完成以下操作：
                - 使能 GPIOA、TIMER0 和 AF 外设时钟
                - 配置 PA8 为复用推挽输出 (TIMER0_CH0)
                - 初始化定时器参数，设置预分频器和周期以产生 1kHz PWM
                - 配置定时器通道输出参数，设置极性和空闲状态
                - 设置 PWM 模式和占空比 (约 45%)
                - 使能定时器主输出并启动定时器
*/
void motor_init(void)
{
    // 使能 GPIOA、TIMER0 和 AF 外设时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_TIMER0);
    rcu_periph_clock_enable(RCU_AF); // 使能复用功能时钟

    // 配置 PA8 为复用推挽输出 (TIMER0_CH0)
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    // 定时器参数结构体
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;

    // 初始化结构体变量
    timer_struct_para_init(&timer_initpara);
    timer_channel_output_struct_para_init(&timer_ocinitpara);

    // 配置定时器基本参数
    // 假设系统时钟为 120MHz，预分频系数 120-1，得到 1MHz 计数时钟
    timer_initpara.prescaler = 120 - 1; // PSC = 119
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 1000 - 1;   // CAR = 999，产生 1kHz PWM
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    // 配置定时器通道输出参数
    timer_ocinitpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity = TIMER_OC_POLARITY_HIGH; // 输出高有效
    timer_ocinitpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);

    // 设置 PWM 模式与影子寄存器
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    
    // 设置占空比为约 45% (比较值 CH0CV = 450)
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 450);

    // 使能高级定时器的主输出 (MOE)
    timer_primary_output_config(TIMER0, ENABLE);

    // 启动定时器
    timer_enable(TIMER0);
}


/* 修改占空比函数 (0~100%) */
void motor_set(uint16_t duty)
{
    if(duty > 100) duty = 100;
    uint16_t pulse = (duty * 999) / 100;
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, pulse);
}

/* 设置方向：0=正转，1=反转 */
void motor_set_direction(uint8_t dir)
{
    if(dir == 0)
        gpio_bit_reset(GPIOB, GPIO_PIN_2);  // 正转
    else
        gpio_bit_set(GPIOB, GPIO_PIN_2);    // 反转
}

/* 安全修改电机方向 */
void setmotor(uint8_t dir)
{
    // 1. 停止PWM输出
    motor_set(0);

    // 2. 关闭电机使能
    gpio_bit_reset(GPIOA, GPIO_PIN_9);  // PA9_EN = 0

    // 3. 切换方向
    motor_set_direction(dir);

    // 4. 重新打开电机使能
    gpio_bit_set(GPIOA, GPIO_PIN_9);    // PA9_EN = 1
}

void motor_test(void)
{
    // 测试正转
    setmotor(0); // 正转
    motor_set(50); // 50% 占空比
    delay_ms(2000); // 运行2秒

    // 测试反转
    setmotor(1); // 反转
    motor_set(75); // 75% 占空比
    delay_ms(2000); // 运行2秒

    // 停止电机
    motor_set(0); // 0% 占空比，停止电机
}