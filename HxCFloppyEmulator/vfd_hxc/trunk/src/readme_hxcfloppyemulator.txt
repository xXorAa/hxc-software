-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-----------H----H--X----X-----CCCCC----22222----0000-----0000------11----------
----------H----H----X-X-----C--------------2---0----0---0----0--1--1-----------
---------HHHHHH-----X------C----------22222---0----0---0----0-----1------------
--------H----H----X--X----C----------2-------0----0---0----0-----1-------------
-------H----H---X-----X---CCCCC-----222222----0000-----0000----1111------------
-------------------------------------------------------------------------------
----------------------------------------------------- http://hxc2001.free.fr --
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
            Virtual Floppy Drive 2.1 For SD HxC Floppy Emulator
-------------------------------------------------------------------------------
                             Release 12/11/2011
-------------------------------------------------------------------------------

Virtual Floppy Drive is a virtual floppy drive for Windows developed by Ken Kato.
Original sourceforge project : http://vfd.sourceforge.net/

This version was modified by Jean-François DEL NERO to directly handle HFE files 
images for the SD HxC Floppy Emulator.
HxC Floppy Emulator project page : http://hxc2001.free.fr/floppy_drive_emulator/

64bits driver version recompiling by Igor Levicki : www.levicki.net

     ---- Tested on Windows XP SP3 (32Bits) and Windows 7 (64Bits). ----
 
>>>> How to install VFD for the SD HxC Floppy Emulator (*.hfe support) ? <<<<

-> Start vfdwin.exe
-> Driver -> Browse -> Select the file vfd.sys present in the folder (vfd_x64.sys for 64bits systems) -> Intall -> Start
-> Association -> Check .hfe -> Apply.

The driver is now ready.

vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
Note for Windows 7 & Vista users: 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since Windows 7 & Vista systems need signed drivers, you need to disable the driver signature checking.
To do this you can use dseo and Enable the "Test Mode". 
This software is in the win7_vista folder, or can be downloaded on this website:
http://www.ngohq.com/home.php?page=dseo


>>>> How to open a HFE file ? <<<<

-> you have just to double click on it, the A: virtual disk should give you access to the files present in the HFE image.

Note : Any change are writing back to the hfe file when the disk is unmounted.

To do this vfdwin.exe -> Drive 0 -> Close

OR 

Reopen the same hfe file.

>>>> How to create a new HFE file image ?<<<<

-> Start vfdwin.exe
-> Open/Create
-> Choose the capacity (Media Type)
-> Create
-> Save
-> In target file/browse : name your file with a .hfe extension (example: myfloppy.hfe)
-> Save.

It's done!


Jeff/HxC2001 
12/11/2011
