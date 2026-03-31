/*!
    \file    gd32f30x_bkp.c
    \brief   BKP driver

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

#include "gd32f30x_bkp.h"

#define TAMPER_FLAG_SHIFT          ((uint8_t)8U)

/*!
    \brief      复位 BKP 寄存器
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_deinit(void)
{
    /* reset BKP domain register*/
    rcu_bkp_reset_enable();
    rcu_bkp_reset_disable();
}

/*!
        \brief      写入 BKP 数据寄存器
        \param[in]  register_number: 参考 bkp_data_register_enum，只能选择其中一个
            \arg        BKP_DATA_x(x = 0..41): BKP 数据寄存器编号 x
        \param[in]  data: 要写入 BKP 数据寄存器的数据
        \param[out] none
        \retval     none
*/
void bkp_write_data(bkp_data_register_enum register_number, uint16_t data)
{
    if((register_number >= BKP_DATA_10) && (register_number <= BKP_DATA_41)){
        BKP_DATA10_41(register_number-1U) = data;
    }else if((register_number >= BKP_DATA_0) && (register_number <= BKP_DATA_9)){
        BKP_DATA0_9(register_number-1U) = data;
    }else{
        /* illegal parameters */
    }
}

/*!
        \brief      读取 BKP 数据寄存器
        \param[in]  register_number: 参考 bkp_data_register_enum，只能选择其中一个
            \arg        BKP_DATA_x(x = 0..41): BKP 数据寄存器编号 x
        \param[out] none
        \retval     BKP 数据寄存器中的数据
*/
uint16_t bkp_read_data(bkp_data_register_enum register_number)
{
    uint16_t data = 0U;
    
    /* get the data from the BKP data register */
    if((register_number >= BKP_DATA_10) && (register_number <= BKP_DATA_41)){
        data = BKP_DATA10_41(register_number-1U);
    }else if((register_number >= BKP_DATA_0) && (register_number <= BKP_DATA_9)){
        data = BKP_DATA0_9(register_number-1U);
    }else{
        /* illegal parameters */
    }
    return data;
}

/*!
    \brief      使能 RTC 校准输出
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_rtc_calibration_output_enable(void)
{
    BKP_OCTL |= (uint16_t)BKP_OCTL_COEN;
}

/*!
    \brief      禁用 RTC 校准输出
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_rtc_calibration_output_disable(void)
{
    BKP_OCTL &= (uint16_t)~BKP_OCTL_COEN;
}

/*!
    \brief      使能 RTC 报警或秒脉冲输出
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_rtc_signal_output_enable(void)
{
    BKP_OCTL |= (uint16_t)BKP_OCTL_ASOEN;
}

/*!
    \brief      禁用 RTC 报警或秒脉冲输出
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_rtc_signal_output_disable(void)
{
    BKP_OCTL &= (uint16_t)~BKP_OCTL_ASOEN;
}

/*!
        \brief      选择 RTC 输出
        \param[in]  outputsel: RTC 输出选择
            \arg        RTC_OUTPUT_ALARM_PULSE: 选择 RTC 报警脉冲作为 RTC 输出
            \arg        RTC_OUTPUT_SECOND_PULSE: 选择 RTC 秒脉冲作为 RTC 输出
        \param[out] none
        \retval     none
*/
void bkp_rtc_output_select(uint16_t outputsel)
{
    uint16_t ctl = 0U;
    
    ctl = BKP_OCTL;
    ctl &= (uint16_t)~BKP_OCTL_ROSEL;
    ctl |= outputsel;
    BKP_OCTL = ctl;
}

/*!
        \brief      选择 RTC 时钟输出
        \param[in]  clocksel: RTC 时钟输出选择
            \arg        RTC_CLOCK_DIV_64: RTC 时钟分频 64
            \arg        RTC_CLOCK_DIV_1: 直接输出 RTC 时钟
        \param[out] none
        \retval     none
*/
void bkp_rtc_clock_output_select(uint16_t clocksel)
{
    uint16_t ctl = 0U;
    
    ctl = BKP_OCTL;
    ctl &= (uint16_t)~BKP_OCTL_CCOSEL;
    ctl |= clocksel;
    BKP_OCTL = ctl;
}

/*!
        \brief      RTC 时钟校准方向
        \param[in]  direction: RTC 校准方向
            \arg        RTC_CLOCK_SLOWED_DOWN: RTC 时钟减慢
            \arg        RTC_CLOCK_SPEED_UP: RTC 时钟加速
        \param[out] none
        \retval     none
*/
void bkp_rtc_clock_calibration_direction(uint16_t direction)
{
    uint16_t ctl = 0U;
    
    ctl = BKP_OCTL;
    ctl &= (uint16_t)~BKP_OCTL_CALDIR;
    ctl |= direction;
    BKP_OCTL = ctl;
}

/*!
        \brief      设置 RTC 校准值
        \param[in]  value: RTC 校准值
            \arg        0x00 - 0x7F
        \param[out] none
        \retval     none
*/
void bkp_rtc_calibration_value_set(uint8_t value)
{
    uint16_t ctl;
    
    ctl = BKP_OCTL;
    ctl &= ~(uint16_t)BKP_OCTL_RCCV;
    ctl |= (uint16_t)OCTL_RCCV(value);
    BKP_OCTL = ctl;
}

/*!
    \brief      使能防篡改检测
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_tamper_detection_enable(void)
{
    BKP_TPCTL |= (uint16_t)BKP_TPCTL_TPEN;
}

/*!
    \brief      禁用防篡改检测
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_tamper_detection_disable(void)
{
    BKP_TPCTL &= (uint16_t)~BKP_TPCTL_TPEN;
}

/*!
        \brief      设置防篡改引脚有效电平
        \param[in]  level: 防篡改引脚有效电平
            \arg        TAMPER_PIN_ACTIVE_HIGH: 引脚高电平为有效
            \arg        TAMPER_PIN_ACTIVE_LOW: 引脚低电平为有效
        \param[out] none
        \retval     none
*/
void bkp_tamper_active_level_set(uint16_t level)
{
    uint16_t ctl = 0U;
    
    ctl = BKP_TPCTL;
    ctl &= (uint16_t)~BKP_TPCTL_TPAL;
    ctl |= level;
    BKP_TPCTL = ctl;
}

/*!
    \brief      使能防篡改中断
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_tamper_interrupt_enable(void)
{
    BKP_TPCS |= (uint16_t)BKP_TPCS_TPIE;
}

/*!
    \brief      禁用防篡改中断
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bkp_tamper_interrupt_disable(void)
{
    BKP_TPCS &= (uint16_t)~BKP_TPCS_TPIE;
}

/*!
        \brief      获取 BKP 标志状态
        \param[in]  flag
            \arg        BKP_FLAG_TAMPER: 防篡改事件标志
        \param[out] none
        \retval     FlagStatus: SET 或 RESET
*/
FlagStatus bkp_flag_get(uint16_t flag)
{
    if(RESET != (BKP_TPCS & flag)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
        \brief      清除 BKP 标志状态
        \param[in]  flag
            \arg        BKP_FLAG_TAMPER: 防篡改事件标志
        \param[out] none
        \retval     none
*/
void bkp_flag_clear(uint16_t flag)
{
        BKP_TPCS |= (uint16_t)(flag >> TAMPER_FLAG_SHIFT);
}

/*!
        \brief      获取 BKP 中断标志状态
        \param[in]  flag
            \arg        BKP_INT_FLAG_TAMPER: 防篡改中断标志
        \param[out] none
        \retval     FlagStatus: SET 或 RESET
*/
FlagStatus bkp_interrupt_flag_get(uint16_t flag)
{
    if(RESET != (BKP_TPCS & flag)){
        return SET;
    }else{
        return RESET;
    }
}

/*!
        \brief      清除 BKP 中断标志状态
        \param[in]  flag
            \arg        BKP_INT_FLAG_TAMPER: 防篡改中断标志
        \param[out] none
        \retval     none
*/
void bkp_interrupt_flag_clear(uint16_t flag)
{
    BKP_TPCS |= (uint16_t)(flag >> TAMPER_FLAG_SHIFT);
}

