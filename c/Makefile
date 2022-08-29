# 声明编译器
CC = g++
# 声明编译选项
FLAGS = -g -c -Wall -std=c11

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

# 编译TARGET
all: $(TARGET)

$(TARGET): $(OBJLIST)
	mkdir -p bin
	$(CC) $^ -o $(BINDIR)/$@

%.o: %.cpp
	$(CC) $(FLAGS) $(DEF) $^ -o $@ 

clean: 
	rm -rf $(OBJLIST) $(BIN)

.PHONY: all clean