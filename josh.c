
/*
** Snake implementation by wayPoint Method
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>
#include "byte.h"

#include "sprite.h"
#include "friendly_ports.h"
#include "graphics.h"


#define SET_BIT_TO(reg, pin, val) (reg ^= ((val) ? ~reg : reg) & 1 << pin)
#define SET_BIT(reg, pin) (reg |= 1 << pin)
#define CLEAR_BIT(reg, pin) (reg &= ~(1 << pin))
#define CHECK_BIT(reg, pin) (reg >> pin & 1)
#define TOGGLE_BIT(reg, pin) (reg ^= 1 << pin)

#define HIGH 1
#define LOW 0

// I/O
#define I 0
#define O 1

// directions
#define NOT_PRESSED 	0
#define KEY_UP 			1
#define KEY_RIGHT 		2
#define KEY_DOWN 		3
#define KEY_LEFT 		4

#define LSB 0
#define MSB 7
#define SNAKE_SPEED 3 //never go higher then 5 plz
#define MAX_SPR_COUNT 	25
#define SNAKE_WIDTH		3




char stringBuf[80];
int spriteCount = 0;
int lives = 5;
int score = 0;
int directionKey=0;
int foodEaten = 0;
int lastb = 0;
int wallon = 0;
int on = 0;
int food_x;
int food_y;
int spriteStationary = 1;
Sprite food;

typedef struct  { // when a sprite gets to a waypoint it sets a new direction and proceeds to the next waypoint
   float x;
   float y;
   float newDX;
   float newDY;
} WAY_POINT;

#define WAY_POINT_BUFFER_SIZE 50

void setup_ten_squares( Sprite sprites[], byte *bitmaps[], int sprite_count, int bitmap_count );
void update_ten_squares( Sprite * sprites );
void draw_ten_squares( Sprite * sprites );
void update_one_square( Sprite * sprite );

void update_squares( Sprite * sprites, int spriteCount );
void draw_squares( Sprite * sprites, int spriteCount );
int wayPointReached( float x1, float y1, float x2, float y2 );
void walls();

void move_food();
static byte bitmap1[] = {
		BYTE( 11111111 ),
		BYTE( 11111111 ),
		BYTE( 11111111 ),

	};
static byte bitmap2[] = {
		BYTE( 11111111 ),
		BYTE( 11111111 ),
		BYTE( 11111111 ),

	};
void play_gamne() {

	Sprite sprites[MAX_SPR_COUNT];
	WAY_POINT wayPoints[WAY_POINT_BUFFER_SIZE];
	int iHead = 0;
	int spriteWaypointIdx[MAX_SPR_COUNT];

	lcd_init(LCD_DEFAULT_CONTRAST);
	SET_BIT_TO(DDRC, PC7, O);
	SET_BIT(PORTC, PC7); // LCD LED ON
	clear_screen();
	draw_string( 0, 0, "n9667261" );
	draw_string( 0, 8, "Josh Arrowsmith" );
	show_screen();

	_delay_ms(1000);


	move_food();
	init_sprite( &sprites[0], (84-3)/2, (48-3)/2, 3, 3, bitmap1 );
	init_sprite( &sprites[1], (84-3)/2+3, (48-3)/2, 3, 3, bitmap1 );
	init_sprite (&food, food_x, food_y, 3, 3, bitmap2);

	spriteCount=2;

	for (int i=0; i < MAX_SPR_COUNT; i++ )
		spriteWaypointIdx[i] = -11;
	spriteWaypointIdx[0] = -99;

	for (int i=0; i < WAY_POINT_BUFFER_SIZE; i++ ) {
		wayPoints[i].x= -1;
		wayPoints[i].y= -1;
	}

	wallon = 0;


	while (1) {

		if(pressed(SW2) || wallon){
			wallon = 1;
			draw_line(30, 0,  30,25 );

			draw_line(0, 42,  40 ,42 );

			draw_line(60, 25,  120 ,25);

			if(sprites->x >= 29 && sprites->x <= 31 && sprites->y >= 0 && sprites->y <=26 ){
				lives --;
				spriteCount = 2;
			}
			if(sprites->y >= 42 && sprites->y <= 43 && sprites->x >= 1 && sprites->x <= 40  ){
				lives --;
				spriteCount = 2;
			}

			if(sprites->y >= 24 && sprites->y <= 26 && sprites->x >= 60 && sprites->x <= 120  ){
				lives --;
				spriteCount = 2;
			}
		}
		if(pressed(SW1)){
			wallon = 0;
		}

		foodEaten =0;
		draw_squares( sprites, spriteCount );
		draw_sprite(&food);

		update_squares( sprites, spriteCount );
		show_screen();

		if (sprites[0].x < food.x && sprites[0].x -1 + 5 > food.x  && sprites[0].y <= food.y && sprites[0].y + 5  >= food.y){
			food_x = rand()%LCD_X;
			food_y = rand()%LCD_Y;
			init_sprite (&food, food_x, food_y, 3, 3, bitmap2);
			foodEaten = 1;
			move_food();
			if(wallon == 1){
				score = score + 2;
			}
			else if(wallon == 0){
				score++;
			}
		}
		if (food.x <= sprites[0].x && food.x + 5   >= sprites[0].x  && food.y <= sprites[0].y && food.y + 5   >= sprites[0].y){
			food_x = rand()%LCD_X;
			food_y = rand()%LCD_Y;
			init_sprite (&food, food_x, food_y, 3, 3, bitmap2);
			foodEaten = 1;
			move_food();

			if(wallon == 1){
				score = score + 2;
			}
			else if(wallon == 0){
				score++;
			}

	}
		directionKey = NOT_PRESSED;
		// probably need to check that snake isn't reversing on itself
		if(pressed(UP)) {
			if(spriteStationary) {
				spriteStationary=0;
				sprites[0].x+= 3;
				sprites[0].y-=3;
				sprites[0].dy = -SNAKE_SPEED;
				sprites[1].dy = -SNAKE_SPEED;
			}
			if( sprites[0].dy > 0.9*SNAKE_SPEED ) {
				lives--;
				spriteCount = 2;
				directionKey = NOT_PRESSED;
			}
			else {
				directionKey = KEY_UP;
				sprites[0].dx = 0;
				sprites[0].dy = -SNAKE_SPEED;
				lastb = 1;
			}
		}
		else if(pressed(DOWN)) {
			if(spriteStationary) {
				spriteStationary=0;
				sprites[0].x+= 3;
				sprites[0].y+=3;
				sprites[0].dy = SNAKE_SPEED;
				sprites[1].dy = SNAKE_SPEED;
			}
			if( sprites[0].dy < -0.9*SNAKE_SPEED ) {
				lives--;
				spriteCount = 2;
				directionKey = NOT_PRESSED;
			}
			else {
				directionKey = KEY_DOWN;
				sprites[0].dx = 0;
				sprites[0].dy = SNAKE_SPEED;
				lastb = 3;
			}
		}
		else if(pressed(LEFT)) {
			if(spriteStationary) {
				spriteStationary=0;
				sprites[0].dx = -SNAKE_SPEED;
				sprites[1].dx = -SNAKE_SPEED;
			}
			else if( sprites[0].dx > 0.9*SNAKE_SPEED ) {
				lives--;
				spriteCount = 2;
				directionKey = NOT_PRESSED;
			}
			else {
				directionKey = KEY_LEFT;
				sprites[0].dx = -SNAKE_SPEED;
				sprites[0].dy = 0;
				lastb = 2;
			}
		}
		else if(pressed(RIGHT)) {
			if(spriteStationary) {
				spriteStationary=0;
				sprites[0].dx = SNAKE_SPEED;
				sprites[1].dx = SNAKE_SPEED;
				sprites[0].x+= 6;
			}
			else if( sprites[0].dx < -0.9*SNAKE_SPEED ) {
				lives--;
				spriteCount = 2;
				directionKey = NOT_PRESSED;
			}
			else {
				directionKey = KEY_RIGHT;
				sprites[0].dx = SNAKE_SPEED;
				sprites[0].dy = 0;
				lastb = 4;
			}
		}
		if( lives < 0){
			break;
		}

		// Add a new waypoint
		if ( directionKey ) {
			iHead++;
			if( iHead >= WAY_POINT_BUFFER_SIZE ) {
				iHead = 0;
			}
			wayPoints[iHead].x = sprites[0].x;
			wayPoints[iHead].y = sprites[0].y;
			wayPoints[iHead].newDX = sprites[0].dx;
			wayPoints[iHead].newDY = sprites[0].dy;

			// need to add new waypoints index to all segments that haven't had one previously assigned
			for( int i=1; i < spriteCount; i++ ) {
				if( spriteWaypointIdx[i] < 0 )
					spriteWaypointIdx[i] = iHead;
			}
		}

		if ( foodEaten && spriteCount < MAX_SPR_COUNT) {
			// add another sprite/segment to tail of the snake
			spriteCount++;
			if( sprites[spriteCount-2].dx > 0.9*SNAKE_SPEED ) { // going right
				init_sprite( &sprites[spriteCount-1], sprites[spriteCount-2].x-3., sprites[spriteCount-2].y, 3, 3, bitmap1 );
				sprites[spriteCount-1].dx = SNAKE_SPEED;
			}
			else if( sprites[spriteCount-2].dx < -0.9*SNAKE_SPEED ) { // going left
				init_sprite( &sprites[spriteCount-1], sprites[spriteCount-2].x+3., sprites[spriteCount-2].y, 3, 3, bitmap1 );
				sprites[spriteCount-1].dx = -SNAKE_SPEED;
			}
			else if( sprites[spriteCount-2].dy > 0.9*SNAKE_SPEED ) { // going down
				init_sprite( &sprites[spriteCount-1], sprites[spriteCount-2].x, sprites[spriteCount-2].y-3., 3, 3, bitmap1 );
				sprites[spriteCount-1].dy = SNAKE_SPEED;
			}
			else if( sprites[spriteCount-2].dy < -0.9*SNAKE_SPEED ) { // going up
				init_sprite( &sprites[spriteCount-1], sprites[spriteCount-2].x, sprites[spriteCount-2].y+3., 3, 3, bitmap1 );
				sprites[spriteCount-1].dy = -SNAKE_SPEED;
			}
			// set wayPoint index for new sprite from the one before
			spriteWaypointIdx[spriteCount-1] = spriteWaypointIdx[spriteCount-2];
		}

		// Test if any of the sprites has reached it's waypoint
		for(int i = spriteCount-1; i > 0; i--) {
			if( spriteWaypointIdx[i] > 0 ) {
				// test if current position is close enough to destination
				if( wayPointReached( sprites[i].x, sprites[i].y, wayPoints[spriteWaypointIdx[i]].x, wayPoints[spriteWaypointIdx[i]].y ) )
				{
					sprites[i].x = wayPoints[spriteWaypointIdx[i]].x;
					sprites[i].y = wayPoints[spriteWaypointIdx[i]].y;
					sprites[i].dx = wayPoints[spriteWaypointIdx[i]].newDX;
					sprites[i].dy = wayPoints[spriteWaypointIdx[i]].newDY;
					spriteWaypointIdx[i]++;
					// if( sprites[0].x == sprites[i].x){
						// break;
					// }
				}
			}
		}

		clear_screen();
		sprintf(stringBuf, "S(%u)", score);
		draw_string(0, 0, stringBuf);
		sprintf(stringBuf, "L(%u)",lives);
		draw_string(0, 10, stringBuf);
		_delay_ms(100);
	}

	clear_screen();
	draw_string( 0, 0, "Game Over" );
	show_screen();
	lives = 3;
	_delay_ms( 200 );
	db_wait();
}

int wayPointReached( float x1, float y1, float x2, float y2 ) {

	int returnValue;
	if (abs(x2-x1) < SNAKE_SPEED && abs(y2-y1) < SNAKE_SPEED )
		returnValue = 1;
	else
		returnValue = 0;

	return(returnValue);
}

void update_squares( Sprite * sprites, int spriteCount ) {
	for ( int i = 0; i < spriteCount; i++ ) {
		update_one_square( &sprites[i] );
	}
}
void draw_squares( Sprite * sprites, int spriteCount ) {
	for ( int i = 0; i < spriteCount; i++ ) {
		draw_sprite( sprites + i );
	}
}

void setup_ten_squares( Sprite sprites[], byte *bitmaps[], int sprite_count, int bitmap_count ) {

	for ( int i = 0; i < sprite_count; i++ ) {
		//Sprite * sprite = &sprites[i];

		byte * bitmap = bitmaps[i % bitmap_count];
		init_sprite( &sprites[i], 30+(8*i), 30, 8, 8, bitmap );
		sprites[i].dx = -SNAKE_SPEED;
	}
}

void update_ten_squares( Sprite * sprites ) {
	for ( int i = 0; i < MAX_SPR_COUNT; i++ ) {
		update_one_square( &sprites[i] );
	}
}
void draw_ten_squares( Sprite * sprites ) {
	for ( int i = 0; i < MAX_SPR_COUNT; i++ ) {
		draw_sprite( sprites + i );

	}
}

void update_one_square( Sprite * sprite ) {
	sprite->x += sprite->dx;
	sprite->y += sprite->dy;
	if ( sprite->x >= LCD_X - sprite->width + 9 ) {
		sprite->x = -0;
	}

	if ( sprite->y >= LCD_Y - sprite->height + 9 ) {
		sprite->y = -0;

	}
	if ( sprite->x <0 ) {
		sprite->x = 84;
	}
	if ( sprite->y <0 ) {
		sprite->y = 48;
	}

	if (sprite->y <= 15 &&  sprite->x <= 15 ){
			if(lastb == 2 ){
				sprite->x = LCD_X-1;
			}
			if(lastb == 1){
				sprite->y = LCD_Y-1;

			}

		}

	if (sprite->y >= LCD_Y - 3 &&sprite->x >= LCD_X - 3  ){
			if(lastb == 3 ){
				sprite->y = 10;
			}
			if(lastb == 4){
				sprite->x = LCD_Y-10;

			}

		}
}

void walls(){
//make the walls
}

void move_food(Sprite *sprites){
	srandom(5198985);
	if (food_x >= 0 && food_x < 20){
		food_x = 30 % rand();
	}
	if (food_y >= 0 && food_y < 20){
		food_y = 30 % rand();
	}
	init_sprite (&food, food_x, food_y, 3, 3, bitmap2);
}

int main(void) {

	draw_string(0, 0, "Hello CAB202!!!");
	show_screen();

	while ( 1 ) {
		play_gamne();
	}
	return 0;
}
