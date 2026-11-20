#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "RS485Parser.h"
#include "main.h"
#include "SensorObj.h"

#define MAX_PACKET_PAYLOAD 128

// Returns HAL_OK on success or the handler's HAL status on failure.
HAL_StatusTypeDef handleRS485Packet(const RS485Packet *packet, SensorObj *sensorArr[MAX_SENSOR_CT]);

#endif
