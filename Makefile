############
#Makefile
#Lucas Manker
#COSC 3750 Spring 2020
#Homework #11
#4/20/20
#
#Makefile for hw11
###########

CC=gcc
CFLAGS=-ggdb -Wall -Wno-unused-function 

.PHONY: clean

wyshell: mmult.c
	$(CC) $(CFLAGS) mmult.c -o mmult


clean:
	/bin/rm -f mmult
