# 程序名
TARGET = compiler

# 当前项目的根目录
ROOT = $(PWD)

# 可执行文件存放路径
BINDIR = bin

# 编译的源码目录, 递归遍历3级子目录
SRCDIR = $(shell find $(ROOT)/src -maxdepth 3 -type d)

# 编译生成可执行文件
BIN = $(ROOT)/$(BINDIR)/$(TARGET)

# 定义宏
DEF = -DLINUX -DENABLE_EPOLL

# 源文件
SRCLIST = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))

# 头文件
INCLIST = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.h))

# 将.c替换成.o
OBJLIST = $(patsubst %.c, %.o, $(SRCLIST))

# 声明编译器
CC = gcc

# 声明头文件路径
CFLAGS = -I./src/

# 声明编译选项
OPTIONS = -g -c -Wall -std=c17

# 编译TARGET
all: $(TARGET)

$(TARGET): $(OBJLIST)
	mkdir -p bin
	$(CC) $^ -o $(BINDIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(OPTIONS) $(DEF) $^ -o $@ 

clean: 
	rm -rf $(OBJLIST) $(BIN)

.PHONY: all clean