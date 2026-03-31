/*!
    \file    gd32f30x_rtc.c
    \brief   RTC driver

   \version 2025-7-31, V3.0.2, firmware for GD32F30x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/


#include "gd32f30x_rtc.h"

/*!
    \brief      使能 RTC 中断
    \param[in]  interrupt: 指定要使能的中断
          \arg    RTC_INT_SECOND: 秒中断
          \arg    RTC_INT_ALARM: 报警中断
          \arg    RTC_INT_OVERFLOW: 溢出中断
    \param[out] none
    \retval     none
*/
void rtc_interrupt_enable(uint32_t interrupt)
{
    RTC_INTEN |= interrupt;
}

/*!
    \brief      禁用 RTC 中断
    \param[in]  interrupt: 指定要禁用的中断
          \arg    RTC_INT_SECOND: 秒中断
          \arg    RTC_INT_ALARM: 报警中断
          \arg    RTC_INT_OVERFLOW: 溢出中断
    \param[out] none
    \retval     none
*/
void rtc_interrupt_disable(uint32_t interrupt)
{
    RTC_INTEN &= ~interrupt;
}

/*!
    \brief      进入 RTC 配置模式
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration_mode_enter(void)
{
    RTC_CTL |= RTC_CTL_CMF;
}

/*!
    \brief      退出 RTC 配置模式
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration_mode_exit(void)
{
    RTC_CTL &= ~RTC_CTL_CMF;
}

/*!
    \brief      等待 RTC 最后写操作完成标志位设置
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_lwoff_wait(void)
{
     /* loop until LWOFF flag is set */
    while (RESET == (RTC_CTL & RTC_CTL_LWOFF)){
    }
}

/*!
    \brief      等待 RTC 寄存器同步标志位设置
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_register_sync_wait(void)
{
     /* clear RSYNF flag */
    RTC_CTL &= ~RTC_CTL_RSYNF;
    /* loop until RSYNF flag is set */
    while (RESET == (RTC_CTL & RTC_CTL_RSYNF)){
    }
}

/*!
    \brief      获取 RTC 计数器值
    \param[in]  none
    \param[out] none
    \retval     RTC 计数器值
*/
uint32_t rtc_counter_get(void)
{
    uint32_t temp = 0x0U;
    temp = RTC_CNTL;
    temp |= (RTC_CNTH << 16);
    return temp;
}

/*!
    \brief      设置 RTC 计数器值
    \param[in]  cnt: RTC 计数器值
    \param[out] none
    \retval     none
*/
void rtc_counter_set(uint32_t cnt)
{
    rtc_configuration_mode_enter();
    /* set the RTC counter high bits */
    RTC_CNTH = cnt >> 16;
    /* set the RTC counter low bits */
    RTC_CNTL = (cnt & RTC_LOW_VALUE);
    rtc_configuration_mode_exit();
}

/*!
    \brief      设置 RTC 预分频器值
    \param[in]  psc: RTC 预分频器值
    \param[out] none
    \retval     none
*/
void rtc_prescaler_set(uint32_t psc)
{
    rtc_configuration_mode_enter();
    /* set the RTC prescaler high bits */
    RTC_PSCH = (psc & RTC_HIGH_VALUE) >> 16;
    /* set the RTC prescaler low bits */
    RTC_PSCL = (psc & RTC_LOW_VALUE);
    rtc_configuration_mode_exit();
}

/*!
    \brief      设置 RTC 报警值
    \param[in]  alarm: RTC 报警值
    \param[out] none
    \retval     none
*/
void rtc_alarm_config(uint32_t alarm)
{
    rtc_configuration_mode_enter();
    /* set the alarm high bits */
    RTC_ALRMH = alarm >> 16;
    /* set the alarm low bits */
    RTC_ALRML = (alarm & RTC_LOW_VALUE);
    rtc_configuration_mode_exit();
}

/*!
    \brief      获取 RTC 分频器值
    \param[in]  none
    \param[out] none
    \retval     RTC 分频器值
*/
uint32_t rtc_divider_get(void)
{
    uint32_t temp = 0x00U;
    temp = (RTC_DIVH & RTC_DIVH_DIV) << 16;
    temp |= RTC_DIVL;
    return temp;
}

/*!
    \brief      获取 RTC 标志状态
    \param[in]  flag: 指定要获取哪个标志状态
          \arg    RTC_FLAG_SECOND: 秒中断标志
          \arg    RTC_FLAG_ALARM: 报警中断标志
          \arg    RTC_FLAG_OVERFLOW: 溢出中断标志
          \arg    RTC_FLAG_RSYN: 寄存器同步标志
          \arg    RTC_FLAG_LWOF: 最后写操作完成标志
    \param[out] none
    \retval     SET 或 RESET
*/
FlagStatus rtc_flag_get(uint32_t flag)
{
    if(RESET != (RTC_CTL & flag)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      清除 RTC 标志状态
    \param[in]  flag: 指定要清除哪个标志状态
          \arg    RTC_FLAG_SECOND: 秒中断标志
          \arg    RTC_FLAG_ALARM: 报警中断标志
          \arg    RTC_FLAG_OVERFLOW: 溢出中断标志
          \arg    RTC_FLAG_RSYN: 寄存器同步标志
    \param[out] none
    \retval     none
*/
void rtc_flag_clear(uint32_t flag)
{
    /* clear RTC flag */
    RTC_CTL &= ~flag;
}

