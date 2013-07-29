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
#include "klime.h"


int write_vaddr_disk(void *, size_t);
int setup_disk(void);
void cleanup_disk(void);

static void disable_dio(void);

static struct file * f = NULL;
extern char * path;
extern int dio;

static void disable_dio() {
	DBG("Direct IO may not be supported on this file system. Retrying.");
	dio = 0;
	cleanup_disk();
	setup_disk();
}

int setup_disk() {
	mm_segment_t fs;
	int err;
	
	fs = get_fs();
	set_fs(KERNEL_DS);
	
	if (dio)	
		f = filp_open(path, O_WRONLY | O_CREAT | O_LARGEFILE | O_SYNC | O_DIRECT, 0444);
	
	if(!dio || (f == ERR_PTR(-EINVAL))) {
		DBG("Direct IO Disabled");
		f = filp_open(path, O_WRONLY | O_CREAT | O_LARGEFILE, 0444);
		dio = 0;
	}

	if (!f || IS_ERR(f)) {
		DBG("Error opening file %ld", PTR_ERR(f));
		set_fs(fs);
		err = (f) ? PTR_ERR(f) : -EIO;
		f = NULL;
		return err;
	}

	set_fs(fs);

	return 0;
}

void cleanup_disk() {
	mm_segment_t fs;
	
	fs = get_fs();
	set_fs(KERNEL_DS);
	if(f) filp_close(f, NULL);
	set_fs(fs);
}

int write_vaddr_disk(void * v, size_t is) {
	mm_segment_t fs;

	long s;

	fs = get_fs();
	set_fs(KERNEL_DS);
	    
	s = vfs_write(f, v, is, &f->f_pos);					
	
	set_fs(fs);

	if (s != is && dio) {
		disable_dio();
		return write_vaddr_disk(v, is);
	}

	return s;
}
