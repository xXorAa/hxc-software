#CC=i386-pc-linux-gcc
CC=m68k-atari-mint-gcc
CFLAGS=-O3 -I . -I ./fat32/
LDFLAGS=-lc -s   -Wl,-Map,foo.map
EXEC=HXCFEMNG.PRG

all: $(EXEC)
	m68k-atari-mint-strip -s $(EXEC)
	./upx  -9 $(EXEC)
	mv $(EXEC) "G:\testdk.fatst160\AUTO"
	cmd /c 'startfe.bat' &

HXCFEMNG.PRG:      fectrl.o gui_utils.o atari_hw.o fat_access.o fat_filelib.o fat_misc.o fat_string.o fat_table.o fat_write.o fat_cache.o
	$(CC) -o $@    $^ $(LDFLAGS)

fectrl.o: fectrl.c
	$(CC) -o $@ -c $< $(CFLAGS)

gui_utils.o: gui_utils.c
	$(CC) -o $@ -c $< $(CFLAGS)


fat_access.o: ./fat32/fat_access.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_filelib.o: ./fat32/fat_filelib.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_misc.o: ./fat32/fat_misc.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_string.o: ./fat32/fat_string.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_table.o: ./fat32/fat_table.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_write.o: ./fat32/fat_write.c
	$(CC) -o $@ -c $< $(CFLAGS)

fat_cache.o: ./fat32/fat_cache.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o


mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper
