set path=d:\djgpp\bin


nasm -f aout kernel\head.s                -o  out\head.o
nasm -f aout driver\font.s                -o  out\font.o
nasm -f aout kernel\asm.s                 -o  out\asm.o
nasm -f aout kernel\system_call.s         -o  out\system_call.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/Main.c           -o out/Main.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/Page.c           -o out/Page.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/Traps.c          -o out/Traps.o
 
gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/Process.c        -o out/Process.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/Schedule.c       -o out/Schedule.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/SysCall.c        -o out/SysCall.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c kernel/MsgManger.c      -o out/MsgManger.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/GraphicDriver.c  -o out/GraphicDriver.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/Vbe.c            -o out/Vbe.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/timer.c          -o out/timer.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/keyboard.c       -o out/keyboard.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/mouse.c          -o out/mouse.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/floppy.c         -o out/floppy.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c driver/harddisk.c       -o out/harddisk.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c lib/vsprintf.c          -o out/vsprintf.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c lib/grub.c              -o out/grub.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c lib/Console.c           -o out/Console.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c lib/Queue.c             -o out/Queue.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c lib/GUI.c               -o out/GUI.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c mm/BootMem.c           -o out/BootMem.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c mm/VMalloc.c           -o out/VMalloc.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c mm/PageAlloc.c         -o out/PageAlloc.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c fs/Fat12.c             -o out/Fat12.o

gcc -Wall -O2 -g -nostdinc -fno-builtin -I ./include -c fs/Fat32.c             -o out/Fat32.o


cd out
make
objcopy -R .note -R .comment -S -O binary vmknl.o vmknl.bin
del *.o
cd..
copy out\vmknl.bin compress\ 



