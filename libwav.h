#ifndef __LIBWAV_H__
#define __LIBWAV_H__

#define WAV_MAGIC 0x0

#define WAV_CHUNK_LEN 512

#define WAV_CHUNKID_RIFF "RIFF"
#define WAV_CHUNKID_FORMAT "fmt "
#define WAV_CHUNKID_DATA "data"

struct _wav_header
{
	char riff_type[5];
};

typedef struct _wav_header wav_header;

struct _wav_format
{
	unsigned short format;
	unsigned short channels;
	unsigned int samplerate;
	unsigned int bytespersec;
	unsigned short blockalign;
	unsigned short bitwidth;
};

typedef struct _wav_format wav_format;

union _wav_chunk_content
{
	wav_header header;
	wav_format format;
	unsigned char data[WAV_CHUNK_LEN];
};

struct _wav_chunk
{
	char chunk_id[5];
	unsigned int chunk_size;
	union _wav_chunk_content content;
};

typedef struct _wav_chunk wav_chunk;

struct _wav_file
{
	wav_header header;
	wav_format format;
	unsigned int datablocks;
	int *data;
};

typedef struct _wav_file wav_file;

void wav_read_format (wav_format*, FILE*);
void wav_print_format (wav_format*);
void wav_write_format (wav_format*, FILE*);

void wav_read_header (wav_header*, FILE*);
void wav_write_header (wav_header*, FILE*);
void wav_print_header (wav_header*);

void wav_read_chunk (wav_chunk*, FILE*);
void wav_print_chunk (wav_chunk*);

void wav_free (wav_file*);
void wav_read (wav_file*, FILE*);
void wav_write (wav_file*, FILE*);

void wav_apply_gain (wav_file*, double);

#endif /* __LIBWAV_H__ */
