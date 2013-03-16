/**********************************************************************
 *
 *  util.c
 *
 *  Author: Stephen V. Rice
 *  
 * Copyright 1996 The Board of Regents of the Nevada System of Higher
 * Education, on behalf, of the University of Nevada, Las Vegas,
 * Information Science Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You
 * may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 **********************************************************************/

#include <stdlib.h>
#include <signal.h>

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <sys\types.h>
#include <sys\stat.h>
#endif

#include "util.h"

char *exec_name;

Boolean usage_when_no_args = True;
void (*usage_routine)();

void (*cleanup_routine)();

int errstatus = 1;

/**********************************************************************/

void *allocate(number, size)
size_t number, size;
{
    void *p;
    p = calloc(number, size);
    if (!p)
	error("unable to allocate memory", Exit);
    return(p);
}
/**********************************************************************/

int ustrcmp(s1, s2)
unsigned char *s1, *s2;
{
    long i;
    for (i = 0; s1[i] && s1[i] == s2[i]; i++);
    if (s1[i] || s2[i])
	return(s1[i] < s2[i] ? -1 : 1);
    else
	return(0);
}
/**********************************************************************/

FILE *open_file(filename, mode)
char *filename, *mode;
{
    FILE *f;
    if (!filename)
	return(mode[0] == 'r' ? stdin : stdout);
    f = fopen(filename, mode);
    if (f)
	return(f);
    if (mode[0] == 'w')
	error_string("unable to create file", filename, Exit);
    else
	error_string("unable to open file", filename, Exit);
    return NULL;
}
/**********************************************************************/

void close_file(f)
FILE *f;
{
    if (f != stdin && f != stdout)
	fclose(f);
}
/**********************************************************************/

Boolean file_exists(filename)
char *filename;
{
    struct stat buffer;
    return(stat(filename, &buffer) == 0 ? True : False);
}
/**********************************************************************/

char *basefilename(pathname)
char *pathname;
{
#ifdef unix
    char delimiter = '/';
#else
    char delimiter = '\\';
#endif
    short i;
    for (i = strlen(pathname) - 1; i >= 0 && pathname[i] != delimiter; i--);
    return(&pathname[i + 1]);
}
/**********************************************************************/

static void show_usage(usage)
char *usage;
{
    if (usage_routine)
	(*usage_routine)();
    else
	fprintf(stderr, "Usage:  %s %s\n", exec_name, usage);
    exit(0);
}
/**********************************************************************/

static Boolean split_option(arg, next_arg, option)
char *arg, *next_arg;
Option option[];
{
    short i;
    if (!option)
	goto invalid_option;
    for (i = 0; option[i].name && arg[1] != option[i].name; i++);
    if (!option[i].name)
	goto invalid_option;
    if (option[i].string && (arg[2] || next_arg))
    {
	if (*option[i].string)
	    goto duplicate_option;
	*option[i].string = (arg[2] ? &arg[2] : next_arg);
	return(arg[2] ? False : True);
    }
    if (option[i].boolean && !arg[2])
    {
	if (*option[i].boolean)
	    goto duplicate_option;
	*option[i].boolean = True;
	return(False);
    }
invalid_option:
    error_string("invalid option", arg, Exit);
duplicate_option:
    error_string("duplicate option", arg, Exit);
    return(False);
}
/**********************************************************************/

static void parse_args(argc, argv, usage, option)
int *argc;
char *argv[], *usage;
Option option[];
{
    short i, j = 0;
    if (*argc == 1 && usage_when_no_args)
	show_usage(usage);
    for (i = 1; i < *argc; i++)
	if (argv[i][0] == '-' && argv[i][1])
	{
	    if (strncmp("-help", argv[i], strlen(argv[i])) == 0)
		show_usage(usage);
	    if (split_option(argv[i], (i + 1 < *argc ? argv[i + 1] : NULL),
	    option))
		i++;
	}
	else
	    argv[j++] = argv[i];
    *argc = j;
}
/**********************************************************************/

void initialize(argc, argv, usage, option)
int *argc;
char *argv[], *usage;
Option option[];
{
    exec_name = basefilename(argv[0]);
    parse_args(argc, argv, usage, option);
}
/**********************************************************************/

static void quit(status)
int status;
{
    if (cleanup_routine)
	(*cleanup_routine)();
    exit(status);
}
/**********************************************************************/

void error(message, exit)
char *message;
Boolean exit;
{
    fprintf(stderr, "%s: %s\n", exec_name, message);
    if (exit)
	quit(errstatus);
}
/**********************************************************************/

void error_status(message, status, exit)
char *message;
int status;
Boolean exit;
{
    fprintf(stderr, "%s: %s, status = %d\n", exec_name, message, status);
    if (exit)
	quit(errstatus);
}
/**********************************************************************/

void error_string(message, string, exit)
char *message, *string;
Boolean exit;
{
    fprintf(stderr, "%s: %s \"%s\"\n", exec_name, message, string);
    if (exit)
	quit(errstatus);
}
