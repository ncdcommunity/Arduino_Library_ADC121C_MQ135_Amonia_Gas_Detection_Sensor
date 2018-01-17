/**************************************************************************/
/*
        Distributed with a free-will license.
        Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
        ADC121C_MQ135
        This code is designed to work with the ADC121C_I2CGAS_MQ135 I2C Mini Module available from ControlEverything.com.
        https://shop.controleverything.com/products/nh3-amonia-benzene-alcohol-gas-sensor
*/
/**************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include "ADC121C_MQ135.h"

/**************************************************************************/
/*
        Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void)
{
    #if ARDUINO >= 100
        return Wire.read();
    #else
        return Wire.receive();
    #endif
}

/**************************************************************************/
/*
        Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x)
{
    #if ARDUINO >= 100
        Wire.write((uint8_t)x);
    #else
        Wire.send(x);
    #endif
}

/**************************************************************************/
/*
        Writes 8-bits to the specified destination register
*/
/**************************************************************************/
static uint8_t writeRegister8(uint8_t i2cAddress, uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)value);
    Wire.endTransmission();
}

/**************************************************************************/
/*
        Writes 16-bits to the specified destination register
*/
/**************************************************************************/
static uint16_t writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value>>8));
    i2cwrite((uint8_t)(value & 0xFF));
    Wire.endTransmission();
}

/**************************************************************************/
/*
        Reads 8-bits from the specified destination register
*/
/**************************************************************************/
static uint8_t readRegister8(uint8_t i2cAddress, uint8_t reg)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddress, (uint8_t)1);
    return (int16_t)(i2cread());
}

/**************************************************************************/
/*
        Reads 16-bits from the specified destination register
*/
/**************************************************************************/
static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg)
{
    Wire.beginTransmission(i2cAddress);
    i2cwrite((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddress, (uint8_t)2);
    return (int16_t)((i2cread() << 8) | i2cread());
}

/**************************************************************************/
/*
        Instantiates a new ADC121C_MQ135 class with appropriate properties
*/
/**************************************************************************/
void ADC121C_MQ135::getAddr_ADC121C_MQ135(uint8_t i2cAddress)
{
    ADC121C_MQ135_i2cAddress = i2cAddress;
    ADC121C_MQ135_conversionDelay = ADC121C_MQ135_CONVERSIONDELAY;
}

/**************************************************************************/
/*
        Sets up the Hardware
*/
/**************************************************************************/
void ADC121C_MQ135::begin()
{
    Wire.begin();
}

/**************************************************************************/
/*
        Get the current status of the alert flag
        If the flag is set, the low or high alert indicators are set as appropriate, 
        and you can access these values with alertLowTriggered() or alertHighTriggered().
*/
/**************************************************************************/
bool ADC121C_MQ135::getAlertStatus()
{
    // high order bit is the alert flag, mask off the rest
    bool alert = (readRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_CONVERSION) & 0x8000) ? true : false;
    
    if (alert)
    {
        // Read the alert low and high values
        // And set the appropriate member variables
        uint8_t status = readRegister8(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_ALERT_STATUS);
        // Under Range Alert Flag
        if (status & 0x01)
            ADC121C_MQ135_alertLow = true;
        else
            ADC121C_MQ135_alertLow = false;
        
        // Over Range Alert Flag
        if (status & 0x02)
            ADC121C_MQ135_alertHigh = true;
        else
            ADC121C_MQ135_alertHigh = false;
    }
    
    return alert;
}

/**************************************************************************/
/*
        Get the current status of the Under Range Alert/adc_alertLow
        getAlertStatus() must be called to update this value
*/
/**************************************************************************/
bool ADC121C_MQ135::alertLowTriggered()
{
    return ADC121C_MQ135_alertLow;
}

/**************************************************************************/
/*
        Get the current status of the Over Range Alert/adc_alertHigh
        getAlertStatus() must be called to update this value
*/
/**************************************************************************/
bool ADC121C_MQ135::alertHighTriggered()
{
    return ADC121C_MQ135_alertHigh;
}

/**************************************************************************/
/*
        Clears the alert low and alert high flags. 
        This also clears the last stored alert values.
*/
/**************************************************************************/
void ADC121C_MQ135::clearAlertStatus()
{
    // Zero out both the low and high alert flags
    uint8_t status = 0x03;
    writeRegister8(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_ALERT_STATUS, status);
    
    ADC121C_MQ135_alertHigh = false;
    ADC121C_MQ135_alertLow = false;
}

/**************************************************************************/
/*
        Sets the Cycle Time
        This configures Automatic Conversion Mode
        When these bits are set to zeros, the automatic conversion
        mode is disabled. This is the case at power-up.
        When these bits are set to a non-zero value, 
        the ADC will begin operating in automatic conversion mode.
        Different values provide various conversion intervals.
*/
/**************************************************************************/
void ADC121C_MQ135::setCycleTime(ADC121C_MQ135CycleTime_t cycletime)
{
    ADC121C_MQ135_cycletime = cycletime;
}

/**************************************************************************/
/*
        Gets the Automatic Conversion Mode
*/
/**************************************************************************/
ADC121C_MQ135CycleTime_t ADC121C_MQ135::getCycleTime()
{
    return ADC121C_MQ135_cycletime;
}

/**************************************************************************/
/*
        Sets the Alert Hold Status
*/
/**************************************************************************/
void ADC121C_MQ135::setAlertHold(ADC121C_MQ135AlertHold_t alerthold)
{
    ADC121C_MQ135_alerthold = alerthold;
}

/**************************************************************************/
/*
        Gets the Alert Hold Status
*/
/**************************************************************************/
ADC121C_MQ135AlertHold_t ADC121C_MQ135::getAlertHold()
{
    return ADC121C_MQ135_alerthold;
}

/**************************************************************************/
/*
        Sets the Alert Flag Enable Status
        This controls the alert status bit [D15] in the Conversion Result register
*/
/**************************************************************************/
void ADC121C_MQ135::setAlertFlag(ADC121C_MQ135AlertFlag_t alertflag)
{
    ADC121C_MQ135_alertflag = alertflag;
}

/**************************************************************************/
/*
        Gets the Alert Flag Enable Status
*/
/**************************************************************************/
ADC121C_MQ135AlertFlag_t ADC121C_MQ135::getAlertFlag()
{
    return ADC121C_MQ135_alertflag;
}

/**************************************************************************/
/*
        Sets the Alert Pin Enable Status
        This controls the ALERT output pin status.
*/
/**************************************************************************/
void ADC121C_MQ135::setAlertPin(ADC121C_MQ135AlertPin_t alertpin)
{
    ADC121C_MQ135_alertpin = alertpin;
}

/**************************************************************************/
/*
        Gets the Alert Pin Enable Status
*/
/**************************************************************************/
ADC121C_MQ135AlertPin_t ADC121C_MQ135::getAlertPin()
{
    return ADC121C_MQ135_alertpin;
}

/**************************************************************************/
/*
        Sets the Polarity
        This configures the active level polarity of the ALERT output pin.
*/
/**************************************************************************/
void ADC121C_MQ135::setPolarity(ADC121C_MQ135Polarity_t polarity)
{
    ADC121C_MQ135_polarity = polarity;
}

/**************************************************************************/
/*
        Gets the Polarity
*/
/**************************************************************************/
ADC121C_MQ135Polarity_t ADC121C_MQ135::getPolarity()
{
    return ADC121C_MQ135_polarity;
}

/**************************************************************************/
/*
        Sets the lower limit threshold used to determine the alert condition
        If the alerts are enabled and the measured conversion moves lower than this limit, 
        a VLOW alert is generated.
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::setAlertLowLimit(uint16_t limit)
{
    // mask off the invalid bits in case they were set
    limit &= 0x0FFF;
    return writeRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_LOW_LIMIT, limit);
}

/**************************************************************************/
/*
        Sets the hysteresis value used to determine the alert condition
        If the alerts are enabled and the measured conversion moves higher than this limit,
        a VHIGH alert is generated.
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::setAlertHighLimit(uint16_t limit)
{
    // mask off the invalid bits in case they were set
    limit &= 0x0FFF;
    return writeRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_HIGH_LIMIT, limit);
}

/**************************************************************************/
/*
        Sets the upper limit threshold used to determine the alert condition
        After a VHIGH or VLOW alert occurs, the conversion result must move within 
        the VHIGH or VLOW limit by more than this value to clear the alert condition
        If the Alert Hold bit is set in the configuration register, alert conditions
        will not self-clear.
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::setHysteresis(uint16_t hysteresis)
{
    // mask off the invalid bits in case they were set
    hysteresis &= 0x0FFF;
    return writeRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_HYSTERESIS, hysteresis);
}

/**************************************************************************/
/*
        Gets the Lowest Conversion result recorded so far
        The value of this register will update automatically when the automatic 
        conversion mode is enabled, but is NOT updated in the normal mode.
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::getLowestConversion()
{
    return readRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_LOWCONV);
}

/**************************************************************************/
/*
        Gets the Highest Conversion result recorded so far
        The value of this register will update automatically when the automatic 
        conversion mode is enabled, but is NOT updated in the normal mode.
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::getHighestConversion()
{
    return readRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_HIGHCONV);
}

/**************************************************************************/
/*
        Clears the Lowest Conversion value recorded so far
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::clearLowestConversion()
{
    uint16_t lowClear = 0x0FFF;
    return writeRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_LOWCONV, lowClear);
}

/**************************************************************************/
/*
        Clears the Highest Conversion value recorded so far
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::clearHighestConversion()
{
    uint16_t highClear = 0x0000;
    return writeRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_HIGHCONV, highClear);
}

/**************************************************************************/
/*
        Reads the result of the most recent conversion. 
        In the normal mode, a new conversion is started whenever this register is read
*/
/**************************************************************************/
uint16_t ADC121C_MQ135::Measure_Voltage()
{
    // Start with default values
    // Set the Cycle Time
    uint8_t config = ADC121C_MQ135_cycletime;
    
    // Set the Alert Hold Status
    config |= ADC121C_MQ135_alerthold;
    
    // Set the Alert Flag Enable Status
    config |= ADC121C_MQ135_alertflag;
    
    // Set the Alert Pin Enable Status
    config |= ADC121C_MQ135_alertpin;
    
    // Set the Polarity
    config |= ADC121C_MQ135_polarity;

    // Write config register to the ADC
    writeRegister8(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_CONFIG, config);

    // Wait for the conversion to complete
    delay(ADC121C_MQ135_conversionDelay);

    // Read the conversion result
    // 12-bit unsigned result for the ADC121C_MQ135
    return readRegister(ADC121C_MQ135_i2cAddress, ADC121C_MQ135_REG_POINTER_CONVERSION);
}

/**************************************************************************/
/*
        This function returns the sensor resistance in clear air (Ro).
        Ro: Sensor Resistance at 100ppm of NH3 in the Clean Air
*/
/**************************************************************************/
void ADC121C_MQ135::Calibration(float Ro)
{
    _Ro = Ro;
    _stateCalibration = true;
}

/**************************************************************************/
/*
        With input as raw value read from the adc and a voltage divider formed
        between this sensor and the load resistor, the voltage across the load
        resistor and its resistance, the resistance of the sensor could be derived.
        This function use MQ gas sensor analog channel as input and
        MeasureResistance is to caculate the sensor resistance in clear air (Ro)
        This assumes that the sensor is in the clean air.
        Ro: Sensor Resistance at 100ppm of NH3 in the Clean Air
*/
/**************************************************************************/
void ADC121C_MQ135::Calibration()
{
    float Ro = 0;
    for (int i = 0; i < ADC121C_MQ135_SAMPLE_TIMES; i++)
    {
        Ro += Measure_Resistance(Measure_Voltage());
        delay(ADC121C_MQ135_SAMPLE_INTERVAL);
    }
    Ro = Ro/ADC121C_MQ135_SAMPLE_TIMES;
    Ro = Ro/Measure_RoInCleanAir();
    Calibration(Ro);
}

/**************************************************************************/
/*
        With input as raw value read from the adc and a voltage divider formed
        between this sensor and the load resistor, the voltage across the load
        resistor and its resistance, the resistance of the sensor could be derived.
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_Resistance(int rawadc)
{
    float VRL = rawadc * (VREF / ADC_RESOLUTION);
    float RsAir = (VREF - VRL) / VRL * Measure_RL();
    return RsAir;
}

/**************************************************************************/
/*
        With input as raw value read from the adc and a voltage divider formed
        between this sensor and the load resistor, the voltage across the load
        resistor and its resistance, the resistance of the sensor could be derived.
        This function use MQ gas sensor analog channel as input and
        MeasureResistance is to caculate the sensor resistance (Rs)
        The Rs changes as the sensor is in the different concentration of the target gas.
        Rs: Sensor Resistance at Various Concentrations of Gases
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_Rs()
{
    float Rs = 0;
    for (int i = 0; i < ADC121C_MQ135_SAMPLE_TIMES; i++)
    {
        Rs += Measure_Resistance(Measure_Voltage());
        delay(ADC121C_MQ135_SAMPLE_INTERVAL);
    }
    Rs = Rs / ADC121C_MQ135_SAMPLE_TIMES;
    return Rs;
}

/**************************************************************************/
/*
        By using the slope and a point of the line, we get the (logarithmic value of ppm)
        of the line and rs_ro_ratio. As it is a logarithmic coordinate, 
        power of 10 is used to convert the result to non-logarithmic
        value. The input is the rs_ro ratio and the gas curve value
        and we get target gas ppm values as output.
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_Scaled(float a, float b)
{
    float ratio = Measure_RatioRsRo();
    return (float)exp((log(ratio) - b) / a);
}

/**************************************************************************/
/*
        By using the sensor resistance (Rs) and sensor resistance in clear air (Ro),
        the rs_ro_ratio could be derived.
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_RatioRsRo()
{
    return (float)(Measure_Rs() / Measure_Ro());
}

/**************************************************************************/
/*
        By using the slope and a point of the line (logarithmic value of ppm),
        we get Carbon Dioxide gas ppm values as output.
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_CarbonDioxide(float a, float b)
{
    return (float)Measure_Scaled(a, b);
}

/**************************************************************************/
/*
        By using the slope and a point of the line (logarithmic value of ppm),
        we get Ammonia gas ppm values as output.
*/
/**************************************************************************/
float ADC121C_MQ135::Measure_Ammonia(float a, float b)
{
    return (float)Measure_Scaled(a, b);
}
