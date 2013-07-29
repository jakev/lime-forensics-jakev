/*
 * liblime - Android Library for LiME
 * Copyright (c) 2013 Jake Valletta
 *
 *
 * Author:
 * Jake Valletta     -javallet@gmail.com, @jake_valletta
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "liblime"

#include <jakev/lime.h>
#include <cutils/log.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */

/* Internal Implementation Functions */
static int __is_ready_kernel() 
{
        int file_desc, ret_val;

        file_desc = open("/dev/"LIME_DEVICE, 0);
        
	if (file_desc < 0) 
	{
                LOGE("Error opening LiME device!\n");
        	ret_val = -1;
		goto out;
	}

        ret_val = ioctl(file_desc, LIME_GET_STATUS);

        if (ret_val < 0) {
                LOGE("Error with ioctl?: %d\n", ret_val);
	}

        close(file_desc);

out:
	return ret_val;
}

static int __dump_memory_disk_kernel(const char *file_name, int mode, int dio) 
{
	int file_desc, ret_val;
	lime_dump_disk ldd;

	file_desc = open("/dev/"LIME_DEVICE, 0);
	
	if (file_desc < 0)
	{
		LOGE("Error opening LiME device!\n");
		ret_val = -1;
		goto out;
	}
	
	
	strncpy(ldd.file_name, file_name, strlen(file_name));
	ldd.file_name[strlen(file_name)] = '\0';
	
	ldd.mode = mode;
	ldd.dio = dio;

	ret_val = ioctl(file_desc, LIME_DUMP_DISK, &ldd);
	
	if (ret_val < 0)
	{
		LOGE("Dump memory failed: %d\n", ret_val);
	}

	close(file_desc);

out:
	return ret_val;	
}

static int __dump_memory_tcp_kernel(int port_number, int mode, int dio)
{
        int file_desc, ret_val;
        lime_dump_tcp ldt;

        file_desc = open("/dev/"LIME_DEVICE, 0);

        if (file_desc < 0)
        {
                LOGE("Error opening LiME device!\n");
		ret_val = -1;
                goto out;
        }

	ldt.port = port_number;
        ldt.mode = mode;
        ldt.dio = dio;

        ret_val = ioctl(file_desc, LIME_DUMP_TCP, &ldt);

        if (ret_val < 0)
        {
                LOGE("Dump memory failed: %d\n", ret_val);
        }

        close(file_desc);

out:
        return ret_val;
}

/* Exposed Functions */
int __is_ready()
{
	return  __is_ready_kernel();
}

int __dump_memory_disk(const char *filename, int mode, int dio)
{
	return __dump_memory_disk_kernel(filename, mode, dio);
}

int __dump_memory_tcp(int port_number, int mode, int dio)
{
        return __dump_memory_tcp_kernel(port_number, mode, dio);
}
