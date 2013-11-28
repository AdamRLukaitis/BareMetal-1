#ifndef BAREMETAL_SHT1x_H
#define BAREMETAL_SHT1x_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup sht1x SHT1x - Relative Humidity & Temperature Sensor
 * \{
 */

#include <stdint.h>

struct sht1x
{
    uint16_t data_gpio;
    uint16_t sck_gpio;
};

#define SHT1X_COMMAND_MEASURE_TEMP 0x03
#define SHT1X_COMMAND_MEASURE_HUMIDITY 0x05
#define SHT1X_COMMAND_WRITE_STATUS 0x07
#define SHT1X_COMMAND_READ_STATUS 0x07
#define SHT1X_COMMAND_RESET 0x1E

#define SHT1X_MEASUREMENT_T 0x0
#define SHT1X_MEASUREMENT_RH 0x1

#define SHT1X_RESOLUTION_14_BIT 0x0
#define SHT1X_RESOLUTION_12_BIT 0x1
#define SHT1X_RESOLUTION_8_BIT 0x2


int sht1x_init_struct(struct sht1x *sht1x, uint16_t sck_gpio, uint16_t data_gpio);

int sht1x_init_gpio(struct sht1x *sht1x);

int sht1x_start(struct sht1x *sht1x);
int sht1x_stop(struct sht1x *sht1x);

int sht1x_send(struct sht1x *sht1x, uint8_t byte);
int sht1x_receive(struct sht1x *sht1x, uint8_t *byte, uint8_t ack);

int sht1x_read_status(struct sht1x *sht1x, uint8_t *status);

int sht1x_read_measure(struct sht1x *sht1x, uint8_t measurement, uint16_t *value);

float sht1x_calc_humidity(uint16_t rh, float temp, int resolution);
float sht1x_calc_temp(uint16_t temp, int resolution, float d1);

//! \} \}

#endif

