c:\SDK\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -c main.c
c:\SDK\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -mlong32 -c pg.c
c:\SDK\ps2dev\gcc\ee\bin\ee-gcc -march=r4000 -g -mgp32 -c -xassembler -O -o startup.o startup.s
c:\SDK\ps2dev\gcc\ee\bin\ee-ld -O0 startup.o main.o pg.o -M -Ttext 8900000 -q -o out > SimpleTurn.map
c:\SDK\ps2dev\share\outpatch
c:\SDK\ps2dev\share\elf2pbp outp "Simple TURN"
pause
copy /y EBOOT.PBP E:\PSP\GAME\SIMPLETURN\
