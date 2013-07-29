LiME Forensics Fork
===================

About
-----
Linux Memory Extractor ("LiME") is a Loadable Kernel Module written by Joe Sylve.  It allows the acquisition of volatile memory from Linux and Linux-based devices, such as those powered by Android. The tool supports acquiring memory either to the file system of the device or over the network. LiME is unique in that it is the first tool that allows full memory captures from Android devices. It also minimizes its interaction between user and kernel space processes during acquisition, which allows it to produce memory captures that are more forensically sound than those of other tools designed for Linux memory acquisition. 

This port of the project was created by Jake Valletta, and adds additional support to the LiME project.  It enables Android applications and user-space utilities to access LiME functionality.  The project includes:

- A modified LiME kernel module, which exposes a device "/dev/lime" to user-space.
- A library ("liblime") for applications to link against, and be loaded into the Android runtime.
- A command-line utility ("lime") for easy access to the LiME functionality.
- A Java API ("android.jakev.Lime") for the Android framework so that applications can access the LiME device.

Prerequisites
-------------
This project is designed to be integrated into your Android source tree and kernel.  Therefore, you will need a copy of the Android source tree (the version should not matter too much), and an Android kernel.

In addition, you will need to read the "porting LiME" document available on my website (http://www.thecobraden.com/projects/lime-forensics/).  This contains all of the additional modifications to the kernel and source tree you are **required** to make in order for this fork to work properly.

Other Thoughts
-------------
You will need to create a custom "android.jar" in order to use the API "android.jakev.Lime" in your Eclipse projects.  This can be accompished by building the "sdk" option of your Android source tree.  Feel free to contact me for more information on this proceedure.

Licenses
--------
The project has two separate licenses.  Code contained in the directory "kernel-src" is intended to be integrated with a Linux kernel designed for Android, and therefore is distributed with the GPLv2.  The code contained in the "src" directory is intended to be included in the Android source tree, and is distributed with the Apache Software License, version 2.

Questions & Comments
--------------------
Any bug reports, questions, comments, and frustrations with this project should be sent to javallet[at]gmail[dot].com.  For questions or issues with LiME itself, see the original project page: http://code.google.com/p/lime-forensics/.
