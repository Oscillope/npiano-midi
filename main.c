#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h> //For sleep
#include <alsa/asoundlib.h>

#define C	12+(octave*12)
#define C_	C+1
#define D	C+2
#define D_	C+3
#define E	C+4
#define F	C+5
#define F_	C+6
#define G	C+7
#define G_	C+8
#define A	C+9
#define A_	C+10
#define B	C+11
#define C1	C+12
#define C_1	C1+1
#define D1	C1+2
#define D_1	C1+3
#define E1	C1+4
#define F1	C1+5
#define F_1	C1+6
#define G1	C1+7
#define G_1	C1+8
#define A1	C1+9
#define A_1	C1+10
#define B1	C1+11
#define C2	C1+12

void play(snd_seq_t* s, int n, int l);
void set(int y, int x, char c, int status);
void draw(int y, int x);

/*
 *	Keyboard Playing Program
 *	Enzo Barrett
 */

int main(int argc, char** argv)
{
	// ALSA seq
	snd_seq_t *seq;
	int err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
	if (err) {
		printf("Error opening ALSA seq: %s\n", snd_strerror(err));
		exit(err);
	}
	int port = snd_seq_create_simple_port(seq, "mnpiano",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SYNC_READ,
		SND_SEQ_PORT_TYPE_APPLICATION | SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	if (port < 0) {
		printf("Error opening ALSA port: %s\n", snd_strerror(port));
		snd_seq_close(seq);
		exit(port);
	}
	err = snd_seq_connect_to(seq, port, atoi(argv[1]), 0);
	if (err) {
		printf("Couldn't connect to synth: %s\n", snd_strerror(err));
		snd_seq_close(seq);
		exit(err);
	}
	err = snd_seq_set_output_buffer_size(seq, 512);
	if (err) {
		printf("Couldn't set buffer size: %s\n", snd_strerror(err));
		snd_seq_close(seq);
		exit(err);
	}
	initscr(); //Start ncurses
	noecho(); //No key output on screen
	curs_set(0); //Cursor be gone!
	keypad(stdscr, TRUE); //Makes arrow keys work
	//Print keyboard
	int posx = (COLS/2)-30;
	int posy = (LINES/2)-5;
	draw(posy,posx);
	mvprintw(0, 0, "client %d port %d connected to %d", snd_seq_client_id(seq), port, atoi(argv[1]));
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
	int octave = 3;
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
				mvprintw(0, COLS-11, "Length %4d", leng);
				break;
			case KEY_UP:
				y = -1;
				if (leng < 1000) {
					leng+=10;
				}
				mvprintw(0, COLS-11, "Length %4d", leng);
				break;
			case KEY_LEFT:
				y = -1;
				if (octave > 1) {
					octave--;
				}
				mvprintw(0, 0, "Octave %d", octave);
				break;
			case KEY_RIGHT:
				y = -1;
				if (octave < 5) {
					octave++;
				}
				mvprintw(0, 0, "Octave %d", octave);
				break;
			case 'z':
				y = row1;
				x = posx+2;
				set(y, x, c, 1);
				play(seq, C, leng);
				break;
			case 'x':
				y = row1;
				x = posx+6;
				set(y, x, c, 1);
				play(seq, D, leng);
				break;
			case 'c':
				y = row1;
				x = posx+10;
				set(y, x, c, 1);
				play(seq, E, leng);
				break;
			case 'v':
				y = row1;
				x = posx+14;
				set(y, x, c, 1);
				play(seq, F, leng);
				break;
			case 'b':
				y = row1;
				x = posx+18;
				set(y, x, c, 1);
				play(seq, G, leng);
				break;
			case 'n':
				y = row1;
				x = posx+22;
				set(y, x, c, 1);
				play(seq, A, leng);
				break;
			case 'm':
				y = row1;
				x = posx+26;
				set(y, x, c, 1);
				play(seq, B, leng);
				break;
			case 'q':
				y = row1;
				x = posx+30;
				set(y, x, c, 1);
				play(seq, C1, leng);
				break;
			case 'w':
				y = row1;
				x = posx+34;
				set(y, x, c, 1);
				play(seq, D1, leng);
				break;
			case 'e':
				y = row1;
				x = posx+38;
				set(y, x, c, 1);
				play(seq, E1, leng);
				break;
			case 'r':
				y = row1;
				x = posx+42;
				set(y, x, c, 1);
				play(seq, F1, leng);
				break;
			case 't':
				y = row1;
				x = posx+46;
				set(y, x, c, 1);
				play(seq, G1, leng);
				break;
			case 'y':
				y = row1;
				x = posx+50;
				set(y, x, c, 1);
				play(seq, A1, leng);
				break;
			case 'u':
				y = row1;
				x = posx+54;
				set(y, x, c, 1);
				play(seq, B1, leng);
				break;
			case 'i':
				y = row1;
				x = posx+58;
				set(y, x, c, 1);
				play(seq, C2, leng);
				break;
			case 's':
				y = row2;
				x = posx+4;
				set(y, x, c, 1);
				play(seq, C_, leng);
				break;
			case 'd':
				y = row2;
				x = posx+8;
				set(y, x, c, 1);
				play(seq, D_, leng);
				break;
			case 'g':
				y = row2;
				x = posx+16;
				set(y, x, c, 1);
				play(seq, F_, leng);
				break;
			case 'h':
				y = row2;
				x = posx+20;
				set(y, x, c, 1);
				play(seq, G_, leng);
				break;
			case 'j':
				y = row2;
				x = posx+24;
				set(y, x, c, 1);
				play(seq, A_, leng);
				break;
			case '2':
				y = row2;
				x = posx+32;
				set(y, x, c, 1);
				play(seq, C_1, leng);
				break;
			case '3':
				y = row2;
				x = posx+36;
				set(y, x, c, 1);
				play(seq, D_1, leng);
				break;
			case '5':
				y = row2;
				x = posx+44;
				set(y, x, c, 1);
				play(seq, F_1, leng);
				break;
			case '6':
				y = row2;
				x = posx+48;
				set(y, x, c, 1);
				play(seq, G_1, leng);
				break;
			case '7':
				y = row2;
				x = posx+52;
				set(y, x, c, 1);
				play(seq, A_1, leng);
				break;
			default:
				y = -1;
				x = -1;
				break;
		}
		if (y != -1) {
			set(y, x, c, 0);
		}
		if (c == KEY_DOWN || c == KEY_UP || c == KEY_LEFT || c == KEY_RIGHT) {
			refresh();
		}
		last = c;
		ly = y;
		lx = x;
	}
	endwin();
	snd_seq_close(seq);
	return 0;
}

void play(snd_seq_t* s, int n, int len)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_noteon(&ev, 0, n, 100);
	int err = snd_seq_event_output_direct(s, &ev);
	if (err < 0) {
		move(0, 0);
		clrtoeol();
		printw("error: %s length %zu bufsize %zu", snd_strerror(err), snd_seq_event_length(&ev), snd_seq_get_output_buffer_size(s));
		refresh();
	} else {
		move(0, 0);
		clrtoeol();
		refresh();
	}
	usleep(len*1000);
	snd_seq_ev_set_noteoff(&ev, 0, n, 0);
	snd_seq_event_output_direct(s, &ev);
}

void set(int y, int x, char c, int state)
{
	//Either turn on or off key highlight
	move(y,x);
	if (state == 1) {
		addch(c | A_STANDOUT);
	} else {
		addch(c);
	}
	refresh();
}

void draw(int posy, int posx)
{
	//Clear the screen and draw the keyboard, terminal size dependent
	clear();
	mvprintw(posy,   posx, "_____________________________________________________________");
	mvprintw(posy+1, posx, "|  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |");
	mvprintw(posy+2, posx, "|  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |");
	mvprintw(posy+3, posx, "|  |s| |d|  |  |g| |h| |j|  |  |2| |3|  |  |5| |6| |7|  |   |");
	mvprintw(posy+4, posx, "|  | | | |  |  | | | | | |  |  | | | |  |  | | | | | |  |   |");
	mvprintw(posy+5, posx, "|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |");
	mvprintw(posy+6, posx, "| z | x | c | v | b | n | m | q | w | e | r | t | y | u | i |");
	mvprintw(posy+7, posx, "|___|___|___|___|___|___|___|___|___|___|___|___|___|___|___|");
	refresh();
}

