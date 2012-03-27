/*************************************************************************//**
 *****************************************************************************
 * @file   hd.c
 * @brief  Hard disk (winchester) driver.
 * The `device nr' in this file means minor device nr.
 * @author Forrest Y. Yu
 * @date   2005~2008
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "hd.h"
#include "identify.h"

PRIVATE void	init_hd			();
PRIVATE void	hd_open			(int device);
PRIVATE void	hd_close		(int device);
PRIVATE void	hd_rdwt			(MESSAGE * p);
PRIVATE void	hd_ioctl		(MESSAGE * p);
PRIVATE void	hd_cmd_out		(struct hd_cmd* cmd);
PRIVATE void	get_part_table		(int drive, int sect_nr, struct part_ent * entry);
PRIVATE void	partition		(int device, int style);
PRIVATE void	print_hdinfo		(struct hd_info * hdi);
PRIVATE int	waitfor			(int mask, int val, int timeout);
PRIVATE void	interrupt_wait		();
PRIVATE	void	hd_identify		(int drive);
PRIVATE void	print_identify_info	(u16* hdinfo);

PRIVATE	u8		hd_status;
PRIVATE	u8		hdbuf[SECTOR_SIZE * 2];
PRIVATE	struct hd_info	hd_info[1];

#define	DRV_OF_DEV(dev) (dev <= MAX_PRIM ? \
			 dev / NR_PRIM_PER_DRIVE : \
			 (dev - MINOR_hd1a) / NR_SUB_PER_DRIVE)

/*****************************************************************************
 *                                task_hd
 *****************************************************************************/
/**
 * Main loop of HD driver.
 * 
 *****************************************************************************/
PUBLIC void task_hd()
{
	MESSAGE msg;

	init_hd();

	while (1) {
		glitter(40, 10);

		send_recv(RECEIVE, ANY, &msg);

		int src = msg.source;

		switch (msg.type) {
		case DEV_OPEN:
			hd_open(msg.DEVICE);
			break;

		case DEV_CLOSE:
			hd_close(msg.DEVICE);
			break;

		case DEV_READ:
		case DEV_WRITE:
			hd_rdwt(&msg);
			break;

		case DEV_IOCTL:
			hd_ioctl(&msg);
			break;

		default:
			dump_msg("HD driver::unknown msg", &msg);
			spin("FS::main_loop (invalid msg.type)");
			break;
		}

		send_recv(SEND, src, &msg);
	}
}

/*****************************************************************************
 *                                init_hd
 *****************************************************************************/
/**
 * <Ring 1> Check hard drive, set IRQ handler, enable IRQ and initialize data
 *          structures.
 *****************************************************************************/
PRIVATE void init_hd()
{
	int i;

	/* Get the number of drives from the BIOS data area */
	u8 * pNrDrives = (u8*)(0x475);
	printl("{HD} NrDrives:%d.\n", *pNrDrives);
	assert(*pNrDrives);

	put_irq_handler(AT_WINI_IRQ, hd_handler);
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);

	for (i = 0; i < (sizeof(hd_info) / sizeof(hd_info[0])); i++)
		memset(&hd_info[i], 0, sizeof(hd_info[0]));
	hd_info[0].open_cnt = 0;
}

/*****************************************************************************
 *                                hd_open
 *****************************************************************************/
/**
 * <Ring 1> This routine handles DEV_OPEN message. It identify the drive
 * of the given device and read the partition table of the drive if it
 * has not been read.
 * 
 * @param device The device to be opened.
 *****************************************************************************/
PRIVATE void hd_open(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0);	/* only one drive */

	hd_identify(drive);

	if (hd_info[drive].open_cnt++ == 0) {
		partition(drive * (NR_PART_PER_DRIVE + 1), P_PRIMARY);
		print_hdinfo(&hd_info[drive]);
	}
}

/*****************************************************************************
 *                                hd_close
 *****************************************************************************/
/**
 * <Ring 1> This routine handles DEV_CLOSE message. 
 * 
 * @param device The device to be opened.
 *****************************************************************************/
PRIVATE void hd_close(int device)
{
	int drive = DRV_OF_DEV(device);
	assert(drive == 0);	/* only one drive */

	hd_info[drive].open_cnt--;
}

/*****************************************************************************
 *                                hd_rdwt
 *****************************************************************************/
/**
 * <Ring 1> This routine handles DEV_READ and DEV_WRITE message.
 * 
 * @param p Message ptr.
 *****************************************************************************/
PRIVATE void hd_rdwt(MESSAGE * p)
{
	int drive = DRV_OF_DEV(p->DEVICE);

	u64 pos = p->POSITION;
	assert((pos >> SECTOR_SIZE_SHIFT) < (1 << 31));

	assert(p->CNT <= MAX_IO_BYTES*SECTOR_SIZE);

	/**
	 * We only allow to R/W from a SECTOR boundary:
	 */
	assert((pos & 0x1FF) == 0);

	u32 sect_nr = (u32)(pos >> SECTOR_SIZE_SHIFT); /* pos / SECTOR_SIZE */
	int logidx = (p->DEVICE - MINOR_hd1a) % NR_SUB_PER_DRIVE;
	sect_nr += p->DEVICE < MAX_PRIM ?
		hd_info[drive].primary[p->DEVICE].base :
		hd_info[drive].logical[logidx].base;

	struct hd_cmd cmd;
	cmd.features	= 0;
	cmd.count	= ((p->CNT + SECTOR_SIZE - 1) / SECTOR_SIZE) % MAX_IO_BYTES;
	cmd.lba_low	= sect_nr & 0xFF;
	cmd.lba_mid	= (sect_nr >>  8) & 0xFF;
	cmd.lba_high	= (sect_nr >> 16) & 0xFF;
	cmd.device	= MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0xF);
	cmd.command	= (p->type == DEV_READ) ? ATA_READ : ATA_WRITE;
	hd_cmd_out(&cmd);

	int bytes_left = p->CNT;
	void * la = (void*)va2la(p->PROC_NR, p->BUF);

	while (bytes_left) {
		int bytes = min(SECTOR_SIZE, bytes_left);
		if (p->type == DEV_READ) {
			interrupt_wait();
			port_read(REG_DATA, hdbuf, SECTOR_SIZE);
			phys_copy(la, (void*)va2la(TASK_HD, hdbuf), bytes);
		}
		else {
			if (!waitfor(STATUS_DRQ, STATUS_DRQ, HD_TIMEOUT)) {
				panic("hd writing error. status reg: 0x%x.",
				      in_byte(REG_STATUS));
			}

			port_write(REG_DATA, la, bytes);
			interrupt_wait();
		}
		bytes_left -= SECTOR_SIZE;
		la += SECTOR_SIZE;
	}
}

/*****************************************************************************
 *                                hd_ioctl
 *****************************************************************************/
/**
 * <Ring 1> This routine handles the DEV_IOCTL message.
 * 
 * @param p  Ptr to the MESSAGE.
 *****************************************************************************/
PRIVATE void hd_ioctl(MESSAGE * p)
{
	int device = p->DEVICE;
	int drive = DRV_OF_DEV(device);

	struct hd_info * hdi = &hd_info[drive];

	if (p->REQUEST == DIOCTL_GET_GEO) {
		void * dst = va2la(p->PROC_NR, p->BUF);
		void * src = va2la(TASK_HD,
				   device < MAX_PRIM ?
				   &hdi->primary[device] :
				   &hdi->logical[(device - MINOR_hd1a) %
						NR_SUB_PER_DRIVE]);

		phys_copy(dst, src, sizeof(struct part_info));
	}
	else {
		assert(0);
	}
}

/*****************************************************************************
 *                                get_part_table
 *****************************************************************************/
/**
 * <Ring 1> Get a partition table of a drive.
 * 
 * @param drive   Drive nr (0 for the 1st disk, 1 for the 2nd, ...)n
 * @param sect_nr The sector at which the partition table is located.
 * @param entry   Ptr to part_ent struct.
 *****************************************************************************/
PRIVATE void get_part_table(int drive, int sect_nr, struct part_ent * entry)
{
	struct hd_cmd cmd;
	cmd.features	= 0;
	cmd.count	= 1;
	cmd.lba_low	= sect_nr & 0xFF;
	cmd.lba_mid	= (sect_nr >>  8) & 0xFF;
	cmd.lba_high	= (sect_nr >> 16) & 0xFF;
	cmd.device	= MAKE_DEVICE_REG(1, /* LBA mode*/
					  drive,
					  (sect_nr >> 24) & 0xF);
	cmd.command	= ATA_READ;
	hd_cmd_out(&cmd);
	interrupt_wait();

	port_read(REG_DATA, hdbuf, SECTOR_SIZE);
	memcpy(entry,
	       hdbuf + PARTITION_TABLE_OFFSET,
	       sizeof(struct part_ent) * NR_PART_PER_DRIVE);

	/* int i; */
	/* struct part_ent * pd = entry; */
	/* printm("ind, h, s, c, sysid, h, s, c, start, nr\n"); */
	/* for (i = 0; i < 4; i++) { */
	/* 	printm("%x, %x, %x, %x, %x, %x, %x, %x, %x, %x\n", */
	/* 	       pd->boot_ind, */
	/* 	       pd->start_head, */
	/* 	       pd->start_sector, */
	/* 	       pd->start_cyl, */
	/* 	       pd->sys_id, */
	/* 	       pd->end_head, */
	/* 	       pd->end_sector, */
	/* 	       pd->end_cyl, */
	/* 	       pd->start_sect, */
	/* 	       pd->nr_sects); */
	/* 	pd++; */
	/* } */

	/* printm("^^^^^^^^^^^^^^^^^^^^^^^^\n"); */
}

/*****************************************************************************
 *                                partition
 *****************************************************************************/
/**
 * <Ring 1> This routine is called when a device is opened. It reads the
 * partition table(s) and fills the hd_info struct.
 * 
 * @param device Device nr.
 * @param style  P_PRIMARY or P_EXTENDED.
 *****************************************************************************/
PRIVATE void partition(int device, int style)
{
	int i;
	int drive = DRV_OF_DEV(device);
	struct hd_info * hdi = &hd_info[drive];

	struct part_ent part_tbl[NR_SUB_PER_DRIVE];

	if (style == P_PRIMARY) {
		get_part_table(drive, drive, part_tbl);

		int nr_prim_parts = 0;
		for (i = 0; i < NR_PART_PER_DRIVE; i++) { /* 0~3 */
			if (part_tbl[i].sys_id == NO_PART) 
				continue;

			nr_prim_parts++;
			int dev_nr = i + 1;		  /* 1~4 */
			hdi->primary[dev_nr].base = part_tbl[i].start_sect;
			hdi->primary[dev_nr].size = part_tbl[i].nr_sects;

			if (part_tbl[i].sys_id == EXT_PART) /* extended */
				partition(device + dev_nr, P_EXTENDED);
		}
		assert(nr_prim_parts != 0);
	}
	else if (style == P_EXTENDED) {
		int j = device % NR_PRIM_PER_DRIVE; /* 1~4 */
		int ext_start_sect = hdi->primary[j].base;
		int s = ext_start_sect;
		int nr_1st_sub = (j - 1) * NR_SUB_PER_PART; /* 0/16/32/48 */

		for (i = 0; i < NR_SUB_PER_PART; i++) {
			int dev_nr = nr_1st_sub + i;/* 0~15/16~31/32~47/48~63 */

			get_part_table(drive, s, part_tbl);

			hdi->logical[dev_nr].base = s + part_tbl[0].start_sect;
			hdi->logical[dev_nr].size = part_tbl[0].nr_sects;

			s = ext_start_sect + part_tbl[1].start_sect;

			/* no more logical partitions
			   in this extended partition */
			if (part_tbl[1].sys_id == NO_PART)
				break;
		}
	}
	else {
		assert(0);
	}
}

/*****************************************************************************
 *                                print_hdinfo
 *****************************************************************************/
/**
 * <Ring 1> Print disk info.
 * 
 * @param hdi  Ptr to struct hd_info.
 *****************************************************************************/
PRIVATE void print_hdinfo(struct hd_info * hdi)
{
	/* printm("C:%d, H:%d, S:%d\n", hdi->cylinders, hdi->heads, hdi->sectors); */

	int i;
	for (i = 0; i < NR_PART_PER_DRIVE + 1; i++) {
		printm("%sPART_%d: base %d(0x%x), size %d(0x%x) (in sector)\n",
		       i == 0 ? " " : "     ",
		       i,
		       hdi->primary[i].base,
		       hdi->primary[i].base,
		       hdi->primary[i].size,
		       hdi->primary[i].size);
	}
	for (i = 0; i < NR_SUB_PER_DRIVE; i++) {
		if (hdi->logical[i].size == 0)
			continue;
		printm("         "
		       "%d: base %d(0x%x), size %d(0x%x) (in sector)\n",
		       i,
		       hdi->logical[i].base,
		       hdi->logical[i].base,
		       hdi->logical[i].size,
		       hdi->logical[i].size);
	}
}

/*****************************************************************************
 *                                hd_identify
 *****************************************************************************/
/**
 * <Ring 1> Get the disk information.
 * 
 * @param drive  Drive Nr.
 *****************************************************************************/
PRIVATE void hd_identify(int drive)
{
	struct hd_cmd cmd;
	cmd.device  = MAKE_DEVICE_REG(0, drive, 0);
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);

	print_identify_info((u16*)hdbuf);

	u16* hdinfo = (u16*)hdbuf;

	hd_info[drive].primary[0].base = 0;

	/* Total Nr of User Addressable Sectors */
	hd_info[drive].primary[0].size = ((int)hdinfo[61] << 16) + hdinfo[60];
}

/*****************************************************************************
 *                            print_identify_info
 *****************************************************************************/
/**
 * <Ring 1> Print the hdinfo retrieved via ATA_IDENTIFY command.
 * 
 * @param hdinfo  The buffer read from the disk i/o port.
 *****************************************************************************/
PRIVATE void print_identify_info(u16* hdinfo)
{
	int i, j, k, z;
	char tmp[32];

	printl("sizeof(info_desc):%d, sizeof(info_desc[0]):%d, nr:%d\n",
		sizeof(info_desc),
		sizeof(info_desc[0]),
		sizeof(info_desc) / sizeof(info_desc[0]));

	const int prntlist[] = {0,  /* Drive classification */
				10, /* SN */
				27, /* Model number */
				49  /* Capabilities */
	};
	const int n = sizeof(prntlist) / sizeof(prntlist[0]);

	for (i = 0; i < (sizeof(info_desc) / sizeof(info_desc[0])); i++) {
		for (j = 0; j < n; j++)
			if (info_desc[i].offset ==  prntlist[j])
				break;
		if (j == n) continue;

		printl("[%d] %s: __",
			info_desc[i].offset,
			info_desc[i].description);

		switch (info_desc[i].type) {
		case DEC:
			assert(info_desc[i].size <= 4);
			z = 0;
			for (j = 0; j < info_desc[i].size / 2; j++)
				z += hdinfo[info_desc[i].offset+j] << (j * 16);
			printl("%d.", z);
			break;
		case BIN:
			assert(info_desc[i].size <= 2);
			for (j = info_desc[i].size / 2 - 1;
			     j >= 0;
			     j--) { /* j : N-1 --> 0 */
				z = hdinfo[info_desc[i].offset+j];
				for (k = (sizeof(hdinfo[0]) * 8) - 1;
				     k >= 0;
				     k--) { /* k : 15 --> 0 (u16) */
					printl("%d", (z >> k) & 1);
					if ((k % 4) == 0)
						printx(" ");
				}
			}
			break;
		case HEX:
			for (j = 0; j < info_desc[i].size / 2; j++)
				printl("%xH ", hdinfo[info_desc[i].offset+j]);
			break;
		case STR:
			for (j = 0; j < info_desc[i].size / 2; j++) {
				z = hdinfo[info_desc[i].offset+j];
				tmp[0] = (z >> 8) & 0xFF;
				tmp[1] = z & 0xFF;
				tmp[2] = 0;
				printl(tmp);
			}
			break;
		case FOO:
			printl("WHATEVER");
			break;
		default:
			break;
		};
		printx("__\n");
	}
}

/*****************************************************************************
 *                                hd_cmd_out
 *****************************************************************************/
/**
 * <Ring 1> Output a command to HD controller.
 * 
 * @param cmd  The command struct ptr.
 *****************************************************************************/
PRIVATE void hd_cmd_out(struct hd_cmd* cmd)
{
	/**
	 * For all commands, the host must first check if BSY=1,
	 * and should proceed no further unless and until BSY=0
	 */
	if (!waitfor(STATUS_BSY, 0, HD_TIMEOUT))
		panic("hd error.");

	/* Activate the Interrupt Enable (nIEN) bit */
	out_byte(REG_DEV_CTRL, 0);
	/* Load required parameters in the Command Block Registers */
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR,  cmd->count);
	out_byte(REG_LBA_LOW,  cmd->lba_low);
	out_byte(REG_LBA_MID,  cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE,   cmd->device);
	/* Write the command code to the Command Register */
	out_byte(REG_CMD,     cmd->command);
}

/*****************************************************************************
 *                                interrupt_wait
 *****************************************************************************/
/**
 * <Ring 1> Wait until a disk interrupt occurs.
 * 
 *****************************************************************************/
PRIVATE void interrupt_wait()
{
	MESSAGE msg;
	send_recv(RECEIVE, INTERRUPT, &msg);
	/* hd_status = in_byte(REG_STATUS); */
	/* while (hd_status & STATUS_BSY) { */
	/* 	send_recv(RECEIVE, INTERRUPT, &msg); */
	/* } */

	/* if ((hd_status & (STATUS_BSY | STATUS_DRDY | STATUS_DWF | STATUS_ERR)) == STATUS_DRDY) { */
	/* 	; */
	/* } */
	/* else { */
	/* 	assert(0); */
	/* } */
}

/*****************************************************************************
 *                                waitfor
 *****************************************************************************/
/**
 * <Ring 1> Wait for a certain status.
 * 
 * @param mask    Status mask.
 * @param val     Required status.
 * @param timeout Timeout in milliseconds.
 * 
 * @return One if sucess, zero if timeout.
 *****************************************************************************/
PRIVATE int waitfor(int mask, int val, int timeout)
{
	int t = get_ticks();

	while(((get_ticks() - t) * 1000 / HZ) < timeout)
		if ((in_byte(REG_STATUS) & mask) == val)
			return 1;

	return 0;
}

/*****************************************************************************
 *                                hd_handler
 *****************************************************************************/
/**
 * <Ring 0> Interrupt handler.
 * 
 * @param irq  IRQ nr of the disk interrupt.
 *****************************************************************************/
PUBLIC void hd_handler(int irq)
{
	/*
	 * Interrupts are cleared when the host
	 *   - reads the Status Register,
	 *   - issues a reset, or
	 *   - writes to the Command Register.
	 */
	hd_status = in_byte(REG_STATUS);

	inform_int(TASK_HD);
}
