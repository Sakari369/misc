/*
 * $Id: umix.c,v 1.9 2003/08/23 12:54:09 sakari Exp $
 *
 * The main function and command-line UI for Umix.
 * Main parses command-line options, initializes the mixer 
 * and then calls the UI. The command-line UI does not
 * support multiple devices in one session.
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
#include "ui.h"
#ifdef HAVE_GETOPT_LONG
#  include <getopt.h>
#else
#  include <unistd.h>
#endif

/* global options */
umix_cfg globalopts =
{
	DEF_DEVICE_PATH,
	DEF_CONFIG_PATH,
	DEF_DRIVER
};

/* function prototypes */
static void check_homeconf(void);
static int cmdline(int argc, char *argv[], int optind);
static void cmdline_loadsettings(void);
static void parse_cmdlineopts(int argc, char *argv[]);
static void parse_cmdlinevols(int argc, char *argv[], int optind);
static void query(void);
static void printlevels(void);
static void usage(int status);
static void version(void);
static void showglobals(void);
static int setall(const char *optarg);

#ifdef UMIX_DEBUG
static int test(int mode);
#endif

/* the name umix is called with */
const char *program_name;

/* command line options */
typedef struct umix_opts umix_opts;
struct umix_opts
{
	int query;
	int load;
	int save;
	int force_interactive;
	int interactive;
} static opts = { 0, 0, 0, 0, 0 };

int main(int argc, char *argv[])
{
#ifdef ENABLE_NLS
	/* internationalization */
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif
	program_name = argv[0];

	/* check if .umixrc exists in the users home dir, and set to the
	 * global config path if exists */
	check_homeconf();
	/* try to load global settings if not loading or saving to stdin */
	if (globalopts.cfgpath[0] != '-' && globalopts.cfgpath[1] != '\0')
		settings_load(globalopts.cfgpath, UMIX_SET_GLOBAL);

	/* parse command-line options */
	parse_cmdlineopts(argc, argv);
	
	/* check if umix was called with only one argument eg `umix foo` */
	if (argc == 2 && optind == 1)
	{
		err_msg(_("Nothing to do!"));
		usage(EXIT_FAILURE);
	}
	/* if any command line options given, parse them */
	if (argc >= 2)
	{
		if (cmdline(argc, argv, optind) == -1)
			return -1;
	}
	/* if no command line options were given, or -i was given, then start
	 * interactive UI, mixers are initialized in the UI */
	if (argc < 2 || opts.interactive == 1) 
	{
		/* no sense in saving or loading settings 
		 * from stdin/stdout in interactive UI's, replace
		 * config path with the default one */
		if (globalopts.cfgpath[0]=='-' && 
			globalopts.cfgpath[1]=='\0')
		{
			strncpy(globalopts.cfgpath, DEF_CONFIG_PATH,
				sizeof(globalopts.cfgpath));
		}
		if (ui_init("ncurses", argc, argv) == -1)
		{
			err_msg(_("Ncurses UI initialization failed"
			      ", try `%s -q'"), program_name);
		}
	}
	/* finally, close all mixers */
	close_mixers();

	return 0;
}

static void check_homeconf(void)
{
	char *str;
	struct stat sbuf;
	char buf[BUFLEN];

	/* check if $HOME/.umixrc exists, and use as the default
	 * config if found */
	if ((str = getenv("HOME")) != NULL)
	{
		snprintf(buf, sizeof(buf), "%s/.umixrc", str);
		if (stat(buf, &sbuf) == 0)
		{
			strncpy(globalopts.cfgpath, buf, 
				sizeof(globalopts.cfgpath));
		}
	}
}

/* the commandline UI */
static int cmdline(int argc, char *argv[], int optind)
{
#ifdef UMIX_DEBUG
	err_msg("cmdline: argc=%d optind=%d", argc, optind);
#endif
	if (init_mixers() == -1)
	{
		err_msg(_("Failed initializing mixer devices"));
		return -1;
	}

	/* process all command-line options conserning channels */
	parse_cmdlinevols(argc, argv, optind);
	/* if any channel options are given, do not start interactive ui */
	if (optind != argc)
		opts.interactive = 0;
	
	/* query settings */
	if (opts.query == 1)
	{
		query();
		/* no point in both querying levels and starting 
		 * the interactive ui */
		opts.interactive = 0;
	}
	
	/* load settings */
	if (opts.load == 1)
		cmdline_loadsettings();

	/* save settings */
	if (opts.save == 1)
	{
		if (settings_save(globalopts.cfgpath) == -1)
			err_str(_("Failed saving settings to \'%s\'"), 
				globalopts.cfgpath);
		else
			printf(_("Saved mixer settings to \'%s\'\n"), 
				globalopts.cfgpath);
		opts.interactive = 0;
	}
	/* -i forces to start the interactive ui after all other
	 * command line options are parsed */
	if (opts.force_interactive == 1)
		opts.interactive = 1; 

	return 0;
}

static void cmdline_loadsettings(void)
{
	int retval;

	retval = settings_load(globalopts.cfgpath, UMIX_SET_MIXER);
	if (retval == 0)
	{
		printf(_("Loaded mixer settings from \'%s\'\n"),
			globalopts.cfgpath);
	}
	else
	{
		err_msg("Loading settings failed");
		switch (retval)
		{
		case UMIX_ESETINVALID: 
			err_msg("%s: %s", 
				globalopts.cfgpath,
				UMIX_UI_EINVALID_STR); 
			break;
		case UMIX_ESETNOTFOUND:
			err_msg("%s: %s", 
				globalopts.cfgpath,
				UMIX_UI_ENOTFOUND_STR);
			break;
		case UMIX_ESETBINARY:
			err_msg("%s: %s", 
				globalopts.cfgpath,
				UMIX_UI_ESETBINARY_STR);
			break;
		default: 
			err_str("%s", globalopts.cfgpath);
			break;
		}
	}
	opts.interactive = 0;
}

static void parse_cmdlineopts(int argc, char *argv[])
{
	int optch; 
	const char *optstr = "d:r:f:ghlLqsSvi";
	
#ifdef HAVE_GETOPT_LONG
        const struct option longopts[] =
        {
		{ "device",	1, 0, 'd' },
		{ "driver",	1, 0, 'r' },	
                { "file",       1, 0, 'f' },
		{ "globals",	0, 0, 'g' },
                { "help",       0, 0, 'h' },
                { "load",       0, 0, 'l' },
                { "query",      0, 0, 'q' },
                { "save",       0, 0, 's' },
                { "version",    0, 0, 'v' },
		{ "interactive",0, 0, 'i' },
                { NULL,         0, 0, 0 }
        };
	while ((optch = getopt_long(argc, argv, optstr, longopts, NULL)) != -1)
	{
#else
	while ((optch = getopt(argc, argv, optstr)) != -1)
        {
#endif /* HAVE_GETOPT_LONG */
		switch(optch)
		{
		case 'd': strncpy(globalopts.devpath, optarg,
				  sizeof(globalopts.devpath));
			  opts.interactive = 1;
			  break;
		case 'f': strncpy(globalopts.cfgpath, optarg,
				  sizeof(globalopts.cfgpath));
			  opts.interactive = 1;
			  break;
		/* showglobals() is called here because we want to be
		 * able to call it even if something fails */
		case 'g': showglobals(); 
			  opts.interactive = 0;
			  break;
		case 'h': usage(EXIT_SUCCESS);
			  break;
		case 'i': opts.force_interactive = 1; 
			  break;
		case 'L': /* compatibility */
		case 'l': opts.load = 1; 
			  break;
		case 'q': opts.query = 1;
			  break;
		case 'r': strncpy(globalopts.driver, optarg,
				  sizeof(globalopts.driver));
			  opts.interactive = 1;
			  break;
		case 'S': /* compatibility */
		case 's': opts.save = 1;
			  break;
		case 'v': version(); 
			  exit(EXIT_SUCCESS);
			  break;
		default:
			  usage(EXIT_FAILURE);
			  break;
		}
        };
}

static void parse_cmdlinevols(int argc, char *argv[], int optind)
{
	int chnum;
	int retval;
	int i;
	
	for (i=optind; i<argc; i++)
	{
		/* the special channel name 'all' sets all channels */
		if ((strcasecmp(argv[i], "all")) == 0)
		{
			i++;
			setall(argv[i]);
		}
		/* get the right channel number for the string argv[i] eg.
		 * "pcm" would return 1 with my system */
		else if ((chnum = mixer_opt_to_chan_num(argv[i])) != -1)
		{
			i++;
			chan_set_curr(chnum);
			/* check if no value is given after the channel
			 * name */
			if (argv[i] == NULL)
			{
				err_msg(_("Value missing for channel %s"),
					chan_get_name());
				break;
			}
			/* parse the actual volume settings for current chan */
			retval = parsechanopt(argv[i]);
			if (retval != 0)
				err_msg(UMIX_EOPTGENERAL_STRFMT, 
					chan_get_name(), argv[i]);
			switch (retval)
			{
			case UMIX_EOPTLIMIT:
				err_msg(UMIX_EOPTLIMIT_STR);
				break;
			case UMIX_EOPTINVALID:
				err_msg(UMIX_EOPTINVALID_STR);
				break;
			case UMIX_EOPTNOTREC:
				err_msg(UMIX_EOPTNOTREC_STR);
				break;
			default:
				break;
			}
		}
		else
			err_msg(_("Invalid channel name: \'%s\'"), argv[i]);
	}
}

static void query(void)
{
	int i;

	for (i=0; i<mixer_get_num_mix(); i++)
	{
		mixer_set_curr(i);
		printlevels();
	}
}

/* parse optarg for all channels */
static int setall(const char *optarg)
{
        int i;

        for (i=0; i<mixer_get_num_chan(); i++)
        {
		chan_set_curr(i);
                if (parsechanopt(optarg) != 0)
			return -1;
        }

        return 0;
}

/* prints the levels and status for all channels on current mixer */
static void printlevels(void)
{
        int i;

	printf("%s::%s::%s\n", mixer_get_name(), 
		mixer_get_path(),
		mixer_get_driver());
        for (i=0; i<mixer_get_num_chan(); i++)
        {
		chan_set_curr(i);
		printf("%-10s %3d:%3d ", chan_get_name(), 
			chan_get_lr(CHAN_LEFT), chan_get_lr(CHAN_RIGHT));

		if (chan_is_stereo())
			printf(_("Stereo"));
		else
			printf(_("Mono  "));
		putchar(' ');

		if (chan_is_recsrc())
			printf(_("Record Source"));
		else if (chan_is_record())
			printf(_("Recordable"));
		putchar('\n');
        }
}

static void showglobals(void)
{
	printf(_("Current global settings:\n"
	       	"Config path\t   %s\n"
		"Device path\t   %s\n"
	       	"Mixer driver\t   %s\n"),
	       	globalopts.cfgpath, globalopts.devpath, 
		globalopts.driver);
}

static void usage(int status)
{
	if (status != 0)
		fprintf(stderr, _("try `%s -h' for more information\n"),
			program_name);
	else
	{
        printf(_("Usage: %s [channame volume|left:right][R|P][+|-] [options]\n"
	     "\nYou can get all available channel names with the '-q' option"
	     "\n\nAdditional options:\n"
             "-q, --query\t\tPrint channel settings and available\n"
	     "\t\t\tchannel names\n"
	     "-s, --save\t\tSave mixer settings to a file\n"
             "-l, --load\t\tLoad mixer settings from a file\n"
             "-f, --file=PATH\t\tSpecify file used to read and save settings\n"
	     "\t\t\t(defaults to %s)\n"
             "-d, --dev=PATH\t\tSpecify device used for mixing\n"
	     "\t\t\t(defaults to %s)\n"
	     "-r, --driver=DRIVER\tSpecify the mixer driver used "
	     "(defaults to %s)\n"
	     "-i, --interactive"
	     "\tStart interactive user interface after processing\n"
	     "\t\t\tcommand-line options (if available)\n"
	     "-g, --globals\t\tPrint current global options\n"
             "-h, --help\t\tDisplay this help and exit\n"
             "-v, --version\t\tDisplay version information and exit\n"),
		program_name, DEF_CONFIG_PATH, DEF_DEVICE_PATH, DEF_DRIVER);
	}
	exit(status);
}

static void version(void)
{
        printf(_("%s\n"
	"Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>\n"
	"\nThis is free software; see the source for copying conditions. " 
	"There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS FOR A" 
	" PARTICULAR PURPOSE.\n"), UMIX_VERSION_STR);
}

#ifdef UMIX_DEBUG
/* tests most of the functions available, and shows how to do things */
static int test(int mode)
{
	int i;
	int j;
	volume orig_vol;

	if (init_mixers() == -1)
	{
		err_msg(_("Failed initializing mixer devices"));
		return -1;
	}
	
	/* update check test */
	if (mode == 1)
	{
		while (1)
		{
			usleep(250000);
			if (mixer_get_status() == 1)
			{
				err_msg("mixer has been updated!");
				printlevels();
				chan_set_curr(0);
			}
		}

		mixer_close();
		return 0;
	}

	printf("Total number of mixers = %d\n", mixer_get_num_mix());
	for (j=0; j<mixer_get_num_mix(); j++)
	{
		printf("Mixer #%d information\n", mixer_get_curr());
		printf("numchan = %d\n", mixer_get_num_chan());
		printf("name    = %s\n", mixer_get_name());
		printf("path    = %s\n", mixer_get_path());
		printf("driver  = %s\n", mixer_get_driver());

		printf("num name       label      lt  rt   s r a m\n");
		for (i=0; i<mixer_get_num_chan(); i++)
		{
			chan_set_curr(i);
			printf("%3d %-10s %-10s ", chan_get_curr(), 
				chan_get_name(), chan_get_label());
			printf("%3d %3d  %d %d %d %d\n", 
				chan_get_lr(CHAN_LEFT), chan_get_lr(CHAN_RIGHT),
				chan_is_stereo(), chan_is_recsrc(), 
				chan_is_record(), chan_is_mute());
		}

		chan_set_curr(1);
		orig_vol = chan_get_vb();

		printf("Setting the channel %s to 100\n", chan_get_name());
		chan_set_lr(100, 100);
		usleep(200000);

		printf("Setting balance to right\n");
		chan_set_lr(0, 100);
		usleep(200000);

		printf("Setting balance to left\n");
		chan_set_lr(100, 0);
		usleep(200000);

		printf("Setting balance back to center\n");
		chan_set_lr(100, 100);

		printf("Decreasing volume with 2\n");
		chan_inc_vol(-2);
		err_msg("left=%d right=%d", chan_get_lr(CHAN_LEFT),
			chan_get_lr(CHAN_RIGHT));
		usleep(200000);

		printf("Increasing volume with 2\n");
		chan_inc_vol(2);
		err_msg("left=%d right=%d", chan_get_lr(CHAN_LEFT),
			chan_get_lr(CHAN_RIGHT));
		usleep(200000);

		printf("Restoring old values: vol=%3f bal=%3f\n", 
			orig_vol.volume, orig_vol.balance);
		chan_set_vb(orig_vol);

		for (i=mixer_get_num_chan()-1; i>=0; i--)
		{
			chan_set_curr(i);
			if (chan_is_record())
			{
				printf("Setting channel #%d as recsrc\n", i);
				chan_set_rec(CHAN_RECSRC);
				if (chan_is_recsrc())
				{
					printf("Channel #%d (%s) is recsrc\n",
						i, chan_get_name());
				}
				else
					printf("Setting recsrc failed!\n");

				break;
			}
		}
	}

	query();
	mixer_close();

	return 0;
}
#endif
