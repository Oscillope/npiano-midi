#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h> //For sleep

#define C	130.81
#define C_	138.59
#define D	146.83
#define D_	155.56
#define E	164.81
#define F	174.61
#define F_	185.00
#define G	196.00
#define G_	207.65
#define A	220.00
#define A_	233.08
#define B	246.94
#define C1	261.63
#define C_1 C_*2.0
#define D1	D*2.0
#define D_1 D_*2.0
#define E1	E*2.0
#define F1	F*2.0
#define F_1 F_*2.0
#define G1	G*2.0
#define G_1 G_*2.0
#define A1	A*2.0
#define A_1 A_*2.0
#define B1	B*2.0
#define C2	C1*2.0

void play(double d, int l);
void set(int y, int x, char c, int status);
void draw(int y, int x);

/*
 *	Keyboard Playing Program
 *	Enzo Barrett
 */

int main()
{
	initscr(); //Start ncurses
	noecho(); //No key output on screen
	curs_set(0); //Cursor be gone!
	keypad(stdscr, TRUE); //Makes arrow keys work
	//Print keyboard
	int posx = (COLS/2)-30;
	int posy = (LINES/2)-5;
	draw(posy,posx);
	//Input holder
	int c;
	//Y for rows
	int row1 = posy+6;
	int row2 = posy+3;
	//Last times vars
	int last = 0;
	int lx;
	int ly;
	//xy
	int x,y;
	//length
	int leng = 100;
	//Loop until user quits
	while ((c = getch()) != EOF) {
		switch (c) {
			case KEY_RESIZE:
				endwin();
				return 1;
			case KEY_DOWN:
				y = -1;
				if (leng > 0) {
					leng-=10;
				}
				mvprintw(0, COLS-4, "%4d", leng);
				break;
			case KEY_UP:
				y = -1;
				if (leng < 1000) {
					leng+=10;
				}
				mvprintw(0, COLS-4, "%4d", leng);
				break;
			case 'z':
				y = row1;
				x = posx+2;
				set(y, x, c, 1);
				play(C, leng);
				break;
			case 'x':
				y = row1;
				x = posx+6;
				set(y, x, c, 1);
				play(D, leng);
				break;
			case 'c':
				y = row1;
				x = posx+10;
				set(y, x, c, 1);
				play(E, leng);
				break;
			case 'v':
				y = row1;
				x = posx+14;
				set(y, x, c, 1);
				play(F, leng);
				break;
			case 'b':
				y = row1;
				x = posx+18;
				set(y, x, c, 1);
				play(G, leng);
				break;
			case 'n':
				y = row1;
				x = posx+22;
				set(y, x, c, 1);
				play(A, leng);
				break;
			case 'm':
				y = row1;
				x = posx+26;
				set(y, x, c, 1);
				play(B, leng);
				break;
			case 'q':
				y = row1;
				x = posx+30;
				set(y, x, c, 1);
				play(C1, leng);
				break;
			case 'w':
				y = row1;
				x = posx+34;
				set(y, x, c, 1);
				play(D1, leng);
				break;
			case 'e':
				y = row1;
				x = posx+38;
				set(y, x, c, 1);
				play(E1, leng);
				break;
			case 'r':
				y = row1;
				x = posx+42;
				set(y, x, c, 1);
				play(F1, leng);
				break;
			case 't':
				y = row1;
				x = posx+46;
				set(y, x, c, 1);
				play(G1, leng);
				break;
			case 'y':
				y = row1;
				x = posx+50;
				set(y, x, c, 1);
				play(A1, leng);
				break;
			case 'u':
				y = row1;
				x = posx+54;
				set(y, x, c, 1);
				play(B1, leng);
				break;
			case 'i':
				y = row1;
				x = posx+58;
				set(y, x, c, 1);
				play(C2, leng);
				break;
			case 's':
				y = row2;
				x = posx+4;
				set(y, x, c, 1);
				play(C_, leng);
				break;
			case 'd':
				y = row2;
				x = posx+8;
				set(y, x, c, 1);
				play(D_, leng);
				break;
			case 'g':
				y = row2;
				x = posx+16;
				set(y, x, c, 1);
				play(F_, leng);
				break;
			case 'h':
				y = row2;
				x = posx+20;
				set(y, x, c, 1);
				play(G_, leng);
				break;
			case 'j':
				y = row2;
				x = posx+24;
				set(y, x, c, 1);
				play(A_, leng);
				break;
			case '2':
				y = row2;
				x = posx+32;
				set(y, x, c, 1);
				play(C_1, leng);
				break;
			case '3':
				y = row2;
				x = posx+36;
				set(y, x, c, 1);
				play(D_1, leng);
				break;
			case '5':
				y = row2;
				x = posx+44;
				set(y, x, c, 1);
				play(F_1, leng);
				break;
			case '6':
				y = row2;
				x = posx+48;
				set(y, x, c, 1);
				play(G_1, leng);
				break;
			case '7':
				y = row2;
				x = posx+52;
				set(y, x, c, 1);
				play(A_1, leng);
				break;
			default:
				y = -1;
				x = -1;
				break;
		}
		if (y != -1) {
			set(y, x, c, 0);
		}
		if (c == KEY_DOWN || c == KEY_UP) {
			refresh();
		}
		last = c;
		ly = y;
		lx = x;
	}
	endwin();
	return 0;
}

void play(double f, int len)
{
	//Play beep based on frequency and length selected
	char command[50];
	snprintf(command, sizeof(command), "beep -f %f -l %d", f, len);
	system(command);
}

void set(int y, int x, char c, int state)
{
	//Either turn on or off key highlight
	move(y,x);
	if(state == 1)
	addch(c | A_STANDOUT);
	else
	addch(c);
	refresh();
}

void draw(int posy, int posx)
{
	//Clear the screen and draw the keyboard, terminal size dependent
	clear();
	mvprintw(posy,	 posx, "_____________________________________________________________");
	mvprintw(posy+1, posx, "|  | | | |	|  | | | | | |	|  | | | |	|  | | | | | |	|	|");
	mvprintw(posy+2, posx, "|  | | | |	|  | | | | | |	|  | | | |	|  | | | | | |	|	|");
	mvprintw(posy+3, posx, "|  |s| |d|	|  |g| |h| |j|	|  |2| |3|	|  |5| |6| |7|	|	|");
	mvprintw(posy+4, posx, "|  | | | |	|  | | | | | |	|  | | | |	|  | | | | | |	|	|");
	mvprintw(posy+5, posx, "|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	|");
	mvprintw(posy+6, posx, "| z | x | c | v | b | n | m | q | w | e | r | t | y | u | i |");
	mvprintw(posy+7, posx, "|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|");
	refresh();
}

