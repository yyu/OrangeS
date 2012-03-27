/*************************************************************************//**
 *****************************************************************************
 * @file   include/sys/identify.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

#ifndef	_ORANGES_IDENTIFY_H_
#define	_ORANGES_IDENTIFY_H_

struct identify_dev_info {
	int offset;
	enum {
		BIN,
		DEC,
		HEX,
		STR,		/* sting */
		FOO		/* whatever we can just ingnore */
	} type;
	int size;
	char description[128];
};

struct identify_dev_info info_desc[] = {{ 0, BIN, 2, "Drive classification"},
				 /*       -- 15(=0): 1=ATAPI device, 0=ATA device
					  -- 14(=0): 1=format speed tolerance gap required
					  -- 13(=0): 1=track offset option available
					  -- 12(=0): 1=data strobe offset option available
					  -- 11(=0): 1=rotational speed tolerance > 0.5%
					  -- 10(=1): 1=disk transfer rate > 10 Mbps
					  --  9(=0): 1=disk transfer rate > 5 Mbps but <= 10 Mbps
					  --  8(=0): 1=disk transfer rate <= 5 Mbps
					  --  7(=0): 1=removable cartridge device
					  --  6(=1): 1=fixed device
					  --  5(=0): 1=spindle motor control option implemented
					  --  4(=1): 1=head switch time > 15 us
					  --  3(=1): 1=not MFM encoded
					  --  2(=0): 1=soft sectored
					  --  1(=1): 1=hard sectored
					  --  0(=0): Reserved */
				 { 1, DEC, 2, "Nr of cylinders in default translate mode"},
				 { 2, DEC, 2, "Nr of removable cylinders"},
				 { 3, DEC, 2, "Nr of heads in default translate mode"},
				 { 4, HEX, 2, "Reserved"},
				 { 5, HEX, 2, "Reserved"},
				 { 6, DEC, 2, "Nr of sectors per track in default translate mode"},
				 { 7, DEC, 2, "Nr of bytes of sector gap"},
				 { 8, DEC, 2, "Nr of bytes in sync field"},
				 { 9, HEX, 2, "Reserved"},
				 {10, STR, 20, "Serial number in ASCII (0 = not specified)"},
				 {20, HEX, 2, "Controller type"},
				 /*       -- 0003: dual ported, multiple sector buffer with look-ahead read */
				 {21, DEC, 2, "Buffer size in 512byte increments (=418KB)"},
				 {22, DEC, 2, "Nr of ECC bytes as currently selected via the set features command"},
				 {23, STR, 8, "Microcode version in ASCII"},
				 {27, STR, 40, "Model number in ASCII"},
				 {47, BIN, 2, "Maximum number of sectors that can be transferred per interrupt on Read and Write Multiple commands"},
				 /*       -- bit 15~8(=80h)
					  -- bit  7~0 : Maximum number of sectors that can be transferred per interrupt. */
				 {48, HEX, 2, "Capable of double word I/O"},
				 /*       -- '0000'= cannot perform */
				 {49, BIN, 2, "Capabilities, bit assignments"},
				 /*       -- bit 15~14(=0) Reserved
					  -- bit    13(=0) Standby timer value are vendor specific
					  -- bit    12(=0) Reserved
					  -- bit    11(=1) IORDY Supported
					  -- bit    10(=1) IORDY can be disabled
					  -- bit     9(=1) 1=LBA Supported
					  -- bit     8(=1) 1=DMA Supported
					  -- bit   7~0(=0) Reserv */
				 {50, BIN, 2, "Capabilities"},
				 /*       --   15(=0) 0=the contents of word 50 are valid
					  --   14(=1) 1=the contents of word 50 are valid
					  -- 13~1(=0) Reserved
					  --    0(=0) 1=the device has a minimum Standby timer value that is device specific */
				 {51, HEX, 2, "PIO data transfer cycle timing mode"},
				 {52, HEX, 2, "DMA data transfer cycle timing mode. Refer Word 62 and 63"},
				 {53, BIN, 2, "Validity flag of the word"},
				 /*       -- 15~3(=0) Reserved
					  --    2(=1) 1=Word 88 is Valid
					  --    1(=1) 1=Word 64~70 are Valid
					  --    0(=1) 1=Word 54~58 are Valid */
				 {54, DEC, 2, "Nr of current cylinders"},
				 {55, DEC, 2, "Nr of current heads"},
				 {56, DEC, 2, "Nr of current sectors per track"},
				 {57, DEC, 4, "Current capacity in sectors"},
				 /*       --  Word 57 specifies the low word of the capacity */
				 {59, BIN, 2, "Current Multiple setting. bit assignments"},
				 /*       -- 15~9(=0) Reserved
					  --    8     1= Multiple Sector Setting is Valid
					  --  7~0     xxh = Current setting for number of sector */
				 {60, DEC, 4, "Total Nr of User Addressable Sectors"},
				 /*       -- Word 60 specifies the low word of the number */
				 {62, HEX, 2, "Reserved"},
				 {63, BIN, 2, "Multiword DMA Transfer Capability"},
				 /*       -- 15~11(=0) Reserved
					  --    10     1=Multiword DMA mode 2 is selected
					  --     9     1=Multiword DMA mode 1 is selected
					  --     8     1=Multiword DMA mode 0 is selected
					  --   7~0(=7) Multi word DMA transfer modes supported (support mode 0,1 and 2) */
				 {64, BIN, 2, "Flow Control PIO Transfer Modes Supported"},
				 /*       -- 15~8(=0) Reserved
					  -- 7~0(=3) Advanced PIO Transfer Modes Supported
					  '11' = PIO Mode 3 and 4 Supported */
				 {65, HEX, 2, "Minimum Multiword DMA Transfer Cycle Time Per Word"},
				 /*       -- 15~0(=78) Cycle time in nanoseconds (120ns, 16.6MB/s) */
				 {66, HEX, 2, "Manufacturer's Recommended Multiword DMA Transfer Cycle Time"},
				 /*       -- 15~0(=78) Cycle time in nanoseconds (120ns, 16.6MB/s) */
				 {67, HEX, 2, "Minimum PIO Transfer Cycle Time Without Flow Control"},
				 /*       -- 15~0(=F0) Cycle time in nanoseconds (240ns, 8.3MB/s) */
				 {68, HEX, 2, "Minimum PIO Transfer Cycle Time With IORDY Flow Control"},
				 /*       -- 15~0(=78) Cycle time in nanoseconds (120ns, 16.6MB/s) */
				 {69, HEX, 22, "Reserved"},
				 {80, HEX, 2, "Major version number"},
				 /*       -- 15~0(=1E) ATA-1, ATA-2, ATA-3 and ATA/ATAPI-4 */
				 {81, HEX, 2, "Minor version number"},
				 /*       -- 15~0(=17) ATA/ATAPI-4 T13 1153D revision 17 */
				 {82, BIN, 2, "Command set supported"},
				 /*       -- 15(=0) Reserved
					  -- 14(=1) 1=NOP command supported
					  -- 13(=1) 1=READ BUFFER command supported
					  -- 12(=1) 1=WRITE BUFFER command supported
					  -- 11(=0) Reserved
					  -- 10(=1) 1=Host Protected Area Feature Set supported
					  --  9(=0) 1=DEVICE RESET command supported
					  --  8(=0) 1=SERVICE interrupt supported
					  --  7(=0) 1=release interrupt supported
					  --  6(=1) 1=look-ahead supported
					  --  5(=1) 1=write cache supported
					  --  4(=0) 1=supports PACKET Command Feature Set
					  --  3(=1) 1=supports Power Management Feature Set
					  --  2(=0) 1=supports Removable Media Feature Set
					  --  1(=1) 1=supports Security Feature Set
					  --  0(=1) 1=supports S.M.A.R.T Feature Set */
				 {83, BIN, 2, "Command set supported"},
				 /*       --   15(=0) Always
					  --   14(=1) Always
					  -- 13~8(=0) Reserved
					  --    7(=1) 1=Address Offset feature supported
					  --  6~5(=0) Reserved
					  --    4(=0) 1=Removable Media Status Notification
					  Feature Set supported
					  --    3(=1) 1=Advanced Power Management
					  Feature Set supported
					  --    2(=0) 1=CFA Feature Set supported
					  --    1(=0) 1=READ/WRITE DMA QUEUED supported
					  --    0(=0) 1=DOWNLOAD MICROCODE command supported */
				 {84, BIN, 2, "Command set/feature supported extension"},
				 /*       --   15(=0) Always
					  --   14(=1) Always
					  -- 13~0(=0) Reserved */
				 {85, BIN, 2, "Command set/feature enabled"},
				 /*       -- 15(=1) Reserved
					  -- 14(=1) 1=NOP command supported
					  -- 13(=1) 1=READ BUFFER command supported
					  -- 12(=1) 1=WRITE BUFFER command supported
					  -- 11(=0) Reserved
					  -- 10(=1) 1=Host Protected Area Feature Set supported
					  --  9(=0) 1=DEVICE RESET command supported
					  --  8(=0) 1=SERVICE interrupt enabled
					  --  7(=0) 1=release interrupt enabled
					  --  6(=X) 1=look-ahead enabled
					  --  5(=X) 1=write cache enabled
					  --  4(=0) 1=supports PACKET Command Feature Set
					  --  3(=X) 1=supports Power Management Feature Set
					  --  2(=0) 1=supports Removable Media Feature Set
					  --  1(=X) 1=Security Feature Set enabled
					  --  0(=X) 1=S.M.A.R.T Feature Set enabled */
				 {86, BIN, 2, "Command set/feature enabled"},
				 /*       -- 15~8(=0) Reserved
					  --    7(=X) 1=Address Offset mode enabled
					  --  6~5(=0) Reserved
					  --    4(=0) 1=Removable Media Status Notification
					  --            Feature Set enabled
					  --    3(=X) 1=Advanced Poser Management
					  --            Feature Set enabled
					  --    2(=0) 1=CFA Feature Set supported
					  --    1(=0) 1=READ/WRITE DMA QUEUED command supported
					  --    0(=0) 1=DOWNLOAD MICROCODE command supported */
				 {87, BIN, 2, "Command set/feature enabled"},
				 /*       --   15(=0) Always
					  --   14(=1) Always
					  -- 13~0(=0) Reserved */
				 {88, BIN, 2, "Ultra DMA Transfer mode (mode 4 supported)"},
				 /*       -- 15~13(=0) Reserved
					  --    12(=X) 1=UltraDMA mode 4 is selected
					  --    11(=X) 1=UltraDMA mode 3 is selected
					  --    10(=X) 1=UltraDMA mode 2 is selected
					  --     9(=X) 1=UltraDMA mode 1 is selected
					  --     8(=X) 1=UltraDMA mode 0 is selected
					  --   7~5(=0) Reserved
					  --     4(=1) 1=UltraDMA mode 4 is supported
					  --     3(=1) 1=UltraDMA mode 3 is supported
					  --     2(=1) 1=UltraDMA mode 2 is supported
					  --     1(=1) 1=UltraDMA mode 1 is supported
					  --     0(=1) 1=UltraDMA mode 0 is supported */
				 {89, HEX, 2, "Time required for security erase unit completion"},
				 /*       -- Time= value(XXXXh)*2 [minutes]
					  -- Refer to 12.22, ]Security Erase Unit (F4h)≠ on page 142 */
				 {90, HEX, 2, "Time required for Enhanced security erase completion"},
				 /*       -- 0000 : Not supported */
				 {91, BIN, 2, "Current Advanced Power Management level"},
				 /*       -- 15~8(=40h) Reserved
					  --  7~0(=X) Currect Advanced Power Management level se
					              by Set Features Command (01h to FEh) */
				 {92, HEX, 2, "Current Master Password Revision Codes"},
				 {93, HEX, 2, "Device detected result"},
				 /*       -- 15~14(=0) Reserved
					  -- 13(=X) 1=Device detected CBLID- above ViH
					  -- 0=Device detected CBLID- below ViL
					  -- 12~0(=0) Reserved */
				 {94, HEX, 68, "Reserved"},
				 {128, BIN, 2, "Security Mode Feature. Bit assignments"},
				 /*       -- 15~9(=0) Reserved
					  --    8(=X) Security Level 1= Maximum, 0= High
					  --  7~6(=0) Reserved
					  --    5(=0) 1=Enhanced security erase supported
					  --    4(=X) 1=Security count expired
					  --    3(=X) 1=Security Frozen
					  --    2(=X) 1=Security locked
					  --    1(=X) 1=Security enabled
					  --    0(=X) 1=Security supported */
				 {129, BIN, 2, "Current Set Feature Option. Bit assignments"},
				 /*       -- 15~4(=0) Reserved
					  --    3(=X) 1=Auto reassign enabled
					  --    2(=X) 1=Reverting enabled
					  --    1(=X) 1=Read Look-ahead enabled
					  --    0(=X) 1=Write Cache enabled */
				 {130, HEX, 2, "Reserved"},
				 {131, BIN, 2, "Initial Power Mode Selection. Bit assignments"},
				 /*       -- 15~2(=0) Reserved
					  --    1(=1) Always
					  --    0(=X) Initial Power Mode 1= Standby, 0= Idle */
				 {132, FOO, 248, "Reserved"}};
/*
  Identify Device Info - Bochs
  ----------------------------
  |*[  0] Drive classification: __0000 0000 0100 0000 __..................................bit6(=1): 1=fixed device
  |*[  1] Nr of cylinders in default translate mode: __60.__..............................60
  | [  2] Nr of removable cylinders: __0.__
  |*[  3] Nr of heads in default translate mode: __16.__..................................16
  | [  4] Reserved: __7E00H __
  | [  5] Reserved: __200H __
  |*[  6] Nr of sectors per track in default translate mode: __63.__......................63
  | [  7] Nr of bytes of sector gap: __0.__
  | [  8] Nr of bytes in sync field: __0.__
  | [  9] Reserved: __0H __
  |*[ 10] Serial number in ASCII (0 = not specified): __BXHD00011           __............BXHD00011（See `bochs-2.3/iodev/harddrv.cc'）
  | [ 20] Controller type: __3H __
  | [ 21] Buffer size in 512byte increments (=418KB): __512.__
  | [ 22] Nr of ECC bytes as currently selected via the set features command: __4.__
  |*[ 23] Microcode version in ASCII: ____................................................<NULL>
  |*[ 27] Model number in ASCII: __Generic 1234                            __.............Generic 1234（See `man bochsrc'）
  | [ 47] Maximum number of sectors that can be transferred per interrupt on
  |       Read and Write Multiple commands: __0000 0000 0001 0000 __
  | [ 48] Capable of double word I/O: __1H __
  |*[ 49] Capabilities, bit assignments: __0000 0011 0000 0000 __.........................bit9(=1): LBA Supported; bit8(=1): DMA Supported
  | [ 50] Capabilities: __0000 0000 0000 0000 __
  | [ 51] PIO data transfer cycle timing mode: __200H __
  | [ 52] DMA data transfer cycle timing mode. Refer Word 62 and 63: __200H __
  |*[ 53] Validity flag of the word: __0000 0000 0000 0000 __.............................NO IDEA
  |*[ 54] Nr of current cylinders: __60.__................................................60
  |*[ 55] Nr of current heads: __16.__....................................................16
  |*[ 56] Nr of current sectors per track: __63.__........................................63
  |*[ 57] Current capacity in sectors: __60480.__.........................................60480（== 60*16*63）
  | [ 59] Current Multiple setting. bit assignments: __0000 0000 0000 0000 __
  |*[ 60] Total Nr of User Addressable Sectors: __60480.__................................60480
  | [ 62] Reserved: __0H __
  | [ 63] Multiword DMA Transfer Capability: __0000 0000 0000 0111 __
  | [ 64] Flow Control PIO Transfer Modes Supported: __0000 0000 0000 0000 __
  | [ 65] Minimum Multiword DMA Transfer Cycle Time Per Word: __0H __
  | [ 66] Manufacturer's Recommended Multiword DMA Transfer Cycle Time: __0H __
  | [ 67] Minimum PIO Transfer Cycle Time Without Flow Control: __0H __
  | [ 68] Minimum PIO Transfer Cycle Time With IORDY Flow Control: __0H __
  | [ 69] Reserved: __0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H __
  | [ 80] Major version number: __EH __
  | [ 81] Minor version number: __0H __
  |*[ 82] Command set supported: __0100 0000 0000 0000 __.................................bit14(=1): NOP command supported
  |*[ 83] Command set supported: __0100 0000 0000 0000 __.................................bit14(=1): Always
  | [ 84] Command set/feature supported extension: __0100 0000 0000 0000 __
  |*[ 85] Command set/feature enabled: __0100 0000 0000 0000 __...........................bit14(=1): NOP command supported
  |*[ 86] Command set/feature enabled: __0000 0000 0000 0000 __...........................All zeroes.
  | [ 87] Command set/feature enabled: __0100 0000 0000 0000 __
  | [ 88] Ultra DMA Transfer mode (mode 4 supported): __0000 0000 0000 0000 __
  | [ 89] Time required for security erase unit completion: __0H __
  | [ 90] Time required for Enhanced security erase completion: __0H __
  | [ 91] Current Advanced Power Management level: __0000 0000 0000 0000 __
  | [ 92] Current Master Password Revision Codes: __0H __
  | [ 93] Device detected result: __0H __
  | [ 94] Reserved: __0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H
  |                   0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H 0H __
  | [128] Security Mode Feature. Bit assignments: __0000 0000 0000 0000 __
  | [129] Current Set Feature Option. Bit assignments: __0000 0000 0000 0000 __
  | [130] Reserved: __0H __
  | [131] Initial Power Mode Selection. Bit assignments: __0000 0000 0000 0000 __
  | [132] Reserved: __WHATEVER__
*/

#endif /* _ORANGES_IDENTIFY_H_ */
