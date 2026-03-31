/*!
    \file    gd32f30x_adc.c
    \brief   ADC driver

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

#include "gd32f30x_adc.h"

/*!
    \brief      reset ADC 
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_deinit(uint32_t adc_periph)
{
    switch(adc_periph){
    case ADC0:
        rcu_periph_reset_enable(RCU_ADC0RST);
        rcu_periph_reset_disable(RCU_ADC0RST);
        break;
    case ADC1:
        rcu_periph_reset_enable(RCU_ADC1RST);
        rcu_periph_reset_disable(RCU_ADC1RST);
        break;
#if (defined(GD32F30X_HD) || defined(GD32F30X_XD))    
    case ADC2:
        rcu_periph_reset_enable(RCU_ADC2RST);
        rcu_periph_reset_disable(RCU_ADC2RST);
        break;
#endif    
    default:
        break;      
    }
}

/*!
    \brief      enable ADC interface
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_enable(uint32_t adc_periph)
{
    if(RESET == (ADC_CTL1(adc_periph) & ADC_CTL1_ADCON)){
        ADC_CTL1(adc_periph) |= (uint32_t)ADC_CTL1_ADCON;
    }       
}

/*!
    \brief      disable ADC interface
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_disable(uint32_t adc_periph)
{
    ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ADCON);
}

/*!
    \brief      ADC calibration and reset calibration
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_calibration_enable(uint32_t adc_periph)
{
    /* reset the selected ADC calibration registers */
    ADC_CTL1(adc_periph) |= (uint32_t) ADC_CTL1_RSTCLB;
    /* check the RSTCLB bit state */
    while((ADC_CTL1(adc_periph) & ADC_CTL1_RSTCLB)){
    }
    /* enable ADC calibration process */
    ADC_CTL1(adc_periph) |= ADC_CTL1_CLB;
    /* check the CLB bit state */
    while((ADC_CTL1(adc_periph) & ADC_CTL1_CLB)){
    }
}

/*!
    \brief      enable DMA request 
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_dma_mode_enable(uint32_t adc_periph)
{
    ADC_CTL1(adc_periph) |= (uint32_t)(ADC_CTL1_DMA);
}

/*!
    \brief      disable DMA request 
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[out] none
    \retval     none
*/
void adc_dma_mode_disable(uint32_t adc_periph)
{
    ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_DMA);
}

/*!
    \brief      enable the temperature sensor and Vrefint channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_tempsensor_vrefint_enable(void)
{
    /* enable the temperature sensor and Vrefint channel */
    ADC_CTL1(ADC0) |= ADC_CTL1_TSVREN;
}

/*!
    \brief      disable the temperature sensor and Vrefint channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_tempsensor_vrefint_disable(void)
{
    /* disable the temperature sensor and Vrefint channel */
    ADC_CTL1(ADC0) &= ~ADC_CTL1_TSVREN;
}

/*!
    \brief      configure ADC resolution 
    \param[in]  adc_periph: ADCx,x=0,1,2
                only one among these parameters can be selected
    \param[in]  resolution: ADC resolution
                only one among these parameters can be selected
      \arg        ADC_RESOLUTION_12B: 12-bit ADC resolution
      \arg        ADC_RESOLUTION_10B: 10-bit ADC resolution
      \arg        ADC_RESOLUTION_8B: 8-bit ADC resolution
      \arg        ADC_RESOLUTION_6B: 6-bit ADC resolution
    \param[out] none
    \retval     none
*/
void adc_resolution_config(uint32_t adc_periph , uint32_t resolution)
{
    ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)ADC_OVSAMPCTL_DRES);
    ADC_OVSAMPCTL(adc_periph) |= (uint32_t)resolution;
}

/*!
        \brief      配置 ADC 不连续转换模式
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 选择通道组
                                只能选择其中一个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
            \arg        ADC_CHANNEL_DISCON_DISABLE: 禁用规则和插入通道的不连续模式
        \param[in]  length: 不连续模式下的转换次数，取值 1..8
                                                对规则通道有效，对插入通道无效
        \param[out] none
        \retval     none
*/
void adc_discontinuous_mode_config(uint32_t adc_periph, uint8_t adc_channel_group, uint8_t length)
{
    ADC_CTL0(adc_periph) &= ~((uint32_t)( ADC_CTL0_DISRC | ADC_CTL0_DISIC ));
    switch(adc_channel_group){
    case ADC_REGULAR_CHANNEL:
        /* config the number of conversions in discontinuous mode  */
        ADC_CTL0(adc_periph) &= ~((uint32_t)ADC_CTL0_DISNUM);
        ADC_CTL0(adc_periph) |= CTL0_DISNUM(((uint32_t)length - 1U));
    
        ADC_CTL0(adc_periph) |= (uint32_t)ADC_CTL0_DISRC;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL0(adc_periph) |= (uint32_t)ADC_CTL0_DISIC;
        break;
    case ADC_CHANNEL_DISCON_DISABLE:
    default:
        break;
    }
}

/*!
        \brief      配置 ADC 同步模式
        \param[in]  mode: ADC 模式
                                只能选择其中一个参数
            \arg        ADC_MODE_FREE: 所有 ADC 独立工作
            \arg        ADC_DAUL_REGULAL_PARALLEL_INSERTED_PARALLEL: ADC0 与 ADC1 联合工作，规则并行 + 插入并行
            \arg        ADC_DAUL_REGULAL_PARALLEL_INSERTED_ROTATION: ADC0 与 ADC1 联合工作，规则并行 + 触发轮换
            \arg        ADC_DAUL_INSERTED_PARALLEL_REGULAL_FOLLOWUP_FAST: ADC0 与 ADC1 联合工作，插入并行 + 快速跟随
            \arg        ADC_DAUL_INSERTED_PARALLEL_REGULAL_FOLLOWUP_SLOW: ADC0 与 ADC1 联合工作，插入并行 + 慢速跟随
            \arg        ADC_DAUL_INSERTED_PARALLEL: 仅插入并行模式
            \arg        ADC_DAUL_REGULAL_PARALLEL: 仅规则并行模式
            \arg        ADC_DAUL_REGULAL_FOLLOWUP_FAST: 仅快速跟随模式
            \arg        ADC_DAUL_REGULAL_FOLLOWUP_SLOW: 仅慢速跟随模式
            \arg        ADC_DAUL_INSERTED_TRRIGGER_ROTATION: 仅触发轮换模式
        \param[out] none
        \retval     none
*/
void adc_mode_config(uint32_t mode)
{
    ADC_CTL0(ADC0) &= ~(ADC_CTL0_SYNCM);
    ADC_CTL0(ADC0) |= mode;
}

/*!
        \brief      使能或禁用 ADC 特殊功能
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  function: 要配置的功能
                                可同时选择多个参数
            \arg        ADC_SCAN_MODE: 扫描模式
            \arg        ADC_INSERTED_CHANNEL_AUTO: 插入通道组自动转换
            \arg        ADC_CONTINUOUS_MODE: 连续转换模式
        \param[in]  newvalue: ENABLE 或 DISABLE
        \param[out] none
        \retval     none
*/
void adc_special_function_config(uint32_t adc_periph , uint32_t function , ControlStatus newvalue)
{
    if(newvalue){
        if(0U != (function & ADC_SCAN_MODE)){
            ADC_CTL0(adc_periph) |= ADC_SCAN_MODE;
        }
        if(0U != (function & ADC_INSERTED_CHANNEL_AUTO)){
            ADC_CTL0(adc_periph) |= ADC_INSERTED_CHANNEL_AUTO;
        } 
        if(0U != (function & ADC_CONTINUOUS_MODE)){
            ADC_CTL1(adc_periph) |= ADC_CONTINUOUS_MODE;
        }        
    }else{
        if(0U != (function & ADC_SCAN_MODE)){
            ADC_CTL0(adc_periph) &= ~ADC_SCAN_MODE;
        }
        if(0U != (function & ADC_INSERTED_CHANNEL_AUTO)){
            ADC_CTL0(adc_periph) &= ~ADC_INSERTED_CHANNEL_AUTO;
        } 
        if(0U != (function & ADC_CONTINUOUS_MODE)){
            ADC_CTL1(adc_periph) &= ~ADC_CONTINUOUS_MODE;
        }       
    }
}

/*!
        \brief      配置 ADC 数据对齐方式
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  data_alignment: 数据对齐选择
                                只能选择一个参数
            \arg        ADC_DATAALIGN_RIGHT: 右对齐（LSB 对齐）
            \arg        ADC_DATAALIGN_LEFT: 左对齐（MSB 对齐）
        \param[out] none
        \retval     none
*/
void adc_data_alignment_config(uint32_t adc_periph , uint32_t data_alignment)
{
    if(ADC_DATAALIGN_RIGHT != data_alignment){
        ADC_CTL1(adc_periph) |= ADC_CTL1_DAL;
    }else{
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_DAL);
    }
}

/*!
        \brief      配置规则通道组或插入通道组的通道长度
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 选择通道组
                                只能选择一个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
        \param[in]  length: 通道个数
                                                规则通道范围 1-16
                                                插入通道范围 1-4
        \param[out] none
        \retval     none
*/
void adc_channel_length_config(uint32_t adc_periph, uint8_t adc_channel_group, uint32_t length)
{
    switch(adc_channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_RSQ0(adc_periph) &= ~((uint32_t)ADC_RSQ0_RL);
        ADC_RSQ0(adc_periph) |= RSQ0_RL((uint32_t)(length-1U));

        break;
    case ADC_INSERTED_CHANNEL:
        ADC_ISQ(adc_periph) &= ~((uint32_t)ADC_ISQ_IL);
        ADC_ISQ(adc_periph) |= ISQ_IL((uint32_t)(length-1U));
    
        break;
    default:
        break;
    }
}

/*!
        \brief      配置 ADC 规则通道
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  rank: 规则组序列位置，取值 0..15
        \param[in]  adc_channel: 选择的 ADC 通道
                                只能选择其中一个参数
            \arg        ADC_CHANNEL_x(x=0..17)(x=16 与 x=17 仅 ADC0 可用): ADC 通道 x
        \param[in]  sample_time: 采样时间
                                只能选择一个参数
            \arg        ADC_SAMPLETIME_1POINT5: 1.5 周期
            \arg        ADC_SAMPLETIME_7POINT5: 7.5 周期
            \arg        ADC_SAMPLETIME_13POINT5: 13.5 周期
            \arg        ADC_SAMPLETIME_28POINT5: 28.5 周期
            \arg        ADC_SAMPLETIME_41POINT5: 41.5 周期
            \arg        ADC_SAMPLETIME_55POINT5: 55.5 周期
            \arg        ADC_SAMPLETIME_71POINT5: 71.5 周期
            \arg        ADC_SAMPLETIME_239POINT5: 239.5 周期
        \param[out] none
        \retval     none
*/
void adc_regular_channel_config(uint32_t adc_periph , uint8_t rank , uint8_t adc_channel , uint32_t sample_time)
{
    uint32_t rsq,sampt;
    
    /* ADC regular sequence config */
    if(rank < 6U){
        rsq = ADC_RSQ2(adc_periph);
        rsq &=  ~((uint32_t)(ADC_RSQX_RSQN << (5U*rank)));
        rsq |= ((uint32_t)adc_channel << (5U*rank));
        ADC_RSQ2(adc_periph) = rsq;
    }else if(rank < 12U){
        rsq = ADC_RSQ1(adc_periph);
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (5U*(rank-6U))));
        rsq |= ((uint32_t)adc_channel << (5U*(rank-6U)));
        ADC_RSQ1(adc_periph) = rsq;
    }else if(rank < 16U){
        rsq = ADC_RSQ0(adc_periph);
        rsq &= ~((uint32_t)(ADC_RSQX_RSQN << (5U*(rank-12U))));
        rsq |= ((uint32_t)adc_channel << (5U*(rank-12U)));
        ADC_RSQ0(adc_periph) = rsq;
    }else{
    }
    
    /* ADC sampling time config */
    if(adc_channel < 10U){
        sampt = ADC_SAMPT1(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*adc_channel)));
        sampt |= (uint32_t)(sample_time << (3U*adc_channel));
        ADC_SAMPT1(adc_periph) = sampt;
    }else if(adc_channel < 18U){
        sampt = ADC_SAMPT0(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*(adc_channel-10U))));
        sampt |= (uint32_t)(sample_time << (3U*(adc_channel-10U)));
        ADC_SAMPT0(adc_periph) = sampt;
    }else{
    }
}

/*!
        \brief      配置 ADC 插入通道
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  rank: 插入组序列位置，取值 0..3
        \param[in]  adc_channel: 选择的 ADC 通道
                                只能选择其中一个参数
            \arg        ADC_CHANNEL_x(x=0..17)(x=16 与 x=17 仅 ADC0 可用): ADC 通道 x
        \param[in]  sample_time: 采样时间
                                只能选择一个参数（同规则通道）
        \param[out] none
        \retval     none
*/
void adc_inserted_channel_config(uint32_t adc_periph , uint8_t rank , uint8_t adc_channel , uint32_t sample_time)
{
    uint8_t inserted_length;
    uint32_t isq,sampt;
    
    inserted_length = (uint8_t)GET_BITS(ADC_ISQ(adc_periph) , 20U , 21U);
    
    isq = ADC_ISQ(adc_periph);
    isq &= ~((uint32_t)(ADC_ISQ_ISQN << (5U * ((3 + rank) - inserted_length))));
    isq |= ((uint32_t)adc_channel << (5U * ((3 + rank) - inserted_length)));
    ADC_ISQ(adc_periph) = isq;

    /* ADC sampling time config */  
    if(adc_channel < 10U){
        sampt = ADC_SAMPT1(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*adc_channel)));
        sampt |= (uint32_t) sample_time << (3U*adc_channel);
        ADC_SAMPT1(adc_periph) = sampt;
    }else if(adc_channel < 18U){
        sampt = ADC_SAMPT0(adc_periph);
        sampt &= ~((uint32_t)(ADC_SAMPTX_SPTN << (3U*(adc_channel-10U))));
        sampt |= ((uint32_t)sample_time << (3U*(adc_channel-10U)));
        ADC_SAMPT0(adc_periph) = sampt;
    }else{
    }
}

/*!
        \brief      配置插入通道的偏移值
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  inserted_channel: 插入通道选择
                                只能选择一个参数
            \arg        ADC_INSERTED_CHANNEL_0: 插入通道0
            \arg        ADC_INSERTED_CHANNEL_1: 插入通道1
            \arg        ADC_INSERTED_CHANNEL_2: 插入通道2
            \arg        ADC_INSERTED_CHANNEL_3: 插入通道3
        \param[in]  offset: 偏移值
        \param[out] none
        \retval     none
*/
void adc_inserted_channel_offset_config(uint32_t adc_periph , uint8_t inserted_channel , uint16_t offset)
{
    uint8_t inserted_length;
    uint32_t num = 0U;

    inserted_length = (uint8_t)GET_BITS(ADC_ISQ(adc_periph) , 20U , 21U);
    num = 3U - (inserted_length - inserted_channel);
    
    if(num <= 3U){
        /* calculate the offset of the register */
        num = num * 4U;
        /* config the offset of the selected channels */
        REG32((adc_periph) + 0x14U + num) = IOFFX_IOFF((uint32_t)offset);
    }  
}

/*!
        \brief      使能或禁用 ADC 外部触发
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 选择通道组
                                可选择一个或多个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
        \param[in]  newvalue: ENABLE 或 DISABLE
        \param[out] none
        \retval     none
*/
void adc_external_trigger_config(uint32_t adc_periph, uint8_t adc_channel_group, ControlStatus newvalue)
{
    if(newvalue){
        if(0U != (adc_channel_group & ADC_REGULAR_CHANNEL)){
            ADC_CTL1(adc_periph) |= ADC_CTL1_ETERC;
        }
        if(0U != (adc_channel_group & ADC_INSERTED_CHANNEL)){
            ADC_CTL1(adc_periph) |= ADC_CTL1_ETEIC;
        }        
    }else{
        if(0U != (adc_channel_group & ADC_REGULAR_CHANNEL)){
            ADC_CTL1(adc_periph) &= ~ADC_CTL1_ETERC;
        }
        if(0U != (adc_channel_group & ADC_INSERTED_CHANNEL)){
            ADC_CTL1(adc_periph) &= ~ADC_CTL1_ETEIC;
        }      
    }
}

/*!
        \brief      配置 ADC 外部触发源
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 选择通道组
                                只能选择一个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
        \param[in]  external_trigger_source: 规则或插入组的触发源
                                只能选择一个参数，具体触发源见宏定义（定时器、外部中断或软件触发等）
        \param[out] none
        \retval     none
*/
void adc_external_trigger_source_config(uint32_t adc_periph, uint8_t adc_channel_group, uint32_t external_trigger_source)
{   
    switch(adc_channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ETSRC);
        ADC_CTL1(adc_periph) |= (uint32_t)external_trigger_source;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL1(adc_periph) &= ~((uint32_t)ADC_CTL1_ETSIC);
        ADC_CTL1(adc_periph) |= (uint32_t)external_trigger_source;
        break;
    default:
        break;
    }
}

/*!
        \brief      使能 ADC 软件触发
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 选择通道组
                                可选择一个或多个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
        \param[out] none
        \retval     none
*/
void adc_software_trigger_enable(uint32_t adc_periph , uint8_t adc_channel_group)
{
    if(0U != (adc_channel_group & ADC_REGULAR_CHANNEL)){
        ADC_CTL1(adc_periph) |= ADC_CTL1_SWRCST;
    }
    if(0U != (adc_channel_group & ADC_INSERTED_CHANNEL)){
        ADC_CTL1(adc_periph) |= ADC_CTL1_SWICST;
    }
}

/*!
    \brief      读取规则组的 ADC 数据寄存器
    \param[in]  adc_periph: ADCx, x=0,1,2
                只能选择其中一个参数
    \param[in]  none
    \param[out] none
    \retval     返回转换结果值
*/
uint16_t adc_regular_data_read(uint32_t adc_periph)
{
    return (uint16_t)(ADC_RDATA(adc_periph));
}

/*!
        \brief      读取插入组的 ADC 数据寄存器
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  inserted_channel: 插入通道选择
                                只能选择一个参数
            \arg        ADC_INSERTED_CHANNEL_0: 插入通道0
            \arg        ADC_INSERTED_CHANNEL_1: 插入通道1
            \arg        ADC_INSERTED_CHANNEL_2: 插入通道2
            \arg        ADC_INSERTED_CHANNEL_3: 插入通道3
        \param[out] none
        \retval     返回转换结果值
*/
uint16_t adc_inserted_data_read(uint32_t adc_periph , uint8_t inserted_channel)
{
    uint32_t idata;
    /* read the data of the selected channel */
    switch(inserted_channel){
    case ADC_INSERTED_CHANNEL_0:
        idata = ADC_IDATA0(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_1:
        idata = ADC_IDATA1(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_2:
        idata = ADC_IDATA2(adc_periph);
        break;
    case ADC_INSERTED_CHANNEL_3:
        idata = ADC_IDATA3(adc_periph);
        break;
    default:
        idata = 0U;
        break;
    }
    return (uint16_t)idata;
}

/*!
    \brief      在同步模式下读取 ADC0/ADC1 的最新转换结果
    \param[in]  none
    \param[out] none
    \retval     返回转换结果值
*/
uint32_t adc_sync_mode_convert_value_read(void)
{
    /* return conversion value */
    return ADC_RDATA(ADC0);
}

/*!
        \brief      获取 ADC 标志位状态
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_flag: ADC 标志位
                                只能选择一个参数
            \arg        ADC_FLAG_WDE: 模拟看门狗事件标志
            \arg        ADC_FLAG_EOC: 规则组转换完成标志
            \arg        ADC_FLAG_EOIC: 插入组转换完成标志
            \arg        ADC_FLAG_STIC: 插入通道组开始标志
            \arg        ADC_FLAG_STRC: 规则通道组开始标志
        \param[out] none
        \retval     FlagStatus: SET 或 RESET
*/
FlagStatus adc_flag_get(uint32_t adc_periph , uint32_t adc_flag)
{
    FlagStatus reval = RESET;
    if(ADC_STAT(adc_periph) & adc_flag){
        reval = SET;
    }
    return reval;
}

/*!
        \brief      清除 ADC 标志位
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_flag: ADC 标志位
                                可选择一个或多个参数
            \arg        ADC_FLAG_WDE: 模拟看门狗事件标志
            \arg        ADC_FLAG_EOC: 规则组转换完成标志
            \arg        ADC_FLAG_EOIC: 插入组转换完成标志
            \arg        ADC_FLAG_STIC: 插入通道组开始标志
            \arg        ADC_FLAG_STRC: 规则通道组开始标志
        \param[out] none
        \retval     none
*/
void adc_flag_clear(uint32_t adc_periph , uint32_t adc_flag)
{
    ADC_STAT(adc_periph) = ~((uint32_t)adc_flag);
}

/*!
        \brief      获取 ADC 中断标志位
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_interrupt: ADC 中断标志位
                                只能选择一个参数
            \arg        ADC_INT_FLAG_WDE: 模拟看门狗中断
            \arg        ADC_INT_FLAG_EOC: 规则组转换完成中断
            \arg        ADC_INT_FLAG_EOIC: 插入组转换完成中断
        \param[out] none
        \retval     FlagStatus: SET 或 RESET
*/
FlagStatus adc_interrupt_flag_get(uint32_t adc_periph , uint32_t adc_interrupt)
{
    FlagStatus interrupt_flag = RESET;
    uint32_t state;
    /* check the interrupt bits */
    switch(adc_interrupt){
    case ADC_INT_FLAG_WDE:
        state = ADC_STAT(adc_periph) & ADC_STAT_WDE;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_WDEIE) && state){
          interrupt_flag = SET;
        }
        break;
    case ADC_INT_FLAG_EOC:
        state = ADC_STAT(adc_periph) & ADC_STAT_EOC;
          if((ADC_CTL0(adc_periph) & ADC_CTL0_EOCIE) && state){
            interrupt_flag = SET;
          }
        break;
    case ADC_INT_FLAG_EOIC:
        state = ADC_STAT(adc_periph) & ADC_STAT_EOIC;
        if((ADC_CTL0(adc_periph) & ADC_CTL0_EOICIE) && state){
            interrupt_flag = SET;
        }
        break;
    default:
        break;
    }
    return interrupt_flag;
}

/*!
        \brief      清除 ADC 中断标志
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_interrupt: ADC 状态/中断标志
                                可选择一个或多个参数
            \arg        ADC_INT_FLAG_WDE: 模拟看门狗中断
            \arg        ADC_INT_FLAG_EOC: 规则组转换完成中断
            \arg        ADC_INT_FLAG_EOIC: 插入组转换完成中断
        \param[out] none
        \retval     none
*/
void adc_interrupt_flag_clear(uint32_t adc_periph , uint32_t adc_interrupt)
{
    ADC_STAT(adc_periph) = ~((uint32_t)adc_interrupt);
}

/*!
        \brief      使能 ADC 中断
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_interrupt: 要使能的中断
                                可选择一个或多个参数
            \arg        ADC_INT_WDE: 模拟看门狗中断
            \arg        ADC_INT_EOC: 规则组转换完成中断
            \arg        ADC_INT_EOIC: 插入组转换完成中断
        \param[out] none
        \retval     none
*/
void adc_interrupt_enable(uint32_t adc_periph , uint32_t adc_interrupt)
{
    if(0U != (adc_interrupt & ADC_INT_WDE)){  
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_WDEIE;
    }  

    if(0U != (adc_interrupt & ADC_INT_EOC)){      
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_EOCIE;
    }  

    if(0U != (adc_interrupt & ADC_INT_EOIC)){      
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_EOICIE;
    }
}

/*!
        \brief      禁用 ADC 中断
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_interrupt: 要禁用的中断标志
                                可选择一个或多个参数
            \arg        ADC_INT_WDE: 模拟看门狗中断
            \arg        ADC_INT_EOC: 规则组转换完成中断
            \arg        ADC_INT_EOIC: 插入组转换完成中断
        \param[out] none
        \retval     none
*/
void adc_interrupt_disable(uint32_t adc_periph, uint32_t adc_interrupt)
{  
    if(0U != (adc_interrupt & ADC_INT_WDE)){  
        ADC_CTL0(adc_periph) &= ~(uint32_t) ADC_CTL0_WDEIE;
    }  

    if(0U != (adc_interrupt & ADC_INT_EOC)){      
        ADC_CTL0(adc_periph) &= ~(uint32_t) ADC_CTL0_EOCIE;
    }  

    if(0U != (adc_interrupt & ADC_INT_EOIC)){      
        ADC_CTL0(adc_periph) &= ~(uint32_t) ADC_CTL0_EOICIE;
    }
}

/*!
        \brief      配置 ADC 模拟看门狗为单通道监测
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel: 选择的 ADC 通道
                                只能选择其中一个参数
            \arg        ADC_CHANNEL_x: ADC 通道 x (x=0..17，16/17 仅 ADC0 可用)
        \param[out] none
        \retval     none
*/
void adc_watchdog_single_channel_enable(uint32_t adc_periph, uint8_t adc_channel)
{
    ADC_CTL0(adc_periph) &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC | ADC_CTL0_WDCHSEL);

    ADC_CTL0(adc_periph) |= (uint32_t)adc_channel;
    ADC_CTL0(adc_periph) |= (uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC);
}

/*!
        \brief      配置 ADC 模拟看门狗为组通道监测
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  adc_channel_group: 使用看门狗的通道组
                                只能选择一个参数
            \arg        ADC_REGULAR_CHANNEL: 规则通道组
            \arg        ADC_INSERTED_CHANNEL: 插入通道组
            \arg        ADC_REGULAR_INSERTED_CHANNEL: 同时监测规则与插入组
        \param[out] none
        \retval     none
*/
void adc_watchdog_group_channel_enable(uint32_t adc_periph, uint8_t adc_channel_group)
{
    ADC_CTL0(adc_periph) &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC);
    /* select the group */
    switch(adc_channel_group){
    case ADC_REGULAR_CHANNEL:
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_RWDEN;
        break;
    case ADC_INSERTED_CHANNEL:
        ADC_CTL0(adc_periph) |= (uint32_t) ADC_CTL0_IWDEN;
        break;
    case ADC_REGULAR_INSERTED_CHANNEL:
        ADC_CTL0(adc_periph) |= (uint32_t)(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN);
        break;
    default:
        break;
    }
}

/*!
    \brief      禁用 ADC 模拟看门狗
    \param[in]  adc_periph: ADCx, x=0,1,2
                只能选择其中一个参数
    \param[out] none
    \retval     none
*/
void adc_watchdog_disable(uint32_t adc_periph)
{
    ADC_CTL0(adc_periph) &= (uint32_t)~(ADC_CTL0_RWDEN | ADC_CTL0_IWDEN | ADC_CTL0_WDSC | ADC_CTL0_WDCHSEL);
}

/*!
    \brief      配置 ADC 模拟看门狗阈值
    \param[in]  adc_periph: ADCx, x=0,1,2
                只能选择其中一个参数
    \param[in]  low_threshold: 看门狗低阈值，范围 0..4095
    \param[in]  high_threshold: 看门狗高阈值，范围 0..4095
    \param[out] none
    \retval     none
*/
void adc_watchdog_threshold_config(uint32_t adc_periph , uint16_t low_threshold , uint16_t high_threshold)
{
    ADC_WDLT(adc_periph) = (uint32_t)WDLT_WDLT(low_threshold);
    ADC_WDHT(adc_periph) = (uint32_t)WDHT_WDHT(high_threshold);
}

/*!
        \brief      配置 ADC 过采样模式
        \param[in]  adc_periph: ADCx, x=0,1,2
                                只能选择其中一个参数
        \param[in]  mode: 过采样工作模式
                                只能选择一个参数
            \arg        ADC_OVERSAMPLING_ALL_CONVERT: 通道的所有过采样转换在一次触发后连续完成
            \arg        ADC_OVERSAMPLING_ONE_CONVERT: 每次过采样转换需要一次触发
        \param[in]  shift: 过采样右移位数
                                只能选择一个参数
            \arg        ADC_OVERSAMPLING_SHIFT_NONE: 无移位
            \arg        ADC_OVERSAMPLING_SHIFT_1B: 1 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_2B: 2 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_3B: 3 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_4B: 4 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_5B: 5 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_6B: 6 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_7B: 7 位移位
            \arg        ADC_OVERSAMPLING_SHIFT_8B: 8 位移位
        \param[in]  ratio: 过采样倍数
                                只能选择一个参数
            \arg        ADC_OVERSAMPLING_RATIO_MUL2: 倍数 2
            \arg        ADC_OVERSAMPLING_RATIO_MUL4: 倍数 4
            \arg        ADC_OVERSAMPLING_RATIO_MUL8: 倍数 8
            \arg        ADC_OVERSAMPLING_RATIO_MUL16: 倍数 16
            \arg        ADC_OVERSAMPLING_RATIO_MUL32: 倍数 32
            \arg        ADC_OVERSAMPLING_RATIO_MUL64: 倍数 64
            \arg        ADC_OVERSAMPLING_RATIO_MUL128: 倍数 128
            \arg        ADC_OVERSAMPLING_RATIO_MUL256: 倍数 256
        \param[out] none
        \retval     none
*/
void adc_oversample_mode_config(uint32_t adc_periph, uint32_t mode, uint16_t shift, uint8_t ratio)
{
    if(ADC_OVERSAMPLING_ONE_CONVERT == mode){
        ADC_OVSAMPCTL(adc_periph) |= (uint32_t)ADC_OVSAMPCTL_TOVS;
    }else{
        ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)ADC_OVSAMPCTL_TOVS);
    }
    /* config the shift and ratio */
    ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)(ADC_OVSAMPCTL_OVSR | ADC_OVSAMPCTL_OVSS));
    ADC_OVSAMPCTL(adc_periph) |= ((uint32_t)shift | (uint32_t)ratio);
}

/*!
    \brief      使能 ADC 过采样模式
    \param[in]  adc_periph: ADCx, x=0,1,2
                只能选择其中一个参数
    \param[out] none
    \retval     none
*/
void adc_oversample_mode_enable(uint32_t adc_periph)
{
    ADC_OVSAMPCTL(adc_periph) |= ADC_OVSAMPCTL_OVSEN;
}

/*!
    \brief      禁用 ADC 过采样模式
    \param[in]  adc_periph: ADCx, x=0,1,2
                只能选择其中一个参数
    \param[out] none
    \retval     none
*/
void adc_oversample_mode_disable(uint32_t adc_periph)
{
    ADC_OVSAMPCTL(adc_periph) &= ~((uint32_t)ADC_OVSAMPCTL_OVSEN);
}
