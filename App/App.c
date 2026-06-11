#include "App.h"
#include "BSP.h"
#include "ProtocolMbRtuSlaveCtrl.h"
#include "flash.h"

App_typedef App;
extern BSP_typedef Bsp;

#define ADC_MAX_TIMEOUT (160)
void app_main(void)
{
  HAL_Delay(100);
  bsp_init();
  app_init();
  HAL_Delay(100);
  BSP_PWR_TENZO_ON;
  while (1) // основной цикл
  {
    if (bsp_get_data_spi_ads1251(ADC_MAX_TIMEOUT) == SPI_ADC_OK)
    {
      app_adc_data_filter(Bsp.ADC_ADS1251.data_raw, ADC_ADS1251);
    }
    else
    {
      app_adc_data_filter(0, ADC_ADS1251);
    }
    
    if (bsp_get_data_spi_ads1231(ADC_MAX_TIMEOUT) == SPI_ADC_OK)
    {
      app_adc_data_filter(Bsp.ADC_ADS1231.data_raw, ADC_ADS1231);
    }
    else
    {
      app_adc_data_filter(0, ADC_ADS1231);
    }

    BSP_OUT_TOGGLE(BSP_LED_1);
    HAL_Delay(5);
  }
}

void app_init()
{
  protocolMbRtuSlaveCtrl_init(1);
  app_setupParam_init();
  app_adc_filter_init();
  return;
}

void app_setupParam_init()
{
  app_setupParam_setDefolt();
  app_flash_load();
  return;
}

void app_setupParam_setDefolt()
{
  // -------------------------- ADC_ADS1251 -------------------------- //
  App.setupParam.ADC_ADS1251_order = 5;
  App.setupParam.ADC_ADS1251_filterN = 20;
  App.setupParam.ADC_ADS1251_shift = 0.0f;
  App.setupParam.ADC_ADS1251_kMul = 1.0f;
  // ------------------------ ADC_ADS1251 END ------------------------ //

  // -------------------------- ADC_ADS1231 -------------------------- //
  App.setupParam.ADC_ADS1231_order = 5;
  App.setupParam.ADC_ADS1231_filterN = 20;
  App.setupParam.ADC_ADS1231_shift = 0.0f;
  App.setupParam.ADC_ADS1231_kMul = 1.0f;
  // ------------------------ ADC_ADS1231 END ------------------------ //

  App.setupParam.Upr_heating = HEATING_AUTO;
  return;
}

#define ADC_FILTERN_MAX (255)
#define ADC_FILTERN_MIN (1)

#define ADC_ORDER_MAX (PROGRAM_ADC_MAX_FILTER_ORDER)
#define ADC_ORDER_MIN (1)

void app_set_ADC_ADS1251_filterN(uint16_t value)
{
  if (value > ADC_FILTERN_MAX)
  {
    value = ADC_FILTERN_MAX;
  }
  else if (value < ADC_FILTERN_MIN)
  {
    value = ADC_FILTERN_MIN;
  }
  App.setupParam.ADC_ADS1251_filterN = value;
  return;
}

void app_set_ADC_ADS1251_order(uint16_t value)
{
  if (value > ADC_ORDER_MAX)
  {
    value = ADC_ORDER_MAX;
  }
  else if (value < ADC_ORDER_MIN)
  {
    value = ADC_ORDER_MIN;
  }
  App.setupParam.ADC_ADS1251_order = value;
  return;
}

void app_set_ADC_ADS1251_shift(uint16_t value)
{
  App.setupParam.ADC_ADS1251_shift = (float)value;
  return;
}

void app_set_ADC_ADS1251_kMul(float value)
{
  App.setupParam.ADC_ADS1251_kMul = value;
  return;
}

void app_set_ADC_ADS1231_filterN(uint16_t value)
{
  if (value > ADC_FILTERN_MAX)
  {
    value = ADC_FILTERN_MAX;
  }
  else if (value < ADC_FILTERN_MIN)
  {
    value = ADC_FILTERN_MIN;
  }
  App.setupParam.ADC_ADS1231_filterN = value;
  return;
}

void app_set_ADC_ADS1231_order(uint16_t value)
{
  if (value > PROGRAM_ADC_MAX_FILTER_ORDER)
  {
    value = PROGRAM_ADC_MAX_FILTER_ORDER;
  }
  else if (value < ADC_ORDER_MIN)
  {
    value = ADC_ORDER_MIN;
  }
  App.setupParam.ADC_ADS1231_order = value;
  return;
}

void app_set_ADC_ADS1231_shift(uint16_t value)
{
  App.setupParam.ADC_ADS1231_shift = (float)value;
  return;
}

void app_set_ADC_ADS1231_kMul(float value)
{
  App.setupParam.ADC_ADS1231_kMul = value;
  return;
}

void app_set_Upr_heating(uint16_t value)
{
  if (value == HEATING_AUTO)
  {
    App.setupParam.Upr_heating = HEATING_AUTO;
  }
  else if (value == HEATING_MANUAL)
  {
    App.setupParam.Upr_heating = HEATING_MANUAL;
  }
  else
  {
    // Ошибка
    asm("Nop");
  }

  return;
}

void app_set_DO_contol(uint16_t value)
{
  if (App.setupParam.Upr_heating == HEATING_AUTO)
  {
    asm("Nop");
  }
  else if (App.setupParam.Upr_heating == HEATING_MANUAL)
  {
    if (BSP_GET_BIT(value, 0) == 1)
    {
      BSP_SET_BIT(App.DO_control, 0);
    }
    else
    {
      BSP_RESET_BIT(App.DO_control, 0);
    }
  }
  else
  {
    // Ошибка
    asm("Nop");
  }
  return;
}

void app_adc_filter_init()
{
  // -------------------------- ADC_ADS1251 -------------------------- //
  App.adc_filter[ADC_ADS1251].value = 0.0f;
  App.adc_filter[ADC_ADS1251].value_last = 0.0f;
  App.adc_filter[ADC_ADS1251].valueRaw = 0.0f;
  for (uint8_t j = 0; j < PROGRAM_ADC_MAX_FILTER_ORDER; j++)
  {
    App.adc_filter[ADC_ADS1251].buf[j] = 0.0f;
  }
  App.adc_filter[ADC_ADS1251].bufIdx = 0;
  App.adc_filter[ADC_ADS1251].filter_N = App.setupParam.ADC_ADS1251_filterN;
  App.adc_filter[ADC_ADS1251].order = App.setupParam.ADC_ADS1251_order;
  App.adc_filter[ADC_ADS1251].shift = 0.0f;
  App.adc_filter[ADC_ADS1251].kMul = 1.0f;
  // ------------------------ ADC_ADS1251 END ------------------------ //

  // -------------------------- ADC_ADS1231 -------------------------- //
  App.adc_filter[ADC_ADS1231].value = 0.0f;
  App.adc_filter[ADC_ADS1231].value_last = 0.0f;
  App.adc_filter[ADC_ADS1231].valueRaw = 0.0f;
  for (uint8_t j = 0; j < PROGRAM_ADC_MAX_FILTER_ORDER; j++)
  {
    App.adc_filter[ADC_ADS1231].buf[j] = 0.0f;
  }
  App.adc_filter[ADC_ADS1231].bufIdx = 0;
  App.adc_filter[ADC_ADS1231].filter_N = App.setupParam.ADC_ADS1231_filterN;
  App.adc_filter[ADC_ADS1231].order = App.setupParam.ADC_ADS1231_order;
  App.adc_filter[ADC_ADS1231].shift = 0.0f;
  App.adc_filter[ADC_ADS1231].kMul = 1.0f;
  // ------------------------ ADC_ADS1231 END ------------------------ //
  return;
}

#define T_BOARD_ON_HEATING_AUTO    (20.0)
#define T_BOARD_OFF_HEATING_AUTO   (30.0)
#define T_BOARD_OFF_HEATING_ALWAYS (40.0)
void bsp_tim7_100ms_callback()
{
  app_update_reg();
  protocolMbRtuSlaveCtrl_update_tables();

  // Управление нагревом в режиме AUTO
  if (App.setupParam.Upr_heating == HEATING_AUTO)
  {
    if (Bsp.AI.NTC[NTC_pcb].value <= T_BOARD_ON_HEATING_AUTO)
    {
      BSP_SET_BIT(App.DO_control, 0);
    }
    else if (Bsp.AI.NTC[NTC_pcb].value >= T_BOARD_OFF_HEATING_AUTO)
    {
      BSP_RESET_BIT(App.DO_control, 0);
    }
  }

  // Общее управление нагревом
  if (Bsp.AI.NTC[NTC_pcb].value < T_BOARD_OFF_HEATING_ALWAYS)
  {
    if (BSP_GET_BIT(App.DO_control, 0) == 1)
    {
      BSP_TERM_OUT_ON;
    }
    else if (BSP_GET_BIT(App.DO_control, 0) == 0)
    {
      BSP_TERM_OUT_OFF;
    }
  }
  else
  {
    BSP_RESET_BIT(App.DO_control, 0);
    BSP_TERM_OUT_OFF;
  }

  return;
}

#define K_X10 (float)(10.0f)
#define K_X1000 (float)(1000.0f)
#define K_X10000 (float)(10000.0f)
void app_update_reg()
{
  // --- ADC_ADS1251
  App.ADC_ADS1251.data_i16_x100  = (int16_t)(App.adc_filter[ADC_ADS1251].value * K_X1000);    // [мВ*100]
  App.ADC_ADS1251.data_i16_x1000 = (int16_t)(App.adc_filter[ADC_ADS1251].value * K_X10000);   // [мВ*1000]

  // --- ADC_ADS1231
  App.ADC_ADS1231.data_i16_x100  = (int16_t)(App.adc_filter[ADC_ADS1231].value * K_X1000);    // [мВ*100]
  App.ADC_ADS1231.data_i16_x1000 = (int16_t)(App.adc_filter[ADC_ADS1231].value * K_X10000);   // [мВ*1000]

  // --- DI_rele_state
  App.DI_rele_state = bsp_get_rele_state();
  
  return;
}

#define ADC_ADS1251_MAX_VAL (float)(4194303.0f)
#define ADC_ADS1251_REF_VOLT (float)(2.5f)
#define ADC_ADS1251_REF_K (float)(0.654761905f)

#define ADC_ADS1231_MAX_VAL (float)(8388607.0f)
#define ADC_ADS1231_REF_VOLT (float)(2.5f)
#define ADC_ADS1231_K (float)(0.757257047f)

void app_adc_data_filter(uint32_t ADC_Buf_raw, ADC_enum adc)
{
  float value = 0.0f;
  float valueLast = 0.0f;
  float kFilter = 0.0f;
  float data = 0.0f;
  float sum = 0.0f;

  if (adc == ADC_ADS1251)
  {
    // Положительное напряжение на входе АЦП
    if (ADC_Buf_raw <= (uint32_t)ADC_ADS1251_MAX_VAL)
    {
      data = (ADC_ADS1251_MAX_VAL - (float)ADC_Buf_raw)/ADC_ADS1251_MAX_VAL*ADC_ADS1251_REF_VOLT*ADC_ADS1251_REF_K;
    }
    // Отрицательное напряжение на входе АЦП
    else
    {
      data = (((float)ADC_Buf_raw - (ADC_ADS1251_MAX_VAL))*(-1))/(ADC_ADS1251_MAX_VAL)*ADC_ADS1251_REF_VOLT*ADC_ADS1251_REF_K;
    }
  }
  else if (adc == ADC_ADS1231)
  {
    // Положительное напряжение на входе АЦП
    if ((float)ADC_Buf_raw <= ADC_ADS1231_MAX_VAL)
    {
      data = (float)ADC_Buf_raw / ADC_ADS1231_MAX_VAL*ADC_ADS1231_REF_VOLT*ADC_ADS1231_K;
    }
    // Отрицательное напряжение на входе АЦП
    else
    {
      data = ((float)ADC_Buf_raw - 2*ADC_ADS1231_MAX_VAL)/ADC_ADS1231_MAX_VAL*ADC_ADS1231_REF_VOLT*ADC_ADS1231_K;
    }
  }

  App.adc_filter[adc].buf[App.adc_filter[adc].bufIdx++] = data;

  if (App.adc_filter[adc].bufIdx == App.adc_filter[adc].order)
  {
    App.adc_filter[adc].bufIdx = 0;
  }

  for (uint8_t idx = 0; idx < App.adc_filter[adc].order; idx++)
  {
    sum += App.adc_filter[adc].buf[idx];
  }

  App.adc_filter[adc].valueRaw = sum / (float)App.adc_filter[adc].order;

  value = (App.adc_filter[adc].valueRaw - App.adc_filter[adc].shift) * App.adc_filter[adc].kMul;

  valueLast = App.adc_filter[adc].value_last;

  kFilter = 2.0f / ((float)App.adc_filter[adc].filter_N + 1.0f);

  value = valueLast + kFilter * (value - valueLast);

  App.adc_filter[adc].value = App.adc_filter[adc].value_last = value;
}

#define PROGRAM_PARAM_SIZE_BYTE sizeof(setupParam_typedef)
void app_flash_save()
{
  write_Flash((void *)(&App.setupParam), PROGRAM_PARAM_SIZE_BYTE);
  asm("Nop");
  NVIC_SystemReset();
}

void app_flash_load()
{
  readFlash ((void *)(&App.setupParam), PROGRAM_PARAM_SIZE_BYTE);
  asm("Nop");
}

void App_reset()
{
  asm("Nop");
  NVIC_SystemReset();
}