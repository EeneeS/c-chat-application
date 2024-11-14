# Makefile

# Variables
CC = gcc
SRC_DIR = src

# Targets
server:
	$(CC) $(SRC_DIR)/server.c -o server

client:
	$(CC) $(SRC_DIR)/client.c -o client

