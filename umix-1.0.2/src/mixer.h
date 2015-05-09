/* 
 * $Id: mixer.h,v 1.4 2003/02/09 09:36:22 sakari Exp $
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

#ifndef UMIX_MIXER_H
#define UMIX_MIXER_H

enum
{
	VOL_MIN = 0,	/* min volume value */
	VOL_MAX = 100,	/* max volume value */
	MAX_MIXERS = 8	/* maximum number of mixers */
};

enum
{
	BAL_CENTER = 0, /* balance center value */
	BAL_RIGHT = 1,	/* balance right value */
	BAL_LEFT = -1 	/* balance left value */
};

enum 
{
	CHAN_LEFT,	/* left channel */
	CHAN_RIGHT,	/* right channel */
	CHAN_MAX,	/* larger value of the two */
	CHAN_RECSRC,	/* recording source */
	CHAN_PLAY	/* normal channel */

};

enum
{
	MUTE,		/* mute, store value */
	UNMUTE		/* unmute, restore value */
};

enum
{
	EMIXOK = 0,	/* mixer ok */
	EMIXERR,	/* general error */
	EMIXALLOPEN	/* MAX_MIXERS limit reached */
};

/* structure definitions */

/* the volume structure that is passed between the mixer interface and 
 * mixer drivers. */
typedef struct volume volume;
struct volume
{
	float volume;
	float balance;
};

/* function pointers for one device */
typedef struct umixer_func umixer_func;
struct umixer_func
{
	int (*mix_open)(const char *path);
	int (*mix_init)(void);
	int (*mix_close)(void);

	int (*mix_opt_to_chan_num)(const char *str);

	int (*mix_get_curr)(void);
	int (*mix_set_curr)(int mixer);

	int (*ch_get_curr)(void);
	int (*ch_set_curr)(int ch);

	float (*ch_get_vol)(void);
	void (*ch_set_vol)(float vol);
	
	float (*ch_get_bal)(void);
	void (*ch_set_bal)(float bal);

	int (*ch_set_rec)(int state);

	int (*ch_is_stereo)(void);
	int (*ch_is_recsrc)(void);
	int (*ch_is_record)(void);
	int (*ch_is_mute)(void);

	const char *(*mix_get_name)(void);
	const char *(*mix_get_path)(void);
	const char *(*mix_get_driver)(void);
	int (*mix_get_numchan)(void);
	int (*mix_get_status)(void);

	const char *(*ch_get_name)(void);
	const char *(*ch_get_label)(void);
};

/* the actual mixer structure */

/* global mixer prototypes */
int init_mixers(void);
int close_mixers(void);

umixer_func *mixer_load_drv(const char *drivername);
int mixer_open(const char *devpath);
int mixer_init(void);
int mixer_close(void);

int mixer_opt_to_chan_num(const char *str);

int mixer_get_curr(void);
int mixer_set_curr(int mixer);
void mixer_inc_curr(int delta);

int chan_get_curr(void);
int chan_set_curr(int ch);
void chan_inc_curr(int delta);

volume chan_get_vb(void);
void chan_set_vb(volume vb);

float chan_get_vol(void);
void chan_set_vol(float vol);
void chan_inc_vol(float delta);

float chan_get_bal(void);
void chan_set_bal(float bal);
void chan_inc_bal(float delta);

int chan_get_lr(int which);
void chan_set_lr(int left, int right);

int chan_set_rec(int state);
void chan_toggle_rec(void);
void chan_toggle_mute(void);
void chan_set_mute(int state);

int chan_is_stereo(void);
int chan_is_recsrc(void);
int chan_is_record(void);
int chan_is_mute(void);

const char *mixer_get_name(void);
const char *mixer_get_path(void);
const char *mixer_get_driver(void);
int mixer_get_num_mix(void);
int mixer_get_num_chan(void);
int mixer_get_status(void);

const char *chan_get_name(void);
const char *chan_get_label(void);

void lr_to_vb(float left, float right, float *vol, float *bal);
void vb_to_lr(float bal, float vol, float *left, float *right);

#endif /* UMIX_MIXER_H */
