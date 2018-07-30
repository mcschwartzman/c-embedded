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

// Function Prototypes
void displayWelcome();
void countDown();
void configUserLED(char inval);
void configButtons();
void configTimer(void);
void selectSong();
void play();
void mapNote();
unsigned int getButton(void);
volatile double fallIncrement = 0;
char button_value[1];
char notes_print[5];
char points_print[4];
void prettyLights(int j);
void resetGame();
unsigned char ret_val;
// Declare globals here
unsigned int state = 0;
long unsigned volatile int timerCount = 0; // global clock variable
int i;
int timerBuffer;
int points = 0;
volatile unsigned int currButton = 0;
void dispEndGame();
void drawNotes(int j);
void writeSong(int *tones, int *durations, int globalrest, int note1, int note2, int note3, int note4);
void PlayNote(int frequency);
tContext sContext; // intialize graphics context

unsigned char constLED;
unsigned char ledpass = ~BIT0 & BIT1; //for making on board LEDs light up in a pattern
int pitches[4]; //each song can have 4 unique notes 

const int limit = 26;//size of the array for use throughout the program
struct note {
	int duration;
	int pitch;
	int rest;
	unsigned int column;
	int y_val;
} song[26];
/*initialize global arrays of each songs 26 notes pitches and durations, gets passed into writeSong which sets 
the elements of each member of the song array.*/
int pokemanzpits[26] = {880, 880, 880, 880, 880, 784, 659, 523, 659, 880, 880, 659, 523, 659, 880, 880, 880, 784, 659, 523, 659, 880, 880, 784, 659, 784};
int pokemanzdurs[26] = {500, 500, 500, 1000, 1000, 500, 500, 500, 500, 1000, 1000, 1000, 500, 1000, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 500, 500, 1000};
int totoropits[26] = {698, 784, 880, 976, 784, 698, 880, 784, 698, 880, 784, 698, 880, 784, 698, 698, 698, 880, 784, 698, 698, 784, 880, 880, 784, 698};
int totorodurs[26] = {500, 500, 500, 1000, 700, 700, 700, 1000, 500, 500, 700, 1000, 700, 1000, 700, 500, 1000, 700, 500, 500, 500, 1000, 700, 500, 500, 1000};
int marypits[26] = {659, 587, 523, 587, 659, 659, 659, 587, 587, 587, 659, 784, 784, 659, 587, 523, 587, 659, 659, 659, 659, 587, 587, 659, 587, 523};
int marydurs[26] = {500, 500, 500, 500, 500, 500, 1000, 500, 500, 1000, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 1000};
// Main
void main(void) {

	GrContextInit(&sContext, &g_sharp96x96LCD);//initializations for graphics 
	GrContextForegroundSet(&sContext, ClrBlack);
	GrContextBackgroundSet(&sContext, ClrWhite);

	_BIS_SR(GIE);
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

	initLeds(); //config LEDs

	configDisplay();	// config display
	configKeypad();		// config keypad

	configTimer(); // Config timer A2

	configButtons(); //config buttons
	GrClearDisplay(&g_sContext);
	// Clear the display

	while (1) {

		configUserLED(~BIT0 & ~BIT1); //set both on board LEDs to off
		switch (state) {
		case 0: //case 0 is the welcome state
			displayWelcome();
			while (getKey() != '*') {//remain in the welcome screen until '*' is pressed
			}
			state = 1;
			GrClearDisplay(&g_sContext); //clear the display
			break;

		case 1: //case 1 allows the user to select a song and then sets the pitches of the 
				//selected song to be assigned to their respective buttons
			selectSong();
			mapNote();

			break;

		case 2: //case 2 displays the countdown
			countDown();
			GrClearDisplay(&g_sContext);
			break;

		case 3: //case 3 plays the actual game
			play();
			break;
		case 4: //case 4 ends the game
			dispEndGame();
			break;

		}
	}

}

/*
writeSong takes in the selected songs pitches and durations of notes as well as some 
universal rest between each note and the 4 available notes to use in the song. This function, and our code in general relies on a song
using no more than 4 tones.
*/
void writeSong(int *tones, int *durations, int globalrest, int note1, int note2, int note3, int note4){

	pitches[0] = note1;
	pitches[1] = note2;
	pitches[2] = note3;
	pitches[3] = note4;

	for(i = 0; i < limit; i++){ //goes through the song[] array and sets each value of the struct to its appropriate values
		song[i].pitch = tones[i];
		song[i].duration = durations[i];
		song[i].rest = globalrest;
	}
}

void selectSong() { //allows the user to select which song they want to play
	//display options
	GrStringDrawCentered(&g_sContext, "Choose A Song", AUTO_STRING_LENGTH, 48,
			10, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "1: Mary Had", AUTO_STRING_LENGTH, 48, 25,
			TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "A Little Lamb", AUTO_STRING_LENGTH, 48, 35,
			TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "2: Pokemon", AUTO_STRING_LENGTH, 48, 45,
			TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "3: Totoro", AUTO_STRING_LENGTH, 48, 55,
			TRANSPARENT_TEXT);
	//register user input and set song accordingly
	if (getKey() == '1') {
		writeSong(marypits, marydurs, 25, 523, 587, 659, 784);	//Use Mary Had a Little Lamb as song
		GrClearDisplay(&g_sContext);
		timerCount = 0; //reset the timer
		state = 2; //go to next state (display the countdown)
	}
	if(getKey() == '2'){

		writeSong(pokemanzpits, pokemanzdurs, 25, 523, 659, 784, 880);	//Use Pokemon Theme song
		GrClearDisplay(&g_sContext);
		timerCount = 0;
		state =2;
	}
	if(getKey() == '3'){
		writeSong(totoropits, totorodurs, 25, 698, 784, 880, 976);	//Use Totoro Theme song
		GrClearDisplay(&g_sContext);
		timerCount = 0;
		state =2;
	}

	GrFlush(&g_sContext);
	//repaint

}

void prettyLights(int j){ //light up LED according to the current note being played
	if (song[j].column == 1){
		setLeds(BIT3);
	}
	if (song[j].column == 2){
			setLeds(BIT2);
		}
	if (song[j].column == 3){
			setLeds(BIT1);
		}
	if (song[j].column == 4){
			setLeds(BIT0);
		}
}

void play() {
	// take note parameters and play note using buzzer

	for (i = 0; i < limit; i++) { //go through the song array (limit is the global size of the array)
		fallIncrement = 0; //fallIncrement is incremented in the interrupt, reset to 0 for each note being displayed
		timerBuffer = timerCount; //record the current start time
		while (timerCount <= timerBuffer + song[i].duration) { //while current time is <= the time at which the note will stop playing
			drawNotes(i); //draw the current note

			if ((currButton == song[i].column)){ //currButton is used as a buffer that only updates every timer tick, making points increment equal to the duration of the note
				PlayNote(song[i].pitch);
				points++; //you get more points the more precise you are at hitting the note for the entire duration
			}
			else{
				BuzzerOff(); //turn off the buzzer
			}
			prettyLights(i); //turn on the light according to which note you are playing
		}
		timerBuffer = timerCount; //update the current time
		while (timerCount <= timerBuffer + song[i].rest) { //turn off the buzzer for the amount of time that the global rest is between notes
			BuzzerOff();
		}
	}
	state = 4; //move on to end game state
}



void dispEndGame(){

	GrClearDisplay(&g_sContext); //clear the display

	if (points < 100){ //be rude to the player if they dont score high enough
		ledpass = BIT0;
		constLED = BIT1;
		configUserLED(ledpass); //turn on red light if you lost
		GrStringDrawCentered(&g_sContext, "wow u rly suck", AUTO_STRING_LENGTH, 48, 50, TRANSPARENT_TEXT);
		timerBuffer = timerCount; 
		/*play a "you lost" song*/
		while(timerCount < timerBuffer + 400){
			PlayNote(500);
		}
		timerBuffer = timerCount;//update time
		BuzzerOff();
		while(timerCount < timerBuffer + 400){
			PlayNote(400);
		}
		timerBuffer = timerCount;//update time
		BuzzerOff();
		while(timerCount < timerBuffer + 700){
			PlayNote(123);
		}
		timerBuffer = timerCount; //update time
		BuzzerOff();
	}
	else{
		constLED = BIT0;
		ledpass = BIT1;
		configUserLED(ledpass); //turn on green light if you scored above the threshold
		GrStringDrawCentered(&g_sContext, "not bad noob", AUTO_STRING_LENGTH, 48, 50, TRANSPARENT_TEXT);//compliment the player
		timerBuffer = timerCount;//update time
		/*play a "you won" tune*/
				while(timerCount < timerBuffer + 400){
					PlayNote(659);
				}
				timerBuffer = timerCount;//update time
				BuzzerOff();
				while(timerCount < timerBuffer + 400){
					PlayNote(698);
				}
				timerBuffer = timerCount;//update time
				BuzzerOff();
				while(timerCount < timerBuffer + 700){
					PlayNote(784);
				}
				timerBuffer = timerCount;//update time
				BuzzerOff();
	}

	while (getKey() != '#'){ //display the end game screen and score for as long as # is not pressed

		timerBuffer = timerCount;

		GrStringDrawCentered(&g_sContext, "ROCK ON", AUTO_STRING_LENGTH, 48, 10, TRANSPARENT_TEXT);
		GrStringDrawCentered(&g_sContext, "score: ", AUTO_STRING_LENGTH, 48, 30, TRANSPARENT_TEXT);
		sprintf(points_print, "%d", points);
		GrStringDrawCentered(&g_sContext, points_print, AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
		GrFlush(&g_sContext);

		if(timerCount = timerBuffer + 100){
			ledpass = ~ledpass;
			configUserLED(ledpass & ~constLED);
		}

	}
	resetGame(); //reset the game when # is pressed

}

void resetGame(){//reset the game, set points to 0 and go back to welcome state
	GrClearDisplay(&g_sContext);
	points = 0;
	state = 0;
}
void mapNote() {//sets each button to a specific note of the song

	for(i = 0; i < limit; i++){
		if(song[i].pitch == pitches[0]){
			song[i].column = 1;
		}
		else if(song[i].pitch == pitches[1]){
			song[i].column = 2;
		}
		else if(song[i].pitch == pitches[2]){
			song[i].column = 3;
		}
		else if(song[i].pitch == pitches[3]){
			song[i].column = 4;
		}
	}

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

void drawNotes(int j){
	int rectwidth = 20; //each note is 20 pixels in width
	int gap = 3; //with 3 pixels between each note 
	//variables for use in drawing the rectangle
	int left;
	int right;
	double top;
	double bottom = 96;
	GrClearDisplay(&g_sContext);
	double duration = song[j].duration / 10;//sets the size of the rectangle proportional to the duration of the note
		song[j].y_val = fallIncrement/10;//fallIncrement increases and the note falls down accordingly

		left = (song[j].column * gap) + ((song[j].column - 1)* rectwidth);	//math for determining location/dimensions based on column of note
		right = (song[j].column * gap) + (song[j].column * rectwidth);
		//math that determines the appropriate locations for the top and bottom of the note rectangle
		top = 96 - duration + song[j].y_val;//note starts at bottom of the screen and diminishes as it is played
		//draw the rectangle
		tRectangle col1rect = { left, top, right, bottom }; // left edge x, top edge y, right edge x, bottom edge y
		GrRectFill(&sContext, &col1rect);
		GrFlush(&g_sContext);
}
/*configure the buttons*/
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

//we only can play one note at a time so we only need to register 1 button press at a time
unsigned int getButton(void) {

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
/*configure the user LEDs (LEDS are active low)*/
void configUserLED(char inval) {
	P1SEL &= ~(BIT0);
	P4SEL &= ~(BIT7);

	P1DIR |= (BIT0);
	P4DIR |= (BIT7);

	P1OUT &= ~BIT0;
	P4OUT &= ~BIT7;

	if (inval & BIT0)
		P1OUT |= BIT0;	//led 1 (1.0) is lit
	if (inval & BIT1)
		P4OUT |= BIT7;	//led 2 (4.7) is lit

}

void displayWelcome() {//display the welcome screen

	// Write some text to the display
	GrStringDrawCentered(&g_sContext, "Welcome", AUTO_STRING_LENGTH, 48, 15,
			TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "to", AUTO_STRING_LENGTH, 48, 25,
			TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Guitar Hero!", AUTO_STRING_LENGTH, 48,
			35, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Press * to play!", AUTO_STRING_LENGTH,
			48, 45, TRANSPARENT_TEXT);


	GrFlush(&g_sContext);//repaint
}

void countDown() {//display the countdown
	timerCount = 0;//reset timer
	while (timerCount <= 450) { //play tone for "3"
		PlayNote(440);
		GrStringDrawCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 15,
				TRANSPARENT_TEXT);
		configUserLED(BIT0 & ~BIT1);
		GrFlush(&g_sContext);
		//repaint
	}
	//reset display
	GrClearDisplay(&g_sContext);
	BuzzerOff();
	GrFlush(&g_sContext);
	//repaint

	while (timerCount <= 1000) {//display "2" and play tone
		PlayNote(440);
		GrStringDrawCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 15,
				TRANSPARENT_TEXT);
		configUserLED(~BIT0 & BIT1);
		GrFlush(&g_sContext);
		//repaint
	}
	//reset display
	GrClearDisplay(&g_sContext);
	BuzzerOff();
	GrFlush(&g_sContext);
	//repaint

	while (timerCount <= 1500) {//display "1" and play tone
		PlayNote(440);
		GrStringDrawCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 15,
				TRANSPARENT_TEXT);
		configUserLED(BIT0 & ~BIT1);
		GrFlush(&g_sContext);
		//repaint
	}
	//reset display
	GrClearDisplay(&g_sContext);
	BuzzerOff();
	GrFlush(&g_sContext);
	//repaint

	while (timerCount <= 2000) {//"Go" (in this case we use "ROCK" cus its guitar hero and you need to rock out to mary had a little lamb)
		PlayNote(880);
		GrStringDrawCentered(&g_sContext, "ROCK", AUTO_STRING_LENGTH, 48, 15,
				TRANSPARENT_TEXT);
		configUserLED(BIT0 | BIT1);
		GrFlush(&g_sContext);
		//repaint
	}
	GrClearDisplay(&g_sContext);
	BuzzerOff();
	GrFlush(&g_sContext);
	//repaint
	state = 3;

}

#pragma vector=TIMER2_A0_VECTOR //timer interrupt
__interrupt void Timer_A2_ISR(void) // happens every 1/100 of a second
{
	//increment timer and fallIncrement, poll for a button pressed
	timerCount++;
	currButton = getButton();
	fallIncrement++;



}
//configure the timer
void configTimer(void) {
	TA2CTL = TASSEL__ACLK + ID__1 + MC__UP; // select ACLK, divide by 1, count up
	TA2CCR0 = 33; // 327+1 = 328 ACLK tics = ~1/100 seconds
	TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}
