#************************************************************************
# Makefile for gif builder tool
# Benjamin Morgan, 2016
# github.com/bmorgan5/led_vest
#************************************************************************

TARGET = gif_builder

CFLAGS = -O2 -Wall -std=gnu99
LDFLAGS = -L/usr/local/lib -lgif -L../../

CC = /usr/bin/gcc

SOURCES = gif_builder.c

$(TARGET): $(SOURCES)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(SOURCES) -o "$@" $(LDFLAGS)
