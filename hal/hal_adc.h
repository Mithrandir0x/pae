/*******************************************************************************
    Filename: hal_adc.h

    Copyright 2010 Texas Instruments, Inc.
***************************************************************************/
#ifndef HAL_ADC_H
#define HAL_ADC_H

#define ACC_PWR_PIN       BIT0
#define ACC_PORT_DIR      P6DIR
#define ACC_PORT_OUT      P6OUT
#define ACC_PORT_SEL      P6SEL
#define ACC_X_PIN         BIT1
#define ACC_Y_PIN         BIT2
#define ACC_Z_PIN         BIT3
#define ACC_X_CHANNEL     ADC12INCH_1
#define ACC_Y_CHANNEL     ADC12INCH_2
#define ACC_Z_CHANNEL     ADC12INCH_3
#define ACC_X_THRESHOLD   200
#define ACC_Y_THRESHOLD   200
#define ACC_X_LOW_OFFSET  1950
#define ACC_X_HIGH_OFFSET 2150
#define ACC_Y_LOW_OFFSET  1950
#define ACC_Y_HIGH_OFFSET 2150
#define ACC_Z_LOW_OFFSET  1950
#define ACC_Z_HIGH_OFFSET 2150

#define AUDIO_PORT_DIR    P6DIR
#define AUDIO_PORT_OUT    P6OUT
#define AUDIO_PORT_SEL    P6SEL
#define MIC_POWER_PIN     BIT4
#define MIC_INPUT_PIN     BIT5
#define MIC_INPUT_CHAN    ADC12INCH_5
#define AUDIO_OUT_PWR_PIN BIT6
#define AUDIO_OUT_SEL     P4SEL
#define AUDIO_OUT_PIN     BIT4

#define TEMP_CHANNEL      ADC12INCH_10
#define VCC_CHANNEL       ADC12INCH_11

enum { ADC_OFF_MODE, ADC_ACC_MODE, ADC_TEMP_MODE };

extern unsigned long temp_temp; 
extern unsigned long temp_vcc;

extern unsigned int buffer[1024];
extern unsigned int buf_index;

/*-------------Accelerometer Functions----------------------------------------*/
extern void halAccelerometerInit(void);
extern void halAccelerometerCalibrate(void);
extern void halAccelerometerSetCalibratedOffset( int x, int y, int z );
extern void halAccelerometerGetCalibratedOffset(int *x, int *y, int*z);
extern void halAccelerometerRead(int* x, int* y, int* z);
extern void halAccelerometerReadWithOffset(int* x, int* y, int* z);
extern void halAccelerometerShutDown(void);

/*-------------Temperature & VCC Functions------------------------------------*/
extern void halAdcInitVcc(void);
extern void halAdcShutDownVcc(void);
extern int  halAdcGetVcc(void);
extern void halAdcConvertVccFromADC(void);
extern void halAdcReadVcc(char *VccStr);

/*-------------Generic ADC12 Functions----------------------------------------*/
extern void halAdcStartRead(void);
extern void halAdcSetQuitFromISR(unsigned char quit);

#endif /* HAL_ADC_H */
