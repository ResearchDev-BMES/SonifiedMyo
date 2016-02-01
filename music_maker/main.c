//*****************************************************************************
//
// Adafruit Music Maker Example for TIVA C Series TM4C123GXL
//
// Jackson Cagle - Jan. 31st, 2016
//
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
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
// The UART interrupt handler. As adapted from uart_echo sample project
//*****************************************************************************
void UARTIntHandler(void){
	uint32_t ui32Status;
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
    ROM_UARTIntClear(UART0_BASE, ui32Status);
    while(ROM_UARTCharsAvail(UART0_BASE)){
        ROM_UARTCharPutNonBlocking(UART0_BASE, ROM_UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
// Send a string to the UART. As adapted from uart_echo sample project
//*****************************************************************************
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count){
    while(ui32Count--){
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
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

void VS1053_Reset(void){
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);
	SysCtlDelay(SysCtlClockGet() / (1000 * 3) * 10);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
	SysCtlDelay(SysCtlClockGet() / (1000 * 3) * 10);
}

void Delay(int millis){
	SysCtlDelay(SysCtlClockGet() / (1000 * 3) * 100);
}

//*****************************************************************************
// This example demonstrates MIDI functionality and control methods
//*****************************************************************************
int main(void){
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    // Set the clocking to run directly from the crystal.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable the peripherals used by this VS1053.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Enable processor interrupts.
    ROM_IntMasterEnable();

    // Set GPIO A0 and A1 as UART pins for PC Debug
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Set GPIO B0 and B1 as UART pins for VS1053 Control
    GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Set GPIO B2 as Hardware Reset pin
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);

    // Configure the UART0 for 115,200, 8-N-1 operation, used by PC
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Configure the UART1 as according to VS1053 Datasheet with baudrate of 31250
	ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 31250, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable the UART interrupt only for UART0 because UART1 does not receive inputs
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // Example Started
	UARTSend((uint8_t *)"VS1053 Test\n\r", 13);

	// Reset the VS1053
	VS1053_Reset();

	// Setup the
	midiSetChannelBank(0, VS1053_BANK_MELODY);
	midiSetInstrument(0, VS1053_GM1_OCARINA);
	midiSetChannelVolume(0, 127);

    // Infinite Loop of execution
    while(1){
    	uint8_t i;
    	for (i = 60; i < 69; i++) {
    		midiNoteOn(0, i, 127);
    		Delay(100);
    		midiNoteOff(0, i, 127);
    	}
		Delay(1000);
    }
}
