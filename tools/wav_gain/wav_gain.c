/* Copyright 2016 Marc Volker Dickmann */
#include <stdio.h>
#include "../../libwav.h"

static void apply_gain (char *filename, char *filename_out)
{
	FILE *f, *outfile;
	wav_file wavfile;
	
	f = fopen (filename, "rb");
	
	if (f == NULL)
	{
		printf ("Error: Couldn't open the file!\n");
		return;
	}
	
	outfile = fopen (filename_out, "wb");
	
	if (outfile == NULL)
	{
		fclose (f);
		return;
	}
	
	wav_read (&wavfile, f);
	
	wav_apply_gain (&wavfile, 2.0);
	
	wav_write (&wavfile, outfile);
	wav_free (&wavfile);
	
	fclose (f);
	fclose (outfile);
}

int main (int argc, char *argv[])
{
	printf ("LibWAV v. 0.0.1 A (c) 2016 Marc Volker Dickmann\n\n");
	
	if (argc == 3)
	{
		apply_gain (argv[1], argv[2]);
	}
	else
	{
		printf ("Usage: wav_gain <filename> <filename_out>!\n");
	}
	
	return 0;
}
