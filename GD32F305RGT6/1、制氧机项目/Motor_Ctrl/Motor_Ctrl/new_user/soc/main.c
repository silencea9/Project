#include "main.h"

#define RX_BUFFER_SIZE 128

static char rx_buffer[RX_BUFFER_SIZE];
static int read_flag = 0;
char *str = "this is a test string\r\n";

typedef void (*usart_rx_callback_t)(uint8_t data);
static usart_rx_callback_t screen_rx_callback = NULL;

/* 注册回调函数 */
void usart0_register_callback(usart_rx_callback_t callback)
{
    screen_rx_callback = callback;
}

#define ADC_PC3_CHANNEL ADC_CHANNEL_13

void my_screen_recv_handler(uint8_t data)
{
    printf("Recv string: %s\r\n", rx_buffer);
}

void pc3_adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);

    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    adc_deinit(ADC0);
    adc_mode_config(ADC_MODE_FREE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, DISABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    adc_regular_channel_config(ADC0, 0, ADC_PC3_CHANNEL, ADC_SAMPLETIME_239POINT5);

    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

    adc_enable(ADC0);
    delay_ms(1);
    adc_calibration_enable(ADC0);
}

uint16_t pc3_adc_read(void)
{
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
    while (RESET == adc_flag_get(ADC0, ADC_FLAG_EOC))
        ;
    return adc_regular_data_read(ADC0);
}

int main()
{
    systick_config();
    delay_ms(500);

    valve_config(); // 相关阀门的控制引脚初始化

    motor_init();        // 初始化电机
    screen_init(115200); // 初始化屏幕

    printf("hello\r\n");
    usart0_register_callback(my_screen_recv_handler);

    pc3_adc_init();

    while (1)
    {
       uint16_t adc_value = pc3_adc_read();
       float voltage = (adc_value / 4095.0f) * 3.3f; // 12-bit

       printf("PC3 ADC: %u, voltage=%.3f V\r\n", adc_value, voltage);
       screen_send_string("PC3 ADC sample done\r\n");

       printf("Serial test start...\r\n");
       screen_send_string("Serial test start...\r\n");
       delay_ms(1000);
       printf("Serial test end.\r\n");
       printf("Motor test start...\r\n");
       motor_test();
       printf("Motor test end.\r\n");
       printf("Valve test start...\r\n");
       valve_test();
       printf("Valve test end.\r\n");
    }
    return 0;
}

// 输出重定向
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
        ;
    return ch;
}

void USART0_IRQHandler(void)
{
    if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) != RESET)
    {
        char data = (char)usart_data_receive(USART0);

        if (data == '\n' || data == '\r') // 检测到结束符
        {
            rx_buffer[read_flag] = '\0'; // 添加字符串结束符
            if (screen_rx_callback != NULL)
            {
                screen_rx_callback((uint8_t)*rx_buffer); // 调用回调函数
            }
            read_flag = 0; // 重置缓冲区索引，准备接收下一条
        }
        else
        {
            if (read_flag < RX_BUFFER_SIZE - 1)
            {
                rx_buffer[read_flag++] = data;
            }
            else
            {
                read_flag = 0; // 防止溢出，丢弃数据
            }
        }
    }
}
