BOOTFILE = ./src/boot/boot.asm
TARGET:=boot.bin

LOADER_BIN := loader.bin
LOADERFILE := ./src/boot/bootloader.asm

INCLUDE_DIR = \
			  ./include/asm/


.PHONY:$(TARGET)

$(TARGET):
	nasm -i $(INCLUDE_DIR) $(LOADERFILE) -o $(LOADER_BIN)
	nasm -i $(INCLUDE_DIR) $(BOOTFILE) -o $(TARGET)
	dd if=boot.bin of=moos.img bs=512 count=1 conv=notrunc


clean:
	rm $(TARGET) 4* 5*
