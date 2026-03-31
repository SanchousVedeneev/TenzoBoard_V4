#ifndef __FLASH__H__
#define __FLASH__H__

#include "main.h"

#define FLASH_WORKER_START_AREA     ((uint32_t)0x0807F800)  //128 page 2 bank
#define FLASH_WORKER_SIZE_AREA      ((uint32_t)2048) // 2 kByte

void write_Flash (void *s, uint16_t sizeByte);

void readFlash (void *s, uint16_t sizeByte);

#endif