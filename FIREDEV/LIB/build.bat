set path=D:\Dev-Cpp\bin
del *.o
make
objcopy -R .note -R .comment -S -O binary vmknl.o compress\vmknl.bin

cd  kernel
del *.o
cd..
cd  lib
del *.o
cd..
cd  driver
del *.o
cd..



