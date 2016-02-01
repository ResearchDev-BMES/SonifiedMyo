//*****************************************************************************
//
// Adafruit Music Maker Example for TIVA C Series TM4C123GXL
//
// Ver.1 => Jackson Cagle - Jan. 31st, 2016
//
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "VS1053.h"

// Define the musical instruments to use
#define VS1053_GM1_OCARINA 80

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
// The initialization of Serial Console for PC Debug.
//*****************************************************************************
void InitSerial(void){
	// Setup Peripheral for Serial Console
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Setup Clockspeed for UART
    // Baudrate at 115200 with 16MHz crystal
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

void UARTIntHandler(void){
    uint32_t ui32Status;
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
    ROM_UARTIntClear(UART0_BASE, ui32Status);
    while(ROM_UARTCharsAvail(UART0_BASE)){
        ROM_UARTCharPutNonBlocking(UART0_BASE, ROM_UARTCharGetNonBlocking(UART0_BASE));
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        SysCtlDelay(SysCtlClockGet() / (1000 * 3));
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    }
}

//*****************************************************************************
// MIDI method for VS1053 Chip
//*****************************************************************************
void midiSetInstrument(uint8_t chan, uint8_t inst){
	if (chan > 15) return;
	inst --;
	if (inst > 127) return;

	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_CHAN_PROGRAM | chan);
	ROM_UARTCharPutNonBlocking(UART1_BASE, inst);
}

void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
	if (chan > 15) return;
	if (vol > 127) return;

	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_CHAN_MSG | chan);
	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_CHAN_VOLUME);
	ROM_UARTCharPutNonBlocking(UART1_BASE, vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
	if (chan > 15) return;
	if (bank > 127) return;

	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_CHAN_MSG | chan);
	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_CHAN_BANK);
	ROM_UARTCharPutNonBlocking(UART1_BASE, bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
	if (chan > 15) return;
	if (n > 127) return;
	if (vel > 127) return;

	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_NOTE_ON | chan);
	ROM_UARTCharPutNonBlocking(UART1_BASE, n);
	ROM_UARTCharPutNonBlocking(UART1_BASE, vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
	if (chan > 15) return;
	if (n > 127) return;
	if (vel > 127) return;

	ROM_UARTCharPutNonBlocking(UART1_BASE, MIDI_NOTE_OFF | chan);
	ROM_UARTCharPutNonBlocking(UART1_BASE, n);
	ROM_UARTCharPutNonBlocking(UART1_BASE, vel);
}

void Delay(int millis){
	SysCtlDelay(SysCtlClockGet() / (1000 * 3) * millis);
}

void VS1053_Reset(void){
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
	Delay(20);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0);
	Delay(20);
}

uint8_t inputMapping(uint32_t voltage, uint32_t minVoltage, uint32_t maxVoltage, uint32_t minVel, uint32_t maxVel){
	return (voltage - minVoltage) * (maxVel - minVel) / (maxVoltage - minVoltage) + minVel;
}

//*****************************************************************************
// This example demonstrates MIDI functionality and control methods
//*****************************************************************************
int main(void){

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable the peripherals used by this VS1053.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);	// VS1053 Serial
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	// VS1053 Serial
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);	// VS1053 Reset + EMG Input
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);		// EMG Input 1

	// Enable PC Console
	InitSerial();

    // Enable processor interrupts.
    IntMasterEnable();

	// Set GPIO B0 and B1 as UART pins for VS1053 Control
    GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Set GPIO E4 as Hardware Reset pin and E5 as ADC Input
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);

    // Configure the UART1 as according to VS1053 Datasheet with baudrate of 31250
	UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 31250, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Setup ADC Sampling Sequences 3, configure step 0.
    // Note: Sequence 1 and 2 has 4 step, which would be great for 3-channel sampling
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH8 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    // Example Started
	UARTprintf("VS1053 Test\n");

	// Reset the VS1053
	VS1053_Reset();

	// Setup the
	midiSetChannelBank(0, VS1053_BANK_MELODY);
	midiSetInstrument(0, VS1053_GM1_OCARINA);
	midiSetChannelVolume(0, 127);

	// Setup variables for ADC
	uint32_t ADC_Output[1];
	uint8_t velocity;

    // Infinite Loop of execution
    while(1)
    {

		// ADC Sampling Procedures
		ADCProcessorTrigger(ADC0_BASE, 3);
		while(!ADCIntStatus(ADC0_BASE, 3, false)) {}
		ADCIntClear(ADC0_BASE, 3);
		ADCSequenceDataGet(ADC0_BASE, 3, ADC_Output);
		UARTprintf("AIN8 = %4d\n", ADC_Output[0]);

		// Play Sound as according to voltage level
		velocity = inputMapping(ADC_Output[0], 3000, 4000, 90, 127);
		midiNoteOff(0, 60, 0);
		midiNoteOn(0, 60, velocity);
		UARTprintf("Volume Level = %4d\n", velocity);

		// Delay
		Delay(100);

    }
}
