NUM_LANGUAGES		?=   2
MAX_ITEMS_PER_DIR	?=  30
MAX_PKMN			?= 898
ACHIEVEMENT_LEN		?= 100
STRING_LEN			?= 250
MAPSTRING_LEN		?= 800
PKMNPHRS_LEN		?= 150
BADGENAME_LEN		?=  50

VERSION				?= \"0.01\"

CC=g++
export LD	:=	$(CC)
LDFLAGS	=-fsanitize=leak
LIBS	=-lpng $(shell pkg-config --libs gtkmm-4.0)
CFLAGS	=-std=c++20 -g3 -ggdb -Wall $(shell pkg-config --cflags gtkmm-4.0) \
		 -DNUM_LANGUAGES=$(NUM_LANGUAGES) -DMAX_ITEMS_PER_DIR=$(MAX_ITEMS_PER_DIR) \
		 -DVERSION=$(VERSION)
CPPFLAGS= $(CFLAGS)
SOURCES=$(wildcard *.cpp)
CSOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.cpp=.o) $(CSOURCES:.c=.o)

sachi:	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -rf *o
