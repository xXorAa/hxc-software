
typedef struct direct_access_status_sector_
{
        char DAHEADERSIGNATURE[8];
        char FIRMWAREVERSION[12];
        unsigned long lba_base;
        unsigned char cmd_cnt;
        unsigned char read_cnt;
        unsigned char write_cnt;
        unsigned char last_cmd_status;
		unsigned char write_locked;
		unsigned char keys_status;
		unsigned char sd_status;
		unsigned char SD_WP;
		unsigned char SD_CD;
		unsigned char number_of_sector;
		unsigned short current_index;		
} direct_access_status_sector ;

typedef struct direct_access_cmd_sector_
{
        char DAHEADERSIGNATURE[8];
        unsigned char cmd_code;
        unsigned char parameter_0;
        unsigned char parameter_1;
        unsigned char parameter_2;
        unsigned char parameter_3;
        unsigned char parameter_4;
        unsigned char parameter_5;
        unsigned char parameter_6;
        unsigned char parameter_7;
        unsigned char cmd_checksum;
}direct_access_cmd_sector  ;

#define LFN_MAX_SIZE 128

typedef struct DirectoryEntry_ {
        unsigned char name[12];
        unsigned char attributes;
        unsigned long firstCluster;
        unsigned long size;
        unsigned char longName[LFN_MAX_SIZE];   // boolean
}DirectoryEntry;

int GetCurrentIndex();
int SetIndex(unsigned short index);
