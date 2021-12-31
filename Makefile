BOOTFILE = ./boot/boot.asm

IMAGE_MOUNT_DIR := 

BIN_OUT := ./out/bin/
IMAGE_OUT := ./out/image/

TARGET:= $(BIN_OUT)boot.bin
LOADER_BIN := $(BIN_OUT)loader.bin
LOADERFILE := ./boot/bootloader.asm

INCLUDE_DIR = \
			  -I./include/asm/	\
			  -I./include/fat12/


.PHONY:all

all:mount_image
	nasm  $(INCLUDE_DIR) $(LOADERFILE) 	-o $(LOADER_BIN)
	nasm  $(INCLUDE_DIR) $(BOOTFILE) 	-o $(TARGET)
	dd if=$(BIN_OUT)boot.bin of=$(IMAGE_OUT)moos.img bs=512 count=1 conv=notrunc


mount_image:
	echo "mount moos.image"


clean:
	rm $(BIN_OUT)* $(IMAGE_OUT)*
