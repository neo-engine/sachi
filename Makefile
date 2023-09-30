NUM_LANGUAGES		?=   2
MAX_ITEMS_PER_DIR	?=  30
MAX_PKMN			?= 898
ACHIEVEMENT_LEN		?= 100
STRING_LEN			?= 250
MAPSTRING_LEN		?= 800
PKMNPHRS_LEN		?= 150
BADGENAME_LEN		?=  50

VERSION				?= \"0.40\"


SRCDIR := src
OBJDIR := obj

CC=g++
export LD	:=	$(CC)
LDFLAGS	= -L /opt/homebrew/lib/

LIBS	= -lpng $(shell pkg-config --libs gtkmm-4.0)
SRCS	:= $(shell find $(SRCDIR) -name "*.cpp")
OBJS	:= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS	:= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d)
TREE	:= $(patsubst %/,%,$(dir $(OBJS)))

CFLAGS	=-std=c++20 -MMD -MP -MF $(@:$(OBJDIR)/%.o=$(OBJDIR)/%.d) \
		 -g3 -ggdb -Wall -Wextra $(shell pkg-config --cflags gtkmm-4.0) \
		 -DNUM_LANGUAGES=$(NUM_LANGUAGES) -DMAX_ITEMS_PER_DIR=$(MAX_ITEMS_PER_DIR) \
		 -DVERSION=$(VERSION) \
		 -I/opt/homebrew/include/
CPPFLAGS= $(CFLAGS)

.PHONY: all clean

all: sachi

.SECONDEXPANSION:
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $$(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TREE): %:
	mkdir -p $@

sachi:	$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	$(RM) -r $(OBJDIR) $(OBJDIR)

ifeq "$(MAKECMDGOALS)" ""
	-include $(DEPS)
endif

