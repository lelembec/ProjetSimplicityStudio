/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"
#include "temperature.h"
#include "gatt_db.h"


// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
float temp;
float rh;
int16_t ble_temp;


/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  app_log_info("%s\n", __FUNCTION__);
  temperature_init();
}


/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}


 SL_WEAK void app_deinit(void)
 {
   app_log_info("%s\n", __FUNCTION__);
   temperature_deinit();
 }

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log_info("%s: Connection_Opened!\n",__FUNCTION__);

      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("%s: Connection_Closed!\n",__FUNCTION__);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////
    ///
    /*case sl_bt_evt_gatt_server_user_read_request_id:
    {
      app_log_info("Demande de lecture de temperature\n");
      if (evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_temperature){
          if (temperature_read_ble(&ble_temp)) {
                  uint8_t connection = evt->data.evt_gatt_server_user_read_request.connection;
                  uint16_t characteristic = evt->data.evt_gatt_server_user_read_request.characteristic;

                  int16_t int_part = ble_temp / 10;
                  int16_t frac_part = (ble_temp % 10);

                  uint8_t temp_data[2];
                  temp_data[0] = (uint8_t)(ble_temp & 0xFF);
                  temp_data[1] = (uint8_t)((ble_temp >> 8) & 0xFF);

                  app_log_info("Temperature = %d.%d \xB0 C\n", int_part, frac_part);

                  sc = sl_status_t sl_bt_gatt_server_send_user_read_response(connection,characteristic,0,sizeof(temp_data),temp_data,NULL);

                }
      }
      break;


    }*/
    case sl_bt_evt_gatt_server_user_read_request_id:
    {
      app_log_info("Demande de lecture de temperature\n");

      if (evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_temperature) {
        int16_t ble_temp;

        if (temperature_read_ble(&ble_temp)) {
          uint8_t connection = evt->data.evt_gatt_server_user_read_request.connection;
          uint16_t characteristic = evt->data.evt_gatt_server_user_read_request.characteristic;

          int16_t int_part = ble_temp / 100;
          int16_t frac_part = (ble_temp % 100);

          int16_t temp_data[2]= {0};
          memcpy(temp_data,&ble_temp,sizeof(int16_t));

          app_log_info("Temperature = %d.%d\xB0""C\n", int_part,frac_part);


          sl_status_t sc = sl_bt_gatt_server_send_user_read_response(
            connection,
            characteristic,
            0,
            sizeof(temp_data),
            (uint8_t*)temp_data,
            NULL
          );

          app_log_info("Temperature envoye = %d (%.2f\xB0""C), status = 0x%04X\n",
                       ble_temp, (double)(ble_temp % 10), (unsigned int)sc);
        } else {
          app_log_warning("Erreur capteur : lecture température echouee\n");
        }
      }

      break;
    }



    break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}
