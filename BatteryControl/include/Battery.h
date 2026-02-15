#pragma once

#include <Arduino.h>

typedef enum
{
    BATTERY_OUTPUT_STATE_OFF = 0u,
    BATTERY_OUTPUT_STATE_ON = 1u,
} Battery_OutputState_E;

typedef enum
{
    BATTERY_OPERATING_STATE_InitialPowerUp = (uint8_t)0u,
    BATTERY_OPERATING_STATE_FinalPowerDown = (uint8_t)1u,
    BATTERY_OPERATING_STATE_Sleep = (uint8_t)2u,
    BATTERY_OPERATING_STATE_ControlCircuitsPowerUp = (uint8_t)3u,
    BATTERY_OPERATING_STATE_ControlCircuitsPowerDown = (uint8_t)4u,
    BATTERY_OPERATING_STATE_ControlCircuitsOn = (uint8_t)5u,
    BATTERY_OPERATING_STATE_PowerSupplyPowerUp = (uint8_t)6u,
    BATTERY_OPERATING_STATE_PowerSupplyPowerDown = (uint8_t)7u,
    BATTERY_OPERATING_STATE_PowerSupplyOn = (uint8_t)8u,
    BATTERY_OPERATING_STATE_BiosTestMode = (uint8_t)0xFFu,
} Battery_OperatingState_E;

typedef enum
{
    BATTERY_FAILURE_LEVEL_OK = (uint8_t)0u,
    BATTERY_FAILURE_LEVEL_Warning = (uint8_t)1u,
    BATTERY_FAILURE_LEVEL_SimpleFailure = (uint8_t)2u,
    BATTERY_FAILURE_LEVEL_Empty = (uint8_t)3u,
    BATTERY_FAILURE_LEVEL_CriticalFailure = (uint8_t)4u,
} Battery_FailureLevel_E;

typedef enum
{
    BATTERY_LPS_TEST_MODE_Normal = (uint8_t)0u,
    BATTERY_LPS_TEST_MODE_BiosTest = (uint8_t)1u,
    BATTERY_LPS_TEST_MODE_ApplicationTest = (uint8_t)2u,
    BATTERY_LPS_TEST_MODE_NoProtectionTest = (uint8_t)3u,
} Battery_LpsTestMode_E;

typedef enum
{
    BATTERY_LPS_WAKEUP_FLAG_Mains = (uint8_t)0b00000001u,
    BATTERY_LPS_WAKEUP_FLAG_Button = (uint8_t)0b00000010u,
    BATTERY_LPS_WAKEUP_FLAG_Remote = (uint8_t)0b00000100u,
    BATTERY_LPS_WAKEUP_FLAG_Engine = (uint8_t)0b00001000u,
    BATTERY_LPS_WAKEUP_FLAG_WakeupsBlocked = (uint8_t)0b00010000u,
} Battery_LpsWakeupFlag_BF;

typedef enum
{
    BATTERY_LPS_INPUT_STATE_Input1 = (uint8_t)0b00000001u, //!< Status of input 1. 0 = Input is off, 1 = Input is on.
    BATTERY_LPS_INPUT_STATE_Input2 = (uint8_t)0b00000010u, //!< Status of input 2. 0 = Input is off, 1 = Input is on.
} Battery_LpsInputState_BF;

typedef enum
{
    BATTERY_LPS_OUTPUT_STATE_Output1 = (uint8_t)0b00000001u, //!< Status of output 1. 0 = Output is off, 1 = Output is on.
    BATTERY_LPS_OUTPUT_STATE_Output2 = (uint8_t)0b00000010u, //!< Status of output 2. 0 = Output is off, 1 = Output is on.
    BATTERY_LPS_OUTPUT_STATE_DCDC1 = (uint8_t)0b00000100u,
    BATTERY_LPS_OUTPUT_STATE_DCDC2 = (uint8_t)0b00001000u,
    BATTERY_LPS_OUTPUT_STATE_DCDC3 = (uint8_t)0b00010000u,
    BATTERY_LPS_OUTPUT_STATE_DCRelay = (uint8_t)0b00100000u,
    BATTERY_LPS_OUTPUT_STATE_PERelay = (uint8_t)0b01000000u,
    BATTERY_LPS_OUTPUT_STATE_LNRelay = (uint8_t)0b10000000u,
} Battery_LpsOutputState_BF;

typedef enum
{
    BATTERY_FUNCTION_OPERATING_STATE_OFF = (uint8_t)0u,
    BATTERY_FUNCTION_OPERATING_STATE_Wakeup = (uint8_t)1u,
    BATTERY_FUNCTION_OPERATING_STATE_ReadyToStart = (uint8_t)2u,
    BATTERY_FUNCTION_OPERATING_STATE_Starting = (uint8_t)3u,
    BATTERY_FUNCTION_OPERATING_STATE_Stopping = (uint8_t)4u,
    BATTERY_FUNCTION_OPERATING_STATE_ON = (uint8_t)5u,
} Battery_FunctionOperatingState_E;

typedef struct
{
    // 0x18FF00xx – Battery Status
    uint16_t soc;            //!< SOC is the pack state of charge. It shall be unsigned 16-bits, with a resolution of 1/65535 (%) per bit, and a range of 0% to 100%.
    int16_t batteryCurrent; //!< Battery Current. Resolution is 100mA Signed integer, where positive values indicate discharging and negative values indicate charging. Range is -3276.8A to +3276.7A.
    int16_t reaminingTime;  //!< Remaining Time, is how many minutes until battery is empty or >97%SOC, Signed integer - negative = discharge time. Range is -32768 to +32767 minutes.
    int16_t batteryAh;     //!< Battery Ah is the amount of Ah available in the battery. Resolution is 0.1Ah, unsigned integer, range is 0 to 6553.5Ah.
    // 0x18FF01xx – DC Input and Output
    uint16_t dcInputVoltage;  //!< DC Input Voltage is voltage measured before the DC/DC converters (Input connector). Resolution is 10mV. Range is 0 to 655.35V.
    int16_t dcInputCurrent;   //!< DC Input Current. Resolution is 10mA Signed integer, where positive values indicate current flowing into the battery and negative values indicate current flowing out of the battery. Range is -3276.8A to +3276.7A.
    uint16_t dcOutputVoltage; //!< DC Output Voltage is voltage measured at the Output connector. Resolution is 10mV. Range is 0 to 655.35V.
    int16_t dcOutputCurrent;  //!< DC Output Current. Resolution is 10mA Signed integer, where positive values indicate current flowing out of the battery and negative values indicate current flowing into the battery. Range is -3276.8A to +3276.7A.
    // 0x18FF03xx – Operating State and status bitmaps
    Battery_OperatingState_E operatingState;   //!< Operating State is the current operating state of the battery.
    Battery_FailureLevel_E failureLevel;       //!< Failure Level is the highest failure level currently active in the battery.
    Battery_LpsTestMode_E lpsTestMode;        //!< LPS Test Mode is the current test mode of the battery.
    Battery_LpsWakeupFlag_BF lpsWakeupFlag;   //!< LPS Wakeup Flag is a bitfield indicating the source of the last wakeup event and if wakeups are currently blocked.
    Battery_LpsInputState_BF lpsInputState;   //!< LPS Input State is a bitfield indicating the state of the inputs.
    Battery_LpsOutputState_BF lpsOutputState; //!< LPS Output State is a bitfield indicating the state of the outputs.
    uint8_t cellCount;                  //!< Cell count, is a counter of the number of cells inside the battery
    uint8_t temperatureSensorCount;     //!< Temperature sensor count, is a counter of the number of Cell temperature sensors inside the battery
    // 0x18FF04xx – Function Operating State
    Battery_FunctionOperatingState_E inverterOperatingState; //!< Inverter Operating State is the current operating state of the inverter function inside the battery.
    Battery_FailureLevel_E inverterFailureLevel;       //!< Inverter Failure Level is the highest failure level currently active in the inverter function inside the battery.
    Battery_FunctionOperatingState_E chargerOperatingState; //!< Charger Operating State is the current operating state of the charger function inside the battery.
    Battery_FailureLevel_E chargerFailureLevel;       //!< Charger Failure Level is the highest failure level currently active in the charger function inside the battery.
    Battery_FunctionOperatingState_E dcInputOperatingState; //!< DC Input Operating State is the current operating state of the DC input function inside the battery.
    Battery_FailureLevel_E dcInputFailureLevel;       //!< DC Input Failure Level is the highest failure level currently active in the DC input function inside the battery.
    Battery_FunctionOperatingState_E dcOutputOperatingState; //!< DC Output Operating State is the current operating state of the DC output function inside the battery.
    Battery_FailureLevel_E dcOutputFailureLevel;       //!< DC Output Failure Level is the highest failure level currently active in the DC output function inside the battery.
    // 0x18FF06xx – LPS Internal and Battery temperature
    int16_t internalTemperature1; //!< Internal Temperature 1. Resolution of all temperatures is 1/256 ⁰C/bit. All temperatures shall be signed 2’s complement, 16 bits, MSB first, with a resolution of 0.0039 deg C per bit. (1/256 ⁰C/bit). Range is -128 ⁰C to +127.996 ⁰C.
    int16_t internalTemperature2; //!< Internal Temperature 2 (Controls board tempeartures). Resolution of all temperatures is 1/256 ⁰C/bit. All temperatures shall be signed 2’s complement, 16 bits, MSB first, with a resolution of 0.0039 deg C per bit. (1/256 ⁰C/bit). Range is -128 ⁰C to +127.996 ⁰C.
    int16_t internalTemperature3; //!< Internal Temperature 3. Resolution of all temperatures is 1/256 ⁰C/bit, signed integer. All temperatures shall be signed 2’s complement, 16 bits, MSB first, with a resolution of 0.0039 deg C per bit. (1/256 ⁰C/bit). Range is -128 ⁰C to +127.996 ⁰C.
    int16_t cellTemperatureAVG; //!< Cell Temperature AVG is the average cell temperature. All temperatures shall be signed 2’s complement, 16 bits, MSB first, with a resolution of 0.0039 deg C per bit. (1/256 ⁰C/bit). Range is -128 ⁰C to +127.996 ⁰C.
    // 0x18FF09xx – AC Input and AC Output
    uint16_t acInputVoltage; //!< AC Input Voltage is voltage measured before the transfer relay Resolution is 100mV
    int16_t acInputCurrent; //!< AC Input Current is measured after the transfer relay. Resolution is 1mA unsigned integer
    uint16_t acOutputVoltage; //!< AC Output Voltage is measured at the Output connector. Resolution is 100mV
    int16_t acOutputCurrent; //!< DC Output Current is measured at the Output connector. Resolution is 10mA unsigned integer
    // 0x18FF10xx Cell Voltage Message
    uint16_t voltageCell1; //!< Cell 1 is the voltage of the first cell (at the negative battery terminal). Cell voltages shall be unsigned 16 bits, MSB first, with a resolution of 1/8192 volts (i.e. 122 μV) per bit.
    uint16_t voltageCell2; //!< Cell voltages shall be unsigned 16 bits, MSB first, with a resolution of 1/8192 volts (i.e. 122 μV) per bit.
    uint16_t voltageCell3; //!< Cell voltages shall be unsigned 16 bits, MSB first, with a resolution of 1/8192 volts (i.e. 122 μV) per bit.
    uint16_t voltageCell4; //!< Cell voltages shall be unsigned 16 bits, MSB first, with a resolution of 1/8192 volts (i.e. 122 μV) per bit.
    // 0x18FF18xx Cell SOC Message
    uint16_t socCell1; //!< Cell SOC shall be unsigned 16 bits, MSB first, with a resolution of 1/65535 % (i.e. 0,00001529%) per bit.
    uint16_t socCell2; //!< Cell SOC shall be unsigned 16 bits, MSB first, with a resolution of 1/65535 % (i.e. 0,00001529%) per bit.
    uint16_t socCell3; //!< Cell SOC shall be unsigned 16 bits, MSB first, with a resolution of 1/65535 % (i.e. 0,00001529%) per bit.
    uint16_t socCell4; //!< Cell SOC shall be unsigned 16 bits, MSB first, with a resolution of 1/65535 % (i.e. 0,00001529%) per bit.
    // Calculated values
    int16_t dcInputPower; //!< Calculated Power of DC input. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing into the battery and negative values indicate power flowing out of the battery.
    int16_t dcOutputPower; //!< Calculated Power of DC output. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing out of the battery and negative values indicate power flowing into the battery.
    int16_t dcPower; //!< Calculated Power of DC input and output. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing out of the battery and negative values indicate power flowing into the battery.
    int16_t acInputPower; //!< Calculated Power of AC input. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing into the battery and negative values indicate power flowing out of the battery.
    int16_t acOutputPower; //!< Calculated Power of AC output. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing out of the battery and negative values indicate power flowing into the battery.
    int16_t acPower; //!< Calculated Power of AC input and output. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing out of the battery and negative values indicate power flowing into the battery.
    int16_t batteryPower; //!< Calculated Power the battery is providing. Resolution is 1W, signed integer, range is -32768W to +32767W. Positive values indicate power flowing out of the battery and negative values indicate power flowing into the battery.

} Battery_Data_T;

/**
 * @brief Initializes the battery module. This should be called once at the start of the program.
 */
extern void Battery_Initialize(void);

/**
 * @brief Updates the battery data. This should be called periodically to update the battery data and send notifications if necessary.
 */
extern void Battery_Update(void);

/**
 * @brief Checks if the battery is currently connected.
 */
extern bool Battery_IsConnected(void);

/**
 * @brief Gets the timestamp of when the battery was connected. This returns 0 if the battery is not currently connected.
 */
extern uint64_t Battery_IsConnectedTimestamp(void);

/**
 * @brief Gets a pointer to the battery data structure. This can be used to read and write the current battery data.
 */
extern Battery_Data_T* Battery_GetDataPtr(void);


extern void Battery_SetDcOutput(Battery_OutputState_E state);

extern void Battery_SetAcOutput(Battery_OutputState_E state);