/************** ECE2049 DEMO CODE ******************/
/**************  20 August 2016   ******************/
/***************************************************/

#include <msp430.h>
#include <grlib.h>
#include "peripherals.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Function Prototypes
void swDelay(char numLoops);
void DrawAlien();
void displayWelcome();
void createAlien(int col);
void shootAlien(char key);
int checkGameEnd();
void displayEndGame();
void dispCount();
void advanceAliens();
void resetGame();
void displayWonGame();
struct alien{
	int x_val;
	int y_val;
	int isDead;
	int column;
} aliens[300];//some max number of aliens possible

// Declare globals here
unsigned int alien_counter = 0;	//for use in creating and destroying aliens
unsigned int radius = 8; 		//radius of alien circle
unsigned int state = 0;
int level = 1;
unsigned int alien_adv = 0; //counter to advance aliens every x number of times you iterate through the switch
unsigned int create_alien_count = 0; //counter to create an alien every x number of advancements
unsigned char currKey=0;
unsigned int col;
int points = 0;
char points_print[15];
int speed;
unsigned int dead_aliens = 0;
int i;		//for use in loops
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

	initLeds();
	configDisplay();
	configKeypad();
	GrClearDisplay(&g_sContext); // Clear the display
	srand(time(NULL)); //initialize random number generator

	while (1)    // Forever loop
	{
		switch(state){
		case 0: // case 0 is the Welcome state

			currKey = getKey();
			displayWelcome();

			break;
		case 1: //case 1 shoots aliens
			if(alien_counter > 0){	//makes
				shootAlien(getKey());
			}
			state = 2;
			break;
		case 2:

			if(level == 1){
				speed = 2000;
			}
			if(level == 2){
				speed = 1500;

			}
			if(level == 3){
				speed = 1000;
			}
			if(level == 4){
				speed = 500;
			}
			if(level == 5){
				speed = 300;
			}

			state = 3;
			break;
		case 3:
			currKey = getKey();

			if(alien_adv == speed){
				advanceAliens();
				alien_adv = 0;
				if ((dead_aliens % 5 == 0) && (dead_aliens !=0)){ // 10 is number of aliens per level
					level++;

				}
				if (level == 5){
					displayWonGame();
				}
			}
			alien_adv++; //loop counter
			state = 4;
			break;

		case 4:
			if(alien_counter > 0){
				if(checkGameEnd() == 1){
					displayEndGame();
				}
			}
			state = 1;
			break;
		}

	}
}
void DrawAlien() {
	GrClearDisplay(&g_sContext); // Clear the display

	for(i= 0;i < alien_counter; i++){			//draw each alien
		if(aliens[i].isDead != 1){
			GrCircleDraw(&g_sContext, aliens[i].x_val, aliens[i].y_val, radius);	//since it doesnt actually tell you what is x, y and r, just guessed
		}
	}
	//	GrStringDrawCentered(&g_sContext, "Score: ", AUTO_STRING_LENGTH, 7, 15, TRANSPARENT_TEXT);
	//	GrStringDrawCentered(&g_sContext, points+'0', AUTO_STRING_LENGTH, 10, 15, TRANSPARENT_TEXT); 		//mess with this to get it to display correctly
	GrFlush(&g_sContext);
}
void createAlien(int column){
	aliens[alien_counter].x_val = (column + 1) * 17;	//column is a value from 0-4, we need from 1-5
	aliens[alien_counter].y_val = radius;							//start alien at top of screen
	aliens[alien_counter].isDead = 0; //0 if alive, 1 if dead
	aliens[alien_counter].column = column+1; //store aliens column number in 1 based indexing
	alien_counter++;
}
void shootAlien(char key){
	int button;
	if(key == '1'){		//This is really gross, but since key is a char and column is an int we need to convert the chars to ints in order to use them as columns
		button = 1;
	}else
		if(key == '2'){
			button = 2;
		}else
			if(key == '3'){
				button = 3;
			}else
				if(key == '4'){
					button = 4;
				}else
					if(key == '5'){
						button = 5;
					}else{
						button = 0;
					}


	for(i = 0; i < alien_counter; i++){			// go through the array and find the first living alien in that column, this will also be the alien that is lowest on the screen in that column
		if((button == aliens[i].column) && (aliens[i].isDead != 1)){
			aliens[i].isDead = 1;
			BuzzerOn(20);
			dead_aliens++;
			DrawAlien();
			BuzzerOn(30);
			BuzzerOff();

		}
	}

}

void resetGame(){
	GrClearDisplay(&g_sContext);
	currKey = getKey();

	for(i = 0; i < create_alien_count; i++){
		aliens[i].isDead = 0;
	}

	displayWelcome();
	state = 0;
	alien_counter = 0;
	level = 1;
	alien_adv = 0;
	create_alien_count = 0;
	points = 0;
	dead_aliens = 0;
	currKey = getKey();

	//	char points_print[15];


}

void advanceAliens(){

	for(i = 0; i <= alien_counter; i++){
		if(aliens[i].isDead != 1){
			aliens[i].y_val = aliens[i].y_val + 2*radius;		//advance the alien each time by the diameter of the circle, this way no aliens will overlap
		}

	}
	//col = rand() % 5;//choose a random column to insert a new alien
	//createAlien(col);
	unsigned int overlap[5];
	for (i = level; i > 0; i--){
		col = rand()%5;
		overlap[i] = col;
		if((i ==1)|(overlap[i+1] != overlap[i])){
			createAlien(col);
		}
	}
	DrawAlien();

}

void displayWelcome(){ // Welcome screen (case 0)

	// Write some text to the display
	GrStringDrawCentered(&g_sContext, "Welcome", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "to", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Space" , AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Invaders!" , AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Press * to begin", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);//actually do this
	GrFlush(&g_sContext); //repaint
	BuzzerOn(60);
	swDelay(1);
	BuzzerOn(40);
	swDelay(1);
	BuzzerOff();
while(getKey() != '*'){
//if(getKey() == '*'){
	//resetGame();
	//state = 0;
//}
}
dispCount();
state = 1;

}
void displayEndGame(){

	GrClearDisplay(&g_sContext);
	int you_suck = rand() % 6;
	switch(you_suck){
	case 0:
		GrStringDrawCentered(&g_sContext,"U Suck", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	case 1:
		GrStringDrawCentered(&g_sContext,"Git Gud", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	case 2:
		GrStringDrawCentered(&g_sContext,"Just Be Better", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	case 3:
		GrStringDrawCentered(&g_sContext,"LOL :P", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	case 4:
		GrStringDrawCentered(&g_sContext,"Just Give Up", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	case 5:
		GrStringDrawCentered(&g_sContext,"RIP", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
		GrFlush(&g_sContext); //repaint
		break;
	}


	GrStringDrawCentered(&g_sContext, "GAME OVER", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
	points = dead_aliens*10;	// 10 is number of points each alien is worth
	sprintf(points_print, "%d", points);
	//char score[10] = itoa(points,points_print,10);
	GrStringDrawCentered(&g_sContext, "Score", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, points_print, AUTO_STRING_LENGTH, 48, 65, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Press # to Retry", AUTO_STRING_LENGTH, 48, 75, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //repaint
	BuzzerOn(128);
	swDelay(1);
	BuzzerOff();
	//swDelay(1);
	BuzzerOn(160);
	swDelay(1);
	BuzzerOff();
	BuzzerOn(200);
	swDelay(2);
	BuzzerOff();
	while(currKey != '#'){
		currKey = getKey();  //stay here
	}
	currKey = 0;
	resetGame();
}
void dispCount(){
	GrClearDisplay(&g_sContext);
	GrStringDrawCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //repaint
	BuzzerOn(140);
	swDelay(1);
	BuzzerOff();
	GrClearDisplay(&g_sContext);
	GrStringDrawCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //repaint
	BuzzerOn(140);
	swDelay(1);
	BuzzerOff();
	GrClearDisplay(&g_sContext);
	GrStringDrawCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //repaint
	BuzzerOn(100);
	swDelay(1);
	BuzzerOff();
}

int checkGameEnd(){
	//go through aliens[] and see if any have hit the bottom of the screen
	//if yes return 1
	//if no return 0
	for(i = 0; i <= alien_counter; i++){
		if((aliens[i].y_val == 104) && (aliens[i].isDead != 1)){ //the final position of a circle just off the screen is calculated to 104
			//return 1;
			return 1;
		}
	}
	return 0;
}
void displayWonGame(){
	GrClearDisplay(&g_sContext);
	GrStringDrawCentered(&g_sContext, "Good Job", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Not Sucking", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "YOU WON", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT);
	points = dead_aliens*10;	// 10 is number of points each alien is worth
	sprintf(points_print, "%d", points);	//convert the points to an array of characters
	GrStringDrawCentered(&g_sContext, "Score", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, points_print, AUTO_STRING_LENGTH, 48, 65, TRANSPARENT_TEXT);
	GrStringDrawCentered(&g_sContext, "Press # to Retry", AUTO_STRING_LENGTH, 48, 75, TRANSPARENT_TEXT);
	GrFlush(&g_sContext); //repaint
	BuzzerOn(128);		//play a short tune
	swDelay(1);
	BuzzerOff();
	BuzzerOn(88);
	swDelay(1);
	BuzzerOff();
	BuzzerOn(48);
	swDelay(2);
	BuzzerOff();
	while(currKey != '#'){
		currKey = getKey();  //stay here until # is pressed
	}
	currKey = 0;
	resetGame();

}

void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent optimization
	// by compiler

	for (j=0; j<numLoops; j++)
	{
		i = 50000 ;					// SW Delay
		while (i > 0)				// could also have used while (i)
			i--;
	}
}
