#include "PacketHandler.h"
#include "I2C.h"

#include <stddef.h>

static HAL_StatusTypeDef compileGenericSections(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{


  return HAL_ERROR;
}

static HAL_StatusTypeDef handleRead(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{
  // Start by pulling data for each sensor in the list
  for (size_t slot = 0; slot < RS485_SENSOR_SLOTS; slot++) {
    const uint8_t requestedId = packet->sensorIDs[slot];
    if (requestedId == 0U) {
      continue;
    }

    SensorObj *sensor = NULL;
    for (size_t i = 0; i < MAX_SENSOR_CT; i++) {
      if (sensorArr[i] != NULL && sensorArr[i]->packetId == requestedId) {
        sensor = sensorArr[i];
        break;
      }
    }
    if (sensor == NULL) {
      return HAL_ERROR;
    }

    const HAL_StatusTypeDef status = getSensorReading(sensor);
    if (status != HAL_OK) {
      // Set all 3 to be bad data
      sensor->recentData[0] =
      sensor->recentData[1] =
      sensor->recentData[2] = SENSOR_BAD_DATA;
    }
  }

  // If all readings have data with actual sensor,
  // complete the generic packet comp.
  HAL_StatusTypeDef status = compileGenericSections(packet, sensorArr);


  return HAL_OK;
}

static HAL_StatusTypeDef handleReset(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{
  return HAL_ERROR;
}

static HAL_StatusTypeDef handleReboot(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{
  return HAL_ERROR;
}

static HAL_StatusTypeDef handleFailure(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{
  return HAL_ERROR;
}

HAL_StatusTypeDef handleRS485Packet(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT])
{
  // 
  uint8_t sendPacket[MAX_PACKET_PAYLOAD];

  HAL_StatusTypeDef result = HAL_OK;
  switch (packet->command) {
    case RS485_READ:
      result = handleRead(packet, sensorArr);
    case RS485_RESET:
      result = handleReset(packet, sensorArr);
    case RS485_REBOOT:
      result = handleReboot(packet, sensorArr);
    case RS485_FAILURE:
      result = handleFailure(packet, sensorArr);
    default:
      result = HAL_ERROR;
  }

  // If any of the handles have a critical failure,
  // rewrite with a failure
  if (result != HAL_OK) {
    handleFailure(packet, sensorArr);
  }
}
