/*
 * $Id: umix_common.c,v 1.6 2003/08/30 15:21:36 sakari Exp $
 *
 * Common functions for Umix. All UIs have to be able to call
 * these, and errors are handled by the caller.
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

/* internal function prototypes */
static void parseglobal(char *buf);
static void parsemixer(char *buf);
static int slidevol(float *left, float *right, const char *optarg);

/* Parses one line of channel options.
 * 
 * Format is the following :
 *
 * vol|left:right[-|+]p|r
 * 
 * where 'name' is the name of the channel. 'vol' is either 
 * volume for both channels, or individual left and right values 
 * separated with a ':'. the last char sets the recording 
 * state of the channel to play 'p' or record source 'r'. 
 *
 * OR
 *
 * fromvol,tovol,noofsteps,delaytime p|r
 *
 * If you want to raise/decrease the volume with 'noofsteps' as the
 * number of steps from 'fromvol' to 'tovol', where 'delaytime' is the
 * delay between two steps.
 *
 * TODO: Okay, this starts to suck. Multiple sscanf etc, rewrite perhaps. 
 * */
int parsechanopt(const char *optarg)
{
	volume vb;
	int recstate;
	int retval;
	int change; 
	int slide;
	size_t n;
	float left;
	float right;

	change = 0;
	slide = 0;
	retval = 0;

	/* skip whitespace */
	while (*optarg == ' ' || *optarg == '\t')
		optarg++;
	/* check for increment or decrement */
	n = strlen(optarg)-1;
	if (optarg[n] == '+')
		change = 1;
	else if (optarg[n] == '-')
		change = -1;

#ifdef UMIX_DEBUG
	err_msg("parsechanopt: mixer=%d ch=%d optarg=%s change=%d", 
		mixer_get_curr(), chan_get_curr(), optarg, change);
#endif
	/* if the first char is not a number, we might be setting only
	 * record sources */
	if (isdigit(optarg[0]))
	{
		/* normal volume setting */
		if (strchr(optarg, ',') == NULL)
		{
			/* if no ':' found, use same value for both */
			if (strchr(optarg, ':') == NULL)
			{
				left = atof(optarg);
				right = left;
			}
			else
			{
				sscanf(optarg, "%f:%f", &left, &right);
				/* if channel is not stereo, use same 
				 * for both left and right */
				if (chan_is_stereo() == 0)
					right = left;
			}
		}
		/* volume sliding */
		else
		{
			slide = 1;
			if ((retval = slidevol(&left, &right, optarg)) != 0)
				return retval;
			else
				retval = 0;
		}
	}
	else
	{
		/* check for garbage */
		recstate = tolower(optarg[0]);
		if (recstate != 'r' && recstate != 'p')
			return UMIX_EOPTINVALID;
	}
	/* check for setting of record source or play */
	n = strcspn(optarg, "rRpP");
	recstate = tolower(optarg[n]);
	if ((recstate == 'r' || recstate == 'p'))
	{
		if (chan_is_record())
		{
			if (recstate == 'r')
				recstate = CHAN_RECSRC;
			else
				recstate = CHAN_PLAY;
			chan_set_rec(recstate);
		}
		else
			retval = UMIX_EOPTNOTREC;

		/* P or R is the only argument for this channel, return */
		if (isdigit(optarg[0]) == 0)
			return retval;
	}

	/* check for overflowing */
	if (left > VOL_MAX || left < 0 || right > VOL_MAX || right < 0)
		return UMIX_EOPTLIMIT;

	/* add or decrement from the old values ? if not sliding the vol */
	if (change != 0 && slide == 0)
	{
		/* change is either 1 or -1 */
		left = chan_get_lr(CHAN_LEFT) + (change * left);
		right = chan_get_lr(CHAN_RIGHT) + (change * right);
	}
	
	lr_to_vb(left, right, &vb.volume, &vb.balance);
	chan_set_vb(vb);
#ifdef UMIX_DEBUG
	err_msg("parsechanopt: left=%d right=%d parsedleft=%f parsedright=%f",
		chan_get_lr(CHAN_LEFT), chan_get_lr(CHAN_RIGHT), 
		left, right);
#endif
	return retval;
}

/* increment the volume in steps
 * the format is this : fromvol,destvol,steps,delay 
 * left and right are handled as pointers because we set the final volume
 * in the parsechanopt() function. 
 */
static int slidevol(float *left, float *right, const char *optarg)
{
	int i;
	float rinc; 
	float linc; 
	float ldir; 
	float rdir;
	float rdiff;
	float ldiff;
	float fleft; 
	float fright;
	volume fvb;
	int steps;
	long int delay;

	steps = 20;
	delay = 20;

	/* no different left:right values specified, use same for both */
	if (strchr(optarg, ':') == NULL)
	{
		sscanf(optarg, "%f,%f,%d,%ld", 
			&fleft, left, &steps, &delay);
		fright = fleft;
		*right = *left;
	}
	else
	{
		sscanf(optarg, "%f:%f,%f:%f,%d,%ld", 
			&fleft, &fright, left, right, &steps, &delay);
	}
	/* we do not support steps going over the wanted
	 * left or right, because then increment would be
	 * zero */
	steps = CLAMP(steps, 0, VOL_MAX);
	ldiff = ABS(*left - fleft);
	rdiff = ABS(*right - fright);
	if (steps > ldiff && steps > rdiff)
		steps = MAX(ldiff, rdiff);

	/* check limits */
	if( (ldiff == 0 && rdiff == 0) || 
		(ldiff > VOL_MAX && rdiff > VOL_MAX) ||steps == 0 )
		return UMIX_EOPTLIMIT;

	/* which way to slide with left */
	linc = *left - fleft;
	if (linc < 0)
	{
		ldir = -1;
		linc = ABS(linc);
	}
	else
		ldir = 1;
	linc /= steps;

	/* which way to slide with right */
	rinc = *right - fright;
	if (rinc < 0)
	{
		rdir = -1;
		rinc = ABS(rinc);
	}
	else
		rdir = 1;
	rinc /= steps;

	/* milliseconds, maximum is 5000 (5 seconds) */
	delay = CLAMP(delay, 0, 5000);
	delay *= 1000;
	/* correct direction */
	linc *= ldir;
	rinc *= rdir;
#ifdef UMIX_DEBUG
	err_msg("slidevol: ldiff=%3f rdiff=%3f steps=%d delay=%d", 
		ldiff, rdiff, steps, delay);
#endif
	for (i=0; i<steps; i++)
	{
		lr_to_vb(fleft, fright, &fvb.volume, &fvb.balance);
		chan_set_vb(fvb);

		fleft += linc;
		fright += rinc;

		usleep(delay);
	}

	return 0;
}

/* the mixer that is used when parsing mixer settings */
static int set_mixer = -1;

/* load settings from file defined in path */
int settings_load(const char *path, int mode)
{
	FILE *fp;
	void (*parseline)(char *buf);
	size_t n;
	int globalfound; 
	int mixersfound;
	char buf[LINEWIDTH];

	globalfound = 0;
	mixersfound = 0;

	/* lines are parsed depending on the mode */
	if (mode == UMIX_SET_GLOBAL)
		parseline = parseglobal;
	else
		parseline = parsemixer;

	if (path[0] == '-' && path[1] == '\0')
		fp = stdin;
	else if ((fp = fopen(path, "r")) == NULL)
		return UMIX_ESETERR;

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		/* try to detect binary files .. this _should_ detect 
		 * garbage/binary .. */
		if (isascii(buf[0]) == 0)
			return UMIX_ESETBINARY;
		/* check for comments */
		n = strcspn(buf, "#");
		if (n < 2)
			continue;

		/* remove newline */
		n = strlen(buf) - 1;
		if (buf[n] == '\n' || buf[n] == '\r')
			buf[n] = '\0';

		/* check for setting sections */
		if (strncmp(buf, "[global]", 8) == 0)
		{
			globalfound = 1;
			if ((mode == UMIX_SET_MIXER) && (mixersfound == 1))
				break;
			continue;
		}
		else if (strncmp(buf, "[mixers]", 8) == 0)
		{
			mixersfound = 1;
			if ((mode == UMIX_SET_GLOBAL) && (globalfound == 1))
				break;
			continue;
		}
		/* parse lines if the mode is right and beginning found */
		if( ((mode == UMIX_SET_GLOBAL) && globalfound == 1) ||
		    ((mode == UMIX_SET_MIXER) && mixersfound == 1 ))
			parseline(buf);
	}

	/* no global or mixers section found, assume invalid config file */
	if (globalfound == 0 && mixersfound == 0)
		return UMIX_ESETINVALID;

	/* mixer settings for specified mixer not found */
	if ((mode == UMIX_SET_MIXER) && (set_mixer == -1))
		return UMIX_ESETNOTFOUND;

	/* we don't want to close stdin */
	if (fp == stdin)
	 	return 0;
	else fclose(fp);

	return 0;
}

/* parses one line in the [global] section of config file */
static void parseglobal(char *buf)
{
	size_t n;

	/* hmm yes there might be a better way to do this .. but this is
	 * sufficient and simple enough */
	n = strcspn(buf, "=");
	buf[n++] = '\0';
	if (strcmp(buf, "device") == 0)
		strncpy(globalopts.devpath, buf + n,
			sizeof(globalopts.devpath));
	else if (strcmp(buf, "config") == 0)
		strncpy(globalopts.cfgpath, buf + n,
			sizeof(globalopts.cfgpath));
	else if (strcmp(buf, "driver") == 0)
		strncpy(globalopts.driver, buf + n,
			sizeof(globalopts.driver));
}

/* parses one line in the [mixers] section of config file */
static void parsemixer(char *buf)
{
	int i;
	int orig_mixer;
	size_t n;

	/* these aren't really required in the config file .. they are
	 * just helping readability */
	if (*buf == '{' || *buf == '}')
		return;
	/* jump over leading whitespace */
	while (*buf == ' ' || *buf == '\t')
		buf++;

	/* find the mixer  */
	if (strncmp(buf, "mixer=", 6) == 0)
	{
		buf += 6;
		orig_mixer = mixer_get_curr();
		for (i=0; i<mixer_get_num_mix(); i++)
		{
			/* compare device paths to find out the right 
			 * mixer */
			mixer_set_curr(i);
			if (strcmp(buf, mixer_get_path()) == 0)
			{
				/* found */
				set_mixer = i;
				break;
			}
		}
		mixer_set_curr(orig_mixer);
	}
	/* mixer already found */
	else if (set_mixer != -1)
	{
		int chnum;
		int orig_chan;

		n = strcspn(buf, " ");
		buf[n] = '\0';

		orig_mixer = mixer_get_curr();
		orig_chan = chan_get_curr();

		mixer_set_curr(set_mixer);
		/* parse the channel option */
		if ((chnum = mixer_opt_to_chan_num(buf)) != -1)
		{
			chan_set_curr(chnum);
			parsechanopt(buf+n+1);
		}
		mixer_set_curr(orig_mixer);
		chan_set_curr(orig_chan);
	}
}

/* save mixer settings to a file defined in path. */
int settings_save(const char *path)
{
	FILE *fp;
	int i;
	int j;
	int n;
	int orig_mixer;
	int orig_chan;
	char record;

	if (path[0] == '-' && path[1] == '\0')
		fp = stdout;
	else if ((fp = fopen(path, "w+")) == NULL)
		return UMIX_ESETERR;

	fprintf(fp, "# Umix config file\n"
		    "[global]\n"
		    "device=%s\n"
		    "config=%s\n"
		    "driver=%s\n\n",
		    globalopts.devpath, globalopts.cfgpath,
		    globalopts.driver);
	fprintf(fp, "[mixers]\n");
	
	orig_mixer = mixer_get_curr();
	orig_chan = chan_get_curr();
	for (i=0; i<mixer_get_num_mix(); i++)
	{
		mixer_set_curr(i);
		/* print the name of the mixer and mixer driver name
		 * just to add readability */
		fprintf(fp, "# %s, %s\n", mixer_get_name(), mixer_get_driver());
		fprintf(fp, "mixer=%s\n{\n", mixer_get_path());

		/* we must do this to get the channels in right order */
		n = mixer_get_num_chan();
		for (j=0; j<n; j++)
		{
			chan_set_curr(j);
			if (chan_is_recsrc())
				record = 'R';
			else if (chan_is_record())
				record = 'P';
			else
				record = ' ';
			fprintf(fp, "\t%s %d:%d %c\n", 
				chan_get_name(), chan_get_lr(CHAN_LEFT),
				chan_get_lr(CHAN_RIGHT), record);
		}
		fprintf(fp, "}\n");
	}
	mixer_set_curr(orig_mixer);
	chan_set_curr(orig_chan);
	
	/* we dont want to close stdout .. */
	if (fp == stdout)
		return 0;
	else fclose(fp);

	return 0;
}
