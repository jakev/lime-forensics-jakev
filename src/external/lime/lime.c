/*
 * "lime" - Command-line frontend for LiME
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jakev/lime.h>

#define PROJECT_NAME "lime"

static int mode = 0;
static int method = 0;

static char path[LIME_MAX_FILENAME_SIZE];
static int dio = 1;
static int port = 0;

static void usage(void)
{
   fprintf(stdout, "LiME Command Line Utility\n");
   fprintf(stdout, "Usage: %s [-h] [OPTIONS] [MODE]\n", PROJECT_NAME);

   fprintf(stdout, "  MODES:\n");
   fprintf(stdout, "   -t[port]          Write data to network socket.\n");
   fprintf(stdout, "   -d[filename]      Write data to file specified.\n");
   fprintf(stdout, "   -r                Check if LiME is ready and exit.\n");
   fprintf(stdout, "\n");

   fprintf(stdout, "  OPTIONS:\n");
   fprintf(stdout, "   -h                    Show this help message and exit.\n");
   fprintf(stdout, "   -f[raw|padded|lime]   Output format.\n");
   fprintf(stdout, "   -i                    Disable direct IO attempt.\n");
   fprintf(stdout, "\n");
   fprintf(stdout, "  Formats:\n");
   fprintf(stdout, "    raw	Simply concatentates all System RAM ranges (default).\n");
   fprintf(stdout, "    padded	Pads all non-System RAM ranges with 0s, starting from physical address 0.\n");
   fprintf(stdout, "    lime	Each range is prepended with a fixed-size header which contains addres space information.\n");
}

static void is_ready(void)
{
   int ret_val = 0;

   ret_val = __is_ready();

   switch(__is_ready()) 
   {
      case LIME_STATUS_READY:
         fprintf(stdout, "LiME device is ready.\n");
         break;

      case LIME_STATUS_BUSY:
         fprintf(stdout, "LiME device is busy.\n");
         break;

      default:
         fprintf(stderr, "Unknown LiME device status: %d\n", ret_val);
         break;
   }
}

static int dump_to_disk(void)
{
   return __dump_memory_disk(path, mode, dio);
}

static int dump_to_tcp(void)
{
   // Is this port realistic?
   if (port < 0 || port > 65535)
   {
      fprintf(stderr, "The port specified is invalid!\n");
      exit(EXIT_FAILURE);
   }

   return __dump_memory_tcp(port, mode, dio);
}

static void parse_args(int argc, char *argv[])
{
   int m, n,                       /* Loop counters. */
      l,                           /* String length. */
      x,                           /* Exit code. */
      ch;

   for( n = 1; n < argc; n++ )     /* Scan through args. */
   {
      switch( (int)argv[n][0] )    /* Check for option character. */
      {
         case '-': 
         {
            x = 0;
            l = strlen( argv[n] );
            for( m = 1; m < l; ++m ) /* Scan through options. */
            {
               ch = (int)argv[n][m];

               switch( ch ) 
               {
                   case 'h':
                      usage();
                      exit(EXIT_SUCCESS);
 
                   case 't':
                      if (m + 1 >= l)
                      { 
                         fprintf(stderr, "Argument \"-t\" requires a port number!\n");
                         exit(EXIT_FAILURE);
                      }
                      else
                      { /* Valid argument */
                         char tmp[5];

                         strncpy(tmp, &argv[n][m+1], strlen(argv[n]) - 2 );
                         port = atoi(tmp);

                         fprintf(stdout, "TCP mode selected: Port %d\n", port);
                         method = LIME_METHOD_TCP;
                      }

                      x = 1;
                      break;

                   case 'd': 
                      if (m + 1 >= l)
                      {
                         fprintf(stderr, "Argument \"-d\" requires a file name!\n");
                         exit(EXIT_FAILURE);
                      }
                      else 
                      { /* Valid argument */
                         strncpy(path, &argv[n][m+1], strlen(argv[n]) - 2);
                         fprintf(stdout, "Disk mode selected: %s\n", path);

                         method = LIME_METHOD_DISK;
                      }

                      x = 1;
                      break;

                   case 'f': 
                      if (m + 1 >= l)
                      {
                         fprintf(stderr, "Argument \"-f\" requires format!\n");
                         exit(EXIT_FAILURE);
                      }
                      else
                      { /* Valid argument */
                         char tmp[7];

                         strncpy(tmp, &argv[n][m+1], strlen(argv[n]) - 2);
                         tmp[strlen(argv[n]) - 2] = '\0';

                         if (strcmp(tmp, "raw") == 0) 
                            mode = LIME_MODE_RAW;
                         else if (strcmp(tmp, "lime") == 0) 
                            mode = LIME_MODE_LIME;
                         else if (strcmp(tmp, "padded") == 0) 
                            mode = LIME_MODE_PADDED;
                         else
                         {
                            fprintf(stderr, "Unknown format: %s\n", tmp);
                            usage();
                            exit(EXIT_FAILURE);
                         }

                         fprintf(stdout, "Output format: %s\n", tmp);
                      }

                      x = 1;
                      break;

                   case 'r':
                      is_ready();
                      exit(EXIT_SUCCESS);

                   case 'i':
                      dio = 0;
                      fprintf(stdout, "Direct I/O attempt is disabled.\n");
                      x = 1;
                      break;

                   default:
                      printf( "Unknown flag = %c\n", ch );
                      usage();
                      exit(EXIT_FAILURE);
               }
               if (x == 1)
                  break;
            }

            break;

         } // case: "-"

         default:
            break;
      } // switch
   } // for
}

int main(int argc, char *argv[])
{
   int ret_val = 0;

   // Parse all the arguments
   parse_args(argc, argv);

   // We need a format
   switch(method)
   {
      case LIME_METHOD_DISK:
         fprintf(stdout, "About to dump memory to disk...\n");
         ret_val = dump_to_disk();
         break;
      
      case LIME_METHOD_TCP:
         fprintf(stdout, "About to dump memory to TCP port %d..\n", port);
         ret_val = dump_to_tcp();
         break;

      default:
         fprintf(stderr, "You must supply either \"-t\" or \"-d\"!\n");
         usage();
         exit(EXIT_FAILURE);
   }

   //How about that return?
   if (ret_val < 0)
   {
      fprintf(stderr, "LiME returned an error: %d\n", ret_val);
      exit(ret_val);
   }
 
   fprintf(stdout, "Done!\n");
   return EXIT_SUCCESS;
}
