set path=d:\Djgpp\bin
cd compress
make
del *.o
cd ..
copy compress\vmknl.z
