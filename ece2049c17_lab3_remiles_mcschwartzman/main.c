/************** ECE2049 DEMO CODE ******************/
/**************  20 August 2016   ******************/
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

#define CALADC12_15V_30C *((unsigned int *)0x01A1A)
#define CALADC12_15V_85C *((unsigned int *)0x01A1C)

// Function Prototypes
void swDelay(char numLoops);
void configTimer(void);
int potVoltage(void);
int tempValue(void);
void configScroll(void);
void buzzOnPot(int inpot);
void PlayNote(int frequency);
void configButtons(void);
void calculateMonth(void);
void configThermometer(void);
unsigned int getButton(void);


// Declare globals here

//unsigned char ret_val = 0x0F;
unsigned char currKey=0;
unsigned char buzzMode = 0;
unsigned int dispSz = 3;
unsigned char dispThree[3];
long volatile unsigned int timerCount = 0;
volatile long unsigned int seconds = 5682825; //last 7 digits of student id num
int currbutton;
long int sec;
long int minute;
long int hour;
int day;
int months;
int i;
volatile int temp;
volatile int pot;
int dayinmonth;
int monthnum;
char date_print[6];
char temp_print[6];
int temp_hour;
int freq;
char* month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
char lengths[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
volatile float tempC;
volatile float tempF;
volatile float degC_per_bit;
//volatile unsigned int bits30, bits85;
// Main
void main(void){


    // Define some local variables
	//ADC12IE = BIT0;
	_BIS_SR(GIE);
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer



    // Useful code starts here
    initLeds();
    configTimer();

    configDisplay();
    configKeypad();

    configButtons();
    configThermometer();
    //configScroll();

  	// *** Intro Screen ***

    char time_print[5];
    char minutes_print[5];
    char hours_print[5];
    char pot_print[10];
    char dayinmonth_print[3];
    char tempf_print[5];
    char tempc_print[5];
    int tempC;
    int tempF;

    while(1){
    	//math to find seconds, minutes, and hour wrt seconds (from interrupt)
    	day = seconds/86400;//86400 seconds in a day
    	hour = (seconds - day * 86400) / 3600;//3600 seconds in an hour
    	minute = (seconds - hour * 3600 - day * 86400) / 60;
    	sec = (seconds - hour * 3600 - minute * 60 - day * 86400);


    	if(hour > 24){
    		temp_hour = 0;
    	}else
    		temp_hour = hour;



//print the time
    	if(sec < 10){
    	    		sprintf(time_print, "0%d", sec); //to make leading 0 only when num is less than 10
    	    	}
    	    	else{
    	    		sprintf(time_print, "%d", sec);
    	    	}

    	    	if(minute < 10){
    	    	    		sprintf(minutes_print, "0%d", minute);
    	    	    	}
    	    	    	else{
    	    	    		sprintf(minutes_print, "%d", minute);
    	    	    	}

    	    	if(hour < 10){
    	    	    		sprintf(hours_print, "0%d", temp_hour);
    	    	    	}
    	    	    	else{
    	    	    		sprintf(hours_print, "%d", temp_hour);
    	    	    	}


    	calculateMonth();


    	tempC = (float)((long)temp - CALADC12_15V_30C) * degC_per_bit +30.0;
    	tempF = (tempC * 9.0/5.0 + 32.0);
    	sprintf(pot_print, "%d", pot);
    	sprintf(tempf_print, "%d", tempF);
    	sprintf(tempc_print, "%d", tempC);

    	sprintf(dayinmonth_print, "%d", dayinmonth);
    	if(timerCount == 1){
    		GrClearDisplay(&g_sContext);
    		timerCount = 0;
    		GrStringDrawCentered(&g_sContext, month[monthnum], AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    		GrStringDrawCentered(&g_sContext, dayinmonth_print, AUTO_STRING_LENGTH, 24, 15, TRANSPARENT_TEXT);

    		GrStringDrawCentered(&g_sContext, "Time", AUTO_STRING_LENGTH, 16, 35, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, time_print, AUTO_STRING_LENGTH, 90, 35, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, ":", AUTO_STRING_LENGTH, 82, 35, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, minutes_print, AUTO_STRING_LENGTH, 74, 35, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, ":", AUTO_STRING_LENGTH, 66, 35, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, hours_print, AUTO_STRING_LENGTH, 58, 35, TRANSPARENT_TEXT);

        	GrStringDrawCentered(&g_sContext, pot_print, AUTO_STRING_LENGTH, 58, 55, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, tempf_print, AUTO_STRING_LENGTH, 58, 65, TRANSPARENT_TEXT);
        	GrStringDrawCentered(&g_sContext, tempc_print, AUTO_STRING_LENGTH, 58, 75, TRANSPARENT_TEXT);


        	GrFlush(&g_sContext);
    	}

    	//	GrClearDisplay(&g_sContext);

    	if(getButton() == 1){
    		configScroll();
    		temp = 0;
    		buzzMode = 1;
    	}
    	if(getButton() == 2){
    		configThermometer();
    		pot = 0;
    		buzzMode = 0;
    	}

    	buzzOnPot(pot);

    }

}

void calculateMonth(){
	int tempday = day;
	for(i = 0; i < 12; i++){ //go through array of all possible months 0-11
		tempday -= lengths[i]; //subtract length of month from the value of day, to tell if we are in the correct month yet or not
		if (tempday <= 0){ //if the day minus the length of the current month is < 0, then the current day is in the current month
			monthnum = i; //ith month to use in printing month from char array month[]
			dayinmonth = lengths[i] + tempday +1; //tempday is negative so calculates current day in the current month, convert from
												  // 0 based indexing into 1 based
			break; //break out of loop
		}
	}
}

//2b
//int potVoltage(){
//
//	unsigned int voltReading;
//
//
//	voltReading = ADC12MEM6;
//
//	return voltReading;
//}


void configScroll(){
	REFCTL0 &= ~REFMSTR;
	ADC12CTL0 = ADC12SHT0_9 + ADC12REFON + ADC12ON;
	ADC12CTL1 = ADC12CSTARTADD_6 + ADC12SHP;// + ADC12CONSEQ_2;
	P6SEL |= (BIT7|BIT0); //p6.7 is the ADC, p6.0 is the potentiometer
	ADC12MCTL6 = ADC12SREF_0 + ADC12INCH_0;// + ADC12EOS;
	ADC12CTL0 |= (ADC12SC|ADC12ENC);
	ADC12IE = BIT6;
	_BIS_SR(GIE);
}

void configThermometer(){
	degC_per_bit = ((float)(85.0 - 30.0))/((float)(CALADC12_15V_85C -CALADC12_15V_30C));
	REFCTL0 &= ~REFMSTR;



	ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON ;     // Internal ref = 1.5V

	ADC12CTL1 = ADC12SHP;

	// Using ADC12MEM0 to store reading
	ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10 +ADC12EOS;  // ADC i/p ch A10 = temp sense
	// ACD12SREF_1 = internal ref = 1.5v

	//ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_5 + ADC12EOS;  // ADC i/p ch A10 = temp sense

	ADC12CTL0 |= ADC12SC + ADC12ENC;           	  // Enable conversion
	ADC12IE = BIT0;
	_BIS_SR(GIE);

}


//3e
unsigned int adcPressure;
double convert(){
	double sensitivity = 0.00122;
	double kpa;
	double psi;
	double atm;
	kpa = adcPressure * sensitivity;
	kpa += 0.18; //add offset
	psi = kpa / (1/.145);
	atm = psi / 14.7;

	return (atm);
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void) // happens every minute
{
	timerCount++;
	seconds++;


}

void configTimer(void) {
	TA2CTL = TASSEL__ACLK + ID__1 + MC__UP; // select ACLK, divide by 1, count up
	TA2CCR0 = 32768; // ACLK tics = 60 seconds
	TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled

}

unsigned int getButton(void) {

	unsigned int ret_val;

	if ((~P7IN & BIT0)==0x01)
		ret_val	= 1;
	else if ((~P3IN & BIT6)==0x40)
		ret_val	= 2;
	else if ((~P2IN & BIT2)==0x04)
		ret_val	= 3;
	else if ((~P7IN & BIT4)==0x10)
		ret_val	= 4;
	else {
		ret_val = 0;
	}

	return (ret_val);//return the button pressed
}

void buzzOnPot(int inpot){
	if (buzzMode == 1){

		freq = inpot/5;
		PlayNote(freq);
	}
	else {
		BuzzerOff();
	}
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void){
//	_BIS_SR(GIE);
	ADC12CTL0 &= ~ADC12SC; 	// clear the start bit
	ADC12CTL0 |= ADC12SC;
	temp =ADC12MEM0;// tempValue();
	pot = ADC12MEM6;//potVoltage();

}


void PlayNote(int frequency)
{
	// Initialize PWM output on P3.5, which corresponds to TB0.5
	P3SEL |= BIT5; // Select peripheral output mode for P3.5
	P3DIR |= BIT5;

	TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
	TB0CTL  &= ~TBIE; 						// Explicitly Disable timer interrupts for safety

	// Now configure the timer period, which controls the PWM period
	// Doing this with a hard coded values is NOT the best method
	// We do it here only as an example. You will fix this in Lab 2.
	TB0CCR0   = 32768 / frequency; 					// Set the PWM period in ACLK ticks prev 128
	TB0CCTL0 &= ~CCIE;					// Disable timer interrupts

	// Configure CC register 5, which is connected to our PWM pin TB0.5
	TB0CCTL5  = OUTMOD_7;					// Set/reset mode for PWM
	TB0CCTL5 &= ~CCIE;						// Disable capture/compare interrupts
	TB0CCR5   = TB0CCR0/2; 					// Configure a 50% duty cycle
}

void configButtons() {
	P7SEL &= ~(BIT4 | BIT0);
	P3SEL &= ~(BIT6);
	P2SEL &= ~(BIT2);

	P7DIR &= ~(BIT4 | BIT0);
	P3DIR &= ~(BIT6);
	P2DIR &= ~(BIT2);

	P7REN |= (BIT4 | BIT0);
	P3REN |= (BIT6);
	P2REN |= (BIT2);

	P7OUT |= (BIT4 | BIT0);
	P3OUT |= (BIT6);
	P2OUT |= (BIT2);

}

//int tempValue(){
//
//
//	return ADC12MEM0;      // Read in results if conversion
//}
