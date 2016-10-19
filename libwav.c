/* Copyright 2016 Marc Volker Dickmann */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "libwav.h"

static bool utils_streq (const char *a, const char *b)
{
	return (strlen (a) == strlen (b)) && (strcmp (a, b) == 0); 
}

/* WAV_HEADER */

int wav_header_write (const wav_header *header, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fwrite (header->riff_type, sizeof (char), 4, f);
	return WAV_OK;
}

int wav_header_read (wav_header *header, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fread (header->riff_type, sizeof (char), 4, f);
	header->riff_type[4] = '\0';
	return WAV_OK;
}

void wav_header_print (const wav_header *header)
{
	printf ("Riff Type:\t%s\n", header->riff_type);
}

/* WAV_FORMAT */

int wav_format_write (const wav_format *format, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fwrite (format, sizeof (wav_format), 1, f);	
	return WAV_OK;
}

int wav_format_read (wav_format *format, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fread (format, sizeof (wav_format), 1, f);
	return WAV_OK;
}

void wav_format_print (const wav_format *format)
{
	printf ("Format:\t\t%hu\n", format->format);
	printf ("Channels:\t%hu\n", format->channels);
	printf ("Samplerate:\t%u\n", format->samplerate);
	printf ("Bytespersec:\t%u\n", format->bytespersec);
	printf ("Blockalign:\t%hu\n", format->blockalign);
	printf ("Bitwidth:\t%hu\n", format->bitwidth);
}

/* WAV_CHUNK */

void wav_chunk_init (wav_chunk *chunk, const char *id, const int size, const void *content)
{
	strcpy (chunk->chunk_id, id);
	chunk->chunk_size = size;
	
	/* Init the content based on the chunkid: */
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		chunk->content.header = *(wav_header*) content;
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		chunk->content.format = *(wav_format*) content;
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_DATA))
	{
		chunk->content.data = (int*) content;
	}
}

int wav_chunk_write (const wav_chunk *chunk, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fwrite (chunk->chunk_id, sizeof (char), 4, f);
	fwrite (&chunk->chunk_size, sizeof (chunk->chunk_size), 1, f);
	
	/* Write the content based on the chunkid: */
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		return wav_header_write (&chunk->content.header, f);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		return wav_format_write (&chunk->content.format, f);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_DATA))
	{
		fwrite (chunk->content.data, sizeof (int), chunk->chunk_size / sizeof (int), f);
		return WAV_OK;
	}
	
	return WAV_UNKNOWN_CHUNKID;
}

int wav_chunk_read (wav_chunk *chunk, FILE *f)
{
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	fread (chunk->chunk_id, sizeof (char), 4, f);
	chunk->chunk_id[4] = '\0';
	
	fread (&chunk->chunk_size, sizeof (chunk->chunk_size), 1, f);
	
	/* Read the content based on the chunkid: */
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		wav_header_read (&chunk->content.header, f);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		wav_format_read (&chunk->content.format, f);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_DATA))
	{
		chunk->content.data = (int*) malloc (chunk->chunk_size);
		fread (chunk->content.data, sizeof (int), chunk->chunk_size / sizeof (int), f);
	}
	
	return WAV_OK;
}

void wav_chunk_print (const wav_chunk *chunk)
{
	printf ("Chunk ID:\t%s\n", chunk->chunk_id);
	printf ("Chunk Size:\t%u\n", chunk->chunk_size);
	
	if (utils_streq (chunk->chunk_id, WAV_CHUNKID_RIFF))
	{
		wav_header_print (&chunk->content.header);
	}
	else if (utils_streq (chunk->chunk_id, WAV_CHUNKID_FORMAT))
	{
		wav_format_print (&chunk->content.format);
	}
}

/* WAV_FILE */

void wav_free (wav_file *wavfile)
{
	free (wavfile->data);
}

int wav_write (const wav_file *wavfile, const char *filename)
{
	FILE *f;
	wav_chunk chunk;
	
	f = fopen (filename, "wb");
	
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	/* Header */
	wav_chunk_init (&chunk, WAV_CHUNKID_RIFF, 36 + (sizeof (int) * wavfile->datablocks), &wavfile->header);
	wav_chunk_write (&chunk, f);
	
	/* Format */
	wav_chunk_init (&chunk, WAV_CHUNKID_FORMAT, 16, &wavfile->format);
	wav_chunk_write (&chunk, f);
	
	/* Data */
	wav_chunk_init (&chunk, WAV_CHUNKID_DATA, sizeof (int) * wavfile->datablocks, wavfile->data);
	wav_chunk_write (&chunk, f);
	
	fclose (f);
	return WAV_OK;
}

int wav_read (wav_file *wavfile, const char *filename)
{
	FILE *f;
	wav_chunk chunk;
	
	f = fopen (filename, "rb");
	
	if (f == NULL)
	{
		return WAV_FILE_NOT_OPENED;
	}
	
	/* Check if its a valid file: */
	wav_chunk_read (&chunk, f);
	if (!utils_streq (chunk.chunk_id, WAV_CHUNKID_RIFF))
	{
		fclose (f);
		return WAV_INVALID_FILE;
	}
	
	wavfile->header = chunk.content.header;
	wavfile->datablocks = 0;
	wavfile->data = NULL;
	
	/* Read the chunks: */
	while (!feof (f))
	{
		wav_chunk_read (&chunk, f);
		
		if (utils_streq (chunk.chunk_id, WAV_CHUNKID_FORMAT))
		{
			wavfile->format = chunk.content.format;
		}
		else if (utils_streq (chunk.chunk_id, WAV_CHUNKID_DATA))
		{	
			wavfile->datablocks = chunk.chunk_size / sizeof (int);
			wavfile->data = chunk.content.data;
			break;
		}
		else
		{
			/* NOTE: Unknown chunk! */
			fseek (f, chunk.chunk_size, SEEK_CUR);
		}
	}
	
	fclose (f);
	return WAV_OK;
}
