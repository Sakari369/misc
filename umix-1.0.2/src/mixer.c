/*
 * $Id: mixer.c,v 1.13 2003/08/30 16:12:28 sakari Exp $
 *
 * Handles all the mixer related stuff, loads plugins and initializes
 * function pointers etc. Wraps things around the sound hardware drivers.
 *
 * Copyright (C) 2001 Sakari Lehtonen <sakari@ionstream.fi>
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

#include "umix_common.h"
#ifdef HAVE_OSS
  #include "driver_oss.h"
#endif
#ifdef HAVE_DUMMY
  #include "driver_dummy.h"
#endif

/* internal variables */
static umixer_func *mixers[MAX_MIXERS];
static int curr_mixer = 0;
static int mixer_count = 0;
static int mixers_initialized = 0;
static volume *saved_vol[MAX_MIXERS];

/* try to init all available mixers */
int init_mixers(void)
{
	int i;

	if (mixers_initialized == 1)
		return 0;

	i = 0;

	while(1)
	{
		curr_mixer = i;
		if ((mixers[i] = mixer_load_drv(globalopts.driver)) == NULL)
			break;
		if (mixer_open(globalopts.devpath) == -1)
			break;
		if (mixer_init() == -1)
			break;
		i++;
	}

	/* failed opening any mixers */
	if (mixer_count < 1)
	{
		mixers_initialized = 0;
		return -1;
	}

#ifdef UMIX_DEBUG
	err_msg("init_mixers: initialized %d mixer(s)", mixer_count);
#endif
	mixers_initialized = 1;
	curr_mixer = 0;

	return 0;
}

/* try to close all open and initialized mixers in reverse order
 * they were opened */
int close_mixers(void)
{
	int i;

	for (i=mixer_count-1; i>=0; i--)
	{
		mixer_set_curr(i);
		mixer_close();
	}

	return 0;
}	

/* loads the mixer driver and gets the function pointers from it */
umixer_func *mixer_load_drv(const char *drivername)
{
	umixer_func *func;

	/* get all the mixer function pointers */
#ifdef HAVE_OSS
	if (strcasecmp(drivername, "oss") == 0)
		func = oss_get_mixer_func();
	else
#endif
#ifdef HAVE_DUMMY
	if (strcasecmp(drivername, "dummy") == 0)
		func = dummy_get_mixer_func();
	else
#endif
		return NULL;
#ifdef UMIX_DEBUG
	err_msg("mixer_load_drv: loaded %s driver", drivername);
#endif

	return func;
}

/* opens the current mixer device, mixer_load_drv must be called first */
int mixer_open(const char *path)
{
	int retval;
	char *errstr;
	
	errstr = _("Failed opening mixer");

	retval = mixers[curr_mixer]->mix_open(path);
	if (retval == EMIXERR)
	{
		err_msg(_("%s"), errstr);
		return -1;
	}
	else if (retval == EMIXALLOPEN)
		return -1;

	mixer_count++;
	if (mixer_count >= MAX_MIXERS)
	{
		err_msg(_("%s: Maximum limit of %d mixers reached!"), 
			errstr, MAX_MIXERS);
		mixer_count--;
		return -1;
	}
	
#ifdef UMIX_DEBUG
	err_msg("mixer_open: opened device %s", path);
#endif

	return 0;
}

/* initializes the current mixer, must be first opened with mixer_open */
int mixer_init(void)
{
	int i;
	const char *errstr; 

	errstr = _("Failed initializing mixer");

	/* no mixers opened if less than 1 */
	if (mixer_count < 1)
	{
		err_msg(_("%s: No mixers to init!"), errstr);
		return -1;
	}
	/* try to init the mixer */
	if (mixers[curr_mixer]->mix_init() == -1)
	{
		err_msg("%s", errstr);
		return -1;
	}
	/* this is used in muting/unmuting w/ interactive UI`s .. not sure
	 * if this should be allocated everytime, because the CLI doesn`t
	 * need this. */
	saved_vol[curr_mixer] = emalloc(sizeof(volume) * mixer_get_num_chan()); 
	for (i=0; i<mixer_get_num_chan(); i++)
	{
		chan_set_curr(i);
		saved_vol[curr_mixer][i] = chan_get_vb();
	}
	chan_set_curr(0);
	
	return 0;
}

/* close the mixer and free resources */
int mixer_close(void)
{
	if (mixer_count < 1)
	{
		err_msg(_("Failed closing mixer: no mixers to close!"));
		return -1;
	}

	mixers[curr_mixer]->mix_close();
	mixers[curr_mixer] = NULL;
	free(saved_vol[curr_mixer]);
	mixer_count--;

#ifdef UMIX_DEBUG
	err_msg("mixer_close: closed mixer #%d", curr_mixer);
#endif
	return 0;
}

/* converts the option string to a channel number */
int mixer_opt_to_chan_num(const char *str)
{
	return mixers[curr_mixer]->mix_opt_to_chan_num(str);
}

int mixer_get_curr(void)
{
	return curr_mixer;
}

/* sets the current mixer */
int mixer_set_curr(int mixer)
{
	if (mixer < 0 || mixer >= mixer_count || mixer > MAX_MIXERS)
	{
		err_msg(_("Failed setting mixer to #%d: out of bounds!"), 
			mixer);
		return -1;
	}

	curr_mixer = mixer;
	mixers[curr_mixer]->mix_set_curr(mixer);

	return 0;
}

/* increase the current mixer */
void mixer_inc_curr(int delta)
{
	curr_mixer += delta;
	curr_mixer = CLAMP(curr_mixer, 0, MAX_MIXERS);
}

/* get the current channel */
int chan_get_curr(void)
{
	return mixers[curr_mixer]->ch_get_curr();
}

/* set the current channel */
int chan_set_curr(int ch)
{
	if( ch < 0 || ch > mixer_get_num_chan())
	{
		err_msg(_("Failed setting channel to #%d: out of bounds!"),
			ch);
		return -1;
	}
	return mixers[curr_mixer]->ch_set_curr(ch);
}

/* increase/decrease current channel */
void chan_inc_curr(int delta)
{
	int ch;

	ch = chan_get_curr();
	ch += delta;
	ch = CLAMP(ch, 0, mixer_get_num_chan());
	chan_set_curr(ch);
}

/* returns both volume and balance */
volume chan_get_vb(void)
{
	volume vb;

	vb.volume = chan_get_vol();
	vb.balance = chan_get_bal();

	return vb;
}

/* sets both volume and balance */
void chan_set_vb(volume vb)
{
	chan_set_vol(vb.volume);
	chan_set_bal(vb.balance);
}

float chan_get_vol(void)
{
	return mixers[curr_mixer]->ch_get_vol();
}

void chan_set_vol(float vol)
{
	vol = CLAMP(vol, VOL_MIN, VOL_MAX);
	mixers[curr_mixer]->ch_set_vol(vol);
}

void chan_inc_vol(float delta)
{
	chan_set_vol(chan_get_vol() + delta);
}

float chan_get_bal(void)
{
	return mixers[curr_mixer]->ch_get_bal();
}

void chan_set_bal(float bal)
{
	bal = CLAMP(bal, BAL_LEFT, BAL_RIGHT);
	mixers[curr_mixer]->ch_set_bal(bal);
}

void chan_inc_bal(float delta)
{
	chan_set_bal(chan_get_bal() + delta);
}

void chan_set_mute(int state)
{
	int ch;
	
	ch = chan_get_curr();

	if (state == MUTE)
	{
		saved_vol[curr_mixer][ch] = chan_get_vb();
		chan_set_vol(0);
	}
	else if (state == UNMUTE)
		chan_set_vb(saved_vol[curr_mixer][ch]);
}

int chan_set_rec(int state)
{
	return mixers[curr_mixer]->ch_set_rec(state);
}	

void chan_toggle_rec(void)
{
	if (chan_is_record())
	{
		if (chan_is_recsrc())
			chan_set_rec(CHAN_PLAY);
		else
			chan_set_rec(CHAN_RECSRC);
	}
}

void chan_toggle_mute(void)
{
	if (chan_is_mute())
		chan_set_mute(UNMUTE);
	else
		chan_set_mute(MUTE);
}

/* gets volume for either left, right or both channels */
int chan_get_lr(int which)
{
	int retval;
	float left;
	float right;
	volume vb;

	vb = chan_get_vb();
	vb_to_lr(vb.volume, vb.balance, &left, &right);

	if (which == CHAN_LEFT)
		retval = left;
	else if (which == CHAN_RIGHT)
		retval = right;
	else
		retval = MAX(left, right);

	return (int)retval;
}

/* sets left and right volumes */
void chan_set_lr(int left, int right)
{
	volume vb;

	lr_to_vb(left, right, &vb.volume, &vb.balance);
	chan_set_vb(vb);
}

int chan_is_stereo(void)
{
	return mixers[curr_mixer]->ch_is_stereo();
}

int chan_is_recsrc(void)
{
	return mixers[curr_mixer]->ch_is_recsrc();
}

int chan_is_record(void)
{
	return mixers[curr_mixer]->ch_is_record();
}

int chan_is_mute(void)
{
	return mixers[curr_mixer]->ch_is_mute();
}

/* returns the name of the mixer */
const char *mixer_get_name(void)
{
	return mixers[curr_mixer]->mix_get_name();
}

/* returns the device path */
const char *mixer_get_path(void)
{
	return mixers[curr_mixer]->mix_get_path();
}

/* returns the name of the mixer driver */
const char *mixer_get_driver(void)
{
	return mixers[curr_mixer]->mix_get_driver();
}

int mixer_get_num_mix(void)
{
	return mixer_count;
}

int mixer_get_num_chan(void)
{
	return mixers[curr_mixer]->mix_get_numchan();
}

/* checks if the mixer has been updated */
int mixer_get_status(void)
{
	return mixers[curr_mixer]->mix_get_status();
}

const char *chan_get_name(void)
{
	return mixers[curr_mixer]->ch_get_name();
}

const char *chan_get_label(void)
{
	return mixers[curr_mixer]->ch_get_label();
}

/* these two functions have been adapted from Wmix 3.0 ..
 * original author timecop <timecop@japan.co.jp> */

/* convert left/right values to volume/balance */
void lr_to_vb(float left, float right, float *vol, float *bal)
{
	*vol = MAX(left, right);

	if (left > right)
		*bal = BAL_LEFT + (right / left);
	else if (right > left)
		*bal = BAL_RIGHT - (left / right);
	else
		*bal = BAL_CENTER;
}

/* convert volume/balance values to left/right */
void vb_to_lr(float vol, float bal, float *left, float *right)
{
	*left = vol * (BAL_RIGHT - MAX(BAL_CENTER, bal));
	*right = vol * (BAL_RIGHT + MIN(BAL_CENTER, bal));
}
