#ifndef API_BME280_H
#define API_BME280_H

#include "API_common.h"

typedef enum {
    BME280_OK = 0,
    BME280_I2C_ERROR,
    BME280_BAD_DEVICE,
    BME280_NOT_INIT
} BME280_Status_t;

typedef struct {
    int32_t temperature_x100;   //centecimas de grado
    uint32_t pressure;          //Pa
    uint32_t humidity_x1024;    //humedad / 1024
} BME280_Data_t;

typedef struct {
    int32_t temperatureOff_x100;
    int32_t pressureOff;
    int32_t humidityOff_x1024;
} BME280_Offsets_t;

typedef struct {
    BME280_Data_t data;
    BME280_Offsets_t offsets;
} BME280_t;

typedef struct {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    uint8_t dig_h1;
    int16_t dig_h2;
    uint8_t dig_h3;
    int16_t dig_h4;
    int16_t dig_h5;
    int8_t dig_h6;
    int32_t t_fine;
} BME280_CalibData_t;

BME280_Status_t BME280_Init(void);

BME280_Status_t BME280_ReadData(BME280_Data_t *data);

void BME280_ReadCorrected(BME280_t *b);

#endif // API_BME280_H
