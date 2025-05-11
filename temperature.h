/*
 * temperature.h
 *
 *  Created on: 11 mai 2025
 *      Author: Chris
 */
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stdint.h>
#include <stdbool.h>

void temperature_init(void);
void temperature_deinit(void);
bool temperature_read_ble(int16_t *ble_temperature);

#endif /* TEMPERATURE_H_ */
