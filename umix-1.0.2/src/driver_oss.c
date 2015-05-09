/* 
 * $Id: driver_oss.c,v 1.4 2003/08/31 13:32:02 sakari Exp $
 *
 * The OSS mixer driver for Umix.
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

/* Hmm not sure about the *BSD includes */
#if defined(__linux__)
  #include <sys/soundcard.h>
#elif defined(__FreeBSD__)
  #include "machine/soundcard.h"
#elif defined(__NetBSD__)
  #include <soundcard.h>
#endif

#define OSS_CHECKMASK(chan, mask) (((1 << (chan)) & (mask)) ? 0 : 1)

enum
{
	OSS_CHAN_RECSRC		= (1<<0), /* is a recording source */
	OSS_CHAN_RECORD		= (1<<1), /* can be a recording source */
	OSS_CHAN_STEREO		= (1<<2), /* supports stereo mixing */
	OSS_CHAN_VB_CHANGED	= (1<<3), /* volume or balance has changed */
	OSS_CHAN_REC_CHANGED	= (1<<4), /* record sources changed */
	OSS_MAX_CHAN		= 32	  /* max number of channels for OSS */
};

enum 
{
	OSS_UPD_READ,	/* read values from the device */
	OSS_UPD_WRITE,	/* write values to the device */
	OSS_UPD_REC	/* read / write record sources */
};

/* TODO: test if definitions in soundcard.h are actually defined, and
 * include own copies if not */

/* OSS channel structure */
typedef struct oss_channel oss_channel;
struct oss_channel
{
	volume vb;
	int flags;
	char name[16];
	char label[16];
};	

/* the structure that holds all OSS specific information */
typedef struct oss_mixer_info oss_mixer_info;
struct oss_mixer_info
{
	int fd;
	int devmask;
	int recmask;
	int recsrc;
	int stmask;
	int numchan;
	int modifycount;
	int curr_chan;
	char path[BUFLEN];
	char name[TINYSTRLEN];
	oss_channel chan[OSS_MAX_CHAN];
};

/* internal variables */

/* global OSS variables that are shared between different OSS mixer
 * devices */
static int curr_mixer = 0;
static int mixer_count = 0;
static int mixers_found = 0;

static const char *chanlabels[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_LABELS;
static const char *channames[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

static const char *driverstr = "OSS";

/* the internal structure table for OSS mixer information */
static oss_mixer_info *oss_mixers[MAX_MIXERS];

/* a pointer pointing always to the current mixer, used for optimization */
struct oss_mixer_info *mixer_ptr;

/* internal function prototypes */
static int oss_readlevel(void);
static int oss_writelevel(int level);
static int oss_update(int mode);
static int oss_check_update(void);

/* exported function prototypes */
int oss_open(const char *path);
int oss_init(void);
int oss_close(void);

int oss_opt_to_chan_num(const char *str);

int oss_get_curr_mix(void);
int oss_set_curr_mix(int mixer);
int oss_get_curr_chan(void);
int oss_set_curr_chan(int ch);

float oss_get_vol(void);
void oss_set_vol(float vol);

float oss_get_bal(void);
void oss_set_bal(float bal);

int oss_set_rec(int state);

int oss_is_stereo(void);
int oss_is_recsrc(void);
int oss_is_record(void);
int oss_is_mute(void);

const char *oss_get_mix_name(void);
const char *oss_get_mix_path(void);
const char *oss_get_mix_driver(void);
int oss_get_mix_numchan(void);

int oss_get_mix_status(void);

const char *oss_get_ch_name(void);
const char *oss_get_ch_label(void);

/* the function pointers */
umixer_func oss_func =
{
	oss_open,
	oss_init,
	oss_close,
	
	oss_opt_to_chan_num,

	oss_get_curr_mix,
	oss_set_curr_mix,

	oss_get_curr_chan,
	oss_set_curr_chan,

	oss_get_vol,
	oss_set_vol,

	oss_get_bal,
	oss_set_bal,

	oss_set_rec,

	oss_is_stereo,
	oss_is_recsrc,
	oss_is_record,
	oss_is_mute,

	oss_get_mix_name,
	oss_get_mix_path,
	oss_get_mix_driver,
	oss_get_mix_numchan,
	oss_get_mix_status,

	oss_get_ch_name,
	oss_get_ch_label
};

umixer_func *oss_get_mixer_func()
{
	return &oss_func;
};

/* opens and returns the file descriptor for the mixer device specified
 * in path */
int oss_open(const char *path)
{
	int fd;
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
	
		/* find out the total number of mixers available to OSS */
		if (mixers_found < 1)
		{
			static int i;

			for (i=1; i<MAX_MIXERS; i++)
			{
				snprintf(buf, sizeof(buf), "%s%d", path, i);
				if ((fd = eopen(buf, O_RDWR, 0)) != -1)
				{
					mixers_found++;
					eclose(fd);
				}
				else
					break;
			}
		}
		if (mixer_count > mixers_found)
			return EMIXALLOPEN;
	}

#ifdef UMIX_DEBUG
	err_msg("oss_open: trying to open %s (%s)", devpath, path);
#endif
	/* open the mixer device */
	if ((fd = eopen(devpath, O_RDWR, 0)) == -1)
	{
		err_msg("%s: %s", devpath, strerror(errno));
		return EMIXERR;
	}

	oss_mixers[mixer_count] = emalloc(sizeof(oss_mixer_info));
	if (oss_mixers[mixer_count] == NULL)
		return EMIXERR;
	oss_mixers[mixer_count]->fd = fd;
	
	/* copy the device path */
	strncpy(oss_mixers[mixer_count]->path, 
		devpath, sizeof(oss_mixers[mixer_count]->path));

#ifdef UMIX_DEBUG
	err_msg("oss_open: opened %s with fd %d", devpath, fd);
#endif

	return EMIXOK;
}

/* Initializes the current mixer device. The device must be opened with 
 * oss_open() before initializing. */
int oss_init(void)
{
	/* this is an OSS structure, defined in soundcard.h */
	struct mixer_info oss_info;
	int old_mixer;
	int ch;
	int i;

#ifdef UMIX_DEBUG
	err_msg("oss_init: trying to init mixer #%d", mixer_count);
#endif
	/* mixer_ptr points to the mixer that is to be initialized, and we
	 * don't want to init already initialized mixers, so current mixer
	 * is set to mixer_count and then restored after init */
	old_mixer = curr_mixer;
	oss_set_curr_mix(mixer_count);

	/* read the device mask so we know what channels are in use */
	if( eioctl(mixer_ptr->fd, SOUND_MIXER_READ_DEVMASK, 
			&mixer_ptr->devmask) == -1 )
		return -1;
	
	/* read the supported recording sources, this is not fatal if fails */
	if( eioctl(mixer_ptr->fd, SOUND_MIXER_READ_RECMASK, 
			&mixer_ptr->recmask) == -1 )
		return -1;
	
	/* read the current recording sources, not fatal  */
	eioctl(mixer_ptr->fd, SOUND_MIXER_READ_RECSRC, &mixer_ptr->recsrc);
	
	/* read the stereo support mask, not fatal */
	eioctl(mixer_ptr->fd, SOUND_MIXER_READ_STEREODEVS, &mixer_ptr->stmask);
	
	/* allocate channels */
	mixer_ptr->numchan = SOUND_MIXER_NRDEVICES;
	mixer_ptr->numchan = CLAMP(mixer_ptr->numchan,0,SOUND_MIXER_NRDEVICES);

	/* get the mixer name */
	eioctl(mixer_ptr->fd, SOUND_MIXER_INFO, &oss_info);
	strncpy(mixer_ptr->name, oss_info.name, sizeof(mixer_ptr->name));
	
	mixer_ptr->curr_chan = 0;
#ifdef UMIX_DEBUG
	err_msg("oss_init: %s::%s", oss_get_mix_name(), oss_get_mix_path());
#endif
	for (i=0, ch=0; i<mixer_ptr->numchan; i++)
	{
		if (OSS_CHECKMASK(i, mixer_ptr->devmask) == 0)
		{
			/* the real channel number is stored in the
			 * second byte of the flags. The real channel number
			 * is used when writing/reading levels to the 
			 * OSS device. */
			mixer_ptr->chan[ch].flags = i; 
			mixer_ptr->chan[ch].flags <<= 8;

			/* check available channel options */
			if (OSS_CHECKMASK(i, mixer_ptr->recsrc) == 0)
				mixer_ptr->chan[ch].flags |= OSS_CHAN_RECSRC;
			if (OSS_CHECKMASK(i, mixer_ptr->recmask) == 0)
				mixer_ptr->chan[ch].flags |= OSS_CHAN_RECORD;
			if (OSS_CHECKMASK(i, mixer_ptr->stmask) == 0)
				mixer_ptr->chan[ch].flags |= OSS_CHAN_STEREO;

			strncpy(mixer_ptr->chan[ch].name, channames[i],
				sizeof(mixer_ptr->chan[ch].name));
			strncpy(mixer_ptr->chan[ch].label, chanlabels[i],
				sizeof(mixer_ptr->chan[ch].label));
			ch++;	
		}
	}
	mixer_ptr->numchan = ch;
	/* get the volumes for every channel */
	for (i=0; i<mixer_ptr->numchan; i++)
	{
		oss_set_curr_chan(i);
		oss_update(OSS_UPD_READ);
	}
	/* init the modify counter */
	eioctl(mixer_ptr->fd, SOUND_MIXER_INFO, &oss_info);
	mixer_ptr->modifycount = oss_info.modify_counter;

#ifdef UMIX_DEBUG
	err_msg("oss_init: initialized #%d with %d channels",
		curr_mixer, mixer_ptr->numchan);
#endif
	mixer_count++;
	/* restore the old mixer */
	oss_set_curr_mix(old_mixer);
	return 0;
}

int oss_close(void)
{
	if (mixer_count < 1)
		return -1;

	eclose(oss_mixers[curr_mixer]->fd);
	free(oss_mixers[curr_mixer]);
	mixer_count--;
	curr_mixer = CLAMP(curr_mixer, 0, mixer_count);

#ifdef UMIX_DEBUG
	err_msg("oss_close: closed mixer #%d", curr_mixer);
#endif 

	return 0;
}

/* TODO: maybe move this to mixer.c and make a function that
 * returns a general table with channel names and numbers, so we
 * don't have to copy this code to every mixer driver */
int oss_opt_to_chan_num(const char *str)
{
	int i;
	size_t n;
	char *chname;

	for (i=0; i<mixer_ptr->numchan; i++)
	{
		oss_set_curr_chan(i);
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

/* checks if the mixer device has been updated, reads the new value from
 * the device and compares it to the old */
static int oss_check_update(void)
{
	struct mixer_info oss_info;

	eioctl(mixer_ptr->fd, SOUND_MIXER_INFO, &oss_info);
	if (oss_info.modify_counter != mixer_ptr->modifycount)
	{
		mixer_ptr->modifycount = oss_info.modify_counter;
		return 1;
	}

	return 0;
}

int oss_get_mix_status(void)
{
	int orig_chan;
	int i;

	if (oss_check_update() == 1)
	{
		orig_chan = mixer_ptr->curr_chan;
		for (i=0; i<mixer_ptr->numchan; i++)
		{
			oss_set_curr_chan(i);
			oss_update(OSS_UPD_READ);
		}
		oss_set_curr_chan(orig_chan);

		return 1;
	}
	
	return 0;
}

int oss_get_curr_mix(void)
{
	return curr_mixer;
}

int oss_set_curr_mix(int mixer)
{
	if (curr_mixer < 0 || curr_mixer > MAX_MIXERS)
		return -1;

	curr_mixer = mixer;
	/* point global mixer_ptr to the current mixer */
	mixer_ptr = oss_mixers[curr_mixer];

	return 0;
}

int oss_get_curr_chan(void)
{
	return mixer_ptr->curr_chan;
}

int oss_set_curr_chan(int ch)
{
	if ((ch < 0) || (ch > mixer_ptr->numchan))
		return -1;

	mixer_ptr->curr_chan = ch;

	return 0;
}	

/* reads the level from the device */
static int oss_readlevel(void)
{
	int level;
	int realch;
	
	realch = mixer_ptr->chan[mixer_ptr->curr_chan].flags>>8;
	if (eioctl(mixer_ptr->fd, MIXER_READ(realch), &level) == -1)
		return -1;

	return level;
}

/* writes the the level to the device */
static int oss_writelevel(int level)
{
	int realch;
	
	realch = mixer_ptr->chan[mixer_ptr->curr_chan].flags>>8;

	if (eioctl(mixer_ptr->fd, MIXER_WRITE(realch), &level) == -1)
		return -1;

	return 0;
}

/* Update values from or to the device to the internal oss_mixers[] structure
 * pointers, depends on the mode. Mode OSS_UPD_WRITE writes updated values
 * to the device, OSS_UPD_READ reads the volume for current channel to the
 * structures and OSS_UPD_REC updates recording sources from the device. */

/* TODO: optimization: maybe figure a better way to buffer writes to the 
 * device, and check if the write is really needed. */
static int oss_update(int mode)
{
	int level;
	int l_left;
	int l_right;
	int curr_chan;
	float left;
	float right;
	
	curr_chan = mixer_ptr->curr_chan;

	/* write to device */
	if (mode == OSS_UPD_WRITE)
	{
		vb_to_lr(mixer_ptr->chan[curr_chan].vb.volume,
			mixer_ptr->chan[curr_chan].vb.balance,
			&left, &right);

		l_left = (int)left;
		l_right = (int)right;

		/* left is the lower byte, right the higher */
		level = (l_left | (l_right << 8));
		oss_writelevel(level);
	}
	/* read from device */
	else if (mode == OSS_UPD_READ)
	{
		level = oss_readlevel();
		/* left is the lower byte, right the higher */
		l_left = (level & 0xFF);
		l_right = ((level >> 8) & 0xFF);

		left = (float)(l_left);
		right = (float)(l_right);
		
		lr_to_vb(left, right,
			&mixer_ptr->chan[curr_chan].vb.volume,
			&mixer_ptr->chan[curr_chan].vb.balance);
	}	
	/* update record source flags. We need to do this because when
	 * setting record sources with chan_set_rec(CHAN_RECSRC), the 
	 * old record source channel may be changed to play mode, and we
	 * need to read the current record sources from the device and 
	 * update our internal flags to match with this */
	else if (mode == OSS_UPD_REC)
	{
		int i, realch;

		/* read the current recording sources */
		eioctl(mixer_ptr->fd, SOUND_MIXER_READ_RECSRC, 
			&mixer_ptr->recsrc);

		for (i=0; i<mixer_ptr->numchan; i++)
		{
			realch = mixer_ptr->chan[i].flags >> 8;
			if (OSS_CHECKMASK(realch, mixer_ptr->recsrc) == 0)
			{
				mixer_ptr->recsrc |= (1<<realch);
				mixer_ptr->chan[i].flags |= OSS_CHAN_RECSRC;
			}
			else
			{
				mixer_ptr->recsrc &= ~(1<<realch);
				mixer_ptr->chan[i].flags &= ~OSS_CHAN_RECSRC;
			}
		}
	}	

	return 0;
}

void oss_set_vol(float vol)
{
	mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume = vol;
	oss_update(OSS_UPD_WRITE);
}

float oss_get_vol(void)
{
	/* check if the device has been updated */
	if (oss_check_update() == 1)	
		oss_update(OSS_UPD_READ);

	return mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume;
}

void oss_set_bal(float bal)
{
	mixer_ptr->chan[mixer_ptr->curr_chan].vb.balance = bal;
	oss_update(OSS_UPD_WRITE);
}

float oss_get_bal(void)
{
	/* check if the device has been updated */
	if (oss_check_update() == 1)	
		oss_update(OSS_UPD_READ);

	return mixer_ptr->chan[mixer_ptr->curr_chan].vb.balance;
}

/* Sets the recording state for the current channel. State CHAN_RECSRC
 * tries to set the current channel as a recording source, else it 
 * tries to set the current channel not as a recording source */
int oss_set_rec(int state)
{
	int recsrc;
	int realch;
	
	recsrc = mixer_ptr->recsrc;
	realch = mixer_ptr->chan[mixer_ptr->curr_chan].flags>>8;

	if (state == CHAN_RECSRC)
		recsrc |= (1<<realch);
	else
		recsrc &= ~(1<<realch);

	if (eioctl(mixer_ptr->fd, SOUND_MIXER_WRITE_RECSRC, &recsrc) == -1)
		return -1;

	oss_update(OSS_UPD_REC);

	return 0;
}

/* check if the current channel supports stereo mixing */
int oss_is_stereo(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & OSS_CHAN_STEREO)
		return 1;
	else
		return 0;
}

/* check if the current channel is a recording source */
int oss_is_recsrc(void)
{
	oss_update(OSS_UPD_REC);

	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & OSS_CHAN_RECSRC)
		return 1;
	else
		return 0;
}

/* check if the current channel can be a recording source */
int oss_is_record(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].flags & OSS_CHAN_RECORD)
		return 1;
	else
		return 0;
}

/* check if volume for current channel is 0 */
int oss_is_mute(void)
{
	if (mixer_ptr->chan[mixer_ptr->curr_chan].vb.volume == 0)
		return 1;
	else
		return 0;
}

/* get the mixer name */
const char *oss_get_mix_name(void)
{
	return mixer_ptr->name;
}

/* get the device path */
const char *oss_get_mix_path(void)
{
	return mixer_ptr->path;
}

/* get the name of the mixer driver */
const char *oss_get_mix_driver(void)
{
	return driverstr;
}

/* get the number of channels for current mixer */
int oss_get_mix_numchan(void)
{
	return mixer_ptr->numchan;
}

/* get the channel name */
const char *oss_get_ch_name(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].name;
}

/* get the short version of channel name */
const char *oss_get_ch_label(void)
{
	return mixer_ptr->chan[mixer_ptr->curr_chan].label;
}
