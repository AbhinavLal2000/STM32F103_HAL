#ifndef __NOKIA_H__
#define __NOKIA_H__

#include "main.h"

#define FONT_SIZE 5

typedef struct
{
    SPI_HandleTypeDef *h;
    GPIO_TypeDef *DC_port;
    uint16_t DC_pin;
    GPIO_TypeDef *RST_port;
    uint16_t RST_pin;
    GPIO_TypeDef *CS_port;
    uint16_t CS_pin;

} nokia_t;

void n_spi_tx(nokia_t*, uint8_t);
void n_init(nokia_t*);
void n_pos(nokia_t*, uint16_t, uint16_t);
void n_clear(nokia_t*);
void n_print(nokia_t*, uint16_t, uint16_t, char*);
void n_raw_print(nokia_t*, char);

#endif