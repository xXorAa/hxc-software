#include "rawfile_loader_window.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern "C"
{
	#include "hxc_floppy_emulator.h"
	#include "../usb_floppyemulator/usb_hxcfloppyemulator.h"
}

#include "loader.h"

extern HXCFLOPPYEMULATOR * flopemu;

void raw_loader_window_datachanged(Fl_Widget* w, void*)
{
	int totalsector,totalsize;
	int temp[256];
	rawfile_loader_window *rlw;
	Fl_Window *dw;

	dw=((Fl_Window*)(w->parent()));
	rlw=(rawfile_loader_window *)dw->user_data();

	totalsector=(int)(rlw->innum_nbtrack->value() * 	rlw->innum_sectorpertrack->value());
	if(rlw->chk_twosides->value())
		totalsector=totalsector*2;

	sprintf((char*)temp,"%d",totalsector);

	rlw->strout_totalsector->value((const char*)temp);
	totalsize=totalsector * (128<<rlw->choice_sectorsize->value());
	sprintf((char*)temp,"%d",totalsize);
	rlw->strout_totalsize->value((const char*)temp);
}

void raw_loader_window_bt_loadrawfile(Fl_Button* bt, void*)
{
	int totalsector,totalsize;
	int temp[256];
	rawfile_loader_window *rlw;
	Fl_Window *dw;

	dw=((Fl_Window*)(bt->parent()));
	rlw=(rawfile_loader_window *)dw->user_data();

	totalsector=(int)(rlw->innum_nbtrack->value() * 	rlw->innum_sectorpertrack->value());
	if(rlw->chk_twosides->value())
		totalsector=totalsector*2;

	sprintf((char*)temp,"%d",totalsector);

	rlw->strout_totalsector->value((const char*)temp);
	totalsize=totalsector * (128<<rlw->choice_sectorsize->value());
	sprintf((char*)temp,"%d",totalsize);
	rlw->strout_totalsize->value((const char*)temp);


}

void raw_loader_window_bt_createemptyfloppy(Fl_Button* bt, void*)
{
	int totalsector,totalsize;
	int temp[256];
	rawfile_loader_window *rlw;
	Fl_Window *dw;
	cfgrawfile rfc;

	dw=((Fl_Window*)(bt->parent()));
	rlw=(rawfile_loader_window *)dw->user_data();

	totalsector=(int)(rlw->innum_nbtrack->value() * 	rlw->innum_sectorpertrack->value());
	if(rlw->chk_twosides->value())
		totalsector=totalsector*2;

	sprintf((char*)temp,"%d",totalsector);

	rlw->strout_totalsector->value((const char*)temp);
	totalsize=totalsector * (128<<rlw->choice_sectorsize->value());
	sprintf((char*)temp,"%d",totalsize);
	rlw->strout_totalsize->value((const char*)temp);

	memset(&rfc,0,sizeof(rfc));
	rfc.autogap3=0xff;
	if(rlw->chk_twosides->value())
		rfc.sidecfg=2;
	else
		rfc.sidecfg=1;
	rfc.bitrate=(unsigned long)rlw->innum_bitrate->value();
	rfc.fillvalue=(unsigned char)rlw->numin_formatvalue->value();
	rfc.numberoftrack=(unsigned long)rlw->innum_nbtrack->value();
	rfc.gap3=(unsigned long)rlw->numin_gap3->value();
	rfc.firstidsector=(unsigned char)rlw->innum_sectoridstart->value();
	rfc.interleave=(unsigned char)rlw->numin_interleave->value();
	rfc.sectorpertrack=(unsigned char)rlw->innum_sectorpertrack->value();
	rfc.rpm=(unsigned long)rlw->innum_rpm->value();
	rfc.sectorsize=128<<(rlw->choice_sectorsize->value());
	rfc.skew=(unsigned char)rlw->numin_skew->value();

	loadrawfile(flopemu,&rfc);

	dw->hide();
}