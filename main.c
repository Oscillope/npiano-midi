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

/*
 *	Keyboard Playing Program
 *	Enzo Barrett
 *	ALSA seq support added by
 *	Jason Rosenman
 */

void play(snd_seq_t* s, unsigned m, int o);
void set(int x, int y, int status);
void draw(int x, int y);

struct point {
	int x;
	int y;
};

struct mnp_data {
	snd_seq_t* seq;
	struct point keyboard_array[25];	/* 25 keys */
	unsigned notemask;
	unsigned offmask;
	unsigned octave;
};

void noteoff_timer_func(union sigval sig) {
	struct mnp_data* data = (struct mnp_data*)sig.sival_ptr;
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	int i = 0;
	for (; i < sizeof(data->keyboard_array); i++) {
		if ((data->offmask >> i) & 0x1) {
			snd_seq_ev_set_noteoff(&ev, 0, i + (data->octave * 12), 0);
			data->offmask &= ~(0x1 << i);
			snd_seq_event_output_direct(data->seq, &ev);
			int x = data->keyboard_array[i].x;
			int y = data->keyboard_array[i].y;
			set(x, y, 0);
		}
	}
}

unsigned set_note(struct mnp_data* data, unsigned note)
{
	if (data->offmask & note) {
		return false;
	} else {
		data->notemask |= note;
		int x = data->keyboard_array[__builtin_ctz(note)].x;
		int y = data->keyboard_array[__builtin_ctz(note)].y;
		set(x, y, 1);
	}
	return true;
}

void build_key_table(struct mnp_data* data, int posx, int posy)
{
	//Y for rows
	int row1 = posy + 6;
	int row2 = posy + 3;
	data->keyboard_array[0]  = (struct point){posx + 2, row1};	// z
	data->keyboard_array[1]  = (struct point){posx + 4, row2};	// s
	data->keyboard_array[2]  = (struct point){posx + 6, row1};	// x
	data->keyboard_array[3]  = (struct point){posx + 8, row2};	// d
	data->keyboard_array[4]  = (struct point){posx + 10, row1};	// c
	data->keyboard_array[5]  = (struct point){posx + 14, row1};	// v
	data->keyboard_array[6]  = (struct point){posx + 16, row2};	// g
	data->keyboard_array[7]  = (struct point){posx + 18, row1};	// b
	data->keyboard_array[8]  = (struct point){posx + 20, row2};	// h
	data->keyboard_array[9]  = (struct point){posx + 22, row1};	// n
	data->keyboard_array[10] = (struct point){posx + 24, row2};	// j
	data->keyboard_array[11] = (struct point){posx + 26, row1};	// m
	data->keyboard_array[12] = (struct point){posx + 30, row1};	// q
	data->keyboard_array[13] = (struct point){posx + 32, row2};	// 2
	data->keyboard_array[14] = (struct point){posx + 34, row1};	// w
	data->keyboard_array[15] = (struct point){posx + 36, row2};	// 3
	data->keyboard_array[16] = (struct point){posx + 38, row1};	// e
	data->keyboard_array[17] = (struct point){posx + 42, row1};	// r
	data->keyboard_array[18] = (struct point){posx + 44, row2};	// 5
	data->keyboard_array[19] = (struct point){posx + 46, row1};	// t
	data->keyboard_array[20] = (struct point){posx + 48, row2};	// 6
	data->keyboard_array[21] = (struct point){posx + 50, row1};	// y
	data->keyboard_array[22] = (struct point){posx + 52, row2};	// 7
	data->keyboard_array[23] = (struct point){posx + 54, row1};	// u
	data->keyboard_array[24] = (struct point){posx + 58, row1};	// i
}

void usage(char* name)
{
	printf("Usage: %s CLIENT:PORT\n", name);
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		usage(argv[0]);
		exit(-1);
	}
	char* port_str = argv[1];
	const char* client_str = strsep(&port_str, ":");
	if (!port_str || !client_str) {
		usage(argv[0]);
		exit(-1);
	}
	unsigned dest_client = atoi(client_str);
	unsigned dest_port = atoi(port_str);
	if (dest_client >= 255) {
		printf("CLIENT should be < 255\n");
		usage(argv[0]);
		exit(-1);
	}
	if (dest_port >= 255) {
		printf("PORT should be < 255\n");
		usage(argv[0]);
		exit(-1);
	}
	// Data token
	struct mnp_data data = {NULL, { 0 }, 0, 0, 3};
	// ALSA seq
	int err = snd_seq_open(&(data.seq), "default", SND_SEQ_OPEN_OUTPUT, 0);
	if (err) {
		printf("Error opening ALSA seq: %s\n", snd_strerror(err));
		exit(err);
	}
	int port = snd_seq_create_simple_port(data.seq, "mnpiano",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SYNC_READ,
		SND_SEQ_PORT_TYPE_APPLICATION | SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	if (port < 0) {
		printf("Error opening ALSA port: %s\n", snd_strerror(port));
		snd_seq_close(data.seq);
		exit(port);
	}
	err = snd_seq_connect_to(data.seq, port, dest_client, dest_port);
	if (err) {
		printf("Couldn't connect to synth: %s\n", snd_strerror(err));
		snd_seq_close(data.seq);
		exit(err);
	}
	err = snd_seq_set_output_buffer_size(data.seq, 512);
	if (err) {
		printf("Couldn't set buffer size: %s\n", snd_strerror(err));
		snd_seq_close(data.seq);
		exit(err);
	}
	// mask of notes on
	unsigned notemask = 0;
	int octave = 3;
	// Note-off timer
	timer_t note_timer;
	struct sigevent sig = (struct sigevent){
		.sigev_notify = SIGEV_THREAD,
		.sigev_value = (union sigval) { .sival_ptr = &data },
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
	build_key_table(&data, posx, posy);
	draw(posx, posy);
	mvprintw(0, 0, "%d:%d connected to %u:%u", snd_seq_client_id(data.seq), port, dest_client, dest_port);
	//Input holder
	int c;
	//length
	int leng = 100;
	//Loop until user quits
	while ((c = getch()) != EOF) {
		//whether or not to play a note
		bool do_play = false;
		switch (c) {
			case KEY_RESIZE:
			case KEY_BACKSPACE:
				goto exit;
			case KEY_DOWN:
				if (leng > 0) {
					leng-=10;
				}
				mvprintw(0, COLS-11, "Length %4d", leng);
				break;
			case KEY_UP:
				if (leng < 1000) {
					leng+=10;
				}
				mvprintw(0, COLS-11, "Length %4d", leng);
				break;
			case KEY_LEFT:
				if (data.octave > 1) {
					data.octave--;
				}
				mvprintw(0, 0, "Octave %d", data.octave);
				break;
			case KEY_RIGHT:
				if (data.octave < 5) {
					data.octave++;
				}
				mvprintw(0, 0, "Octave %d", data.octave);
				break;
			case 'z':
				do_play = set_note(&data, C);
				break;
			case 'x':
				do_play = set_note(&data, D);
				break;
			case 'c':
				do_play = set_note(&data, E);
				break;
			case 'v':
				do_play = set_note(&data, F);
				break;
			case 'b':
				do_play = set_note(&data, G);
				break;
			case 'n':
				do_play = set_note(&data, A);
				break;
			case 'm':
				do_play = set_note(&data, B);
				break;
			case 'q':
				do_play = set_note(&data, C1);
				break;
			case 'w':
				do_play = set_note(&data, D1);
				break;
			case 'e':
				do_play = set_note(&data, E1);
				break;
			case 'r':
				do_play = set_note(&data, F1);
				break;
			case 't':
				do_play = set_note(&data, G1);
				break;
			case 'y':
				do_play = set_note(&data, A1);
				break;
			case 'u':
				do_play = set_note(&data, B1);
				break;
			case 'i':
				do_play = set_note(&data, C2);
				break;
			case 's':
				do_play = set_note(&data, C_);
				break;
			case 'd':
				do_play = set_note(&data, D_);
				break;
			case 'g':
				do_play = set_note(&data, F_);
				break;
			case 'h':
				do_play = set_note(&data, G_);
				break;
			case 'j':
				do_play = set_note(&data, A_);
				break;
			case '2':
				do_play = set_note(&data, C_1);
				break;
			case '3':
				do_play = set_note(&data, D_1);
				break;
			case '5':
				do_play = set_note(&data, F_1);
				break;
			case '6':
				do_play = set_note(&data, G_1);
				break;
			case '7':
				do_play = set_note(&data, A_1);
				break;
			default:
				break;
		}
		if (do_play) {
			play(data.seq, data.notemask, data.octave);
			data.offmask |= data.notemask;
			data.notemask = 0;
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
exit:
	endwin();
	snd_seq_close(data.seq);
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
}

void set(int x, int y, int state)
{
	//Either turn on or off key highlight
	mvchgat(y, x, 1, (state ? A_STANDOUT : A_NORMAL), 0, NULL);
	refresh();
}

void draw(int posx, int posy)
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

