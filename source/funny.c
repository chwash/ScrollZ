/*
 * funny.c: Well, I put some stuff here and called it funny.  So sue me. 
 *
 * Written by Michael Sandrof
 *
 * Copyright (c) 1990 Michael Sandrof.
 * Copyright (c) 1991, 1992 Troy Rollo.
 * Copyright (c) 1992-1998 Matthew R. Green.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************** PATCHED by Flier ******************************
#ifndef lint
static	char	rcsid[] = "@(#)$Id: funny.c,v 1.1.1.1 1998-09-10 17:31:13 f Exp $";
#endif
****************************************************************************/

#include "irc.h"

#include "ircaux.h"
#include "hook.h"
#include "vars.h"
#include "funny.h"
#include "names.h"
#include "server.h"
#include "lastlog.h"
#include "ircterm.h"
#include "output.h"
#include "numbers.h"
#include "parse.h"

/**************************** PATCHED by Flier ******************************/
#include "myvars.h"

extern void PrintNames _((char *, char *));
#if defined(OPERVISION) && defined(WANTANSI)
extern void OperVision _((char *, char *, char *));
#endif
/****************************************************************************/

static	char	*match_str = (char *) 0;

static	int	funny_min;
static	int	funny_max;
static	int	funny_flags;

void
funny_match(stuff)
	char	*stuff;
{
	malloc_strcpy(&match_str, stuff);
}

void
set_funny_flags(min, max, flags)
	int	min,
		max,
		flags;
{
	funny_min = min;
	funny_max = max;
	funny_flags = flags;
}

struct	WideListInfoStru
{
	char	*channel;
	int	users;
};

typedef	struct WideListInfoStru WideList;

static	WideList **wide_list = (WideList **) 0;
static	int	wl_size = 0;
static	int	wl_elements = 0;

static	int	funny_widelist_users _((WideList **, WideList **));
static	int	funny_widelist_names _((WideList **, WideList **));

static	int
funny_widelist_users(left, right)
	WideList	**left,
			**right;
{
	if ((**left).users > (**right).users)
		return -1;
	else if ((**right).users > (**left).users)
		return 1;
	else
		return my_stricmp((**left).channel, (**right).channel);
}

static	int
funny_widelist_names(left, right)
	WideList	**left,
			**right;
{
	int	comp;

	if ((comp = my_stricmp((**left).channel, (**right).channel)) != 0)
		return comp;
	else if ((**left).users > (**right).users)
		return -1;
	else if ((**right).users > (**left).users)
		return 1;
	else
		return 0;
}


void
funny_print_widelist()
{
	int	i;
	char	buffer1[BIG_BUFFER_SIZE];
	char	buffer2[BIG_BUFFER_SIZE];
	char	*ptr;

	if (!wide_list)
		return;

	if (funny_flags & FUNNY_NAME)
		qsort((void *) wide_list, wl_elements, sizeof(WideList *),
			(int (*)()) funny_widelist_names);
	else if (funny_flags & FUNNY_USERS)
		qsort((void *) wide_list, wl_elements, sizeof(WideList *),
			(int (*)()) funny_widelist_users);

	*buffer1 = '\0';
	for (i = 0; i < wl_elements; i++)
	{
		sprintf(buffer2, "%s(%d) ", wide_list[i]->channel,
				wide_list[i]->users);
		ptr = index(buffer1, '\0');
		if ((int) strlen(buffer1) + (int) strlen(buffer2) > CO - 5)
		{
			if (do_hook(WIDELIST_LIST, "%s", buffer1))
				say("%s", buffer1);
			*buffer1 = '\0';
			strcat(buffer1, buffer2);
		}
		else
			strcpy(ptr, buffer2);
	}
	if (*buffer1 && do_hook(WIDELIST_LIST, "%s", buffer1))
		say("%s" , buffer1);
	for (i = 0; i < wl_elements; i++)
	{
		new_free(&wide_list[i]->channel);
		new_free(&wide_list[i]);
	}
	new_free(&wide_list);
	wl_elements = wl_size = 0;
}

/*ARGSUSED*/
void
funny_list(from, ArgList)
	char	*from;
	char	**ArgList;
{
	char	*channel,
		*user_cnt,
		*line;
	WideList **new_list;
	int	cnt;
	static	char	format[25];
	static	unsigned int	last_width = -1;

	if (last_width != get_int_var(CHANNEL_NAME_WIDTH_VAR))
	{
/**************************** PATCHED by Flier ******************************/
		/*if ((last_width = get_int_var(CHANNEL_NAME_WIDTH_VAR)) != 0)
			sprintf(format, "*** %%-%u.%us %%-5s  %%s",
				(unsigned char) last_width,
				(unsigned char) last_width);
		else
			strcpy(format, "*** %s\t%-5s  %s");*/
		if ((last_width = get_int_var(CHANNEL_NAME_WIDTH_VAR)) != 0)
			sprintf(format, "%s %%-%u.%us %%-5s  %%s",ScrollZstr,
				(unsigned char) last_width,
				(unsigned char) last_width);
		else
			sprintf(format, "%s %%s\t%%-5s  %%s",ScrollZstr);
/****************************************************************************/
	}
	channel = ArgList[0];
	user_cnt = ArgList[1];
	line = PasteArgs(ArgList, 2);
	if (funny_flags & FUNNY_TOPIC && !(line && *line))
			return;
	cnt = atoi(user_cnt);
	if (funny_min && (cnt < funny_min))
		return;
	if (funny_max && (cnt > funny_max))
		return;
	if ((funny_flags & FUNNY_PRIVATE) && (*channel != '*'))
		return;
	if ((funny_flags & FUNNY_PUBLIC) && (*channel == '*'))
		return;
	if (match_str)
	{
		if (wild_match(match_str, channel) == 0)
			return;
	}
	if (funny_flags & FUNNY_WIDE)
	{
		if (wl_elements >= wl_size)
		{
			new_list = (WideList **) new_malloc(sizeof(WideList *) *
			    (wl_size + 50));
			bzero(new_list, sizeof(WideList *) * (wl_size + 50));
			if (wl_size)
				bcopy(wide_list, new_list, sizeof(WideList *)
					* wl_size);
			wl_size += 50;
			new_free(&wide_list);
			wide_list = new_list;
		}
		wide_list[wl_elements] = (WideList *)
			new_malloc(sizeof(WideList));
		wide_list[wl_elements]->channel = (char *) 0;
		wide_list[wl_elements]->users = cnt;
		malloc_strcpy(&wide_list[wl_elements]->channel,
				(*channel != '*') ? channel : "Prv");
		wl_elements++;
		return;
	}
	if (do_hook(current_numeric, "%s %s %s %s", from,  channel, user_cnt,
	    line) && do_hook(LIST_LIST, "%s %s %s", channel, user_cnt, line))
	{
		if (channel && user_cnt)
		{
			if (*channel == '*')
				put_it(format, "Prv", user_cnt, line);
			else
				put_it(format, channel, user_cnt, line);
		}
	}
}

void
funny_namreply(from, Args)
	char	*from;
	char	**Args;
{
	char	*type,
		*nick,
		*channel;
	static	char	format[40];
	static	unsigned int	last_width = -1;
	int	cnt;
	char	*ptr;
	char	*line;
	ChannelList	*tmp = (ChannelList *) 0;

	PasteArgs(Args, 2);
	type = Args[0];
	channel = Args[1];
	line = Args[2];
	message_from(channel, LOG_CRAP);
	if ((tmp = lookup_channel(channel, parsing_server_index, CHAN_NOUNLINK)) && !((tmp->status & CHAN_NAMES) && (tmp->status & CHAN_MODE)))
	{
		if (do_hook(current_numeric, "%s %s %s %s", from, type, channel,
			line) && get_int_var(SHOW_CHANNEL_NAMES_VAR))
/**************************** PATCHED by Flier ******************************/
			/*say("Users on %s: %s", channel, line);
		while ((nick = next_arg(line, &line)) != NULL)
			add_to_channel(channel, nick, parsing_server_index, 0, 0);*/
                        PrintNames(channel,line);
                while ((nick=next_arg(line,&line))!=NULL)
                    add_to_channel(channel,nick,parsing_server_index,0,0,NULL,tmp);
/****************************************************************************/
		tmp->status |= CHAN_NAMES;
		message_from(NULL, LOG_CURRENT);
		return;
	}
	if (last_width != get_int_var(CHANNEL_NAME_WIDTH_VAR))
	{
		if ((last_width = get_int_var(CHANNEL_NAME_WIDTH_VAR)) != 0)
			sprintf(format, "%%s: %%-%u.%us %%s",
				(unsigned char) last_width,
				(unsigned char) last_width);
		else
			strcpy(format, "%s: %s\t%s");
	}
	ptr = line;
	for (cnt = -1; ptr; cnt++)
	{
		if ((ptr = index(ptr, ' ')) != NULL)
			ptr++;
	}
	if (funny_min && (cnt < funny_min))
		return;
	else if (funny_max && (cnt > funny_max))
		return;
	if ((funny_flags & FUNNY_PRIVATE) && (*type == '='))
		return;
	if ((funny_flags & FUNNY_PUBLIC) && (*type == '*'))
		return;
	if (type && channel)
	{
		if (match_str)
		{
			if (wild_match(match_str, channel) == 0)
				return;
		}
		if (do_hook(current_numeric, "%s %s %s %s", from, type, channel,
			line) && do_hook(NAMES_LIST, "%s %s", channel, line))
		{
			switch (*type)
			{
			case '=':
				if (last_width &&(strlen(channel) > last_width))
				{
					channel[last_width-1] = '>';
					channel[last_width] = (char) 0;
				}
/**************************** PATCHED by Flier ******************************/
				/*put_it(format, "Pub", channel, line);*/
				say("Users on %s are : %s",channel,line);
/****************************************************************************/				
				break;
			case '*':
/**************************** PATCHED by Flier ******************************/
				/*put_it(format, "Prv", channel, line);*/
				say("Users on %s are : %s",channel,line);
/****************************************************************************/				
				break;
			case '@':
/**************************** PATCHED by Flier ******************************/
				/*put_it(format, "Sec", channel, line);*/
				say("Users on %s are : %s",channel,line);
/****************************************************************************/				
				break;
			}
		}
	}
	message_from(NULL, LOG_CURRENT);
}

void
funny_mode(from, ArgList)
	char	*from,
		**ArgList;
{
	char	*mode, *channel;
	ChannelList	*tmp = (ChannelList *) 0;

	if (!ArgList[0])
		return;
	if (get_server_version(parsing_server_index) < Server2_6)
	{
		channel = (char *) 0;
		mode = ArgList[0];
		PasteArgs(ArgList, 0);
	}
	else
	{
		channel = ArgList[0];
		mode = ArgList[1];
		PasteArgs(ArgList, 1);
	}
	/* if (ignore_mode) */
	if (channel && (tmp = lookup_channel(channel, parsing_server_index, CHAN_NOUNLINK)) && !((tmp->status & CHAN_NAMES) && (tmp->status & CHAN_MODE)))
	{
/**************************** PATCHED by Flier ******************************/
		/*update_channel_mode(channel, parsing_server_index, mode);*/
		update_channel_mode(channel,parsing_server_index,mode,NULL,NULL,NULL,NULL,tmp);
/****************************************************************************/
		tmp->status |= CHAN_MODE;
		update_all_status();
	}
	else
	{
		if (channel)
		{
			message_from(channel, LOG_CRAP);
			if (do_hook(current_numeric, "%s %s %s", from,
					channel, mode))
				put_it("%s Mode for channel %s is \"%s\"",
					numeric_banner(), channel, mode);
		}
		else
		{
			if (do_hook(current_numeric, "%s %s", from, mode))
				put_it("%s Channel mode is \"%s\"",
					numeric_banner(), mode);
		}
	}
}

/**************************** PATCHED by Flier ******************************/
void update_user_mode(modes)
char *modes;
{
    int	onoff=1;

    while (*modes) {
        if (*modes=='-') onoff=0;
        else if (*modes=='+') onoff=1;
        else {
            if (*modes=='o' || *modes=='O')
                set_server_operator(parsing_server_index,onoff);
            set_server_umode_flag(parsing_server_index,*modes,onoff);
        }
        modes++;
    }
}

void reinstate_user_modes()
{
    int  i;
    char modes[32];
    char *c;

    if (get_server_version(parsing_server_index)<Server2_7) return;
    c=modes;
    for (i=0;i<25;i++)
        if ('a'+i!='o' && get_server_umode_flag(parsing_server_index,'a'+i)) *c++='a'+i;
    *c='\0';
    if (PermUserMode)
        send_to_server("MODE %s %s",get_server_nickname(parsing_server_index),
                       PermUserMode);
    else if (c!=modes) send_to_server("MODE %s +%s",
                                      get_server_nickname(parsing_server_index),modes);
#if defined(OPERVISION) && defined(WANTANSI)
    if (OperV) {
        OperVision(NULL,"OFF",NULL);
        OperVision(NULL,"ON",NULL);
    }
#endif
}
/****************************************************************************/
