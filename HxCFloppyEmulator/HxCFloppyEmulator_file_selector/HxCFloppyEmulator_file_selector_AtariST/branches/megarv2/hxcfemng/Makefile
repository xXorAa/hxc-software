
##########################
# You must fill ARCH
# and C, CFLAGS, LFLAGS
# before compiling !
#########################

# Part 1 : Arch
# Arch is the system used to build the project.
# Supplied arch : linux-i386, linux-amd64, cygwin
ARCH             = cygwin




GCCBIN           = m68k-atari-mint-gcc
GCCOPT           = -Os -Ifat32 -I.
GCLOPT           = -Os -nostdlib -Wl,-Map,symbols.map -Wl,--traditional-format -fomit-frame-pointer -m68000

VASMBIN          = ../build-tools/vbcc/bin-$(ARCH)/vasmm68k_mot
VASMOPT          = -Faout -showopt -m68000 -spaces -quiet

# GCC : PRG size about 44054 bytes
C                = $(GCCBIN)
CFLAGS           = $(GCCOPT)

#LFLAGS           = $(GCLOPT) -s    # STRIP SYMBOLS
LFLAGS           = $(GCLOPT)  	    # KEEP SYMBOLS (debug)

EXEC             = HXCFEMNG.PRG


.PHONY: all clean push-mina push-gbo-dsi push-gbo-pc

all: $(EXEC)

clean:
	rm -rf $(EXEC) *.o *.asm fat32/*.o fat32/*.asm libc/*.o libc/*.asm libc/snprintf/*.o symbols.map

push-mina: $(EXEC)
	rsync --perms $< Gilles@mina.openvpn:/cygdrive/c/data/dropbox/emul/atari/hd/c/tmp/

push-gbo-dsi: $(EXEC)
	rsync --perms $< gbouthen@gbo-dsi.openvpn:/cygdrive/c/data/dropbox/emul/atari/hd/c/tmp/

push-gbo-pc: $(EXEC)
	rsync --perms $< gbouthenot@gbo-pc.openvpn:/cygdrive/c/data/dropbox/emul/atari/hd/c/tmp/


$(EXEC):          libc/mStartupAsmC.o libc/mStartup.o quickersort.o fectrl.o filelist.o dir.o gui_utils.o atari_hw.o fat32/fat_access.o fat32/fat_filelib.o fat32/fat_misc.o fat32/fat_string.o fat32/fat_table.o fat32/fat_write.o fat32/fat_cache.o libc/snprintf/snprintf.o
	$(C)          $(LFLAGS)           $^ -o $@

atari_hw.o:       atari_hw.c       atari_hw.h keysfunc_defs.h keys_defs.h
	$(C)          $(CFLAGS)        -c $< -o $@

dir.o:            dir.c            filelist.h fat32/fat_access.h cfg_file.h fat32/fat_defs.h fat32/fat_types.h compiler_helper.h atari_hw.h fat32/fat_opts.h conf.h atari_hw.h
	$(C)          $(CFLAGS)        -c $< -o $@

gui_utils.o:      gui_utils.c      gui_utils.h atari_hw.h conf.h libc/snprintf/snprintf.h graphx/data_bmp_hxc2001logo_bmp.h graphx/data_bmp_font_bmp.h graphx/data_bmp_font8x8_bmp.h graphx/data_bmp_sdhxcfelogo_bmp.h
	$(C)          $(CFLAGS)        -c $< -o $@

fectrl.o:         fectrl.c         keysfunc_defs.h gui_utils.h conf.h cfg_file.h hxcfeda.h atari_hw.h fat32/fat_opts.h fat32/fat_misc.h fat32/fat_types.h fat32/fat_defs.h fat32/fat_filelib.h fat32/fat_access.h compiler_helper.h filelist.h dir.h
	$(C)          $(CFLAGS)        -c $< -o $@

filelist.o:       filelist.c       fat32/fat_access.h atari_hw.h fat32/fat_defs.h fat32/fat_types.h compiler_helper.h fat32/fat_opts.h hxcfeda.h quickersort.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_misc.o: fat32/fat_misc.c fat32/fat_misc.h fat32/fat_defs.h compiler_helper.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_filelib.o: fat32/fat_filelib.c fat32/fat_filelib.h fat32/fat_access.h fat32/fat_defs.h compiler_helper.h fat32/fat_types.h fat32/fat_opts.h fat32/fat_table.h fat32/fat_misc.h fat32/fat_write.h fat32/fat_string.h fat32/fat_cache.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_access.o: fat32/fat_access.c   fat32/fat_access.h fat32/fat_defs.h compiler_helper.h fat32/fat_types.h fat32/fat_opts.h fat32/fat_table.h fat32/fat_write.h fat32/fat_misc.h fat32/fat_string.h fat32/fat_cache.h fat32/fat_filelib.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_string.o: fat32/fat_string.c   fat32/fat_string.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_table.o: fat32/fat_table.c     fat32/fat_opts.h fat32/fat_misc.h fat32/fat_defs.h compiler_helper.h fat32/fat_types.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_write.o: fat32/fat_write.c     fat32/fat_write.h fat32/fat_access.h fat32/fat_defs.h compiler_helper.h fat32/fat_types.h fat32/fat_opts.h fat32/fat_table.h fat32/fat_misc.h fat32/fat_string.h
	$(C)          $(CFLAGS)        -c $< -o $@

fat32/fat_cache.o: fat32/fat_cache.c     fat32/fat_cache.h
	$(C)          $(CFLAGS)        -c $< -o $@

libc/mStartup.o: libc/mStartup.c
	$(C)          $(CFLAGS)        -c $< -o $@

libc/snprintf/snprintf.o: libc/snprintf/snprintf.c libc/snprintf/snprintf.h
	$(C)          $(CFLAGS)        -c $< -o $@

libc/mStartupAsmC.o: libc/mStartupAsmC.s
	$(VASMBIN)    $(VASMOPT)          $< -o $@

quickersort.o: quickersort.s
	$(VASMBIN)    $(VASMOPT)          $< -o $@
