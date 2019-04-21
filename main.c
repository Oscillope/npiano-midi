#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h> //For sleep

#include <alsa/asoundlib.h>

#define C	0x00000001
#define C_	0x00000002
#define D	0x00000004
#define D_	0x00000008
#define E	0x00000010
#define F	0x00000020
#define F_	0x00000040
#define G	0x00000080
#define G_	0x00000100
#define A	0x00000200
#define A_	0x00000400
#define B	0x00000800
#define C1	0x00001000
#define C_1	0x00002000
#define D1	0x00004000
#define D_1	0x00008000
#define E1	0x00010000
#define F1	0x00020000
#define F_1	0x00040000
#define G1	0x00080000
#define G_1	0x00100000
#define A1	0x00200000
#define A_1	0x00400000
#define B1	0x00800000
#define C2	0x01000000

void play(snd_seq_t* s, unsigned m, int o);
void set(int y, int x, char c, int status);
void draw(int y, int x);

struct notify {
	unsigned offmask;
	int octave;
	snd_seq_t* seq;
};

void noteoff_timer_func(union sigval sig) {
	struct notify* nfy = (struct notify*)sig.sival_ptr;
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	//mvprintw(0, 0, "off-mask %08x", nfy->offmask);
	//refresh();
	int i = 0;
	for (; i < sizeof(unsigned)*8; i++) {
		if ((nfy->offmask >> i) & 0x1) {
			snd_seq_ev_set_noteoff(&ev, 0, i + (nfy->octave * 12), 0);
			nfy->offmask &= ~(0x1 << i);
			snd_seq_event_output_direct(nfy->seq, &ev);
		}
	}
}

/*
 *	Keyboard Playing Program
 *	Enzo Barrett
 *	ALSA seq support added by
 *	Jason Rosenman
 */

int main(int argc, char** argv)
{
	// ALSA seq
	snd_seq_t* seq;
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
	// mask of notes on
	unsigned notemask = 0;
	int octave = 3;
	// Note-off timer
	timer_t note_timer;
	struct notify signotify = { 0, octave, seq };
	struct sigevent sig = (struct sigevent){
		.sigev_notify = SIGEV_THREAD,
		.sigev_value = (union sigval) { .sival_ptr = &signotify },
		.sigev_notify_function = noteoff_timer_func
	};
	timer_create(CLOCK_MONOTONIC, &sig, &note_timer);
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
				notemask |= C;
				break;
			case 'x':
				y = row1;
				x = posx+6;
				set(y, x, c, 1);
				notemask |= D;
				break;
			case 'c':
				y = row1;
				x = posx+10;
				set(y, x, c, 1);
				notemask |= E;
				break;
			case 'v':
				y = row1;
				x = posx+14;
				set(y, x, c, 1);
				notemask |= F;
				break;
			case 'b':
				y = row1;
				x = posx+18;
				set(y, x, c, 1);
				notemask |= G;
				break;
			case 'n':
				y = row1;
				x = posx+22;
				set(y, x, c, 1);
				notemask |= A;
				break;
			case 'm':
				y = row1;
				x = posx+26;
				set(y, x, c, 1);
				notemask |= B;
				break;
			case 'q':
				y = row1;
				x = posx+30;
				set(y, x, c, 1);
				notemask |= C1;
				break;
			case 'w':
				y = row1;
				x = posx+34;
				set(y, x, c, 1);
				notemask |= D1;
				break;
			case 'e':
				y = row1;
				x = posx+38;
				set(y, x, c, 1);
				notemask |= E1;
				break;
			case 'r':
				y = row1;
				x = posx+42;
				set(y, x, c, 1);
				notemask |= F1;
				break;
			case 't':
				y = row1;
				x = posx+46;
				set(y, x, c, 1);
				notemask |= G1;
				break;
			case 'y':
				y = row1;
				x = posx+50;
				set(y, x, c, 1);
				notemask |= A1;
				break;
			case 'u':
				y = row1;
				x = posx+54;
				set(y, x, c, 1);
				notemask |= B1;
				break;
			case 'i':
				y = row1;
				x = posx+58;
				set(y, x, c, 1);
				notemask |= C2;
				break;
			case 's':
				y = row2;
				x = posx+4;
				set(y, x, c, 1);
				notemask |= C_;
				break;
			case 'd':
				y = row2;
				x = posx+8;
				set(y, x, c, 1);
				notemask |= D_;
				break;
			case 'g':
				y = row2;
				x = posx+16;
				set(y, x, c, 1);
				notemask |= F_;
				break;
			case 'h':
				y = row2;
				x = posx+20;
				set(y, x, c, 1);
				notemask |= G_;
				break;
			case 'j':
				y = row2;
				x = posx+24;
				set(y, x, c, 1);
				notemask |= A_;
				break;
			case '2':
				y = row2;
				x = posx+32;
				set(y, x, c, 1);
				notemask |= C_1;
				break;
			case '3':
				y = row2;
				x = posx+36;
				set(y, x, c, 1);
				notemask |= D_1;
				break;
			case '5':
				y = row2;
				x = posx+44;
				set(y, x, c, 1);
				notemask |= F_1;
				break;
			case '6':
				y = row2;
				x = posx+48;
				set(y, x, c, 1);
				notemask |= G_1;
				break;
			case '7':
				y = row2;
				x = posx+52;
				set(y, x, c, 1);
				notemask |= A_1;
				break;
			default:
				y = -1;
				x = -1;
				break;
		}
		if (y != -1) {
			set(y, x, c, 0);
			play(seq, notemask, octave);
			signotify.offmask |= notemask;
			notemask = 0;
			signotify.octave = octave;
			if (timer_gettime(note_timer, NULL)) {
				struct itimerspec time = {{ 0 }};
				time.it_value.tv_nsec = leng * 1000000;
				timer_settime(note_timer, 0, &time, NULL);
			}
		}
		if (c == KEY_DOWN || c == KEY_UP || c == KEY_LEFT || c == KEY_RIGHT) {
			refresh();
		}
	}
	endwin();
	snd_seq_close(seq);
	return 0;
}

void play(snd_seq_t* s, unsigned mask, int octave)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	int i = 0;
	int err = 0;
	for (; i < sizeof(unsigned)*8; i++) {
		if (mask >> i & 0x1) {
			snd_seq_ev_set_noteon(&ev, 0, i + (octave * 12), 100);
			err = snd_seq_event_output_direct(s, &ev);
			if (err < 0) {
				break;
			}
		}
	}
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
	//usleep(len*1000);
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

