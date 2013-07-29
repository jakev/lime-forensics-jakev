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
#include <sys/types.h>
#include <sys/ioctl.h>

/* From LiME "lime.h" */
#define LIME_MAX_FILENAME_SIZE 256

#define LIME_MODE_RAW 0
#define LIME_MODE_LIME 1
#define LIME_MODE_PADDED 2

#define LIME_METHOD_UNKNOWN 0
#define LIME_METHOD_TCP 1
#define LIME_METHOD_DISK 2
/* End from "lime.h" */

#define LIME_DEVICE     "lime"
#define __LIMEIO        0xAF

/* LiME device ioctl definitions */
#define LIME_GET_STATUS         _IO(__LIMEIO, 1) /* Get status of driver */
#define LIME_DUMP_TCP           _IO(__LIMEIO, 2) /* Dump memory to socket */
#define LIME_DUMP_DISK          _IO(__LIMEIO, 3) /* Dump memory to disk */

/* LiME device statuses */
#define LIME_STATUS_READY       0x1
#define LIME_STATUS_BUSY        0x0

#ifdef __cplusplus
extern "C" {
#endif

/* Structs */
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

/* Function Prototypes */
int __is_ready();
int __dump_memory_disk(const char *, int, int);
int __dump_memory_tcp(int, int, int);

#ifdef __cplusplus
}
#endif
