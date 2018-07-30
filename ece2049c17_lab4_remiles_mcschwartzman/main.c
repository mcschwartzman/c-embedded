/************** ECE 2049 Lab 4 ******************/
/**************  2 March 2017   ******************/
/***************************************************/

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <grlib.h>
#include <stdlib.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void configTimer(void);
void configScroll(void);
void configButtons(void);
unsigned int getButton(void);
void DAC_Init(void);
void DACSetValue(unsigned int dac_code);
void squareWave(double dc, int freq);
void sawToothWave(int freq);
void triangleWave(int freq);
void stairCase(int freq);
void initSPI_B1(void);
void triangleWaveVariable();
float readVoltage();

// Declare globals here
unsigned char currKey=0;
long volatile unsigned int timerCount = 0;
int i;
int state = 0;
int currButton = 0;
volatile int pot;
int leapcnt;


// Main
void main(void){

	_BIS_SR(GIE);	// Enable general interrupts
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer


    // Useful code starts here

    DAC_Init();	// Initialize DAC SPI Device
    configTimer();	// Set up timer

    configDisplay();	// Set up LCD display

    configButtons();	// Configure lab board buttons
    configScroll();		// Configure scroll wheel potentiometer analog device

    char pot_print[10];	// character array to print potentiometer for debugging purposes
    char volt_read[16];	// character array to print voltage for debugging purposes

  	// *** Intro Screen ***

    while(1){

    	switch (state){

    	case 0:

    		// Draw text and instructions for function generator operation
    		GrStringDrawCentered(&g_sContext, "Function", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, "Generator", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, "Button1->DC", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, "Button2->SqWv", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, "Button3->SwTth", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, "Button4->TriWv", AUTO_STRING_LENGTH, 48, 65, TRANSPARENT_TEXT);

    		GrFlush(&g_sContext);	//refresh the display

    		timerCount = 0;	//set global timer value to 0
    		currButton = getButton();	// update current button value
    		state = currButton;	// Switch to corresponding case
    		break;	//escape case

    	case 1:	// if button 1 is pressed
    		DACSetValue(4095);	//set voltage to max value
    		float VoltageReading = readVoltage();	// Create float for voltage reading from ADC

    		sprintf(pot_print, "%d", pot);	// Attach pot to array to display for debugging purposes

    		GrClearDisplay(&g_sContext);	// Clear the display

    		GrStringDrawCentered(&g_sContext, "Button1->DC", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);	// Display current button
    		GrStringDrawCentered(&g_sContext, pot_print, AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);	// Draw pot value
    		GrFlush(&g_sContext);	//refresh the display
    		while(1){	//Loop forever
    			DACSetValue(pot); // Set voltage to pot value
    		}

    	case 2:	// if button 2 is pressed

    		sprintf(pot_print, "%d", pot);	// attach pot value to array to display for debugging purposes
    		GrClearDisplay(&g_sContext);	//clear the display

    		GrStringDrawCentered(&g_sContext, "Button2->SqWv", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);	//draw current button
    		GrFlush(&g_sContext);	//refresh the display

    		squareWave(0.5, 250);	//call the square wave function with a 50% duty cycle and 250 Hz frequency

    	case 3: // if button 3 is pressed

    		GrClearDisplay(&g_sContext);	//Clear the display

    		GrStringDrawCentered(&g_sContext, "Button3->SwTth", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT); // draw current button
    		GrFlush(&g_sContext);	// refresh the display

    		sawToothWave(250);	//Call the sawtooth function with 250 Hz frequency

    	case 4:	// if button 4 is pressed

    		GrClearDisplay(&g_sContext);	// Clear the display

    		GrStringDrawCentered(&g_sContext, "Button4->TriWv", AUTO_STRING_LENGTH, 48, 65, TRANSPARENT_TEXT); // redraw current button
    		GrFlush(&g_sContext);	// refresh the display

    		triangleWave(250);	// call triangle wave function


    	}
    }
}

void configScroll(){
	REFCTL0 &= ~REFMSTR;	// Set master reference
	ADC12CTL0 = ADC12SHT0_9 + ADC12REFON + ADC12ON;	// set reference formula
	ADC12CTL1 = ADC12CSTARTADD_6 + ADC12SHP;// + ADC12CONSEQ_2;
	P6SEL |= (BIT7|BIT0); //p6.7 is the ADC, p6.0 is the potentiometer
	ADC12MCTL6 = ADC12SREF_0 + ADC12INCH_0;// + ADC12EOS;
	ADC12CTL0 |= (ADC12SC|ADC12ENC);	// start conversion
	ADC12IE = BIT6;	//enable ADC interrupts
	_BIS_SR(GIE);	// enable general interrupts
}

#pragma vector=TIMER2_A0_VECTOR	//Timer Interrupt Service Routine
__interrupt void Timer_A2_ISR(void)
{
	if(leapcnt < 122){	// Leap count based on error occurring at 122 ticks
		timerCount++;	// tick global timer value
		leapcnt++;	// tick leap counting variable
	} else{
		timerCount +=2;	// jog global timer value
		leapcnt = 0;	//reset leap counting variable
	}
}

void configTimer(void) {	//set up timer
	TA2CTL = TASSEL__SMCLK + ID__1 + MC__UP; // select SMCLK, divide by 1, count up
	TA2CCR0 = 103; // SMCLK tics
	TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}

unsigned int getButton(void) {	// get current button value

	unsigned int ret_val;	// local return variable

	if ((~P7IN & BIT0)==0x01)	// if port 7.0
		ret_val	= 1;	// value is button 1
	else if ((~P3IN & BIT6)==0x40)	// if port 3.6
		ret_val	= 2;	//	value is button 2
	else if ((~P2IN & BIT2)==0x04)	// if port 2.2
		ret_val	= 3;	// value is button 3
	else if ((~P7IN & BIT4)==0x10) // if port 7.4
		ret_val	= 4;	// value is button 4
	else {	// if no buttons are pressed
		ret_val = 0;	//value is ZERO
	}
	return (ret_val);//return the button pressed
}


#pragma vector = ADC12_VECTOR	// ADC interrupt service routine
__interrupt void ADC12ISR(void){
	ADC12CTL0 &= ~ADC12SC; 	// clear the start bit
	ADC12CTL0 |= ADC12SC;	// assert
	pot = ADC12MEM6;//potVoltage();	
}

void configButtons() {	// configure lab board buttons
	
	// Port select for IO
	P7SEL &= ~(BIT4 | BIT0);
	P3SEL &= ~(BIT6);
	P2SEL &= ~(BIT2);

	// Port direction for IO
	P7DIR &= ~(BIT4 | BIT0);
	P3DIR &= ~(BIT6);
	P2DIR &= ~(BIT2);

	// Port pullup resistor enable for IO
	P7REN |= (BIT4 | BIT0);
	P3REN |= (BIT6);
	P2REN |= (BIT2);

	// Port pullup direction for IO
	P7OUT |= (BIT4 | BIT0);
	P3OUT |= (BIT6);
	P2OUT |= (BIT2);

}


void DAC_Init(void)	// Initialize DAC
{
// Configure LDAC and CS for digital IO outputs
DAC_PORT_LDAC_SEL &= ~DAC_PIN_LDAC;	// Select LDAC Pin
DAC_PORT_LDAC_DIR |= DAC_PIN_LDAC;	// Select LDAC Direction
DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC; // Deassert LDAC
DAC_PORT_CS_SEL &= ~DAC_PIN_CS;	// Select CS Pin
DAC_PORT_CS_DIR |= DAC_PIN_CS;	// Select CS Direction
DAC_PORT_CS_OUT |= DAC_PIN_CS; // Deassert CS
}


void DACSetValue(unsigned int dac_code)	// SET DAC VOLTAGE 
{
	// Start the SPI transmission by asserting CS (active low)
	// This assumes DACInit() already called
	DAC_PORT_CS_OUT &= ~DAC_PIN_CS;
	// Write in DAC configuration bits. From DAC data sheet
	// 3h=0011 to highest nibble.
	// 0=DACA, 0=buffered, 1=Gain=1, 1=Out Enbl
	dac_code |= 0x3000; // Add control bits to DAC word
	uint8_t lo_byte = (unsigned char)(dac_code & 0x00FF);
	uint8_t hi_byte = (unsigned char)((dac_code & 0xFF00) >> 8);
	// First, send the high byte
	DAC_SPI_REG_TXBUF = hi_byte;
	// Wait for the SPI peripheral to finish transmitting
	while(!(DAC_SPI_REG_IFG & UCTXIFG)) {
		_no_operation();
	}
	// Then send the low byte
	DAC_SPI_REG_TXBUF = lo_byte;
	// Wait for the SPI peripheral to finish transmitting
	while(!(DAC_SPI_REG_IFG & UCTXIFG)) {
		_no_operation();
	}
	// We are done transmitting, so de-assert CS (set = 1)
	DAC_PORT_CS_OUT |= DAC_PIN_CS;
	// This DAC is designed such that the code we send does not
	// take effect on the output until we toggle the LDAC pin.
	// This is because the DAC has multiple outputs. This design
	// enables a user to send voltage codes to each output and
	// have them all take effect at the same time.
	DAC_PORT_LDAC_OUT &= ~DAC_PIN_LDAC; // Assert LDAC
	__delay_cycles(10); // small delay
	DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC; // De-assert LDAC
}

void sawToothWave(int freq){	// Sawtooth waveform function taking in frequency int
	int count = 0;	// local value of voltage
	int maxcnt = (10000/freq);	// local maximum of waveform
	long unsigned int timerBuffer;	// timer Buffer for clock
	int increment = pot/maxcnt; // change for stepsize, but beware of accuracy, increment is the amplitude divided by the total ticks in wavelength (2* period)
	while(1){	// forever loop
		timerBuffer = timerCount;	// timer buffer is timer count
		while(timerCount <= timerBuffer + maxcnt){	// until global timer count reaches maxcount after
			DACSetValue(count);	// Set voltage on DAC to incremented count value
			count = count + increment; // count incremented
			increment = pot/maxcnt;	// increment is refreshed to check for different pot value
		}
		count = 0;	// voltage is reset to zero for sawtooth
	}
}

void triangleWave(int freq){
	int count = 0;	//
	int maxcnt = (5000/freq);
	long unsigned int timerBuffer;
	int increment =  1.5 * pot/maxcnt; //0.5 is multiplier, change for stepsize, but beware of accuracy, increment is the amplitude divided by the total ticks in a period

	while(1){
		timerBuffer = timerCount;
		while(timerCount <= timerBuffer + maxcnt){
			DACSetValue(count);
			count = count + increment;
			increment = pot/maxcnt;
		}
		timerBuffer = timerCount;
		while(timerCount <= timerBuffer + maxcnt){
			DACSetValue(count);
			count = count - increment;
			increment = pot/maxcnt;
		}
	}
}



void squareWave(double dc, int freq){
	long unsigned int timerBuffer = timerCount;
	int amplitude;
	int hitime = dc* (2* (5000/(freq))); // plus one fifth of freq to compensate for error
	int lotime = (1-dc)* (2* (5000/(freq))); // plus one fifth of freq to compensate for error
	while(1){
		timerBuffer = timerCount;
		while(timerCount <= (timerBuffer + (hitime))){
			amplitude = pot;
			DACSetValue(amplitude);
		}
		timerBuffer = timerCount;
		while(timerCount <= (timerBuffer + (lotime))){
			DACSetValue(0);
		}
	}
}

void stairCase(int freq){

	int count = 4095;
	int maxcnt = (10000/freq);
	long unsigned int timerBuffer;
	int increment = 0.75 * pot/maxcnt; //0.5 is multiplier, change for stepsize, but beware of accuracy, increment is the amplitude divided by the total ticks in wavelength (2* period)
	while(1){
		timerBuffer = timerCount;
		while(timerCount <= timerBuffer + maxcnt){
			DACSetValue(count);
			count = count - increment;
			increment = pot/maxcnt;
		}
		count = 4095;
	}
}

void triangleWaveVariable(){

	long unsigned int frequency = (((220 * pot)/91) + 100);
	float period = 5000/frequency;
	long int steptime;
	int numsteps = 5;
	int increment = 4095 / numsteps;
	long unsigned int timerBuffer;
	int count = 0;


	while(1){

		frequency = (((220 * pot)/91) + 100); //in hertz
		period = 5000/frequency; //in ticks
		steptime = period / numsteps; //in ticks

		timerBuffer = timerCount;
		while(count < numsteps){
			if(timerCount >= timerBuffer + steptime){
				count++;
				timerBuffer = timerCount;
			}
			DACSetValue(count * increment);
			//count = count + increment;
		}
		timerBuffer  = timerCount;
		while(count > 0){
			if(timerCount >= timerBuffer + steptime){
				count--;
				timerBuffer = timerCount;
			}
			DACSetValue(count * increment);
			//count = count + increment;
		}

	}
}

float readVoltage(){

	float voltReading;

	REFCTL0 &= ~REFMSTR;
	ADC12CTL0 = ADC12SHT0_9+ ADC12REFON + ADC12ON + ADC12MSC;
	ADC12CTL1 = ADC12CSTARTADD_1 + ADC12SHP;
	P6SEL |= (BIT7|BIT1); //p6.7 is the ADC, p6.1 is the measured voltage
	ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1 + ADC12EOS;
	ADC12CTL0 |= (ADC12SC|ADC12ENC);

	voltReading = ADC12MEM1 & 0x0FFF;

	return voltReading;
}

