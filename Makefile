# $Id$

# GNU Compiler
#CC = g++
#CFLAGS = -DYAPS_TIME

# Intel Compiler
CC = icpc
CFLAGS = -openmp -DYAPS_TIME
LDFLAGS = -openmp

SRC_DIR = src
OBJS_SIM = common.o io.o iobin.o vec.o eos.o kernel.o calc.o yaps_sim.o
OBJS_POST = common.o io.o iobin.o vec.o render.o yaps_post.o
OBJS_SIM1 = $(addprefix $(SRC_DIR)/,$(OBJS_SIM))
OBJS_POST1 = $(addprefix $(SRC_DIR)/,$(OBJS_POST))
LDLIBS_POST = -lGL -lGLU -lglut

all : yaps_sim yaps_post

yaps_sim : $(OBJS_SIM1)
	$(CC) $(LDFLAGS) $^ -o $@ 

yaps_post : $(OBJS_POST1) $(LDLIBS_POST)
	$(CC) $(LDFLAGS) $^ -o $@ 

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(SRC_DIR)/*.o yaps_sim yaps_post
