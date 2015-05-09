/* 
 * $Id: driver_dummy.c,v 1.2 2003/08/23 12:37:42 sakari Exp $
 *
 * The Dummy mixer driver for Umix. Based on the OSS driver.
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

#include "umix_common.h"

enum
{
	DUM_MAX_CHAN		= 32,
	DUM_CHAN_RECSRC		= (1<<0), /* is a recording source */
	DUM_CHAN_RECORD		= (1<<1), /* can be a recording source */
	DUM_CHAN_STEREO		= (1<<2) /* supports stereo mixing */
};

/* Dummy channel structure */
typedef struct dum_channel dum_channel;
struct dum_channel
{
	volume vb;
	int flags;
	char name[16];
	char label[16];
};	

/* the structure that holds all dummy specific information */
typedef struct dum_mixer_info dum_mixer_info;
struct dum_mixer_info
{
	int recmask;
	int recsrc;
	int stmask;
	int numchan;
	int curr_chan;
	char path[BUFLEN];
	char name[TINYSTRLEN];
	dum_channel chan[DUM_MAX_CHAN];
};

/* internal variables */

/* global variables that are shared between different mixer devices */
static int curr_mixer = 0;
static int mixer_count = 0;

static const char *driverstr = "Dummy";

/* the internal structure table for OSS mixer information */
static dum_mixer_info *dum_mixers[MAX_MIXERS];

/* a pointer pointing always to the current mixer, used for optimization */
struct dum_mixer_info *mixer_ptr;

/* function prototypes */
int dum_open(const char *path);
int dum_init(void);
int dum_close(void);

int dum_opt_to_chan_num(const char *str);

int dum_get_curr_mix(void);
int dum_set_curr_mix(int mixer);
int dum_get_curr_chan(void);
int dum_set_curr_chan(int ch);

float dum_get_vol(void);
void dum_set_vol(float vol);

float dum_get_bal(void);
void dum_set_bal(float vol);

int dum_set_rec(int state);

int dum_is_stereo(void);
int dum_is_recsrc(void);
int dum_is_record(void);
int dum_is_mute(void);

const char *dum_get_mix_name(void);
const char *dum_get_mix_path(void);
const char *dum_get_mix_driver(void);
int dum_get_mix_numchan(void);

int dum_get_mix_status(void);

const char *dum_get_ch_name(void);
const char *dum_get_ch_label(void);

/* the function pointers */
umixer_func dum_func =
{
	dum_open,
	dum_init,
	dum_close,
	
	dum_opt_to_chan_num,

	dum_get_curr_mix,
	dum_set_curr_mix,

	dum_get_curr_chan,
	dum_set_curr_chan,

	dum_get_vol,
	dum_set_vol,
	
	dum_get_bal,
	dum_set_bal,

	dum_set_rec,

	dum_is_stereo,
	dum_is_recsrc,
	dum_is_record,
	dum_is_mute,

	dum_get_mix_name,
	dum_get_mix_path,
	dum_get_mix_driver,
	dum_get_mix_numchan,
	dum_get_mix_status,

	dum_get_ch_name,
	dum_get_ch_label
};

umixer_func *dummy_get_mixer_func()
{
	return &dum_func;
};

/* opens and returns the file descriptor for the mixer device specified
 * in path */
int dum_open(const char *path)
{
	char devpath[STRLEN];
	char buf[TINYSTRLEN];

	/* copy the path to the device path */
	strncpy(devpath, path, sizeof(devpath));

	/* try to init /dev/mixer1 /dev/mixer2 etc if more than one mixer 
	 * already */
	if (mixer_count > 0)
	{
		snprintf(buf, sizeof(buf), "%d", mixer_count);
		strcat(devpath, buf);
	
		if (mixer_count > 3)
			return EMIXALLOPEN;
	}

	dum_mixers[mixer_count] = emalloc(sizeof(dum_mixer_info));
	if (dum_mixers[mixer_count] == NULL)
		return EMIXERR;
	
	/* copy the device path */
	strncpy(dum_mixers[mixer_count]->path, 
		devpath, sizeof(dum_mixers[mixer_count]->path));

#ifdef UMIX_DEBUG
	err_msg("dum_open: opened %s", devpath);
#endif

	return EMIXOK;
}

/* Initializes the current mixer device. The device must be opened with 
 * dum_open() before initializing. */
int dum_init(void)
{
	int i;
	int old_mixer;
	char buf[TINYSTRLEN];

#ifdef UMIX_DEBUG
	err_msg("dum_init: trying to init mixer #%d", mixer_count);
#endif
	old_mixer = curr_mixer;
	dum_set_curr_mix(mixer_count);
	
	/* set up imaginery values so we can debug */
	if (mixer_count == 0)
	{
		strncpy(mixer_ptr->name, "Dummy Blaster", 
			sizeof(mixer_ptr->name));
		mixer_ptr->chan[0].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[1].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[4].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[4].flags |= DUM_CHAN_RECSRC|DUM_CHAN_RECORD;
		mixer_ptr->chan[7].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[6].flags |= DUM_CHAN_RECORD;
		mixer_ptr->chan[8].flags |= DUM_CHAN_RECORD;
		mixer_ptr->numchan = 10;
	}
	else if (mixer_count == 1)
	{
		strncpy(mixer_ptr->name, "Wooden Dummy", 
			sizeof(mixer_ptr->name));
		mixer_ptr->chan[0].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[1].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[2].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[2].flags |= DUM_CHAN_RECSRC|DUM_CHAN_RECORD;
		mixer_ptr->numchan = 5;
	}
	else if (mixer_count == 2)
	{
		strncpy(mixer_ptr->name, "Dummy Equalizer", 
			sizeof(mixer_ptr->name));
		mixer_ptr->chan[0].flags &= ~DUM_CHAN_STEREO;
		mixer_ptr->chan[1].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[2].flags &= ~DUM_CHAN_STEREO;
		mixer_ptr->numchan = 3;
	}
	else if (mixer_count == 3)
	{
		strncpy(mixer_ptr->name, "Maximum DummyBlaster 32", 
			sizeof(mixer_ptr->name));
		mixer_ptr->chan[0].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[1].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[2].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[4].flags |= DUM_CHAN_RECSRC|DUM_CHAN_RECORD;
		mixer_ptr->chan[5].flags |= DUM_CHAN_RECORD;
		mixer_ptr->chan[8].flags |= DUM_CHAN_RECORD;
		mixer_ptr->chan[20].flags &= ~DUM_CHAN_STEREO;
		mixer_ptr->chan[21].flags &= ~DUM_CHAN_STEREO;
		mixer_ptr->chan[22].flags |= DUM_CHAN_STEREO;
		mixer_ptr->chan[23].flags |= DUM_CHAN_STEREO;
		mixer_ptr->numchan = DUM_MAX_CHAN;
	}
	for (i=0; i<mixer_ptr->numchan; i++)
	{
		snprintf(buf, sizeof(buf), "dummychan%d", i);
		strncpy(mixer_ptr->chan[i].name, buf,
			sizeof(mixer_ptr->chan[i].name));
		snprintf(buf, sizeof(buf), "DumLabel%d", i);
		strncpy(mixer_ptr->chan[i].label, buf,
			sizeof(mixer_ptr->chan[i].label));
		
		mixer_ptr->chan[i].vb.volume = 
		CLAMP((i+1)*(VOL_MAX/mixer_ptr->numchan), VOL_MIN, VOL_MAX);
		mixer_ptr->chan[i].vb.balance = BAL_CENTER;
	}
	mixer_ptr->curr_chan = 0;
	
#ifdef UMIX_DEBUG
	err_msg("dum_init: %s::%s", dum_get_mix_name(), dum_get_mix_path());
	err_msg("dum_init: initialized #%d with %d channels",
		curr_mixer, mixer_ptr->numchan);
#endif
	mixer_count++;
	/* restore the old mixer */
	dum_set_curr_mix(old_mixer);
	return 0;
}

int dum_close(void)
{
	if (mixer_count < 1)
		return -1;

	free(dum_mixers[curr_mixer]);
	mixer_count--;
	curr_mixer = CLAMP(curr_mixer, 0, mixer_count);

#ifdef UMIX_DEBUG
	err_msg("dum_close: closed mixer #%d", curr_mixer);
#endif 

	return 0;
}

/* TODO: maybe move this to mixer.c and make a function that
 * returns a general table with channel names and numbers, so we
 * don't have to copy this code to every mixer driver */
int dum_opt_to_chan_num(const char *str)
{
	int i;
	size_t n;
	char *chname;

	for (i=0; i<mixer_ptr->numchan; i++)
	{
		dum_set_curr_chan(i);
		chname = mixer_ptr->chan[mixer_ptr->curr_chan].name;
		n = strlen(str);
		/* compare only so many letters that the string
		 * to be compared to has, so you can use p instead
		 * of pcm etc */
		if (strncasecmp(chname, str, n) == 0)
			return mixer_ptr->curr_chan;
	}

	return -1;
}

int dum_get_mix_status(void)
{
	return 0;
}

int dum_get_curr_mix(void)
{
	return curr_mixer;
}

int dum_set_curr_mix(int mixer)
{
	if (curr_mixer < 0 || curr_mixer > MAX_MIXERS)
		return -1;

	curr_mixer = mixer;
	/* point global mixer_ptr to the current mixer */
	mixer_ptr = dum_mixers[curr_mixer];

	return 0;
}

int dum_get_curr_chan(void)
{
	return mixer_ptr->curr_chan;
}

int dum_set_curr_chan(int ch)
{
	if( mixer_ptr->curr_chan > mixer_ptr->numchan || 
		mixer_ptr->curr_chan < 0 )
		return -1;

	mixer_ptr->curr_chan = ch;

	return 0;
}	

float dum_get_vol(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume;
}

void dum_set_vol(float vol)
{
	mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume = vol;
}

float dum_get_bal(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].vb.balance;
}

void dum_set_bal(float bal)
{
	mixer_ptr->chan[mixer_ptr->curr_chan].vb.balance = bal;
}

int dum_set_rec(int state)
{
	return 0;
}

/* check if the current channel supports stereo mixing */
int dum_is_stereo(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & DUM_CHAN_STEREO)
		return 1;
	else
		return 0;
}

/* check if the current is a recording source */
int dum_is_recsrc(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & DUM_CHAN_RECSRC)
		return 1;
	else
		return 0;
}

/* check if the current channel can be a recording source */
int dum_is_record(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & DUM_CHAN_RECORD)
		return 1;
	else
		return 0;
}

/* check if volume for current channel is 0 */
int dum_is_mute(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume == 0)
		return 1;
	else
		return 0;
}

/* get the mixer name */
const char *dum_get_mix_name(void)
{
	return mixer_ptr->name;
}

/* get the device path */
const char *dum_get_mix_path(void)
{
	return mixer_ptr->path;
}

/* get the name of the mixer driver */
const char *dum_get_mix_driver(void)
{
	return driverstr;
}

/* get the number of channels for current mixer */
int dum_get_mix_numchan(void)
{
	return mixer_ptr->numchan;
}

/* get the channel name */
const char *dum_get_ch_name(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].name;
}

/* get the short version of channel name */
const char *dum_get_ch_label(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].label;
}
