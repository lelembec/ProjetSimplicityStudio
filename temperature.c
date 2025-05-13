/*
 * temperature.c
 *
 *  Created on: 11 mai 2025
 *      Author: Chris
 */

#include "sl_sensor_rht.h"
#include "app_log.h"
#include "temperature.h"

void temperature_init(void) {
  sl_sensor_rht_init();
  app_log_info("Capteur temperature initialise\n");
}

void temperature_deinit(void) {
  sl_sensor_rht_deinit();
  app_log_info("Capteur temperature desinitialise\n");
}

bool temperature_read_ble(int16_t *ble_temperature) {
  uint32_t rh;
  int32_t temp_milli_celsius;
  sl_status_t status = sl_sensor_rht_get(&rh, &temp_milli_celsius);

  if (status == SL_STATUS_OK) {
    // Convert from milli°C to BLE format (0.01°C)
    *ble_temperature = (int16_t)(temp_milli_celsius / 10);
    app_log_info("Temperature : %ld (x0.0001) \xB0 C => BLE : %d (x0.001) \xB0 C)\n",
                temp_milli_celsius, *ble_temperature);
    return true;
  } else {
    app_log_warning("Erreur de lecture capteur : %lu\n", status);
    return false;
  }
}


