/* Copyright 2016 - 2017 Marc Volker Dickmann */
#include <stdio.h>
#include "../../libwav.h"

static void
apply_gain (wav_file *wavfile, const double gain)
{
	for (size_t i = 0; i < wavfile->datablocks; i++)
	{
		// Gain the left and right channel of the sample!
		wavfile->data[i] = (int)((wavfile->data[i] & 0xFFFF) * gain) | ((int)((wavfile->data[i] >> 16) * gain) << 16);
	}
}

static void
gain_file (const char *filename, const char *filename_out)
{
	wav_file wavfile;
	
	if (wav_read (&wavfile, filename) != WAV_OK)
	{
		printf ("Error: Couldn't read the file!\n");
		return;
	}
	
	apply_gain (&wavfile, 2.0);
	
	if (wav_write (&wavfile, filename_out) != WAV_OK)
	{
		printf ("Error: Couldn't write the file!\n");
	}
	
	wav_free (&wavfile);
}

int
main (int argc, char *argv[])
{
	printf ("LibWAV v. 0.0.1 A (c) 2016 - 2017 Marc Volker Dickmann\n\n");
	
	if (argc == 3)
	{
		gain_file (argv[1], argv[2]);
	}
	else
	{
		printf ("Usage: wav_gain <filename> <filename_out>!\n");
	}
	return 0;
}
