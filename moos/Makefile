# 镜像挂载目录，用于挂载镜像、复制文件
IMAGE_MOUNT_DIR := ./mnt
KERNEL_DIR	:= ./kernel

# 架构目录
ARCH_DIR := ./arch/i386

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

# BOOT 源文件
BOOTFILE = $(ARCH_DIR)/boot/boot.asm
LOADERFILE := $(ARCH_DIR)/boot/bootloader.asm


# BOOT头文件目录
BOOT_INCLUDE_DIR = \
			  -I$(ARCH_DIR)/boot/include/asm/	\
			  -I$(ARCH_DIR)/boot/include/fat12/




# KENREL 头文件目录
KERNEL_INCLUDE_DIR = \
				-I./include/	\
				-I./arch/i386/include/

# KERNEL 编译文件
KERNEL_OBJ_FILES = \
				$(KERNEL_DIR)/main.o	\
				$(KERNEL_DIR)/printk/printk.o	\
				$(KERNEL_DIR)/printk/string.o	\
				$(KERNEL_DIR)/printk/vsprint.o

# 架构 KERNEL 编译文件
ARCH_OBJ_FILES = \
				$(ARCH_DIR)/kernel/video-vga.o	\
				$(ARCH_DIR)/kernel/gdt.o	\
				$(ARCH_DIR)/kernel/idt.o	\
				$(ARCH_DIR)/kernel/main.o


# 编译 GCC flags
C_FLAGS := -fno-pic -m32 -fno-stack-protector -fno-builtin


BIN_FILES := loader_bin kernel_bin
IMAGE_FILES := mount_image

.PHONY:all

all: $(BIN_FILES) $(IMAGE_FILES)
	nasm  $(BOOT_INCLUDE_DIR) $(BOOTFILE) 	-o $(BOOT_BIN)
	dd if=$(BOOT_BIN) of=$(IMAGE_OUT)/moos.img bs=512 count=1 conv=notrunc


# loader.bin 文件
loader_bin:
	nasm  $(BOOT_INCLUDE_DIR) $(LOADERFILE)  -o $(LOADER_BIN)

# kernel.bin 文件
kernel_bin: $(KERNEL_OBJ_FILES) $(ARCH_OBJ_FILES)
	nasm -f elf $(BOOT_INCLUDE_DIR) $(KERNEL_DIR)/asm/kernel.asm -o $(OUT_DIR)/kernel.o
	nasm -f elf $(BOOT_INCLUDE_DIR) $(KERNEL_DIR)/asm/kernel_lib32.asm -o $(OUT_DIR)/kernel_lib32.o
	nasm -f elf $(BOOT_INCLUDE_DIR) $(ARCH_DIR)/kernel/i386_lib32.asm -o $(OUT_DIR)/i386_lib32.o
	ld -melf_i386 -Tkernel.ld -s -o $(KERNEL_BIN) $(ARCH_OBJ_FILES) $(KERNEL_OBJ_FILES) $(OUT_DIR)/*.o

.c.o:
	gcc -c $(C_FLAGS) -m32 $(KERNEL_INCLUDE_DIR) -o $@ $^


# 挂载 fat12 复制文件
mount_image:
	sudo mount -o loop $(IMAGE_FILE) $(IMAGE_MOUNT_DIR)
	sudo rm $(IMAGE_MOUNT_DIR)/* -rf
	sudo cp $(LOADER_BIN) $(IMAGE_MOUNT_DIR) -rf
	sudo cp $(KERNEL_BIN) $(IMAGE_MOUNT_DIR) -rf
	sudo umount ./mnt


# 运行
run:
	bochs -q



mount:
	sudo mount -o loop $(IMAGE_FILE) $(IMAGE_MOUNT_DIR)

umount:
	sudo umount $(IMAGE_MOUNT_DIR)

clean:
	rm $(BIN_OUT)/* $(OUT_DIR)/*.o $(KERNEL_OBJ_FILES) $(ARCH_OBJ_FILES)
