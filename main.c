#include <stdio.h>
#include <pigpio.h>

typedef unsigned char uchar;
typedef unsigned int uint;

#define ADC_CS 17
#define ADC_CLK 18
#define ADC_DIO 27
#define LedPin 24

uchar get_ADC_Result(uint channel)
{
    uchar i;
    uchar dat1 = 0, dat2 = 0;
    int sel = channel >> 1 & 1;
    int odd = channel & 1;

    printf("sel odd %d %d\n", sel, odd);

    gpioSetMode(ADC_DIO, PI_OUTPUT);
    gpioWrite(ADC_CS, 0);
    // Start bit
    gpioWrite(ADC_CLK, 0);
    gpioWrite(ADC_DIO, 1);
    gpioDelay(2);
    gpioWrite(ADC_CLK, 1);
    gpioDelay(2);
    // Single End mode
    gpioWrite(ADC_CLK, 0);
    gpioWrite(ADC_DIO, 1);
    gpioDelay(2);
    gpioWrite(ADC_CLK, 1);
    gpioDelay(2);
    // ODD
    gpioWrite(ADC_CLK, 0);
    gpioWrite(ADC_DIO, odd);
    gpioDelay(2);
    gpioWrite(ADC_CLK, 1);
    gpioDelay(2);
    // Select
    gpioWrite(ADC_CLK, 0);
    gpioWrite(ADC_DIO, sel);
    gpioDelay(2);
    gpioWrite(ADC_CLK, 1);

    gpioWrite(ADC_DIO, 1);
    gpioDelay(2);
    gpioWrite(ADC_CLK, 0);
    gpioWrite(ADC_DIO, 1);
    gpioDelay(2);

    for (i = 0; i < 8; i++)
    {
        gpioWrite(ADC_CLK, 1);
        gpioDelay(2);
        gpioWrite(ADC_CLK, 0);
        gpioDelay(2);

        gpioSetMode(ADC_DIO, PI_INPUT);
        dat1 = dat1 << 1 | gpioRead(ADC_DIO);
    }

    gpioWrite(ADC_CS, 1);
    gpioSetMode(ADC_DIO, PI_OUTPUT);
    return dat1;
}

int main(void)
{
    uchar analogVal;
    if (gpioInitialise() < 0)
    {
        printf("Failed to initialize pigpio");
        return 1;
    }

    gpioSetMode(ADC_CS, PI_OUTPUT);
    gpioSetMode(ADC_CLK, PI_OUTPUT);
    gpioSetMode(LedPin, PI_OUTPUT);
    gpioWrite(LedPin, 0);

    while (1)
    {
        analogVal = get_ADC_Result(0);
        printf("Current analogVal : %d\n", analogVal);
        gpioPWM(LedPin, analogVal);
        gpioDelay(100000);
    }

    gpioTerminate();
    return 0;
}
