/*
 * android/jakev/Lime.java - Android API for LiME
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
package android.jakev;

/**
 * API for interacting with LiME Forensics.
 *
 * This class allows an application to interact with the LiME
 * Forensics (http://code.google.com/p/lime-forensics/) kernel module
 * written by Joe Sylve (Joe.Sylve@gmail.com). Please see the documentation
 * for the LiME Forensics project for more information on using this API.
 */
public final class Lime {

	/** @hide */ private final static String DEFAULT = "/mnt/sdcard/memory.dump";

	/**
	 * Constant for dumping memory in "raw" mode.
	 */
	public final static int LIME_MODE_RAW = 0;
	
        /**
         * Constant for dumping memory in "lime" mode.
         */	
	public final static int LIME_MODE_LIME = 1;
	
        /**
         * Constant for dumping memory in "padded" mode.
         */	
	public final static int LIME_MODE_PADDED = 2;

        /**
         * Constant for dumping memory with direct I/O disabled.
         */
	public final static int LIME_DIO_DISABLED = 0;

        /**
         * Constant for dumping memory with direcet I/O enabled.
         */
	public final static int LIME_DIO_ENABLED = 1;

	/**
	 * Determine if the system is ready to dump memory.
	 * @return Boolean value indicating that the system is or is not ready.
	 */
	public static boolean isReady() {
		return is_ready();
	}


	/* Disk Dumps */
	/**
         * Dump memory to a file on disk.
         * This method will use the <code>DEFAULT<code> value for the filename,
	 * <code>LIME_MODE_RAW<code> for the mode, and <code>LIME_DIO_ENABLED<code>
	 * as the direct I/O attempt.
	 * 
	 * @return The return value from the LiME Forensics kernel module.
         */
	public static int dumpMemoryToDisk() {
		return dump_memory_disk(DEFAULT, LIME_MODE_RAW, LIME_DIO_ENABLED);
	}
	
	/**
         * Dump memory to a file on disk.
	 *
	 * This method will use the <code>DEFAULT<code> value for the filename,
	 * and use <code>LIME_DIO_ENABLED<code> as the direct I/O attempt.
	 *
         * @param mode The output format for the memory dump file.
         * @return The return value from the LiME Forensics kernel module.
         */
	public static int dumpMemoryToDisk(int mode) {
		return dump_memory_disk(DEFAULT, mode, LIME_DIO_ENABLED);
	}

	/**
         * Dump memory to a file on disk.
         *
	 * This method will use <code>LIME_MODE_RAW<code> for the mode, and 
	 * <code>LIME_DIO_ENABLED<code> as the direct I/O attempt.
	 *
	 * @param fileName The path to use for the memory dump file.
         * @return The return value from the LiME Forensics kernel module.
         */
	public static int dumpMemoryToDisk(String fileName) {
		return dump_memory_disk(fileName, LIME_MODE_RAW, LIME_DIO_ENABLED);
	}

	/**
         * Dump memory to a file on disk.
         *
	 * This method will use <code>LIME_DIO_ENABLED<code> as the direct I/O attempt.
	 *
	 * @param fileName The path to use for the memory dump file.
         * @param mode The output format for the memory dump file.
         * @return The return value from the LiME Forensics kernel module.
         */
	public static int dumpMemoryToDisk(String fileName, int mode) {
		return dump_memory_disk(fileName, mode, LIME_DIO_ENABLED);
	}

	/**
         * Dump memory to a file on disk.
         * @param fileName The path to use for the memory dump file.
	 * @param mode The output format for the memory dump file.
	 * @param dio Set if direct I/O is enabled or disabled.
         * @return The return value from the LiME Forensics kernel module.
	 */
        public static int dumpMemoryToDisk(String fileName, int mode, int dio) {
                return dump_memory_disk(fileName, mode, dio);
        }


	/* Network Dumps */
        /**
         * Dump memory to a specified TCP port.
         *
	 * This method will use <code>LIME_MODE_RAW<code> for the mode, and
         * <code>LIME_DIO_ENABLED<code> as the direct I/O attempt.
	 *
	 * @param port The TCP port to dump memory to.
         * @return The return value from the LiME Forensics kernel module.
         */
	public static int dumpMemoryToPort(int port) {
                return dump_memory_port(port, LIME_MODE_RAW, LIME_DIO_ENABLED);
        }

        /**
         * Dump memory to a specified TCP port.
         *
	 * This method will use <code>LIME_DIO_ENABLED<code> as the direct I/O attempt.
	 *
	 * @param port The TCP port to dump memory to.
         * @param mode The output format for the memory dump file.
         * @return The return value from the LiME Forensics kernel module.
         */
        public static int dumpMemoryToPort(int port, int mode) {
                return dump_memory_port(port, mode, 0);
        }

        /**
         * Dump memory to a specified TCP port.
         * @param port The TCP port to dump memory to.
         * @param mode The output format for the memory dump file.
         * @param dio Set if direct I/O is enabled or disabled.
         * @return The return value from the LiME Forensics kernel module.
         */
        public static int dumpMemoryToPort(int port, int mode, int dio) {
                return dump_memory_port(port, mode, dio);
        }

       	/** @hide */ public static native int dump_memory_disk(String file_name, int mode, int dio);
	/** @hide */ public static native int dump_memory_port(int port, int mode, int dio);
	/** @hide */ public static native boolean is_ready();
}
