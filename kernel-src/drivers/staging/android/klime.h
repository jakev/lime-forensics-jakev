/*
 * LiME - Linux Memory Extractor
 * Copyright (c) 2011-2013 Joe Sylve - 504ENSICS Labs
 *
 *
 * Author(s):
 * Joe Sylve       - joe.sylve@gmail.com, @jtsylve
 * Jake Valletta   - javallet@gmail.com, @jake_valletta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* This fork of LiME allows Android applications and programs
 * in userspace the ability to arbitrarily dump system memory using a
 * device driver. All credit for LiME functionality goes to Joe;
 * I simply created an API around it.
 *
 * A NOTE ON SECURITY:
 * This initial fork of LiME lacks any UAC or security model.  The driver
 * is accessible to userspace and applications.  Any application or
 * program can link against the LiME code and potentially dump memory.
 * This will be addressed in further releases.
 *
 * TL;DR - Don't install this on a production ROM (yet)!!!!!
 */
/* Added includes */
#include <linux/types.h>
#include <linux/ioctl.h>
/* End added */

/* From "lime.h" */
#ifndef __LIME_H_
#define __LIME_H_

#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/pfn.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/string.h>

#include <net/sock.h>
#include <net/tcp.h>

#define LIME_RAMSTR "System RAM"
#define LIME_MAX_FILENAME_SIZE 256
#define LIME_MAGIC 0x4C694D45 //LiME

#define LIME_MODE_RAW 0
#define LIME_MODE_LIME 1
#define LIME_MODE_PADDED 2

#define LIME_METHOD_UNKNOWN 0
#define LIME_METHOD_TCP 1
#define LIME_METHOD_DISK 2


#undef LIME_DEBUG
//#define LIME_DEBUG

#ifdef LIME_DEBUG
#define DBG(fmt, args...) do { printk("[LiME] "fmt"\n", ## args); } while (0)
#else
#define DBG(fmt, args...) do {} while(0)
#endif

//structures

typedef struct {
        unsigned int magic;
        unsigned int version;
        unsigned long long s_addr;
        unsigned long long e_addr;
        unsigned char reserved[8];
} __attribute__ ((__packed__)) lime_mem_range_header;
/* End "lime.h" */

/* Added */
#define LIME_DEVICE     "lime"
#define __LIMEIO        0xAF

#define LIME_GET_STATUS         _IO(__LIMEIO, 1) /* Get status of driver */
#define LIME_DUMP_TCP		_IO(__LIMEIO, 2) /* Dump memory to socket */
#define LIME_DUMP_DISK		_IO(__LIMEIO, 3) /* Dump memory to disk */

#define LIME_STATUS_READY       0x1
#define LIME_STATUS_BUSY        0x0

typedef struct {
	char file_name[LIME_MAX_FILENAME_SIZE];
	int mode;
	int dio;

} lime_dump_disk;

typedef struct {
	int port;
	int mode;
	int dio;

} lime_dump_tcp;
/* End added */

#endif //__LIME_H_

