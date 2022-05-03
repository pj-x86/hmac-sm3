###本 Makefile 在 linux 下 GNU 编译器使用，也可在 windows 下 mingw 编译器使用

CXX = g++
CC = gcc
EXE = 
OBJ = .o

MYSRC = ./src
#增加VPATH，搜索指定目录下的源文件
VPATH = $(MYSRC):.
MYINCLUDES = -I. -I$(MYSRC) -I$(MYSRC)/include
CFLAGS = -g $(MYINCLUDES)
CPPFLAGS = -g $(MYINCLUDES)

LIBS = -L/usr/include -lz -lpthread

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CXX) -c $(CPPFLAGS) $< -o $@

SERVER1 = test_sm3$(EXE)
SERVER2 = hmac_sm3$(EXE)

S1OBJS = test_sm3$(OBJ) sm3$(OBJ) 
S2OBJS = hmac_sm3$(OBJ) sm3$(OBJ)

all : $(SERVER1) $(SERVER2)

$(SERVER1) : $(S1OBJS)
	rm -f $@
	$(CXX) -o $@ $(S1OBJS) $(LIBS)
	@echo "--------------------generate $@ successful-------------------------------"
	
$(SERVER2) : $(S2OBJS)
	rm -f $@
	$(CXX) -o $@ $(S2OBJS) $(LIBS)
	@echo "--------------------generate $@ successful-------------------------------"

clean :
	rm -f *$(OBJ) $(SERVER1) $(SERVER2)


