/* Copyright 2016 Marc Volker Dickmann */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "libwav.h"

static bool utils_streq (char *a, char *b)
{
	return (strlen (a) == strlen (b)) && (strcmp (a, b) == 0); 
}

void wav_read_format (wav_format *format, FILE *f)
{
	fread (&format->format, sizeof (format->format), 1, f);
	fread (&format->channels, sizeof (format->channels), 1, f);
	fread (&format->samplerate, sizeof (format->samplerate), 1, f);
	fread (&format->bytespersec, sizeof (format->bytespersec), 1, f);
	fread (&format->blockalign, sizeof (format->blockalign), 1, f);
	fread (&format->bitwidth, sizeof (format->bitwidth), 1, f);
}

void wav_print_format (wav_format *format)
{
	printf ("Format:\t\t%hi\n", format->format);
	printf ("Channels:\t%hi\n", format->channels);
	printf ("Samplerate:\t%i\n", format->samplerate);
	printf ("Bytespersec:\t%i\n", format->bytespersec);
	printf ("Blockalign:\t%hi\n", format->blockalign);
	printf ("Bitwidth:\t%hi\n", format->bitwidth);
}

void wav_write_format (wav_format *format, FILE *f)
{
	fwrite (&format->format, sizeof (format->format), 1, f);
	fwrite (&format->channels, sizeof (format->channels), 1, f);
	fwrite (&format->samplerate, sizeof (format->samplerate), 1, f);
	fwrite (&format->bytespersec, sizeof (format->bytespersec), 1, f);
	fwrite (&format->blockalign, sizeof (format->blockalign), 1, f);
	fwrite (&format->bitwidth, sizeof (format->bitwidth), 1, f);
}

void wav_read_header (wav_header *header, FILE *f)
{
	fread (header->riff_type, sizeof (char) * 4, 1, f);
	header->riff_type[4] = '\0';
}

void wav_write_header (wav_header *header, FILE *f)
{	
	fwrite (header->riff_type, sizeof (char) * 4, 1, f);
}

void wav_print_header (wav_header *header)
{
	printf ("Riff Type:\t%s\n", header->riff_type);
}

void wav_read_chunk (wav_chunk *chunk, FILE *f)
{
	fread (chunk->chunk_id, sizeof (char) * 4, 1, f);
	chunk->chunk_id[4] = '\0';
	
	fread (&chunk->chunk_size, sizeof (chunk->chunk_size), 1, f);
	
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		wav_read_header (&chunk->content.header, f);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		wav_read_format (&chunk->content.format, f);
	}
}

void wav_print_chunk (wav_chunk *chunk)
{
	printf ("Chunk ID:\t%s\n", chunk->chunk_id);
	printf ("Chunk Size:\t%i\n", chunk->chunk_size);
	
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		wav_print_header (&chunk->content.header);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		wav_print_format (&chunk->content.format);
	}
}

void wav_free (wav_file *wavfile)
{
	free (wavfile->data);
}

void wav_read (wav_file *wavfile, FILE *f)
{
	wav_chunk chunk;
	
	wav_read_chunk (&chunk, f);	
	if (!utils_streq (chunk.chunk_id, WAV_CHUNKID_RIFF))
	{
		printf ("Error: Not a WAV file!\n");
		return;
	}
	wavfile->header = chunk.content.header;
	
	while (!feof (f))
	{
		wav_read_chunk (&chunk, f);
		
		if (utils_streq (chunk.chunk_id, WAV_CHUNKID_FORMAT))
		{
			wavfile->format = chunk.content.format;
			wavfile->datablocks = 0;
			wavfile->data = NULL;
		}
		else if (utils_streq (chunk.chunk_id, WAV_CHUNKID_DATA))
		{
			if (wavfile->data != NULL)
			{
				printf ("Error: More than one data chunks!\n");
				break;
			}
			
			wavfile->datablocks = chunk.chunk_size / sizeof (int);
			
			wavfile->data = (int*) malloc (sizeof (int) * wavfile->datablocks);
			fread (wavfile->data, sizeof (int) * wavfile->datablocks, 1, f);
			break;
		}
		else
		{
			printf ("Unknown chunk: %s!\n", chunk.chunk_id);
			fseek (f, chunk.chunk_size, SEEK_CUR);
		}
	}
}

void wav_write (wav_file *wavfile, FILE *f)
{
	unsigned int chunk_size;
	char chunk_id[5];
	
	/* Header */
	chunk_size = 36 + (wavfile->datablocks * sizeof (int));
	strcpy (chunk_id, WAV_CHUNKID_RIFF);
	
	fwrite (chunk_id, sizeof (char) * 4, 1, f);
	fwrite (&chunk_size, sizeof (chunk_size), 1, f);
	
	wav_write_header (&wavfile->header, f);
	
	/* Format */
	chunk_size = 16;
	strcpy (chunk_id, WAV_CHUNKID_FORMAT);
	
	fwrite (chunk_id, sizeof (char) * 4, 1, f);
	fwrite (&chunk_size, sizeof (chunk_size), 1, f);
	
	wav_write_format (&wavfile->format, f);
	
	/* Data */
	chunk_size = wavfile->datablocks * sizeof (int);
	strcpy (chunk_id, WAV_CHUNKID_DATA);
	
	fwrite (chunk_id, sizeof (char) * 4, 1, f);
	fwrite (&chunk_size, sizeof (chunk_size), 1, f);
	
	fwrite (wavfile->data, sizeof (int) * wavfile->datablocks, 1, f);
}

void wav_apply_gain (wav_file *wavfile, double gain)
{
	unsigned int i;
	
	for (i = 0; i < wavfile->datablocks; i++)
	{
		/* Gain the left and right channel of the sample! */
		wavfile->data[i] = (int)((wavfile->data[i] & 0xFFFF) * gain) | ((int)(((wavfile->data[i] & 0xFFFF0000) >> 16) * gain) << 16);
	}
}
