# Makefile for Project 1: Parallel Matrix Extraction

# 1. 定義變數 (Variables)
CC = gcc
# CFLAGS 是一連串的編譯選項：
#   -Wall -Wextra: 開啟所有常見的警告。工程師最喜歡警告了，它能幫我們抓出潛在的 Bug。
#   -O3: 最高等級的編譯器最佳化。因為我們要測效能，這個參數絕對不能少。
#   -pthread: 告訴編譯器我們要連結 POSIX Thread 函式庫，這是多執行緒的靈魂。
#   -g: 加入除錯資訊，萬一程式當掉 (Segmentation fault)，我們可以用 gdb 追蹤。
CFLAGS = -Wall -Wextra -O3 -pthread -g

# exe file name
TARGET = extractor

# 所有的 .c 原始碼檔案
SRCS = src/main.c src/worker.c

# 預設目標
# make = 執行第一個 Target（通常就是 all）。
# make all = 明確指定執行名為 all 的 Target。
all: $(TARGET)

# 編譯規則：告訴 Make 如何把 SRCS 編譯成 TARGET
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# 清理編譯出來的檔案 (方便重新編譯)
clean:
	rm -f $(TARGET)