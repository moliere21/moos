# 镜像挂载目录，用于挂载镜像、复制文件
IMAGE_MOUNT_DIR := ./mnt
KERNEL_DIR	:= ./kernel

# out 输出目录
OUT_DIR := ./out
BIN_OUT := ./out/bin
IMAGE_OUT := ./out/image

# 生成的二进制文件
BOOT_BIN:= $(BIN_OUT)/boot.bin
LOADER_BIN := $(BIN_OUT)/loader.bin
KERNEL_BIN	:= $(BIN_OUT)/kernel.bin

# 镜像文件
IMAGE_FILE	:= $(IMAGE_OUT)/moos.img

# 源文件
BOOTFILE = ./boot/boot.asm
LOADERFILE := ./boot/bootloader.asm

# 头文件目录
INCLUDE_DIR = \
			  -I./include/asm/	\
			  -I./include/fat12/

BIN_FILES := loader_bin kernel_bin
IMAGE_FILES := mount_image

.PHONY:all

all: $(BIN_FILES) $(IMAGE_FILES)
	nasm  $(INCLUDE_DIR) $(BOOTFILE) 	-o $(BOOT_BIN)
	dd if=$(BOOT_BIN) of=$(IMAGE_OUT)/moos.img bs=512 count=1 conv=notrunc


run:
	bochs -q
# loader.bin 文件
loader_bin:
	nasm  $(INCLUDE_DIR) $(LOADERFILE)  -o $(LOADER_BIN)


kernel_bin:
	nasm -f elf $(KERNEL_DIR)/asm/kernel.asm -o $(OUT_DIR)/kernel.o
	nasm -f elf $(KERNEL_DIR)/asm/kernel_lib32.asm -o $(OUT_DIR)/kernel_lib32.o
	gcc -c -m32 -o $(OUT_DIR)/main.o $(KERNEL_DIR)/main.c
	ld -melf_i386 -Ttext 0x1000 -o $(KERNEL_BIN) $(OUT_DIR)/*.o

# 挂载 fat12 复制文件
mount_image:
	sudo mount -o loop $(IMAGE_FILE) $(IMAGE_MOUNT_DIR)
	sudo rm $(IMAGE_MOUNT_DIR)/* -rf
	sudo cp $(LOADER_BIN) $(IMAGE_MOUNT_DIR) -rf
	sudo cp $(KERNEL_BIN) $(IMAGE_MOUNT_DIR) -rf
	sudo umount ./mnt







mount:
	sudo mount -o loop $(IMAGE_FILE) $(IMAGE_MOUNT_DIR)

umount:
	sudo umount $(IMAGE_MOUNT_DIR)

clean:
	rm $(BIN_OUT)/* $(OUT_DIR)/*.o
