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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include "klime.h"

#include <asm/ioctls.h>

/* From main.c */
// This file
static int write_lime_header(struct resource *);
static int write_padding(size_t);
static int write_range(struct resource *);
static int write_vaddr(void *, size_t);
static int setup(void);
static void cleanup(void);
static int init(void);

// External
extern int write_vaddr_tcp(void *, size_t);
extern int setup_tcp(void);
extern void cleanup_tcp(void);

extern int write_vaddr_disk(void *, size_t);
extern int setup_disk(void);
extern void cleanup_disk(void);

static int mode = 0;
static int method = 0;
static char zero_page[PAGE_SIZE];

char * path = 0;
int dio = 1;
int port = 0;

extern struct resource iomem_resource;

static int init() {
        struct resource *p;
        int err = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
        resource_size_t p_last = -1;
#else
        __PTRDIFF_TYPE__ p_last = -1;
#endif

        DBG("Initilizing Dump...");

        if((err = setup())) {
                DBG("Setup Error");
                cleanup();
                return err;
        }

       
        for (p = iomem_resource.child; p ; p = p->sibling) {
                if (strncmp(p->name, LIME_RAMSTR, sizeof(LIME_RAMSTR)))
                        continue;

                if (mode == LIME_MODE_LIME && (err = write_lime_header(p))) {
                        DBG("Error writing header 0x%lx - 0x%lx", (long) p->start, (long) p->end);
                        break;
                } else if (mode == LIME_MODE_PADDED && (err = write_padding((size_t) ((p->start - 1) - p_last)))) {
                        DBG("Error writing padding 0x%lx - 0x%lx", (long) p_last, (long) p->start - 1);
                        break;
                }

                if ((err = write_range(p))) {
                        DBG("Error writing range 0x%lx - 0x%lx", (long) p->start, (long) p->end);
                        break;
                }

                p_last = p->end;
        }
       

        cleanup();

        return err;
}

static int write_lime_header(struct resource * res) {
        long s;

        lime_mem_range_header header;

        memset(&header, 0, sizeof(lime_mem_range_header));
        header.magic = LIME_MAGIC;
        header.version = 1;
        header.s_addr = res->start;
        header.e_addr = res->end;
       
        s = write_vaddr(&header, sizeof(lime_mem_range_header));
       
        if (s != sizeof(lime_mem_range_header)) {
                DBG("Error sending header %ld", s);
                return (int) s;
        }                              

        return 0;
}

static int write_padding(size_t s) {
        size_t i = 0;
        int r;

        while(s -= i) {

                i = min((size_t) PAGE_SIZE, s);
                r = write_vaddr(zero_page, i);

                if (r != i) {
                        DBG("Error sending zero page: %d", r);
                        return r;
                }
        }

        return 0;
}

static int write_range(struct resource * res) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
        resource_size_t i, is;
#else
        __PTRDIFF_TYPE__ i, is;
#endif
        struct page * p;
        void * v;
       
        int s;

        for (i = res->start; i <= res->end; i += PAGE_SIZE) {

                p = pfn_to_page((i) >> PAGE_SHIFT);
       
        is = min((size_t) PAGE_SIZE, (size_t) (res->end - i + 1));

                v = kmap(p);
                s = write_vaddr(v, is);
                kunmap(p);

                if (s != is) {
                        DBG("Error sending page %d", s);
                        return (int) s;
                }                              
        }

        return 0;
}

static int write_vaddr(void * v, size_t is) {
        return (method == LIME_METHOD_TCP) ? write_vaddr_tcp(v, is) : write_vaddr_disk(v, is);
}

static int setup(void) {
        return (method == LIME_METHOD_TCP) ? setup_tcp() : setup_disk();
}

static void cleanup(void) {
        return (method == LIME_METHOD_TCP) ? cleanup_tcp() : cleanup_disk();
}
/* End main.c code */


/* Added fuctions */
static int lime_status = 1;

static int get_status(void)
{
	return lime_status;
}

static void set_status(int new_status)
{
	lime_status = new_status;
}

struct device_lime {
        struct miscdevice misc;
};

//TODO: This is for more recent kernels (specifically more recent than 2.6.11)
static long lime_ioctl(struct file *file, unsigned int cmd, unsigned long ioctl_param)
{
	int ret_val = 0;

	switch (cmd)
	{ 
		/* ioctl to dump memory to a file on disk. */
		case LIME_DUMP_DISK:
		{
			lime_dump_disk *temp;

                        if (get_status() == LIME_STATUS_BUSY)
			{
                                DBG("Device is busy!");
                                ret_val = -EBUSY;
                                goto out;
                        }

                        set_status(LIME_STATUS_BUSY);

			temp = (lime_dump_disk *)ioctl_param;
			if (copy_from_user((char *)temp, (char *)ioctl_param, sizeof(*temp)) != 0)
			{
				DBG("Couldn't copy lime_dump_disk struct to kernel space!");
				ret_val = -1;
				set_status(LIME_STATUS_READY);
				goto out;
			}

			DBG("Starting memory dump to file: %s", temp->file_name);

			mode = temp->mode;
			dio = temp->dio;
			method = LIME_METHOD_DISK;
			path = temp->file_name;

			memset(zero_page, 0, sizeof(zero_page));

			// Call memory dump code
			ret_val = init();
			set_status(LIME_STATUS_READY);
			break;
		}

		/* ioctl to dump memory to a network socket. */
		case LIME_DUMP_TCP:
		{
			lime_dump_tcp *temp;

			if (get_status() == LIME_STATUS_BUSY)
			{
				DBG("Device is busy!");
				ret_val = -EBUSY;
				goto out;
			}
			
			set_status(LIME_STATUS_BUSY);

                        temp = (lime_dump_tcp *)ioctl_param;
                        if (copy_from_user((char *)temp, (char *)ioctl_param, sizeof(*temp)) != 0)
                        {
                                DBG("Couldn't copy lime_dump_tcp struct to kernel space!");
                                ret_val = -1;
				set_status(LIME_STATUS_READY);
                                goto out;
                        }

                        DBG("Starting memory dump to port: %d", temp->port);

                        mode = temp->mode;
                        dio = temp->dio;
                        method = LIME_METHOD_TCP;
                        port = temp->port;

                        memset(zero_page, 0, sizeof(zero_page));

                        // Call memory dump code
                        ret_val = init();

			DBG("Done!");
			set_status(LIME_STATUS_READY);
                        break;
		}
		
		/* ioctl to determine if the device is ready. */
		case LIME_GET_STATUS:
		{
			ret_val = get_status();
			break;
		}
	}

out:
	return ret_val;
}

static struct file_operations device_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = lime_ioctl,
};

static struct device_lime lime_dev = {
	.misc = {
		.minor = MISC_DYNAMIC_MINOR,
        	.name = LIME_DEVICE,
        	.fops = &device_fops,
        	.parent = NULL,
	},
};

static int __init lime_init(void)
{
        int ret_val = 0;
	
        ret_val = misc_register(&lime_dev.misc);
        
	if (unlikely(ret_val)) {
                DBG("Failed to register misc device. %d", ret_val);
                goto out;
        }

        DBG("LiME device successfully created.");

out:
	return ret_val;
}
device_initcall(lime_init);
/* End added LiME Device Functions */
