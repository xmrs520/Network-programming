TARGET = main
LIB = ./lib/libmymedia.a
LIBSO = ./lib/libmymedia.so.1.0
DIRS = /mnt/hgfs/嵌入式/myXmrs/project_04/src
INC = -I /mnt/hgfs/嵌入式/myXmrs/project_04/inc
SRCS = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJ = $(patsubst %.c,%.o,$(SRCS))
CC := arm-linux-gcc
ARCR := ar -cr
RMRF:=rm -rf
SHARED = -fPIC -shared

#静态库打包
# $(LIB):$(OBJ)
# 	$(ARCR) $@ $^
#编译
$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -L ./lib  ./lib/libfont.a -lpthread -lm 
#动态库打包
# $(LIBSO):$(OBJ)
# 	$(CC) $(SHARED) $^ -o $@
# $(TARGET):$(OBJ)
# 	$(CC) $^ -o ^@
.PHONY:
arm:
	arm-linux-gcc main.c -o $(TARGET) -L ./lib ./lib/libfont.a -lmymedia -lpthread -lm 
gcc:
#	$(CC) main.c -o $(TARGET) $(LIB)
	$(CC) main.c -o $(TARGET) -L ./lib ./lib/libfont.a -lmymedia -lpthread -lm
clean:
	$(RMRF) $(OBJ)
cleanAll:
	$(RMRF) $(OBJ) $(TARGET) $(LIB)