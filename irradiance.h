/*
 * irradiance.h
 *
 *  Created on: 2 juin 2025
 *      Author: Chris
 */

#ifndef IRRADIANCE_H
#define IRRADIANCE_H

#include <stdint.h>
#include <stdbool.h>

void irradiance_init(void);
void irradiance_deinit(void);
bool irradiance_read_ble(uint16_t *ble_irradiance);

#endif // IRRADIANCE_H
