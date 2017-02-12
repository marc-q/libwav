/* Copyright 2016 - 2017 Marc Volker Dickmann */
#include <stdio.h>
#include "../../libwav.h"

static void
print_info (const char *filename)
{
	wav_file wavfile;
	
	if (wav_read (&wavfile, filename) != WAV_OK)
	{
		printf ("Error: Couldn't open the file!\n");
		return;
	}
	
	wav_header_print (&wavfile.header);
	wav_format_print (&wavfile.format);
	wav_free (&wavfile);
}

int
main (int argc, char *argv[])
{
	printf ("LibWAV v. 0.0.1 A (c) 2016 - 2017 Marc Volker Dickmann\n\n");
	
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
