#include "flash.h"


typedef union
{
  uint8_t bytes[8];
  uint64_t dword;
} packDword_typedef;

void write_Flash(void *s, uint16_t sizeByte)
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  /* Get the bank */
  uint32_t PAGEError;

  /* Fill EraseInit structure*/
  FLASH_EraseInitTypeDef EraseInitStruct;
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages = 1;
  EraseInitStruct.Banks = FLASH_BANK_2;
  EraseInitStruct.Page = 127;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    /*
      Error occurred while sector erase.
      User can add here some code to deal with this error.
      SECTORError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
     */
    Error_Handler(); // err
  }

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  uint32_t dWordStructSize = sizeByte / 8;
  if (sizeByte % 8 != 0)
  {
    dWordStructSize += 1;
  }

  for (uint8_t w64 = 0; w64 < dWordStructSize; w64++)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_WORKER_START_AREA + 8 * w64, *((uint64_t *)(s) + w64)) != HAL_OK)
    {
      Error_Handler(); // err
    }
  }
  HAL_FLASH_Lock();
  return;
}

void readFlash(void *s, uint16_t sizeByte)
{

  uint8_t *loadArea = NULL;
  loadArea = (uint8_t *)FLASH_WORKER_START_AREA;
  for (uint8_t i = 0; i < sizeByte; i++)
  {
    *((uint8_t *)(s) + i) = *(loadArea + i);
  }
  return;
}