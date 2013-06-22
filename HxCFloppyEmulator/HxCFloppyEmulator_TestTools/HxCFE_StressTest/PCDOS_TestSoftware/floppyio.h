void trackseek     (unsigned char drive,unsigned char track,unsigned char head);
void calibratedrive(unsigned char drive);
int  write_sector (unsigned char deleted,unsigned index,unsigned char drive,unsigned char head, unsigned char track,unsigned char sector,unsigned char size,unsigned char density,unsigned char precomp,int rate);
int  read_sector  (unsigned char deleted,unsigned index,unsigned char drive,unsigned char head, unsigned char track,unsigned char sector,unsigned char size,unsigned char density,int rate);
int  chs_biosdisk (int cmd, int drive, int head, int track,int sector, int nsects, void *buf);
void init_floppyio(void);
void fdc_specify(unsigned char t);
void reset_drive(unsigned char drive);
int fd_result(int sensei);
