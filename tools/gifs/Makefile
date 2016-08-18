#************************************************************************
# Makefile for gif builder tool
# Benjamin Morgan, 2016
# github.com/bmorgan5/led_vest
#************************************************************************

TARGET = gif_builder

CFLAGS = -O2 -Wall -I/usr/local/include -L/usr/local/lib

CC = gcc

SOURCES = gif_builder.c

$(TARGET): $(SOURCES)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $(SOURCES) -o "$@" -c gif_builder.o
