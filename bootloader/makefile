
all: bootloader pacman image copy

#bootloader assembly generate bin
bootloader: bootloader.asm
	@nasm -f bin bootloader.asm -o bootloader.bin
	@echo Bootloader bin generated

#pacman assembly generate bin
pacman: pacman.asm 
	@nasm -f bin pacman.asm -o pacman.bin
	@echo pacman bin generated

#image to boot 
image:
	@dd if=/dev/zero of=pacman.img bs=1024 count=720
	@echo Disk image created

#copy all files to create a final boot image
copy: bootloader.bin pacman.img pacman.bin
	@sudo dd if=bootloader.bin of=pacman.img conv=notrunc
	@sudo dd if=pacman.bin of=pacman.img bs=512 seek=1 conv=notrunc
	@echo All copies finished
