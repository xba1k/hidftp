#include <Arduino.h>

#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "hidftp.h"
#include "hidgeneric.h"
#include "sdmmc_cmd.h"

#define USB_VENDOR "xba1k"
#define USB_SERIALNO "0000-0001"
#define USB_PRODUCT "Data Exfil PoC"
#define USB_VENDOR_ID 0xfeed
#define USB_PRODUCT_ID 0xc0de

#define TRANSFER_BUFSIZ 63

enum ftp_state_t { CMD_WAIT, LS_INPROGRESS, GET_INPROGRESS, PUT_INPROGRESS };
enum ftp_state_t ftp_state = CMD_WAIT;

HIDgeneric dev(HID_REPORT_TYPE_OUTPUT);
sdmmc_card_t *card;

#define SD_PIN_CLK GPIO_NUM_36  // SCK
#define SD_PIN_CMD GPIO_NUM_35  // MOSI
#define SD_PIN_D0 GPIO_NUM_37   // MISO

#define MOUNT_POINT "/sdcard"
const char mount_point[] = MOUNT_POINT;

esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 5,
    .allocation_unit_size = 16 * 1024};

class DataReceivedCallback : public HIDCallbacks {
  void onData(uint8_t report_id, hid_report_type_t report_type,
              uint8_t const *buffer, uint16_t bufsize) {
    char path[128];

    //Serial.printf("onData: report_id=%hhu report_type=%hhu size=%hu state=%hhu\r\n", report_id, report_type, bufsize, ftp_state);

    switch (ftp_state) {
      case CMD_WAIT: {
        char *cmd, *arg;
        int moreData = 0;
        parse_cmd(buffer, bufsize, &cmd, &arg);
        sprintf(path, "%s/%s", MOUNT_POINT, arg);

        if (strcmp(cmd, "ls") == 0) {
          ftp_state = LS_INPROGRESS;
          moreData =
              do_ls_r(strcmp(arg, ".") != 0 ? path : (char *)MOUNT_POINT);
        } else if (strcmp(cmd, "get") == 0) {
          ftp_state = GET_INPROGRESS;
          moreData = do_get_r(path);
        } else if (strcmp(cmd, "put") == 0) {
          ftp_state = PUT_INPROGRESS;
          moreData = do_put_r(path, buffer, bufsize);
        }

        if (!moreData) {
          ftp_state = CMD_WAIT;
        }

        break;
      }

      case LS_INPROGRESS:
        if (!do_ls_r(NULL)) {
          ftp_state = CMD_WAIT;
        }
        break;

      case GET_INPROGRESS:
        if (!do_get_r(NULL)) {
          ftp_state = CMD_WAIT;
        }
        break;

      case PUT_INPROGRESS:
        if (!do_put_r(NULL, buffer, bufsize)) {
          ftp_state = CMD_WAIT;
        }
        break;
    }
  };
};

void setup() {
  Serial.begin(115200);
  Serial.printf("Starting...\r\n");

  dev.manufacturer((char *)USB_VENDOR);
  dev.serial((char *)USB_SERIALNO);
  dev.product((char *)USB_PRODUCT);
  dev.deviceID(USB_VENDOR_ID, USB_PRODUCT_ID);
  dev.setCallbacks(new DataReceivedCallback());
  dev.begin();

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  slot_config.width = 1;

  slot_config.clk = SD_PIN_CLK;
  slot_config.cmd = SD_PIN_CMD;
  slot_config.d0 = SD_PIN_D0;

  if (esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config,
                              &card) != ESP_OK) {
    Serial.printf("Failed to mount the SD card\r\n");
  }

  Serial.printf("initialized...");
}

void loop() {
  delay(100);
  yield();
}
