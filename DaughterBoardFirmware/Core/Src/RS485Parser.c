#include "RS485Parser.h"
#include <stddef.h>
#include <string.h>

static const uint8_t marker[4] = {0xDE, 0xAD, 0xBE, 0xEF};

static uint32_t readU32(const uint8_t *bytes)
{
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
}

static bool validHeader(uint8_t byte)
{
    /* Match ARC: command is the upper six bits; low bits are preserved. */
    switch (byte >> 2) {
    case RS485_FAILURE: case RS485_READ: case RS485_RESET_START:
    case RS485_RESET_CONTINUE: case RS485_RESET_END: case RS485_REBOOT:
        return true;
    default:
        return false;
    }
}

void resetRS485Parser(RS485Parser *parser)
{
    parser->state = PARSER_HEADER;
    parser->rxIndex = 0;
    parser->bodyLength = 0;
    parser->checksum = 0;
}

void initRS485Parser(RS485Parser *parser)
{
    memset(parser, 0, sizeof(*parser));
    resetRS485Parser(parser);
}

void timeoutRS485Parser(RS485Parser *parser, uint32_t nowMs)
{
    if ((uint32_t)(nowMs - parser->lastByteTime) >= RS485_TIMEOUT_MS) {
        resetRS485Parser(parser);
    }
}

static void decodePacket(RS485Parser *parser)
{
    RS485Packet *packet = &parser->packet;
    const uint8_t *bytes = parser->rxBuffer;
    memset(packet, 0, sizeof(*packet));
    packet->header = bytes[0];
    packet->command = (RS485Command)(bytes[0] >> 2);
    packet->bodyLength = parser->bodyLength;
    packet->timeSinceLastReceive = readU32(bytes + 3);
    packet->timeSent = readU32(bytes + 7);
    for (uint8_t i = 0; i < 8; ++i) {
        packet->sensorIDs[2U * i] = bytes[11U + i] >> 4;
        packet->sensorIDs[2U * i + 1U] = bytes[11U + i] & 0x0FU;
    }
    packet->sensorDataLength = parser->bodyLength - RS485_BODY_MIN;
    memcpy(packet->sensorData, bytes + 19, packet->sensorDataLength);
}

bool feedRS485Parser(RS485Parser *parser, uint8_t byte)
{
    if (parser->state == PARSER_DISCARD) {
        return false; /* Recover on timeout or an explicit transport reset. */
    }
    if (parser->rxIndex >= RS485_FRAME_CAPACITY) {
        parser->state = PARSER_DISCARD;
        return false;
    }
    parser->rxBuffer[parser->rxIndex++] = byte;
    const uint16_t bodyEnd = (uint16_t)(3U + parser->bodyLength);

    switch (parser->state) {
    case PARSER_HEADER:
        parser->state = validHeader(byte) ? PARSER_LENGTH : PARSER_DISCARD;
        break;
    case PARSER_LENGTH:
        if (parser->rxIndex == 3U) {
            parser->bodyLength = (uint16_t)(
                ((uint16_t)parser->rxBuffer[1] << 8) | parser->rxBuffer[2]);
            if (parser->bodyLength < RS485_BODY_MIN ||
                parser->bodyLength > RS485_BODY_MAX) {
                parser->state = PARSER_DISCARD;
            } else {
                parser->state = PARSER_BODY;
            }
        }
        break;
    case PARSER_BODY:
        parser->checksum = (parser->checksum + byte) & 0xFFU;
        if (parser->rxIndex == bodyEnd) {
            parser->state = PARSER_FIRST_MARKER;
        }
        break;
    case PARSER_FIRST_MARKER:
        if (byte != marker[parser->rxIndex - bodyEnd - 1U]) {
            parser->state = PARSER_DISCARD;
        } else if (parser->rxIndex == bodyEnd + 4U) {
            parser->state = PARSER_CHECKSUM;
        }
        break;
    case PARSER_CHECKSUM:
        if (parser->rxIndex == bodyEnd + 8U) {
            /* ARC stores sum(body) modulo 256 in a four-byte BE field. */
            parser->state = readU32(parser->rxBuffer + bodyEnd + 4U) ==
                            parser->checksum ? PARSER_SECOND_MARKER : PARSER_DISCARD;
        }
        break;
    case PARSER_SECOND_MARKER:
        if (byte != marker[parser->rxIndex - bodyEnd - 9U]) {
            parser->state = PARSER_DISCARD;
        } else if (parser->rxIndex == bodyEnd + 12U) {
            decodePacket(parser);
            resetRS485Parser(parser);
            return true;
        }
        break;
    default:
        parser->state = PARSER_DISCARD;
        break;
    }
    return false;
}

bool readRS485SensorValue(const RS485Packet *packet, uint8_t slot,
                          uint8_t component, uint16_t *value)
{
    if (slot >= RS485_SENSOR_SLOTS || component >= 3U || value == NULL) {
        return false;
    }
    const uint16_t offset = (uint16_t)(6U * slot + 2U * component);
    if (packet->sensorIDs[slot] == 0U || offset + 2U > packet->sensorDataLength) {
        return false;
    }
    *value = (uint16_t)(((uint16_t)packet->sensorData[offset] << 8) |
                        packet->sensorData[offset + 1U]);
    return true;
}
