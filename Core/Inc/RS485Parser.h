#ifndef RS485PARSER_H
#define RS485PARSER_H

#include <stdbool.h>
#include <stdint.h>

#define RS485_FRAME_CAPACITY 128U
#define RS485_BODY_MIN 16U
#define RS485_BODY_MAX 113U
#define RS485_SENSOR_SLOTS 16U
/* Inter-byte timeout in milliseconds; transport may reset explicitly on idle. */
#define RS485_TIMEOUT_MS 20U

typedef enum {
    RS485_FAILURE = 0x00, RS485_READ = 0x20,
    RS485_RESET = 0x3F, RS485_REBOOT = 0x15
} RS485Command;

typedef enum {
    PARSER_HEADER, PARSER_LENGTH, PARSER_BODY, PARSER_FIRST_MARKER,
    PARSER_CHECKSUM, PARSER_SECOND_MARKER, PARSER_DISCARD
} ParserState;

typedef struct {
    uint8_t header;             /* Original byte, including the low two bits. */
    RS485Command command;
    uint16_t bodyLength;
    uint32_t timeSinceLastReceive;
    uint32_t timeSent;
    uint8_t sensorIDs[RS485_SENSOR_SLOTS];
    uint8_t sensorData[RS485_BODY_MAX - RS485_BODY_MIN];
    uint16_t sensorDataLength;
} RS485Packet;

typedef struct {
    ParserState state;
    uint16_t rxIndex;
    uint8_t rxBuffer[RS485_FRAME_CAPACITY];
    uint16_t bodyLength;
    uint32_t lastByteTime;
    uint32_t checksum;
    RS485Packet packet;
} RS485Parser;

void initRS485Parser(RS485Parser *parser);
/* Clears partial reception; leaves the last decoded packet available. */
void resetRS485Parser(RS485Parser *parser);
/* True once per complete valid frame. Consume parser->packet before feeding
 * the next frame. Caller provides time through timeoutRS485Parser. */
bool feedRS485Parser(RS485Parser *parser, uint8_t byte);
/* Call before feeding each byte and while idle, using a monotonic ms clock.
 * Set lastByteTime to nowMs after feeding a byte.
 * Unsigned subtraction supports clock rollover. */
void timeoutRS485Parser(RS485Parser *parser, uint32_t nowMs);
/* Read big-endian 16-bit values from a six-byte sensor slot. False if absent. */
bool readRS485SensorValue(const RS485Packet *packet, uint8_t slot,
                          uint8_t component, uint16_t *value);

#endif
