#define FDCBASEADR 0x3F0

//
// FDC Commands
//

#define FDC_CMD_READTRACK   0x02
#define FDC_CMD_SPECIFY     0x03        // Specify drive timings
#define FDC_CMD_CHECKDRIVESTATUS 0x04
#define FDC_CMD_WRITE       0x05        // Write data (+ MT,MFM)
#define FDC_CMD_READ        0x06        // Read data (+ MT,MFM,SK)
#define FDC_CMD_RECAL       0x07        // Recalibrate
#define FDC_CMD_SENSEI      0x08        // Sense interrupt status
#define FDC_CMD_WRITEDELETEDDATA 0x09   // Write data (+ MT,MFM)
#define FDC_CMD_FORMAT  0x0D            // Format track (+ MFM)
#define FDC_CMD_READDELETEDDATA 0x0C    // Read data (+ MT,MFM,SK)
#define FDC_CMD_SEEK    0x0F            // Seek track
#define FDC_CMD_VERSION 0x10            // FDC version

#define FDC_DOR    0x3F2        // Digital Output Register
#define FDC_MSR    0x3F4        // Main Status Register (input)
#define FDC_DRS    0x3F4        // Data Rate Select Register (output)
#define FDC_DATA   0x3F5        // Data Register
#define FDC_DIR    0x3F7        // Digital Input Register (input)
#define FDC_CCR    0x3F7        // Configuration Control Register

//
// Transfer mode
//

#define FD_MODE_READ    0
#define FD_MODE_WRITE   1

