#ifndef BAREMETAL_MCP9804_H
#define BAREMETAL_MCP9804_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup mcp9804 MCP9804 - Digital Temperature Sensor
 * \{
 */

#include <stdint.h>
#include <bm/i2c.h>

#define MCP9804_BASE_ADDRESS 0x18

#define MCP9804_REGISTER_CONFIG 0x1
#define MCP9804_REGISTER_UPPER_TEMP 0x2
#define MCP9804_REGISTER_LOWER_TEMP 0x3
#define MCP9804_REGISTER_CRITICAL_TEMP 0x4
#define MCP9804_REGISTER_AMBIENT_TEMP 0x5
#define MCP9804_REGISTER_MANUFACTURER_ID 0x6
#define MCP9804_REGISTER_DEVICE_ID 0x7
#define MCP9804_REGISTER_RESOLUTION 0x8

#define MCP9804_CONFIG_SHDN (1 << 8)

#define MCP9804_RESOLUTION_0_5C 0x0
#define MCP9804_RESOLUTION_0_25C 0x1
#define MCP9804_RESOLUTION_0_125C 0x2
#define MCP9804_RESOLUTION_0_0625C 0x3

struct mcp9804
{
    struct i2c_client client;
};

int mcp9804_init_struct(struct i2c_adapter *adapter, struct mcp9804 *mcp9804, uint8_t address_pins_value);

int mcp9804_power_on(struct mcp9804 *mcp9804);
int mcp9804_power_off(struct mcp9804 *mcp9804);

int mcp9804_set_resolution(struct mcp9804 *mcp9804, uint8_t resolution);

int mcp9804_read_temperature(struct mcp9804 *mcp9804, float *temperature);

//! \} \}

#endif
