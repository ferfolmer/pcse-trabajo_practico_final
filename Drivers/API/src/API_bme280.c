#include "API_bme280.h"
#include "API_i2c.h"

/*! @name Register Address */
#define BME280_REG_CHIP_ID                        (0xD0)
#define BME280_REG_RESET                          (0xE0)
#define BME280_REG_TEMP_PRESS_CALIB_DATA          (0x88)
#define BME280_REG_HUMIDITY_CALIB_DATA            (0xE1)
#define BME280_REG_CTRL_HUM                       (0xF2)
#define BME280_REG_STATUS                         (0xF3)
#define BME280_REG_PWR_CTRL                       (0xF4)
#define BME280_REG_CTRL_MEAS                      (0xF4)
#define BME280_REG_CONFIG                         (0xF5)
#define BME280_REG_DATA                           (0xF7)

/*! @name Macros related to size */
#define BME280_LEN_TEMP_PRESS_CALIB_DATA          (26)
#define BME280_LEN_HUMIDITY_CALIB_DATA            (7)
#define BME280_LEN_P_T_H_DATA                     (8)

#define CHIP_ID                                     0x60U
#define SOFT_RESET_CMD                              0xB6U

/* ────────── oversampling (1x)────────── */
#define OSRS_T            1U
#define OSRS_P            1U
#define OSRS_H            1U
#define MODE_NORMAL       3U
#define BME280_I2C_ADDRESS (0x76 << 1) // Dirección I2C del BME280
#define BME280_I2C_ADDRESS_SEC (0x77 << 1)

static uint8_t            device_addr_;
static bool_t             is_init_ = false;
static BME280_CalibData_t calib_;
static int32_t            t_fine_;

static BME280_Status_t BME280_write_reg(uint8_t reg, uint8_t val);
static BME280_Status_t BME280_read_buf(uint8_t reg, uint8_t *buf, uint16_t n);
static BME280_Status_t BME280_get_calib_data(void);

static int32_t  compensate_temp(int32_t adc_T);
static uint32_t compensate_press(int32_t adc_P);
static uint32_t compensate_hum (int32_t adc_H);


/* ====================================================== */
BME280_Status_t BME280_Init(void)
{
    if (I2C_isInit() != I2C_OK && I2C_Init() != I2C_OK)
        return BME280_I2C_ERROR;

    uint8_t chip_id;
    device_addr_ = BME280_I2C_ADDRESS;
    if (BME280_read_buf(BME280_REG_CHIP_ID, &chip_id, 1) != BME280_OK || chip_id != CHIP_ID)
    {
        device_addr_ = BME280_I2C_ADDRESS_SEC;
        if (BME280_read_buf(BME280_REG_CHIP_ID, &chip_id, 1) != BME280_OK || chip_id != CHIP_ID)
        {
            return BME280_BAD_DEVICE;
        }
    }

    if (BME280_write_reg(BME280_REG_RESET, SOFT_RESET_CMD) != BME280_OK)
    {
        return BME280_I2C_ERROR;
    }

    uint8_t st;
    do {
        if (BME280_read_buf(BME280_REG_STATUS, &st, 1) != BME280_OK)
        {
            return BME280_I2C_ERROR;
        }

    } while (st & 0x01);

    if (BME280_get_calib_data() != BME280_OK)
        return BME280_I2C_ERROR;

    if (BME280_write_reg(BME280_REG_CTRL_HUM,  OSRS_H) != BME280_OK)
    {
        return BME280_I2C_ERROR;
    }

    if (BME280_write_reg(BME280_REG_CTRL_MEAS, (OSRS_T<<5) | (OSRS_P<<2) | MODE_NORMAL) != BME280_OK)
    {
        return BME280_I2C_ERROR;
    }

    is_init_ = true;
    return BME280_OK;
}

BME280_Status_t BME280_ReadData(BME280_Data_t *out)
{
    if (!is_init_) return BME280_NOT_INIT;

    uint8_t raw[BME280_LEN_P_T_H_DATA];

    if (BME280_read_buf(BME280_REG_DATA, raw, BME280_LEN_P_T_H_DATA) != BME280_OK)
    {
        return BME280_I2C_ERROR;
    }

    int32_t adc_P = (raw[0]<<12) | (raw[1]<<4) | (raw[2]>>4);
    int32_t adc_T = (raw[3]<<12) | (raw[4]<<4) | (raw[5]>>4);
    int32_t adc_H = (raw[6]<<8)  |  raw[7];

    out->temperature_x100 = compensate_temp(adc_T);
    out->pressure	      = compensate_press(adc_P);
    out->humidity_x1024   = compensate_hum(adc_H);

    
    return BME280_OK;
}


/* ====================================================== */

static BME280_Status_t BME280_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2] = { reg, val };
    return (I2C_Send(BME280_I2C_ADDRESS, tx, 2) == I2C_OK) ? BME280_OK : BME280_I2C_ERROR;
}

static BME280_Status_t BME280_read_buf(uint8_t reg, uint8_t *buf, uint16_t n)
{
    if (I2C_Send(BME280_I2C_ADDRESS, &reg, 1) != I2C_OK) return BME280_I2C_ERROR;
    return (I2C_Receive(BME280_I2C_ADDRESS, buf, n) == I2C_OK) ? BME280_OK : BME280_I2C_ERROR;
}

static BME280_Status_t BME280_get_calib_data(void)
{
    uint8_t tp[BME280_LEN_TEMP_PRESS_CALIB_DATA];
    uint8_t h [BME280_LEN_HUMIDITY_CALIB_DATA];

    if (BME280_read_buf(BME280_REG_TEMP_PRESS_CALIB_DATA, tp, sizeof(tp)) != BME280_OK) return BME280_I2C_ERROR;
    if (BME280_read_buf(BME280_REG_HUMIDITY_CALIB_DATA  , h , sizeof(h )) != BME280_OK) return BME280_I2C_ERROR;

    calib_.dig_t1 = (tp[1]<<8)|tp[0];   calib_.dig_t2 = (tp[3]<<8)|tp[2];   calib_.dig_t3 = (tp[5]<<8)|tp[4];
    calib_.dig_p1 = (tp[7]<<8)|tp[6];   calib_.dig_p2 = (tp[9]<<8)|tp[8];   calib_.dig_p3 = (tp[11]<<8)|tp[10];
    calib_.dig_p4 = (tp[13]<<8)|tp[12]; calib_.dig_p5 = (tp[15]<<8)|tp[14]; calib_.dig_p6 = (tp[17]<<8)|tp[16];
    calib_.dig_p7 = (tp[19]<<8)|tp[18]; calib_.dig_p8 = (tp[21]<<8)|tp[20]; calib_.dig_p9 = (tp[23]<<8)|tp[22];

    calib_.dig_h1 = tp[25];
    calib_.dig_h2 = (h[1]<<8)|h[0];
    calib_.dig_h3 = h[2];
    calib_.dig_h4 = (h[3]<<4) | (h[4] & 0x0F);
    calib_.dig_h5 = (h[5]<<4) | (h[4] >> 4);
    calib_.dig_h6 = (int8_t)h[6];
    return BME280_OK;
}

static int32_t compensate_temp(int32_t adc_T)
{
    int32_t var1 = ((((adc_T>>3) - ((int32_t)calib_.dig_t1<<1))) * calib_.dig_t2) >> 11;
    int32_t var2 = (((((adc_T>>4) - calib_.dig_t1)*((adc_T>>4)-calib_.dig_t1))>>12) * calib_.dig_t3) >> 14;
    t_fine_ = var1 + var2;
    return (t_fine_ * 5 + 128) >> 8;                     /* °C ×100 */
}

static uint32_t compensate_press(int32_t adc_P)
{
    int64_t var1 = (int64_t)t_fine_ - 128000;
    int64_t var2 = var1 * var1 * calib_.dig_p6;
    var2 += (var1 * calib_.dig_p5) << 17;
    var2 += ((int64_t)calib_.dig_p4) << 35;
    var1 = ((var1 * var1 * calib_.dig_p3)>>8) + ((var1 * calib_.dig_p2)<<12);
    var1 = (((((int64_t)1)<<47)+var1) * calib_.dig_p1) >> 33;
    if (var1 == 0) return 0;
    int64_t p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = ((int64_t)calib_.dig_p9 * (p>>13) * (p>>13)) >> 25;
    var2 = ((int64_t)calib_.dig_p8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + ((int64_t)calib_.dig_p7 << 4);
    return (uint32_t)(p >> 8);                           /* Pa */
}

static uint32_t compensate_hum(int32_t adc_H)
{
    int32_t v_x1 = t_fine_ - 76800;
    v_x1 = (((adc_H<<14) - (calib_.dig_h4<<20) - (calib_.dig_h5 * v_x1) + 16384)>>15) *
           (((((((v_x1 * calib_.dig_h6)>>10)*(((v_x1 * calib_.dig_h3)>>11)+32768))>>10)+2097152)*
           calib_.dig_h2 + 8192)>>14);
    v_x1 -= (((((v_x1>>15)*(v_x1>>15))>>7) * calib_.dig_h1)>>4);
    v_x1 = (v_x1 < 0) ? 0 : v_x1;
    v_x1 = (v_x1 > 419430400) ? 419430400 : v_x1;
    return (uint32_t)(v_x1 >> 12);                       /* % ×1024 */
}


    
