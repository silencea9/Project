#include "valve.h"

/*!
    \brief      配置 GPIO 引脚及相关外设时钟
    \param[in]  无
    \param[out] 无
    \retval     无
    \note       本函数完成以下操作：
                - 使能 AF 外设时钟并配置 SWJ 引脚重映射
                - 使能 GPIOA 时钟，初始化 PA15 为推挽输出并置位
                - 使能 GPIOC 时钟，初始化 PC6 为推挽输出并置位，
                  初始化 PC7 为推挽输出并复位
                - 使能 GPIOD 时钟，初始化 PD2 为推挽输出并置位
*/
void valve_config(void)
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

    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_bit_reset(GPIOC, GPIO_PIN_8);

    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_bit_reset(GPIOC, GPIO_PIN_9);

    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_bit_reset(GPIOA, GPIO_PIN_10);


}

/*!
    \brief      控制指定阀门的状态
    \param[in]  valve_id: 阀门编号 (VALVE1 或 VALVE2)
    \param[in]  state: 阀门状态
                - VALVE_FORWARD: 打开阀门（置位引脚）
                - VALVE_REVERSE: 关闭阀门（复位引脚）
                - 其他值: 停止阀门（复位引脚）
    \param[out] 无
    \retval     无
    \note       本函数通过控制 GPIO 引脚电平来设置阀门状态：
                - 阀门1 对应 PC6
                - 阀门2 对应 PC7
                预留 default 分支以扩展更多阀门控制。
*/
void xxod(uint8_t valve_id, uint8_t state)
{
    switch (valve_id)
    {
    case VALVE1: // 阀门1 (PC6)
        if (state == VALVE_FORWARD)
            gpio_bit_set(GPIOC, GPIO_PIN_6);
        else if (state == VALVE_REVERSE)
            gpio_bit_reset(GPIOC, GPIO_PIN_6);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_6); // 停止
        break;

    case VALVE2: // 阀门2 (PC7)
        if (state == VALVE_FORWARD)
            gpio_bit_set(GPIOC, GPIO_PIN_7);
        else if (state == VALVE_REVERSE)
            gpio_bit_reset(GPIOC, GPIO_PIN_7);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_7); // 停止
        break;

    default:
        // 预留位置，可扩展更多阀门
        break;
    }
}

/*!
    \brief      控制指定平衡阀门的状态
    \param[in]  valve_id: 平衡阀门编号 (BALANCE_VALVE1 或 BALANCE_VALVE2)
    \param[in]  state: 阀门状态
                - VALVE_FORWARD: 打开阀门（置位引脚）
                - VALVE_REVERSE: 关闭阀门（复位引脚）
                - 其他值: 停止阀门（复位引脚）
    \param[out] 无
    \retval     无
    \note       本函数通过控制 GPIO 引脚电平来设置平衡阀门状态：
                - 平衡阀门1 对应 PC8
                - 平衡阀门2 对应 PC9
                预留 default 分支以扩展更多阀门控制。
*/
void single(uint8_t valve_id, uint8_t state)
{
    switch (valve_id)
    {
    case VALVE1: // 平衡阀门1
        if (state == VALVE_FORWARD)
            gpio_bit_set(GPIOC, GPIO_PIN_8);
        else if (state == VALVE_REVERSE)
            gpio_bit_reset(GPIOC, GPIO_PIN_8);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_8); // 停止
        break;

    case VALVE2: // 平衡阀门2 (PC9)
        if (state == VALVE_FORWARD)
            gpio_bit_set(GPIOC, GPIO_PIN_9);
        else if (state == VALVE_REVERSE)
            gpio_bit_reset(GPIOC, GPIO_PIN_9);
        else
            gpio_bit_reset(GPIOC, GPIO_PIN_9); // 停止
        break;

    default:
        // 预留位置，可扩展更多阀门
        break;
    }
}

/*!
    \brief      控制放氧阀门的状态
    \param[in]  state: 阀门状态
                - VALVE_FORWARD: 打开阀门（置位引脚）
                - VALVE_STOP: 关闭阀门（复位引脚）
                - 其他值: 保持不变
    \param[out] 无
    \retval     无
    \note       本函数通过控制 GPIOA 的 PA10 引脚电平来设置放氧阀门状态。
                当 valve_id 等于 OXYGEN_VALVE 时生效。
*/
void release(uint8_t state)
{
    switch (state)
    {
    case VALVE_FORWARD:
        gpio_bit_set(GPIOA, GPIO_PIN_10); // 打开
        break;
    case VALVE_STOP:
        gpio_bit_reset(GPIOA, GPIO_PIN_10); // 停止
        break;
    default:
        break;
    }
}

void valve_test(void)
{
    printf("One-way valve control test:\r\n");
    printf("Opening valve 1...\r\n");
    xxod(VALVE1, VALVE_FORWARD);
    valve_monitor();
    delay_ms(2000);
    printf("Closing valve 1...\r\n");
    xxod(VALVE1, VALVE_REVERSE);
    valve_monitor();
    delay_ms(2000);
    printf("Opening valve 2...\r\n");
    xxod(VALVE2, VALVE_FORWARD);
    valve_monitor();
    delay_ms(2000);
    printf("Closing valve 2...\r\n");
    xxod(VALVE2, VALVE_REVERSE);
    valve_monitor();
    delay_ms(2000);
    printf("Balance valve control test:\r\n");
    printf("Opening balance valve 1...\r\n");
    single(VALVE1, VALVE_FORWARD);
    valve_monitor();
    delay_ms(2000);
    printf("Closing balance valve 1...\r\n");
    single(VALVE1, VALVE_REVERSE);
    valve_monitor();
    delay_ms(2000);
    printf("Opening balance valve 2...\r\n");
    single(VALVE2, VALVE_FORWARD);
    valve_monitor();
    delay_ms(2000);
    printf("Closing balance valve 2...\r\n");
    single(VALVE2, VALVE_REVERSE);
    valve_monitor();
    delay_ms(2000);
    printf("Oxygen release valve control test:\r\n");
    printf("Opening oxygen valve...\r\n");
    release(VALVE_FORWARD);
    valve_monitor();
    delay_ms(2000);
    printf("Closing oxygen valve...\r\n");
    release(VALVE_STOP);
    valve_monitor();
    delay_ms(2000);
    printf("Valve test complete.\r\n");
}

/* 阀门 GPIO 引脚定义 */
#define VALVE1_PIN      GPIO_PIN_6   // 换向阀门1
#define VALVE2_PIN      GPIO_PIN_7   // 换向阀门2
#define BALANCE1_PIN    GPIO_PIN_8   // 平衡阀门1
#define BALANCE2_PIN    GPIO_PIN_9   // 平衡阀门2
#define OXYGEN_PIN      GPIO_PIN_10  // 放氧阀门

#define VALVE_PORT      GPIOC        // 换向阀门和平衡阀门在 PC 口
#define OXYGEN_PORT     GPIOA        // 放氧阀门在 PA 口

/* 监听并打印阀门引脚电平 */
void valve_monitor(void)
{
    uint8_t level_valve1  = gpio_input_bit_get(VALVE_PORT, VALVE1_PIN);
    uint8_t level_valve2  = gpio_input_bit_get(VALVE_PORT, VALVE2_PIN);
    uint8_t level_balance1= gpio_input_bit_get(VALVE_PORT, BALANCE1_PIN);
    uint8_t level_balance2= gpio_input_bit_get(VALVE_PORT, BALANCE2_PIN);
    uint8_t level_oxygen  = gpio_input_bit_get(OXYGEN_PORT, OXYGEN_PIN);

    printf("Valve1 (PC6): %s\r\n", level_valve1 ? "HIGH" : "LOW");
    printf("Valve2 (PC7): %s\r\n", level_valve2 ? "HIGH" : "LOW");
    printf("Balance1 (PC8): %s\r\n", level_balance1 ? "HIGH" : "LOW");
    printf("Balance2 (PC9): %s\r\n", level_balance2 ? "HIGH" : "LOW");
    printf("Oxygen (PA10): %s\r\n", level_oxygen ? "HIGH" : "LOW");
}

