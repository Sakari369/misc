/*
 * $Id: ui_ncurses.c,v 1.5 2003/08/30 16:10:40 sakari Exp $
 *
 * Ncurses user interface for Umix. Probably could use a total rewrite. But
 * hey, this works for me.
 *
 * Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <ncurses.h>
#include "umix_common.h"
#include "ui.h"

#define UMIX_UI_NCURSES_VERSION "0.94"

static int ui_ncurses_initialized = 0;

/* the width of the channel labels */
static int labelwidth[MAX_MIXERS];	
/* store number of channels for every mixer globally for optimization */
static int numchan[MAX_MIXERS];
/* hack hackity */
static int ownupdate = 0;
/* the current mixer, stored here for optimization */
static int currmixer = 0;

/* position constants */
enum
{
	LABELMAX	= 10,		 /* max width of labels */
	BEGX		= 0,		 /* beginning x of the layout */
	BEGY		= 2,		 /* beginning y of the layout */
	VOLCHG		= (VOL_MAX / 50),/* amount of volume change */
	BALCHG		= 8,		 /* amount of balance change */
	NUMWIDTH	= 3,		 /* width of volume numbers */
};

/* position macros */

/* width of the bars */
#define BALWIDTH	((COLS/8) + 2)
#define VOLWIDTH	(COLS - BALWIDTH - NUMWIDTH - labelwidth[currmixer] - 8)

/* where the bars begin */
#define VOLBEGX		(BEGX+labelwidth[currmixer])
#define VOLBEGY		(BEGY+curr_chan)
#define BALBEGX		((COLS-3) - BALWIDTH)
#define BALBEGY		(BEGY+curr_chan)
#define STATBEGX	(VOLBEGX+NUMWIDTH)

/* the current channel marker positions */
#define VOLPOSX1	(VOLBEGX+NUMWIDTH+1)
#define VOLPOSX2	(VOLPOSX1+VOLWIDTH+1)
#define BALPOSX1	(BALBEGX-1)
#define BALPOSX2	(BALBEGX+BALWIDTH+1)

#ifndef CTRL
  #define CTRL(a)       ((a) & 0x1F) 
#endif

/* settings */
enum
{
	SET_LOAD,
	SET_SAVE,
	SET_CONFIG
};

/* which border */
enum
{
	BORD_UPPER,
	BORD_LOWER
};

/* mute action */
enum
{
	M_MUTE,
	M_EXCLUDE
};

/* the characters used in drawing */
enum
{
	CHAR_SLIDER	= '*',
	CHAR_DIV	= ':',
	CHAR_EMPTY	= '.',
	CHAR_MONO	= '|'
};

/* saved cols and lines */
static int saved_cols = 0;
static int saved_lines = 0;

/* global function prototypes */
int ui_ncurs_main(int argc, char *argv[]);
void ui_ncurs_exit(void);

/* internal ui_ncurses function prototypes */
static int main_loop(void);
static void init_screen(void);
static void cycle_mixer(void);
static void mute(int ch);
static void update_stat(void);
static void draw_slider(void);
static void draw_balance(void);
static void draw_status(void);
static void draw_all(void);
static void draw_border(int where, const char *str);
static void draw_pos(int menu, int status);
static void redraw_screen(void);
static void help(void);
static void about(void);
static void settings(int action);
static void input(char *buf, const char *prompt, int limit);
static void set_vol_manual(void);
static void calc_label_widths();
static int window_size_changed(void);

ui_func ncurses_func =
{
	ui_ncurs_main,
	ui_ncurs_exit
};

ui_func *ncurses_get_ui_func(void)
{
	return &ncurses_func;
}

int ui_ncurs_main(int argc, char *argv[])
{
	/* initialize mixers */
	if (init_mixers() == -1)
	{
		err_msg(_("Failed initializing mixer devices"));
		return -1;
	}
	
	/* initialize curses */
	init_screen();
	calc_label_widths();
	/* set current mixer to 0 */
	mixer_set_curr(0);
	currmixer = 0;
	ui_ncurses_initialized = 1;
	/* enter main ncurses UI loop */
	main_loop();
	
	return 0;
}
	

static int main_loop(void)
{
	int ch;
	int curr_chan; 
	int menudir;
	int curr_menu;
	int slidedir;
	int extradir;
	int quit;
	float change; 

	quit = 0;
	ch = 0;
	curr_menu = 0;
	curr_chan = 0;
	chan_set_curr(curr_chan);

	while(1)
	{
		menudir = 0;
		slidedir = 0;
		extradir = 0;

		/* this is called at least once in the beginning */
		if (window_size_changed() == 1)
			draw_pos(curr_menu, 1);

		/* process input */
		if ((ch = getch()) != ERR)
		{
			switch(ch)
			{
			case KEY_DOWN:
			case CTRL('n'):
			case 'j': menudir = 1; 
				  break;
			case KEY_UP: 
			case CTRL('p'):
			case 'k': menudir = -1; 
				  break;
			case KEY_LEFT:
			case CTRL('b'):
			case 'h': slidedir = -1; 
				  break;
			case KEY_RIGHT:
			case CTRL('f'):
			case 'l': slidedir = 1; 
				  break;
			case ',':
			case '[': extradir = -(VOL_MAX/5); 
				  break;
			case '.':
			case ']': extradir = VOL_MAX/5; 
				  break;
			case ' ': chan_toggle_rec(); 
				  update_stat(); 
				  break;
			case CTRL('l'): redraw_screen(); 
					break;
			case 'H': help(); 
				  break; 
			case 'A': about(); 
				  break;
			case 'L': settings(SET_LOAD); 
				  break;
			case 'S': settings(SET_SAVE);
				  break;
			case 'e': set_vol_manual(); 
				  break;
			case 'f': settings(SET_CONFIG);
				  break;
			case 'x': mute(curr_chan); 
				  break;
			case 'm': mute(-1); 
				  break;
			case 'n': cycle_mixer(); 
				  break;
			case 'q': quit = 1; 
				  break;
			default:  break;
			}
			if (quit == 1)
				break;
			
			/* yes hack */
			ownupdate = 1;
		}
		/* check if mixer has been updated */
		if (mixer_get_status() == 1)
			/* if this process didn't cause the update, redraw */
			if (ownupdate == 0)
				draw_all();
		ownupdate = 0;

		/* erase the old menu position */ 
		draw_pos(curr_menu, 0);
		
		curr_chan += menudir;
		curr_chan = CLAMP(curr_chan, 0, numchan[currmixer]-1);
		/* this is just stupid .. TODO: implement scrolling of
		 * channels if too small to fit on screen */
		if (curr_chan > (LINES - 6))
			curr_chan = LINES - 6;

		chan_set_curr(curr_chan);
		/* change the menu .. needs to be here for
		 * drawing the menupos */
		if (ch == '\t')
			curr_menu = ~curr_menu;

		/* draw the menu position */
		draw_pos(curr_menu, 1);
#ifdef UMIX_DEBUG
		mvprintw(20, 2, "loop: curr_chan=%2d curr_mixer=%2d", 
			curr_chan, currmixer);
#endif
		if (slidedir != 0 || extradir != 0)
		{
			/* the volume menu */
			if (curr_menu == 0)
			{
				change = (int)(slidedir * VOLCHG);
				change += (int) extradir;
				chan_inc_vol(change);
				draw_slider();
			}	
			/* balance menu */
			else if (chan_is_stereo())
			{
				if (extradir == 0)
					change = 0.10 * slidedir;
				else
				{
					if (extradir > 1)
						slidedir = 1;
					else
						slidedir = -1;
					change = 1.00 * slidedir;
				}
				chan_inc_bal(change);
				draw_balance();
			}
#ifdef UMIX_DEBUG
			mvprintw(23, 2, "loop: volume is %3d:%3d",
				chan_get_lr(CHAN_LEFT), 
				chan_get_lr(CHAN_RIGHT));
#endif
		}
		refresh();
		napms(20);
	}
	
	return 0;
}

/* calculate the max length of the labels */
static void calc_label_widths(void)
{
	int i;
	int j;
	int n;
	int numch;

	for (i=0; i<mixer_get_num_mix(); i++)
	{
		mixer_set_curr(i);
		numch = mixer_get_num_chan();
		numchan[i] = numch;
		for (j=0; j<numch; j++)
		{
			/* labelwidth for every mixer is the longest
			 * channel label name in that mixer 
			 * clamped to LABELMAX+1 */
			chan_set_curr(j);
			n = strlen(chan_get_label());
			if (n > labelwidth[i])
			{
				n = CLAMP(n, n, LABELMAX+1);
				labelwidth[i] = n;
			}
		}

	}
}
		
/* check if the window size has changed, and update if has */
static int window_size_changed(void)
{
	if ((saved_cols != COLS) || (saved_lines != LINES))
	{
		redraw_screen();
		saved_cols = COLS;
		saved_lines = LINES;

		return 1;
	}

	return 0;
}

/* update status for all channels */
static void update_stat(void)
{
	int orig_chan;
	int i;

	orig_chan = chan_get_curr();
	for (i=0; i<numchan[currmixer]; i++)
	{
		chan_set_curr(i);
		draw_status();
	}
	chan_set_curr(orig_chan);
}

/* cycles through available mixer devices */
static void cycle_mixer(void)
{
	int num_mix;
	int old;

	num_mix = mixer_get_num_mix();
	old = currmixer;

	if (currmixer+1 < num_mix)
		currmixer++;
	else if (currmixer+1 >= num_mix)
		currmixer = 0;

	if (currmixer != old)
	{
		mixer_set_curr(currmixer);
		redraw_screen();
	}
}

/* Mutes and demutes either all channels or one channel */
static void mute(int ch)
{
	int i;
	int orig_chan;

	orig_chan = chan_get_curr();
	/* all channels */
	if (ch == -1)
	{
		for (i=0; i<numchan[currmixer]; i++)
		{
			chan_set_curr(i);
			chan_toggle_mute();
			draw_slider();
		}
	}
	/* the current channel */
	else
	{
		chan_set_curr(ch);
		chan_toggle_mute();
		draw_slider();
	}
	chan_set_curr(orig_chan);
	draw_border(BORD_LOWER, UMIX_UI_MUTE);
}

/* draws the menu position markers */
static void draw_pos(int menu, int status)
{
	int x1;
	int x2;
	int ch;
	char start;
	char end;
	char buf[LABELMAX];

	ch = chan_get_curr();

	if (status == 1)
	{
		start = '>';
		end = '<';
	}
	else
		start = end = ' ';
	/* what menu are we in ? */
	if (menu == 0)
	{
		x1 = VOLPOSX1;
		x2 = VOLPOSX2;		
	}
	else
	{
		x1 = BALPOSX1;
		x2 = BALPOSX2; 
	}
	
	attron(A_BOLD);
	mvaddch(BEGY+ch, x1, start);
	mvaddch(BEGY+ch, x2, end);
	attroff(A_BOLD);
	
	if (status == 1)
		attron(A_BOLD);
	snprintf(buf, labelwidth[currmixer]+1, "%s", chan_get_label());
	mvaddstr(BEGY+ch, BEGX, buf);
	if (status == 1) 
		attroff(A_BOLD);
}

/* draws channel status information */
static void draw_status(void)
{
	int ch;
	char rec;

	ch = chan_get_curr();

	if (chan_is_record())
	{
		if (chan_is_recsrc())
			rec = 'R';
		else
			rec = 'P';
		
		attron(A_BOLD);
		mvprintw(BEGY+ch, STATBEGX, "%c", rec);
		attroff(A_BOLD);
	}
}

/* draws the volume level and slider */
static void draw_slider(void)
{
	int i;
	int max;
	int left;
	int right;
	int sliderwidth;
	int x;
	int y;

	x = VOLBEGX;
	y = BEGY + chan_get_curr();

	left = chan_get_lr(CHAN_LEFT);
	right = chan_get_lr(CHAN_RIGHT);
	max = MAX(left, right);
	/* Width of the slider */
	sliderwidth = (max * VOLWIDTH) / VOL_MAX;
	mvprintw(y, x, "%3d", max);
	x += NUMWIDTH + 2;

	/* draw the characters indicating empty */
	for (i=0; i<VOLWIDTH; i++)
		mvaddch(y, x+i, CHAR_EMPTY);
	mvaddch(y, x, CHAR_DIV);
	mvaddch(y, x+(VOLWIDTH-1), CHAR_DIV);

	/* draw the slider */
	move(y, x);
	attron(A_BOLD);
	hline(CHAR_SLIDER, sliderwidth);
	attroff(A_BOLD);
}

/* draws the balance slider */
static void draw_balance(void)
{
	int ch;
	int y;
	int x;
	char mark;
	float bal;
	float pos;

	ch = chan_get_curr();
	x = BALBEGX;
	y = BEGY + ch;

	bal = chan_get_bal();
	
	if (bal == BAL_CENTER)
		pos = BALWIDTH/2;
	else
		pos = (BALWIDTH/2) + (bal * (BALWIDTH/2));

	/* draw the base */
	move(y, x);
	hline(CHAR_EMPTY, BALWIDTH);
	/* draw the start, end and center markers */
	mvaddch(y, x, CHAR_DIV);
	mvaddch(y, x+BALWIDTH, CHAR_DIV);
	mvaddch(y, x+(BALWIDTH/2), CHAR_DIV);
	
	if (chan_is_stereo())
		mark = CHAR_SLIDER;
	else
		mark = CHAR_MONO;

	/* draw the marker */
	attron(A_BOLD);
	mvaddch(y, x+(int)pos, mark);
	attroff(A_BOLD);
}

/* draws the label, volume, balance slider and status for every channel */ 
static void draw_all(void)
{
	int y;
	int i;
	int origch;
	char buf[LABELMAX];

	y = BEGY;

	origch = chan_get_curr();
	for (i=0; i<numchan[currmixer]; i++)
	{
		chan_set_curr(i);
		snprintf(buf, labelwidth[currmixer]+1, "%s", 
			chan_get_label());
		mvaddstr(y, BEGX, buf);
		draw_slider();
		draw_balance();
		draw_status();
		y++;
		/* TODO: fix when nothing better to do :) 
		 * kludge .. don't draw more volume bars than lines on 
		 * screen */
		if (y > (LINES - 4))
			break;
	}
	chan_set_curr(origch);
}

/* draws the upper or lower border with optional string centered */
static void draw_border(int where, const char *str)
{
	int y;
	int x;

	if (where == BORD_UPPER)
		y = 0;
	else
		y = LINES - 2;

	attron(A_REVERSE);
	move(y, 0);
	hline(' ', COLS);
	if (str != NULL)
	{
		x = COLS/2 - strlen(str)/2;
		mvaddstr(y, x, str);
	}
	attroff(A_REVERSE);
}

/* refreshes the screen */
static void redraw_screen(void)
{
	char buf[LINEWIDTH]; 

	clear();
	snprintf(buf, sizeof(buf), "%s (%s::%s) %d/%d", UMIX_VERSION_STR, 
		mixer_get_name(), mixer_get_path(), mixer_get_curr()+1,
		mixer_get_num_mix());
	draw_border(BORD_UPPER, buf);
	snprintf(buf, sizeof(buf), _("Press H for help"));
	draw_border(BORD_LOWER, buf);
	draw_all();
	refresh();
}

/* loads or saves settings depending on action */
static void settings(int action)
{
	int n;
	int retval;
	char buf[BUFLEN];

	if (action == SET_LOAD)
	{
		retval = settings_load(globalopts.cfgpath, UMIX_SET_MIXER);
		if (retval == 0)
		{
			snprintf(buf, sizeof(buf), UMIX_UI_LOADOK_STRFMT,
				globalopts.cfgpath);
			draw_all();
		}
		else switch (retval)
		{
		case UMIX_ESETERR:
			snprintf(buf, sizeof(buf), "%s: %s",
				globalopts.cfgpath, strerror(errno));
			break;
		case UMIX_ESETINVALID:
			snprintf(buf, sizeof(buf), UMIX_UI_EINVALID_STR);
			break;
		case UMIX_ESETNOTFOUND:
			snprintf(buf, sizeof(buf), UMIX_UI_ENOTFOUND_STR);
			break;
		case UMIX_ESETBINARY:
			snprintf(buf, sizeof(buf), UMIX_UI_ESETBINARY_STR);
			break;
		}
	}
	else if (action == SET_SAVE)
	{
		if ((settings_save(globalopts.cfgpath) == UMIX_ESETERR ))
			snprintf(buf, sizeof(buf), UMIX_UI_SAVEFAIL_STRFMT, 
				globalopts.cfgpath, strerror(errno));
		else
			snprintf(buf, sizeof(buf), UMIX_UI_SAVEOK_STRFMT, 
				globalopts.cfgpath);
	}
	/* falls to inputting the path for the config */
	else
	{
		input(buf, _("Enter config path: "), 64);
		n = strlen(buf);
		if ((n > 0) && (n < COLS))
		{
			strncpy(globalopts.cfgpath, buf, 
				sizeof(globalopts.cfgpath));
			snprintf(buf, sizeof(buf), UMIX_UI_CFGSET_STRFMT,
				globalopts.cfgpath);
		}
		else
			snprintf(buf, sizeof(buf), UMIX_UI_CFGNOTSET_STRFMT,
				globalopts.cfgpath);

		draw_border(BORD_LOWER, buf);
		return;
	}
	/* draw the message */
	draw_border(BORD_LOWER, buf);
}

/* gets input from the keyboard to inbuf, with optional prompt */
static void input(char *inbuf, const char *prompt, int limit)
{
	if (limit <= 1)
		return;
	nodelay(stdscr, FALSE);
	refresh();
	if (prompt != NULL)
		mvaddstr(LINES-1, 0, prompt); 	
	echo();
	getnstr(inbuf, limit-1);
	noecho();
	hline(' ', COLS);
	nodelay(stdscr, TRUE);
}

static void set_vol_manual()
{
	int retval;
	int ch;
	char *str;
	char buf[STRLEN];
	char inbuf[TINYSTRLEN/2];

	ch = chan_get_curr();
	retval = 0;

	snprintf(buf, sizeof(buf), _("Enter volume for channel %d: "), ch);
	input(inbuf, buf, sizeof(inbuf));
	if (inbuf[0] != '\0')
	{
		retval = parsechanopt(inbuf);
		if (retval != 0)
		{
			switch(retval)
			{
			case UMIX_EOPTLIMIT:
				str = UMIX_EOPTLIMIT_STR;
				break;
			case UMIX_EOPTINVALID:
				str = UMIX_EOPTINVALID_STR;
				break;
			case UMIX_EOPTNOTREC:
				str = UMIX_EOPTNOTREC_STR;
				break;
			default:
				str = inbuf;
				break;
			}
				
			snprintf(buf, sizeof(buf), UMIX_EOPTGENERAL_STRFMT, 
				chan_get_name(), str);
			draw_border(BORD_LOWER, buf);
		}
		else
		{
			draw_border(BORD_LOWER, NULL);
			draw_slider();
			draw_balance();
			update_stat();
		}
	}
}

static void help(void)
{
	clear();
	draw_border(BORD_UPPER, _("Umix - Help"));
	draw_border(BORD_LOWER, _("Press any key to return"));
	mvaddstr(BEGY, BEGX,
		_(
		"j / ^n / arrow down\tMove down\n"
		"k / ^p / arrow up\tMove up\n"
		"h / ^b / arrow left\tDecrease volume / balance left\n"
		"l / ^f / arrow right\tIncrease volume / balance right\n"
		"] / ,\t\t\tIncrease volume by fifth of maximum amount\n"
		"[ / .\t\t\tDecrease volume by fifth of maximum amount\n"
		"Tab\t\t\tToggle between volume and balance menus\n"
		"Space\t\t\tToggle play / record source\n"
		"L\t\t\tLoad settings\n"
		"S\t\t\tSave settings\n"
		"f\t\t\tSpecify the config path for settings\n"
		"e\t\t\tEnter volume manually for current channel\n"
		"m\t\t\tMute all channels, undo if already mute\n"
		"n\t\t\tCycle through available mixers\n"
		"x\t\t\tSame as m but with only with the current channel\n"
		"H\t\t\tThis help\n"
		"A\t\t\tAbout Umix\n"
		"q\t\t\tExit the program\n"
		)
	);
	refresh();
	nodelay(stdscr, FALSE);
	getch();
	nodelay(stdscr, TRUE);
	redraw_screen();
}

static void about(void)
{
	char ver_buf[LINEWIDTH];

	snprintf(ver_buf, sizeof(ver_buf), 
		"%s - Ncurses UI v%s\n\n", UMIX_VERSION_STR,
		UMIX_UI_NCURSES_VERSION);

	clear();
	draw_border(BORD_UPPER, _("Umix - About"));
	draw_border(BORD_LOWER, _("Press any key to return"));
	mvaddstr(BEGY, BEGX, ver_buf);
	mvaddstr(BEGY+2, BEGX,
		_(
		"A program for adjusting soundcard volumes and other\n"
		"features in soundcard mixers.\n\n"
		"Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>\n"
		)
	);
	refresh();
	nodelay(stdscr, FALSE);
	getch();
	nodelay(stdscr, TRUE);
	redraw_screen();
}

static void init_screen(void)
{
	initscr();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	nonl();
	cbreak();
	noecho();
}

/* deinit ncurses */
void ui_ncurs_exit(void)
{
	if (ui_ncurses_initialized == 0)
		return;

	clear();
	refresh();
	endwin();
#ifdef UMIX_DEBUG
	err_msg("ui_ncurs_exit: Ncurses UI deinitialized");
#endif 
}
