#ifndef SENSOR_OBJ
#define SENSOR_OBJ

#define SENSOR_NONETYPE 0xFF
#define SENSOR_BAD_DATA 0xFF

typedef enum {
    LIMIT_SWITCH,
    COLOR_SENSOR
} SensorType;

typedef struct {
    uint8_t muxChannel;
    uint8_t packetId;
    uint16_t recentData[3];
    SensorType type;
} SensorObj;


#endif