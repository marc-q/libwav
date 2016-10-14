CC = gcc
OBJECTS = ../../libwav.c wav_gain.c
LIBS =
CFLAGS = -Wall -O2
BINDIR = $(DESTDIR)/usr/bin
NAME = wav_gain

wav_gain: $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm $(NAME)
