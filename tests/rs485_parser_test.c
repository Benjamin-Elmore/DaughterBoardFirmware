#include "RS485Parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned makeFrame(uint8_t *b, unsigned length, unsigned command)
{
    static const uint8_t marker[] = {0xDE, 0xAD, 0xBE, 0xEF};
    memset(b, 0, 128);
    b[0] = (uint8_t)((command << 2) | 2);
    b[1] = (uint8_t)(length >> 8); b[2] = (uint8_t)length;
    for (unsigned i = 3; i < length + 3; ++i) b[i] = (uint8_t)i;
    unsigned sum = 0;
    for (unsigned i = 3; i < length + 3; ++i) sum += b[i];
    memcpy(b + length + 3, marker, 4);
    b[length + 10] = (uint8_t)sum;
    memcpy(b + length + 11, marker, 4);
    return length + 15;
}
static unsigned feed(RS485Parser *p, const uint8_t *b, unsigned n)
{
    unsigned count = 0;
    for (unsigned i = 0; i < n; ++i) count += feedRS485Parser(p, b[i]);
    return count;
}
int main(void)
{
    RS485Parser p;
    uint8_t b[128];
    const unsigned commands[] = {0, 0x20, 0x3F, 0x3E, 0x2A, 0x15};
    for (unsigned c = 0; c < 6; ++c) {
        for (unsigned length = 16; length <= 113; ++length) {
            initRS485Parser(&p);
            unsigned n = makeFrame(b, length, commands[c]);
            assert(feed(&p, b, n) == 1);
            assert((unsigned)p.packet.command == commands[c]);
            assert(p.packet.bodyLength == length);
            assert(p.packet.timeSent == 0x0708090AU);
            assert(p.packet.sensorIDs[0] == 0 && p.packet.sensorIDs[1] == 11);
            assert(p.packet.sensorDataLength == length - 16);
            assert(feed(&p, b, n) == 1); /* Back-to-back frames. */
        }
    }
    unsigned n = makeFrame(b, 112, RS485_READ);
    for (unsigned i = 115; i < n; ++i) {
        initRS485Parser(&p);
        b[i] ^= 1;
        assert(feed(&p, b, n) == 0); /* Every trailer byte. */
        b[i] ^= 1;
        timeoutRS485Parser(&p, RS485_TIMEOUT_MS);
        assert(feed(&p, b, n) == 1);
    }
    initRS485Parser(&p);
    b[20] ^= 1; assert(feed(&p, b, n) == 0); b[20] ^= 1;
    for (unsigned length = 0; length < 65536; ++length) {
        if (length >= 16 && length <= 113) continue;
        initRS485Parser(&p);
        feedRS485Parser(&p, 0x82);
        feedRS485Parser(&p, (uint8_t)(length >> 8));
        feedRS485Parser(&p, (uint8_t)length);
        assert(p.state == PARSER_DISCARD);
    }
    initRS485Parser(&p);
    assert(!feedRS485Parser(&p, 0x04)); assert(p.state == PARSER_DISCARD);
    timeoutRS485Parser(&p, 20);
    p.lastByteTime = 20;
    assert(feed(&p, b, 7) == 0);
    timeoutRS485Parser(&p, 39); assert(p.state != PARSER_HEADER);
    timeoutRS485Parser(&p, 40); assert(p.state == PARSER_HEADER);
    p.lastByteTime = UINT32_MAX - 9;
    timeoutRS485Parser(&p, 10); assert(p.state == PARSER_HEADER);
    assert(feed(&p, b, n) == 1);
    uint16_t value;
    assert(readRS485SensorValue(&p.packet, 1, 0, &value));
    assert(value == 0x191AU);
    assert(!readRS485SensorValue(&p.packet, 16, 0, &value));
    assert(!readRS485SensorValue(&p.packet, 1, 3, &value));
    assert(!readRS485SensorValue(&p.packet, 1, 0, NULL));
    puts("Parser tests passed: all commands and lengths, corruption, recovery, bounds.");
    return 0;
}
