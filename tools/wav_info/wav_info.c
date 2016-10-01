/* Copyright 2016 Marc Volker Dickmann */
#include <stdio.h>
#include "../../libwav.h"

static void print_info (char *filename)
{
	FILE *f;
	wav_file wavfile;
	
	f = fopen (filename, "rb");
	
	if (f == NULL)
	{
		printf ("Error: Couldn't open the file!\n");
		return;
	}
	
	wav_read (&wavfile, f);
	wav_print_header (&wavfile.header);
	wav_print_format (&wavfile.format);
	
	wav_free (&wavfile);
	
	fclose (f);
}

int main (int argc, char *argv[])
{
	printf ("LibWAV v. 0.0.1 A (c) 2016 Marc Volker Dickmann\n\n");
	
	if (argc == 2)
	{
		print_info (argv[1]);
	}
	else
	{
		printf ("Usage: wav_info <filename>!\n");
	}
	
	return 0;
}
