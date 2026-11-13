#ifndef MUX_H
#define MUX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "main.h"

#define MUX_ADDR_7BIT 0x70U
#define MUX_TIMEOUT_MS 100U


static inline HAL_StatusTypeDef MuxWriteMask(I2C_HandleTypeDef* i2cHandle, uint8_t mask) {
  // Write to the mux
  return HAL_I2C_Master_Transmit (
    i2cHandle,
    (uint16_t)(MUX_ADDR_7BIT << 1),
    &mask,
    1,
    MUX_TIMEOUT_MS
  );
}

static inline HAL_StatusTypeDef MuxSelectChannel(I2C_HandleTypeDef* i2cHandle, uint8_t channel) {
  // Set the channel of the mux
  if (channel > 3U) {
    return HAL_ERROR;
  }

  return MuxWriteMask(i2cHandle, (uint8_t)(1U << channel));
}

static inline HAL_StatusTypeDef MuxInit(I2C_HandleTypeDef* i2cHandle) {
    HAL_GPIO_WritePin(MUX_RESET_GPIO_Port, MUX_RESET_Pin, GPIO_PIN_RESET);

    HAL_Delay(1);

    HAL_GPIO_WritePin(MUX_RESET_GPIO_Port, MUX_RESET_Pin, GPIO_PIN_SET);

    HAL_Delay(1);

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        i2cHandle,
        (uint16_t)(MUX_ADDR_7BIT << 1),
        3U,
        MUX_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return status;
    }

    return MuxWriteMask(i2cHandle, 0);
}

#endif /* MUX_H */
