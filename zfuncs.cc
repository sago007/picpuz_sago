/**************************************************************************
   zfuncs.cpp   collection of Linux and GDK/GTK utility functions

   Copyright 2006-2016 Michael Cornelison
   source code URL: http://kornelic.net
   contact: kornelix@posteo.de

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.

   These programs originate from the author's web site: http://kornelic.net
   Other web sites may offer them for download. Modifications could have been made.
   If you have questions, suggestions or a bug to report: kornelix@posteo.de

***************************************************************************/

//     zfuncs.cpp   version  v.6.2

#include "zfuncs.h"

/**************************************************************************

   Table of Contents
   =================

   System Utility Functions
   ------------------------
   zmalloc zfree zstrdup   replace malloc() etc. to add statistics
   printz                  printf() with immediate fflush()
   zpopup_message          popup message window, thread safe (no GTK)
   zbacktrace              callable backtrace dump
   zappcrash               abort with traceback dump to popup window and stdout
   catch_signals           trap segfault, crash with zappcrash()
   TRACE                   trace() and tracedump() implement the TRACE macro
   beroot                  restart image as root, if password is OK
   timer functions         elapsed time, CPU time, process time functions
   compact_time            convert time_t type to yyyymmddhhmmss format
   pretty_datetime         convert time_t type to yyyy-mm-dd hh:mm:ss format
   proc file functions     parse data from various /proc files
   zsleep                  sleep for any amount of time (e.g. 0.1 seconds)
   global_lock             lock/unlock a global resource (all processes/threads)
   zget_locked, etc.       safely access parameters from multiple threads
   start_detached_thread   simplified method to start a detached thread
   synch_threads           make threads pause and resume together
   shell_quiet             format and run a shell command, return status
   shell_ack                  ""  + popup error message if error
   shell_asynch               ""  + return immediately and query status later
   comand_output           start shell command and read the output as records
   signalProc              pause, resume, or kill a child process
   runroot                 run a command or program as root user
   fgets_trim              fgets() with trim of trailing \r \n and optionally blanks
   samedirk                test if two files/directories have the same directory path
   parsefile               parse filespec into directory, file, extension
   renamez                 like rename() but works across file systems
   check_create_dir        check if directory exists, ask to create if not
   cpu_profile             measure CPU time spent per function or code block
   pagefaultrate           monitor and report own process hard page fault rate

   String Functions
   ----------------
   strField                get delimited substrings from input string
   strParms                parse a string in the form "parm1=1.23, parm2=22 ..."
   strHash                 hash string to random number in a range
   strncpy0                strcpy() with insured null delimiter
   strnPad                 add blank padding to specified length
   strTrim                 remove trailing blanks
   strTrim2                remove leading and trailing blanks
   strCompress             remove imbedded blanks
   strncatv                catenate multiple strings with length limit
   strmatchV               compare 1 string to N strings
   strToUpper              convert string to upper case
   strToLower              convert string to lower case
   repl_1str               replace substring within string
   repl_Nstrs              replace multiple substrings within string
   breakup_text            insert newline chars to limit text line lengths
   strncpyx                convert string to hex format
   StripZeros              remove trailing zeros (1.23000E+8 >> 1.23E+8)
   blank_null              test string for null pointer, zero length, and all blanks
   clean_escapes           replace 2-character escapes ("\n") with the escaped characters
   UTF-8 functions         deal with UTF-8 multibyte character strings

   Number Conversion and Formatting
   --------------------------------
   convSI            string to integer with optional limits check
   convSD            string to double with optional limits check
   convSF            string to float with optional limits check
   convIS            integer to string with returned length
   convDS            double to string with specified digits of precision
   formatKBMB        format a byte count with specified precision and B/KB/MB/GB units

   Wildcard Functions
   ------------------
   MatchWild               match string to wildcard string (multiple * and ?)
   MatchWildIgnoreCase     works like MatchWild() but ignores case
   SearchWild              wildcard file search (multiple * and ? in path or file name)
   SearchWildIgnoreCase    works like SearchWild() but ignores case in file name

   Search and Sort Functions
   -------------------------
   bsearch           binary search of sorted list
   HeapSort          sort list of integer / float / double / records / pointers to records
   MemSort           sort records with multiple keys (data types and sequence)

   Misc Functions
   --------------
   pvlist_create, etc.     functions to manage a list of variable strings
   random numbers          int and double random numbers with improved distributions
   spline1/2               cubic spline curve fitting function
   Qtext                   FIFO queue for text strings, dual thread access

   Application Admin Functions
   ---------------------------
   zinitapp etc.           initialize application directory and data files
   locale_filespec         find a locale-dependent installation file (translate-xx.po etc.)
   showz_userguide         display user guide (optional help topic)
   showz_logfile           display application log file
   showz_textfile          show application text file (README, changelog, etc.)
   showz_html              show a local or remote HTML file
   zmake_menu_launcher     create desktop icon / launcher
   ZTX(), etc.             translate GUI and message text strings for non-English locales

   GTK Utility Functions
   ---------------------
   zmainloop               do a main loop to process menu events, etc.
   zthreadcrash            crash if called from a thread other than main()
   get_hardware_info       get hardware information (display, screen, mouse, DPI)
   move_pointer            move the mouse pointer within a widget/window
   wprintx etc.            printf() to window at specified row or next row
   wscroll                 scroll text window to put line on screen
   wclear                  clear a wprintf window
   wscanf                  read text from edit window, line at a time
   wfiledump               dump text window to a file
   wfilesave               wfiledump with save-as dialog
   wprintp                 print text window to default printer
   textwidget funcs        intercept mouse clicks on text windows, get clicked text
   menus / toolbars        simplified GTK menus, toolbars and status bars
   popup menus             create popup menus
   gmenuz                  customizable graphic popup menu
   Vmenu                   vertical menu/toolbar in vertical packing box
   zdialog_new                create new zdialog
   zdialog_set_title          change a zdialog title
   zdialog_set_modal          set a zdialog to be modal
   zdialog_set_decorated      set a zdialog to be decorated or not
   zdialog_present            present a zdialog (visible and on top)
   zdialog_add_widget         add widget to existing zdialog
   zdialog_valid              return 1/0 if zdialog is valid/invalid
   zdialog_widget             get GTK widget from zdialog and widget name
   zdialog_add_ttip           add a popup tool tip to a zdialog widget       
   zdialog_set_group          set a common group for a set of radio buttons
   zdialog_resize             resize dialog greater than initial size
   zdialog_set_limits         set new limits for numeric data entry widget
   zdialog_rescale            expande scale around neutral value
   zdialog_run                run the dialog and send events to event function
   zdialog_send_event         send an event to an active zdialog
   zdialog_send_response      complete a dialog and assign status
   zdialog_show               show or hide a zdialog window
   zdialog_destroy            destroy a zdialog (data remains available)
   zdialog_free               free zdialog memory (data is gone)
   zdialog_wait               wait for zdialog completion, get status
   zdialog_goto               put cursor at named widget
   zdialog_set_cursor         set zdialog cursor (e.g. busy)
   zdialog_stuff              stuff data into zdialog widget
   zdialog_labelfont          set label text with font
   zdialog_fetch              fetch data from zdialog widget
   zdialog_cb_app             append item to combo box list widget
   zdialog_cb_prep            prepend item to combo box list widget
   zdialog_cb_get             get data from combo box entry widget
   zdialog_cb_delete          delete combo box entry from list
   zdialog_cb_clear           clear combo box list
   zdialog_cb_popup           make combo box show list of all entries
   zdialog_cb_save            save combo box list to a file
   zdialog_cb_load            load combo box list from a file
   zdialog_positions          load/save zdialog positions at app start/exit
   zdialog_inputs             load/save zdialog inputs at app start/exit
   zdialog_restore_inputs     retrieve prior zdialog input fields
   write_popup_text        write rows of text to a popup window
   popup_command           run a shell command with output in a popup window
   zmessageACK             popup message, printf format, wait for user ACK
   zmessLogACK             same with parallel output to STDOUT
   zmessageYN              popup message, printf format, wait for user Yes / No
   zmessage_post           popup message, printf format, show until killed
   zdialog_text            popup dialog to get 1-2 lines of text input from user
   zdialog_choose          popup dialog to show a message, select a button, return choice
   poptext_mouse           popup message at current mouse position + offset
   poptext_window          popup message at given window position
   poptext_killnow         kill popup message
   popup_image             show an image in a small popup window
   zgetfile                simplified file chooser dialog
   print_image_file        dialog to print an image file using GTK functions
   drag_drop_connect       connect window drag-drop event to user function
   get_thumbnail           get thumbnail image for given image file
   zmakecursor             make a cursor from an image file (.png .jpg)
   gdk_pixbuf_rotate       rotate a pixbuf through any angle
   gdk_pixbuf_stripalpha   remove an alpha channel from a pixbuf
   text_pixbuf             create pixbuf containing text 

   C++ Classes
   -----------
   xstring              string manipulation (= / + / insert / overlay)
   Vxstring             array of xstrings with auto growth
   HashTab              hash table: add, delete, find, step through
   Queue                queue of xstrings: push, pop first or last
   Tree                 store / retrieve data by node names or numbers, any depth


***************************************************************************/


namespace zfuncs
{
   GdkDisplay        *display;                                                   //  X workstation (KB, mouse, screen)
   GdkDeviceManager  *manager;                                                   //  knows screen / mouse associations
   GdkScreen         *screen;                                                    //  monitor
   GdkDevice         *mouse;                                                     //  pointer device
   GtkSettings       *settings;                                                  //  screen settings                    6.2
   std::string        zappname;                                                     //  app name/version
   std::string		zprefix;
   char        zdatadir[200], zdocdir[200];                        //  app directories
   char        zicondir[200], zlocalesdir[200], zuserdir[200];
   char        zlang[8] = "en";                                                  //  "lc" or "lc_RC"
   char        JPGquality[4] = "90";                                             //  JPG file save quality
   int         open_popup_windows = 0;                                           //  open popup window count
   pthread_t   tid_main = 0;                                                     //  main() thread ID
   int         Nmalloc, Nstrdup, Nfree;                                          //  zmalloc() zstrdup() and zfree() calls
   int         vmenuclickposn;                                                   //  Vmenu menu icon click posn. 0-100
   int         vmenuclickbutton;                                                 //   "" button: 1/2/3 = L/M/R mouse
   #define     zdialog_max 20                                                    //  max. parallel zdialogs             6.0
   zdialog     *zdialog_list[zdialog_max];                                       //  active zdialog list                6.0
   int         zdialog_count = 0;                                                //  total zdialogs (new - free)
   int         zdialog_busy = 0;                                                 //  open zdialogs (run - destroy)
}


/**************************************************************************
   system-level utility functions
***************************************************************************/

//  replacements for malloc(), free(), and strdup() with call counters           //  5.8

void *zmalloc(unsigned int cc)
{
   static unsigned int allocated = 0;
   static bool fwarn = false;

   ///printz("zmalloc %p %d \n",__builtin_return_address(0),cc);
   allocated += cc;                                                              //  check memory each 1 MB allocated
   if (allocated > 1000*1000*1000) {
      allocated = 0;
      if (! fwarn) {                                               
         fwarn = true;
         printz("HI MEMORY USAGE - performance may be poor \n");
         zpopup_message(10,"HI MEMORY USAGE - performance may be poor");
      }
   }

   zfuncs::Nmalloc++;
   void *pp = malloc(cc);
   if (pp) {
      memset(pp,0,cc);                                                           //  force real memory allocation    5.9
      return pp;
   }
   cc = cc >> 10;
   zappcrash("malloc(%u KB) failure, OUT OF MEMORY",cc);
   exit(12);
}

void zfree(void *pp)
{
   ///printz("zfree %p \n",__builtin_return_address(0));
   int *ppp = (int *) pp;                                                        //  clobber released memory         5.9
   *ppp = -1;
   zfuncs::Nfree++;
   free(pp);
   return;
}

char *zstrdup(cchar *string, int addcc)
{
   ///printz("zstrdup %p \n",__builtin_return_address(0));
   zfuncs::Nstrdup++;
   char *pp = (char *) zmalloc(strlen(string) + 1 + addcc);                      //  bugfix  6.0
   strcpy(pp,string);
   return pp;
}


/**************************************************************************/

//  printf() and flush every output immediately even if stdout is a file

void printz(cchar *format, ...)
{
   va_list  arglist;

   va_start(arglist,format);
   vprintf(format,arglist);
   va_end(arglist);

   fflush(stdout);
   return;
}


//  output a message to a popup window
//  works like printf()
//  separate shell process, no GTK
//  returns immediately
//  popup message disappears after 'secs' seconds if secs > 0
//  popup message disappears if user presses the OK button

void zpopup_message(int secs, cchar *format, ... )                               //  6.0
{
   va_list     arglist;
   char        message[400], command[500], timeout[20];
   int         err;

   va_start(arglist,format);                                                     //  format user message
   vsnprintf(message,400,format,arglist);
   va_end(arglist);

   printz("popup message: \n %s \n",message);                                    //  message to log file stdout

   strcpy(command,"xmessage -buttons OK:0 -center ");                            //  create popup with message file

   if (secs > 0) {                                                               //  6.0
      snprintf(timeout,20,"-timeout %d ",secs);
      strcat(command,timeout);
   }

   strcat(command,message);
   strcat(command," &");                                                         //  return immediately
   err = system(command);
   if (err) return;                                                              //  stop compiler warning
   return;
}


/**************************************************************************/

//  Write an error message and backtrace dump to a file and to a popup window.
//  Error message works like printf().
//  Depends on library program addr2line().

void zappcrash(cchar *format, ... )                                              //  rev. 6.0
{
   using namespace zfuncs;

   static int     crash = 0;
   va_list        arglist;
   char           message[300];

   if (crash++) return;                                                          //  re-entry or multiple threads crash

   va_start(arglist,format);
   vsnprintf(message,sizeof(message),format,arglist);
   va_end(arglist);

   printz("*** zappcrash: %s %s \n",zappname.c_str(),message);                   //  output message to stdout

   exit(1);
}




//  catch fatal signals and produce backtrace dumps on-screen

void sighandler(int signal)
{
   const char  *signame = "unknown";

   if (signal == SIGTERM) {
      printz("process terminated by system \n");
      exit(1);
   }

   if (signal == SIGSEGV) signame = "segment fault (bad address)";
   if (signal == SIGILL) signame = "illegal operation";
   if (signal == SIGFPE) signame = "arithmetic exception";
   if (signal == SIGBUS) signame = "bus error (bad address)";
   if (signal == SIGABRT) signame = "bug detected";

   zappcrash("fatal signal: %s",signame);
   return;
}


/**************************************************************************/

//  Implement the TRACE macro.                                                   //  5.7
//  Trace program execution by function and source code line number.
//  tracedump() dumps last 50 uses of TRACE macro, latest first.

namespace tracenames
{
   char  filebuff[50][100];                                                      //  last 50 TRACE calls
   char  funcbuff[50][60];
   int   linebuff[50];
   void  *addrbuff[50];
   int   ii, ftf = 1;
};


//  Args are source file, source function name, source code line number,
//  caller address. These all come from the GCC compiler and TRACE macro.

void trace(cchar *file, cchar *func, int line, void *addr)
{
   using namespace tracenames;

   if (ftf) {
      ftf = 0;
      for (ii = 0; ii < 50; ii++) {
         filebuff[ii][99] = 0;
         funcbuff[ii][39] = 0;
         linebuff[ii] = 0;
         addrbuff[ii] = 0;
      }
      ii = 0;
   }

   if (line == linebuff[ii] &&
      strmatch(func,funcbuff[ii])) return;                                       //  same as last call, don't duplicate

   if (++ii > 49) ii = 0;                                                        //  add data to list
   strncpy(&filebuff[ii][0],file,99);
   strncpy(&funcbuff[ii][0],func,39);
   linebuff[ii] = line;
   addrbuff[ii] = addr;
   return;
}


//  dump trace records to STDOUT

void tracedump()
{
   using namespace tracenames;

   FILE     *fid;
   int      kk;

   printz(" *** tracedump *** \n");

   kk = ii;
   while (linebuff[kk]) {
      printz("TRACE %s %s %d %p \n",&filebuff[kk][0],
              &funcbuff[kk][0],linebuff[kk],addrbuff[kk]);
      if (--kk == ii) break;
   }

   fid = fopen("tracedump","w");
   if (! fid) {
      perror("tracedump fopen() failure \n");
      return;
   }

   fprintf(fid, " *** tracedump *** \n");

   kk = ii;
   while (linebuff[kk]) {
      fprintf(fid, "TRACE %s %s %d %p \n",&filebuff[kk][0],
                    &funcbuff[kk][0],linebuff[kk],addrbuff[kk]);
      if (--kk == ii) break;
   }

   fclose(fid);
   return;
}


/**************************************************************************/

//  get time in real seconds (since 2000.01.01 00:00:00)
//  (microsecond resolution until at least 2030)

double get_seconds()
{
   timeval  time1;
   double   time2;

   gettimeofday(&time1,0);
   time2 = time1.tv_sec + 0.000001 * time1.tv_usec - 946684800.0;                //  5.2
   return time2;
}


/**************************************************************************/

//  start a timer or get elapsed time with millisecond resolution.

void start_timer(double &time0)
{
   timeval  timev;

   gettimeofday(&timev,0);
   time0 = timev.tv_sec + 0.000001 * timev.tv_usec;
   return;
}

double get_timer(double &time0)
{
   timeval  timev;
   double   time;

   gettimeofday(&timev,0);
   time = timev.tv_sec + 0.000001 * timev.tv_usec;
   return time - time0;
}




/**************************************************************************/

//  convert a time_t date/time (e.g. st_mtime from stat() call)
//    into a compact date/time format "yyyymmddhhmmss"

void compact_time(const time_t &DT, char *compactDT)                              //  5.5
{
   struct tm   *fdt;
   int         year, mon, day, hour, min, sec;

   fdt = localtime(&DT);

   year = fdt->tm_year + 1900;
   mon = fdt->tm_mon + 1;
   day = fdt->tm_mday;
   hour = fdt->tm_hour;
   min = fdt->tm_min;
   sec = fdt->tm_sec;

   compactDT[0] = year / 1000 + '0';
   compactDT[1] = (year % 1000) / 100 + '0';
   compactDT[2] = (year % 100) / 10 + '0';
   compactDT[3] = year % 10 + '0';
   compactDT[4] = mon / 10 + '0';
   compactDT[5] = mon % 10 + '0';
   compactDT[6] = day / 10 + '0';
   compactDT[7] = day % 10 + '0';
   compactDT[8] = hour / 10 + '0';
   compactDT[9] = hour % 10 + '0';
   compactDT[10] = min / 10 + '0';
   compactDT[11] = min % 10 + '0';
   compactDT[12] = sec / 10 + '0';
   compactDT[13] = sec % 10 + '0';
   compactDT[14] = 0;

   return;
}


/**************************************************************************/

//  convert a time_t date/time (e.g. st_mtime from stat() call)
//    into a pretty date/time format "yyyy-mm-dd hh:mm:ss"

void pretty_datetime(const time_t &DT, char *prettyDT)                            //  6.2
{
   struct tm   *fdt;
   int         year, mon, day, hour, min, sec;

   fdt = localtime(&DT);

   year = fdt->tm_year + 1900;
   mon = fdt->tm_mon + 1;
   day = fdt->tm_mday;
   hour = fdt->tm_hour;
   min = fdt->tm_min;
   sec = fdt->tm_sec;

   prettyDT[0] = year / 1000 + '0';
   prettyDT[1] = (year % 1000) / 100 + '0';
   prettyDT[2] = (year % 100) / 10 + '0';
   prettyDT[3] = year % 10 + '0';
   prettyDT[4] = '-';
   prettyDT[5] = mon / 10 + '0';
   prettyDT[6] = mon % 10 + '0';
   prettyDT[7] = '-';
   prettyDT[8] = day / 10 + '0';
   prettyDT[9] = day % 10 + '0';
   prettyDT[10] = ' ';
   prettyDT[11] = hour / 10 + '0';
   prettyDT[12] = hour % 10 + '0';
   prettyDT[13] = ':';
   prettyDT[14] = min / 10 + '0';
   prettyDT[15] = min % 10 + '0';
   prettyDT[16] = ':';
   prettyDT[17] = sec / 10 + '0';
   prettyDT[18] = sec % 10 + '0';
   prettyDT[19] = 0;

   return;
}



//  Parse /proc record of the type  "xxx xxxxx xxxxx xxxxxxxx xxx"
//  Return numeric values for requested fields (starting with 1)

int parseprocrec(char *prec, int field, double *value, ...)                      //  EOL = 0
{
   va_list     arglist;
   int         xfield = 1, found = 0;

   va_start(arglist,value);

   while (*prec == ' ') prec++;                                                  //  skip leading blanks

   while (field > 0)
   {
      while (xfield < field)                                                     //  skip to next wanted field
      {
         prec = strchr(prec,' ');                                                //  find next blank
         if (! prec) break;
         while (*prec == ' ') prec++;                                            //  skip multiple blanks
         xfield++;
      }

      if (! prec) break;
      *value = atof(prec);                                                       //  convert, return double
      found++;

      field = va_arg(arglist,int);                                               //  next field number
      value = va_arg(arglist,double *);                                          //  next output double *
   }

   while (field > 0)
   {
      *value = 0;                                                                //  zero values not found
      field = va_arg(arglist,int);
      value = va_arg(arglist,double *);
   }

   va_end(arglist);
   return found;
}


/**************************************************************************/

//  sleep for specified time in seconds (double)
//  signals can cause early return

void zsleep(double dsecs)
{
   unsigned    isecs, nsecs;
   timespec    tsecs;

   if (dsecs == 0.0) return;
   isecs = unsigned(dsecs);
   nsecs = unsigned(1000000000.0 * (dsecs - isecs));
   tsecs.tv_sec = isecs;
   tsecs.tv_nsec = nsecs;
   nanosleep(&tsecs,null);
   return;
}


/**************************************************************************/

//  Lock or unlock a multi-process multi-thread resource.
//  Only one process/thread may possess a given lock.
//  A reboot or process exit or crash releases the lock.
//  fd = global_lock(lockfile) returns fd > 0 if success, -1 otherwise.

int global_lock(cchar *lockfile)                                                 //  5.2
{
   int       err, fd;

   fd = open(lockfile,O_RDWR|O_CREAT,0666);                                      //  open or create the lock file
   if (fd < 0) {
      printz("*** global_lock(), %s \n",strerror(errno));
      return -1;
   }

   err = flock(fd,LOCK_EX|LOCK_NB);                                              //  request exclusive non-blocking lock
   if (err) {
      close(fd);
      return -1;
   }

   return fd + 1;                                                                //  return value is >= 1
}


int global_unlock(int fd, cchar *lockfile)
{
   int err = close(fd-1);
   remove(lockfile);
   if (err < 0) return -1;
   else return 1;
}




/**************************************************************************/

//  Safely access and update parameters from multiple threads.
//  A mutex lock is used to insure one thread at a time has access to the parameter.
//  Many parameters can be used but there is only one mutex lock.

mutex_t zget_lock = PTHREAD_MUTEX_INITIALIZER;

int zget_locked(int &param)                                                      //  lock and return parameter
{
   mutex_lock(&zget_lock);
   return param;
}

void zput_locked(int &param, int value)                                          //  set and unlock parameter
{
   param = value;
   mutex_unlock(&zget_lock);
   return;
}

int zadd_locked(int &param, int incr)                                            //  lock, increment, unlock, return
{
   int      retval;

   mutex_lock(&zget_lock);
   retval = param + incr;
   param = retval;
   mutex_unlock(&zget_lock);
   return retval;
}


/**************************************************************************/

//  Start a detached thread using a simplified protocol.
//  Will not make a zombie out of the calling thread if it exits
//  without checking the status of the created thread.
//  Thread should exit with pthread_exit(0);

void start_detached_thread(void * threadfunc(void *), void * arg)
{
   pthread_t         ptid;
   pthread_attr_t    ptattr;
   int               pterr;

   pthread_attr_init(&ptattr);
   pthread_attr_setdetachstate(&ptattr,PTHREAD_CREATE_DETACHED);
   pterr = pthread_create(&ptid,&ptattr,threadfunc,arg);
   if (pterr) zappcrash("start_detached_thread() failure");
   return;
}


/**************************************************************************/

//  Synchronize execution of multiple threads.
//  Simultaneously resume NT calling threads.
//  from main():        synch_threads(NT)    /* setup to synch NT threads */
//  from each thread:   synch_threads()      /* suspend, resume simultaneously */
//
//  Each calling thread will suspend execution until all threads have suspended,
//  then they will all resume execution at the same time. If NT is greater than
//  the number of calling threads, the threads will never resume.

void synch_threads(int NT)
{
   static pthread_barrier_t   barrier;
   static int                 bflag = 0;

   if (NT) {                                                                     //  main(), initialize
      if (bflag) pthread_barrier_destroy(&barrier);
      pthread_barrier_init(&barrier,null,NT);
      bflag = 1;
      return;
   }

   pthread_barrier_wait(&barrier);                                               //  thread(), wait for NT threads
   return;                                                                       //  unblock
}


/**************************************************************************/

//  Format and run a shell command.
//  Print a status message to stdout if there is an error.
//  returns 0 if OK, +N if error

int shell_quiet(cchar *command, ...)                                             //  5.5
{
   char        *cbuff;
   va_list     arglist;
   int         cc, err;

   cc = strlen(command) + 1000;
   cbuff = (char *) zmalloc(cc+1);

   va_start(arglist,command);                                                    //  format command
   vsnprintf(cbuff,cc,command,arglist);
   va_end(arglist);

   err = system(cbuff);
   if (! err) { zfree(cbuff); return 0; }

   err = WEXITSTATUS(err);                                                       //  special BS for subprocesses     5.8

   if (strmatchN(command,"diff",4)) { zfree(cbuff); return err; }                //  no diagnostic for these
   if (strmatchN(command,"cmp",3)) { zfree(cbuff); return err; }
   if (strmatchN(command,"which",5)) { zfree(cbuff); return err; }

   if (cc > 200) cbuff[200] = 0;
   if (err == 127)                                                               //  special case                    5.8
      printz("%s *** %s \n",cbuff,"command not found");
   else printz("%s *** %s \n",cbuff,strerror(err));                              //  log error message

   zfree(cbuff);
   return err;
}


//  Format and run a shell command.
//  Print a status message to stdout if there is an error.
//  Also pop-up an error message window if error.
//  Thread safe: GTK is not used.
//  returns 0 if OK, +N if error

int shell_ack(cchar *command, ...)                                               //  5.5
{
   char        *cbuff;
   va_list     arglist;
   int         cc, err;

   cc = strlen(command) + 1000;
   cbuff = (char *) zmalloc(cc+1);

   va_start(arglist,command);                                                    //  format command
   vsnprintf(cbuff,cc,command,arglist);
   va_end(arglist);

   printz("shell: %s \n",cbuff);                                                 //  5.6
   err = system(cbuff);
   if (! err) { zfree(cbuff); return 0; }

   err = WEXITSTATUS(err);                                                       //  special BS for subprocesses     5.8

   if (strmatchN(command,"diff",4)) { zfree(cbuff); return err; }                //  no diagnostic for these
   if (strmatchN(command,"cmp",3)) { zfree(cbuff); return err; }
   if (strmatchN(command,"which",5)) { zfree(cbuff); return err; }

   if (cc > 200) cbuff[200] = 0;
   if (err == 127) {
      printz("%s *** %s \n",cbuff,"command not found");                          //  special case                    5.8
      zmessageACK(0,"%s \n %s",cbuff,"command not found");
   }
   else {
      printz("%s *** %s \n",cbuff,wstrerror(err));                               //  log error message
      zmessageACK(0,"%s \n %s",cbuff,wstrerror(err));                            //  popup error message
   }

   zfree(cbuff);
   return err;
}


/**************************************************************************/

//  Start a shell command from a new thread and return immediately.
//  The thread waits for the shell command and gets its status.
//  If there is an error, it is logged to STDOUT.
//  The calling process can get the status asynchronously.
//  The command format works like printf().
//
//  handle = shell_asynch(command, ...)
//     Start the command and return a reference handle.
//
//  err = shell_asynch_status(handle)
//     Return the command status for the given handle:
//       -1 = busy, 0 = complete OK, +N = error (use wstrerror(err)).
//
//  The status MUST be queried for allocated space to be freed.

namespace shell_asynch_names {
   char     *command[10];
   int      status[10];
   mutex_t  mlock = PTHREAD_MUTEX_INITIALIZER;
}

int shell_asynch(cchar *Fcommand, ...)                                           //  5.5
{
   using namespace shell_asynch_names;

   void * shell_asynch_thread(void *);

   va_list     arglist;
   static int  ii;

   mutex_lock(&mlock);                                                           //  block other callers

   for (ii = 0; ii < 10; ii++)
      if (command[ii] == 0) break;
   if (ii == 10) zappcrash("shell_asynch > 10 calls");

   command[ii] = (char *) zmalloc(2000);                                         //  allocate memory

   va_start(arglist,Fcommand);                                                   //  format command
   vsnprintf(command[ii],2000,Fcommand,arglist);
   va_end(arglist);

   start_detached_thread(shell_asynch_thread,&ii);                               //  pass command to thread
   status[ii] = -1;                                                              //  status = busy
   return ii;                                                                    //  return handle
}

void * shell_asynch_thread(void *arg)                                            //  thread function
{
   using namespace shell_asynch_names;

   int      err;
   int      ii = *((int *) arg);                                                 //  capture handle

   mutex_unlock(&mlock);                                                         //  unblock other callers
   err = system(command[ii]);                                                    //  start command, wait until done
   if (! err) {
      status[ii] = 0;
      return 0;
   }

   err = WEXITSTATUS(err);                                                       //  special BS for subprocesses     5.8
   if (err == 127)
      printz("%s \n *** %s \n",command[ii],"command not found");                 //  special case                    5.8
   else printz("%s \n *** %s \n",command[ii],wstrerror(err));                    //  log error message
   status[ii] = err;                                                             //  set status for caller
   return 0;
}

int shell_asynch_status(int handle)                                              //  get command status
{
   using namespace shell_asynch_names;

   int ii = handle;
   if (status[ii] == -1) return -1;                                              //  return busy status
   zfree(command[ii]);                                                           //  free memory
   command[ii] = 0;
   return status[ii];                                                            //  return completion status
}


/**************************************************************************

   Run a shell command and get its outputs one record at a time.
   The outputs are returned one record at a time, until a NULL is returned,
   indicating the command has finished and has exited.
   The new line character is removed from the returned output records.
   Use contx = 0 to start a new command. Do not change the returned value.
   Up to 9 commands can run in parallel, with contx values 1-9.
   To get the command exit status: status = command_status(contx).
   If the command is still busy, -1 is returned.
   To kill a command before output is complete: command_kill(contx);

***/

FILE *   CO_contx[10] = { 0,0,0,0,0,0,0,0,0,0 };
int      CO_status[10];

char * command_output(int &contx, cchar *command, ...)                           //  simplify, allow parallel usage
{
   FILE        *fid;
   va_list     arglist;
   char        buff[10000], *prec;

   if (contx == 0)                                                               //  start new command
   {
      for (contx = 1; contx < 10; contx++)
         if (CO_contx[contx] == 0) break;
      if (contx == 10) {
         printz("*** command_output(), parallel usage > 9 \n");
         return 0;
      }
      va_start(arglist,command);                                                 //  format command
      vsnprintf(buff,9999,command,arglist);
      va_end(arglist);

      fid = popen(buff,"r");                                                     //  execute command, output to FID
      if (fid == 0) {
         CO_status[contx] = errno;                                               //  failed to start
         printz("*** command_output: %s\n %s\n",buff,strerror(errno));
         return 0;
      }
      CO_contx[contx] = fid + 1000;
      CO_status[contx] = -1;                                                     //  mark context busy
   }

   fid = CO_contx[contx] - 1000;
   prec = fgets_trim(buff,9999,fid,1);                                           //  next output, less trailing \n
   if (prec) return zstrdup(prec);
   CO_status[contx] = pclose(fid);                                               //  EOF, set status
   CO_contx[contx] = 0;                                                          //  mark context free
   return 0;
}

int command_status(int contx)                                                    //  get command exit status
{
   int err = CO_status[contx];
   return WEXITSTATUS(err);                                                      //  special BS for subprocess
}

int command_kill(int contx)                                                      //  kill output before completion   5.5
{
   FILE     *fid;
   if (! CO_contx[contx]) return 0;                                              //  context already closed          5.8
   fid = CO_contx[contx] - 1000;
   CO_status[contx] = pclose(fid);                                               //  close context and set status
   CO_contx[contx] = 0;                                                          //  mark context free
   return 0;
}


/**************************************************************************/

//  Signal a running subprocess by name (name of executable or shell command).
//  Signal is "pause", "resume" or "kill". If process is paused, kill may not work,
//  so issue resume first if process is paused.

int signalProc(cchar *pname, cchar *signal)
{
   pid_t       pid;
   FILE        *fid;
   char        buff[100], *pp;
   int         err, nsignal = 0;

   snprintf(buff,100,"ps -C %s h o pid",pname);
   fid = popen(buff,"r");                                                        //  popen() instead of system()
   if (! fid) return 2;
   pp = fgets(buff,100,fid);
   pclose(fid);
   if (! pp) return 4;

   pid = atoi(buff);
   if (! pid) return 5;

   if (strmatch(signal,"pause")) nsignal = SIGSTOP;
   if (strmatch(signal,"resume")) nsignal = SIGCONT;
   if (strmatch(signal,"kill")) nsignal = SIGKILL;

   err = kill(pid,nsignal);
   return err;
}


/**************************************************************************/

//  run a command or program as root user
//  sucomm:  root user access command, "su" or "sudo"
//  command:  shell command or filespec of the program to start
//  returns 0 if successfully started, else returns an error code

int runroot(cchar *sucomm, cchar *command)
{
   char     xtcommand[500];
   int      err;

   if (strmatch(sucomm,"sudo"))
   {
      snprintf(xtcommand,499,"xterm -geometry 40x3 -e sudo -S %s",command);
      err = system(xtcommand);
      if (err) err = WEXITSTATUS(err);                                           //  special BS for subprocesses     5.8
      return err;
   }

   if (strmatch(sucomm,"su"))
   {
      snprintf(xtcommand,499,"xterm -geometry 40x3 -e su -c %s",command);
      err = system(xtcommand);
      if (err) err = WEXITSTATUS(err);
      return err;
   }

   return -1;
}


/**************************************************************************/

//  fgets() with additional feature: trailing \n \r are removed.
//  optional bf flag: true if trailing blanks are to be removed.
//  trailing null character is assured.

char * fgets_trim(char *buff, int maxcc, FILE *fid, int bf)
{
   int      cc;
   char     *pp;

   pp = fgets(buff,maxcc,fid);
   if (! pp) return pp;
   cc = strlen(buff);
   if (bf) while (cc && buff[cc-1] > 0 && buff[cc-1] <= ' ') --cc;
   else    while (cc && buff[cc-1] > 0 && buff[cc-1] < ' ') --cc;
   buff[cc] = 0;
   return pp;
}


/**************************************************************************/

//  Return 1 if both filespecs have the same directory, else return 0.
//  Both directories must be specified, at least one with ending '/'
//  (true if a file name is present)

int samedirk(cchar *file1, cchar *file2)                                         //  5.9
{
   cchar    *p1, *p2;
   int      cc1, cc2, cc;

   p1 = strrchr(file1,'/');                                                      //  /dir1/dir2
   p2 = strrchr(file2,'/');                                                      //  /dir1/dir2/file
   cc1 = cc2 = 0;
   if (p1) cc1 = p1 - file1;                                                     //  /dir1/dir2/file
   if (p2) cc2 = p2 - file2;                                                     //  |         |
   if (cc2 > cc1) cc = cc2;                                                      //  0         cc
   else cc = cc1;
   if (cc == 0) return 0;
   if (strmatchN(file1,file2,cc)) return 1;
   return 0;
}


/**************************************************************************

   Parse a pathname (filespec) and return its components.
   Returned strings are allocated in static memory (no zfree needed).
   Missing components are returned as null pointers.

   input ppath         outputs

   /name1/name2/       directory /name1/name2/ with no file
   /name1/name2        directory /name1/name2/ if name2 a directory,
                       otherwise directory /name1/ and file name2
   /name1/name2.xxx    if .xxx < 8 chars, returns file name2 and ext .xxx,
                       otherwise returns file name2.xxx and no ext

   returns 0 if no error, else 1

***************************************************************************/

int parsefile(cchar *ppath, char **pdirk, char **pfile, char **pext)
{
   STATB          statb;
   static char    dirk[1000], file[200], ext[8];
   char           *pp;
   int            err, cc1, cc2;

   *pdirk = *pfile = *pext = null;

   cc1 = strlen(ppath);
   if (cc1 > 999) return 1;                                                      //  ppath too long

   strcpy(dirk,ppath);
   *pdirk = dirk;

   err = stat(dirk,&statb);                                                      //  have directory only
   if (! err && S_ISDIR(statb.st_mode)) return 0;

   pp = (char *) strrchr(dirk,'/');
   if (! pp) return 1;                                                           //  illegal

   pp++;
   cc2 = pp - dirk;
   if (cc2 < 2 || cc2 == cc1) return 0;                                          //  have /xxxx  or  /xxxx/

   if (strlen(pp) > 199) return 1;                                               //  filename too long

   strcpy(file,pp);                                                              //  file part
   *pfile = file;
   *pp = 0;                                                                      //  remove from dirk part

   pp = (char *) strrchr(file,'.');
   if (! pp || strlen(pp) > 7) return 0;                                         //  file part, no .ext

   strcpy(ext,pp);                                                               //  .ext part
   *pext = ext;
   *pp = 0;                                                                      //  remove from file part
   return 0;
}


/**************************************************************************/

//  Move a source file to a destination file and delete the source file.
//  Equivalent to rename(), but the two files MAY be on different file systems.
//  Pathnames must be absolute (start with '/').
//  Returns 0 if OK, +N if not.

int renamez(cchar *file1, cchar *file2)                                          //  5.8
{
   const char     *pp1;
   const char     *pp2;
   int      err, Frename = 0;

   if (*file1 != '/' || *file2 != '/') return 1;                                 //  not absolute pathnames

   pp1 = strchr((char *) file1+1,'/');
   pp2 = strchr((char *) file2+1,'/');
   if (! pp1 || ! pp2) return 2;

   if (strmatch(file1,file2)) Frename = 1;

   if (Frename) {                                                                //  same top directory
      err = rename(file1,file2);
      if (err) return errno;
      else return 0;
   }

   err = shell_quiet("mv -f %s %s",file1,file2);                                 //  not
   return err;
}


/**************************************************************************/

//  Check if a directory exists. If not, ask user if it should be created.
//  Returns 0 if OK or +N if error or user refused to create.
//  The user is notified of failure, no other message needed.

int  check_create_dir(const char *path)
{
   int      err, yn;
   STATB    statbuf;

   err = stat(path,&statbuf);                                                    //  check status
   if (! err) {
      if (S_ISDIR(statbuf.st_mode)) return 0;                                    //  exists, directory, OK
      else {
         zmessageACK(0,"%s \n %s",path,strerror(ENOTDIR));                       //  exists, not a directory
         return ENOTDIR;
      }
   }

   if (errno != ENOENT) {
      zmessageACK(0,"%s \n %s",path,strerror(errno));                            //  error other than missing
      return errno;
   }

   yn = zmessageYN(0,ZTX("create directory? \n %s"),path);                       //  ask to create
   if (! yn) return ENOENT;

   err = shell_ack("mkdir -p -m 0750 %s",path);                                  //  create. rwx, rx, no access
   if (! err) return 0;

   zmessageACK(0,"%s \n %s",path,strerror(errno));                               //  failed to create
   return errno;
}


/**************************************************************************

   utility to measure CPU time spent in various functions or code blocks

   cpu_profile_init()            initialize at start of test
   cpu_profile_enter(fnum)       at entry to a function
   cpu_profile_exit(fnum)        at exit from a function
   cpu_profile_report()          report CPU time per function

   Methodology: cpu_profile_init() starts a thread that suspends and runs every
   1 millisecond and updates a timer.
   cpu_profile_enter() and cpu_profile_exit() accumulate the time difference between
   entry and exit of code being measured. This may be zero because of the long interval
   between timer updates. Accuracy comes from statistical sampling over many seconds,
   so that if the time spent in a monitored function is significant, it will be accounted
   for. The accuracy is better than 1% as long as the measured function or code block
   consumes a second or more of CPU time during the measurement period.
   The "fnum" argument (1-99) designates the function or code block being measured.
   cpu_profile_report() stops the timer thread and reports time consumed per function,
   using the "fnum" tags in the report.
   The functions cpu_profile_enter() and cpu_profile_exit() subtract the timer
   difference and add to a counter per fnum, so the added overhead is insignificant.
   They are inline functions defined as follows:
   enter:  cpu_profile_timer = cpu_profile_elapsed;
   exit:   cpu_profile_table[fnum] += cpu_profile_elapsed - cpu_profile_timer;

***************************************************************************/

volatile double   cpu_profile_table[100];
volatile double   cpu_profile_timer;
volatile double   cpu_profile_elapsed;
volatile int      cpu_profile_kill = 0;

void cpu_profile_init()
{
   void *   cpu_profile_timekeeper(void *);

   for (int ii = 0; ii < 99; ii++)
      cpu_profile_table[ii] = 0;
   cpu_profile_elapsed = 0;
   start_detached_thread(cpu_profile_timekeeper,null);
}

void cpu_profile_report()
{
   cpu_profile_kill++;

   printz("elapsed: %.2f \n",cpu_profile_elapsed);

   for (int ii = 0; ii < 100; ii++)
   {
      double dtime = cpu_profile_table[ii];
      if (dtime) printz("cpu profile func: %d  time: %.2f \n",ii,dtime);
   }
}

void * cpu_profile_timekeeper(void *)
{
   timeval  time0, time1;

   gettimeofday(&time0,0);

   while (true)
   {
      gettimeofday(&time1,0);
      cpu_profile_elapsed = time1.tv_sec - time0.tv_sec
              + 0.000001 * (time1.tv_usec - time0.tv_usec);
      zsleep(0.001);
      if (cpu_profile_kill) break;
   }

   cpu_profile_kill = 0;
   pthread_exit(0);
}


/**************************************************************************

    strField()

    cchar * strField(cchar *string, cchar *delim, int Nth)

    Get the Nth field in input string, which contains at least N fields
    delimited by the character(s) in delim (e.g. blank, comma).

    Returns a pointer to the found field (actually a pointer to a
    copy of the found field, with a null terminator appended).

    If a delimiter is immediately followed by another delimiter, it is
    considered a field with zero length, and the string "" is returned.

    Leading blanks in a field are omitted from the returned field.
    A field with only blanks is returned as a single blank.

    The last field may be terminated by null or a delimiter.

    Characters within quotes (") are treated as data within a field,
    i.e. blanks and delimiters are not processed as such.
    The quotes are removed from the returned field.

    If there are less than Nth fields, a null pointer is returned.

    The last 100 fields are saved and recycled in a circular manner.
    The caller does not have to free memory. If more memory depth is
    needed, caller must copy the returned data elsewhere.

    The output string may be modified if the length is not increased.
    Fields within the input string must not exceed 2000 characters.

    Example: input string: ,a,bb,  cc,   ,dd"ee,ff"ggg,
             (first and last characters are comma)
             delimiter: comma
             Nth   returned string
              1:   (null string)
              2:   a
              3:   bb
              4:   cc
              5:   (one blank)
              6:   ddee,ffggg
              7:   (null pointer >> no more fields)

***************************************************************************/

cchar * strField(cchar *string, cchar *delim, int Nth)
{
   static int     ftf = 1, nret = 0;
   static char    *retf[100];
   char           *pf1, pf2[2000];                                               //  2000 limit       5.2
   cchar          quote = '"';
   int            ii, nf, fcc = 0;
   static char    blankstring[2], nullstring[1];

   if (! string) return 0;                                                       //  bad call         5.3
   if (Nth < 1) return 0;

   if (ftf)                                                                      //  overall first call
   {
      ftf = 0;
      for (ii = 0; ii < 100; ii++) retf[ii] = 0;
      strcpy(blankstring," ");
      *nullstring = 0;
   }

   pf1 = (char *) string - 1;                                                    //  start parse
   nf = 0;

   while (nf < Nth)
   {
      pf1++;                                                                     //  start field
      nf++;
      fcc = 0;

      while (*pf1 == ' ') pf1++;                                                 //  skip leading blanks

      while (true)
      {
         if (*pf1 == quote) {                                                    //  pass chars between single quotes
            pf1++;                                                               //  (but without the quotes)
            while (*pf1 && *pf1 != quote) pf2[fcc++] = *pf1++;
            if (*pf1 == quote) pf1++;
         }

         else if (strchr(delim,*pf1) || *pf1 == 0) break;                        //  found delimiter or null

         else pf2[fcc++] = *pf1++;                                               //  pass normal character

         if (fcc > 1999) zappcrash("strField() too long");                       //  5.2
      }

      if (*pf1 == 0) break;
   }

   if (nf < Nth) return 0;                                                       //  no Nth field

   if (fcc == 0) {                                                               //  empty field
      if (*string && pf1[-1] == ' ' && ! strchr(delim,' '))                      //  all blanks and non-blank delimeter,
         return blankstring;                                                     //     return one blank
      if (*pf1 == 0) return 0;                                                   //  no field
      return nullstring;                                                         //  return null string
   }

   if (++nret == 100) nret = 0;                                                  //  use next return slot
   if (retf[nret]) zfree(retf[nret]);
   retf[nret] = (char *) zmalloc(fcc+2);
   strncpy0(retf[nret],pf2,fcc+1);
   return retf[nret];
}

cchar * strField(cchar *string, cchar delim, int Nth)                            //  alternative with one delimiter
{
   char     delims[2] = "x";

   *delims = delim;
   return strField(string,delims,Nth);
}


/**************************************************************************

   stat = strParms(begin, input, pname, maxcc, pval)

   Parse an input string with parameter names and values:
     "pname1=pval1 | pname2 | pname3=pval3 | pname4 ..."

   begin    int &          must be 1 to start new string, is modified
   input    cchar *   input string
   pname    char *         output parameter name
   maxcc    int            max. length for pname, including null
   pval     double &       output parameter value
   stat     int            status: 0=OK, -1=EOL, 1=parse error

   Each call returns the next pname and pval.
   A pname with no pval is assigned a value of 1 (present).
   Input format:  pname1 | pname2=pval2 | pname3 ... null
   Leading blanks are ignored, and pnames may have imbedded blanks.
   pvals must convert to double using convSD (accepts decimal point or comma)

***/

int strParms(int &begin, cchar *input, char *pname, int maxcc, double &pval)
{
   static int     ii, beginx = 3579246;
   cchar          *pnamex, *delim;
   int            cc, err;

   if (begin == 1) {                                                             //  start new string
      begin = ++beginx;
      ii = 0;
   }

   if (begin != beginx) zappcrash("strParms call error");                        //  thread safe, not reentrant

   *pname = 0;                                                                   //  initz. outputs to nothing
   pval = 0;

   while (input[ii] == ' ') ii++;                                                //  skip leading blanks
   if (input[ii] == 0) return -1;                                                //  no more data

   pnamex = input + ii;                                                          //  next pname

   for (cc = 0; ; cc++)
   {                                                                             //  look for delimiter
      if (pnamex[cc] == '=') break;
      if (pnamex[cc] == '|') break;
      if (pnamex[cc] == 0) break;
   }

   if (cc == 0) return 1;                                                        //  err: 2 delimiters
   if (cc >= maxcc) return 1;                                                    //  err: pname too big

   strncpy0(pname,pnamex,cc+1);                                                  //  pname >> caller
   strTrim(pname);                                                               //  remove trailing blanks

   if (pnamex[cc] == 0) {                                                        //  pname + null
      ii += cc;                                                                  //  position for next call
      pval = 1.0;                                                                //  pval = 1 >> caller
      return 0;
   }

   if (pnamex[cc] == '|') {                                                      //  pname + |
      ii += cc + 1;                                                              //  position for next call
      pval = 1.0;                                                                //  pval = 1 >> caller
      return 0;
   }

   ii += cc + 1;                                                                 //  pname = pval
   err = convSD(input + ii, pval, &delim);                                       //  parse pval   (was strtod()
   if (err > 1) return 1;
   while (*delim == ' ') delim++;                                                //  skip poss. trailing blanks
   if (*delim && *delim != '|') return 1;                                        //  err: delimiter not | or null
   ii = delim - input;
   if (*delim) ii++;                                                             //  position for next call
   return 0;
}


/**************************************************************************/

//  Produce random value from hashed input string.
//  Output range is 0 to max-1.
//  Benchmark: 0.036 usec for 20 char. string  3.3 GHz Core i5

int strHash(cchar *string, int max)
{
   unsigned int     hash = 1;
   uchar    byte;

   while ((byte = *string++))
   {
      hash *= byte;
      hash = hash ^ (hash >> 7);
      hash = hash & 0x00FFFFFF;
   }

   hash = hash % max;
   return hash;
}


/**************************************************************************/

//  Copy string with specified max. length (including null terminator).
//  truncate if needed. null terminator is always supplied.
//  Returns 0 if no truncation, 1 if input string was truncated to fit.

int strncpy0(char *dest, cchar *source, unsigned int cc)
{
   strncpy(dest,source,cc);
   dest[cc-1] = 0;
   if (strlen(source) >= cc) return 1;                                           //  truncated
   else return 0;
}


/**************************************************************************/

//  Copy string with blank pad to specified length.  No null is added.

void strnPad(char *dest, cchar *source, int cc)
{
   strncpy(dest,source,cc);
   int ii = strlen(source);
   for (int jj = ii; jj < cc; jj++) dest[jj] = ' ';
}


/**************************************************************************/

//  Remove trailing blanks from a string. Returns remaining length.

int strTrim(char *dest, cchar *source)
{
   if (dest != source) strcpy(dest,source);
   return strTrim(dest);
}

int strTrim(char *dest)
{
   int  ii = strlen(dest);
   while (ii && (dest[ii-1] == ' ')) dest[--ii] = 0;
   return ii;
}


/**************************************************************************/

//  Remove leading and trailing blanks from a string.
//  Returns remaining length, possibly zero.

int strTrim2(char *dest, cchar *source)
{
   cchar       *pp1, *pp2;
   int         cc;

   pp1 = source;
   pp2 = source + strlen(source) - 1;
   while (*pp1 == ' ') pp1++;
   while (*pp2 == ' ' && pp2 > pp1) pp2--;
   cc = pp2 - pp1 + 1;
   memmove(dest,pp1,cc);
   dest[cc] = 0;
   return cc;
}

int strTrim2(char *string)
{
   return strTrim2(string,(cchar *) string);
}


/**************************************************************************/

//  Remove all blanks from a string. Returns remaining length.

int strCompress(char *dest, cchar *source)
{
   if (dest != source) strcpy(dest,source);
   return strCompress(dest);
}

int strCompress(char *string)
{
   int   ii, jj;

   for (ii = jj = 0; string[ii]; ii++)
   {
      if (string[ii] != ' ')
      {
         string[jj] = string[ii];
         jj++;
      }
   }
   string[jj] = 0;
   return jj;
}


/**************************************************************************/

//  Concatenate multiple strings, staying within a specified overall length.
//  The destination string is also the first source string.
//  Null marks the end of the source strings (omission --> crash).
//  Output is truncated to fit within the specified length.
//  A final null is assured and is included in the length.
//  Returns 0 if OK, 1 if truncation was needed.

int strncatv(char *dest, int maxcc, cchar *source, ...)
{
   cchar       *ps;
   va_list     arglist;

   maxcc = maxcc - strlen(dest) - 1;
   if (maxcc < 0) return 1;
   va_start(arglist,source);
   ps = source;

   while (ps)
   {
      strncat(dest,ps,maxcc);
      maxcc = maxcc - strlen(ps);
      if (maxcc < 0) break;
      ps = va_arg(arglist,cchar *);
   }

   va_end(arglist);
   if (maxcc < 0) return 1;
   return 0;
}


/**************************************************************************/

//  Match 1st string to N additional strings.
//  Return matching string number 1 to N or 0 if no match.
//  Supply a null argument for end of list.

int strmatchV(cchar *string, ...)
{
   int         match = 0;
   char        *stringN;
   va_list     arglist;

   va_start(arglist,string);

   while (1)
   {
      stringN = va_arg(arglist, char *);
      if (stringN == null)
      {
         va_end(arglist);
         return 0;
      }

      match++;
      if (strmatch(string,stringN))
      {
         va_end(arglist);
         return match;
      }
   }
}


/**************************************************************************/

//  convert string to upper case

void strToUpper(char *string)
{
   int         ii;
   char        jj;
   const int   delta = 'A' - 'a';

   for (ii = 0; (jj = string[ii]); ii++)
        if ((jj >= 'a') && (jj <= 'z')) string[ii] += delta;
}

void strToUpper(char *dest, cchar *source)
{
   strcpy(dest,source);
   strToUpper(dest);
}


/**************************************************************************/

//  convert string to lower case

void strToLower(char *string)
{
   int         ii;
   char        jj;
   const int   delta = 'a' - 'A';

   for (ii = 0; (jj = string[ii]); ii++)
        if ((jj >= 'A') && (jj <= 'Z')) string[ii] += delta;
}

void strToLower(char *dest, cchar *source)
{
   strcpy(dest,source);
   strToLower(dest);
}


/**************************************************************************/

//  Copy string strin to strout, replacing every occurrence
//  of the substring ssin with the substring ssout.
//  Returns the count of replacements, if any.
//  Replacement strings may be longer or shorter or have zero length.

int repl_1str(cchar *strin, char *strout, cchar *ssin, cchar *ssout)
{
   int         ccc, cc1, cc2, nfound;
   cchar       *ppp;

   cc1 = strlen(ssin);
   cc2 = strlen(ssout);
   nfound = 0;

   while ((ppp = strstr(strin,ssin)))
   {
      nfound++;
      ccc = ppp - strin;
      strncpy(strout,strin,ccc);
      strout += ccc;
      strin += ccc;
      strncpy(strout,ssout,cc2);
      strin += cc1;
      strout += cc2;
   }

   strcpy(strout,strin);
   return nfound;
}


/**************************************************************************/

//  Copy string strin to strout, replacing multiple substrings with replacement strings.
//  Multiple pairs of string arguments follow strout, a substring and a replacement string.
//  Last pair of string arguments must be followed by a null argument.
//  Returns the count of replacements, if any.
//  Replacement strings may be longer or shorter or have zero length.

int repl_Nstrs(cchar *strin, char *strout, ...)
{
   va_list     arglist;
   cchar       *ssin, *ssout;
   char        ftemp[XFCC];
   int         ftf, nfound;

   ftf = 1;
   nfound = 0;
   va_start(arglist,strout);

   while (true)
   {
      ssin = va_arg(arglist, char *);
      if (! ssin) break;
      ssout = va_arg(arglist, char *);

      if (ftf) {
         ftf = 0;
         nfound += repl_1str(strin,strout,ssin,ssout);
      }

      else {
         strcpy(ftemp,strout);
         nfound += repl_1str(ftemp,strout,ssin,ssout);
      }
   }

   va_end(arglist);
   return nfound;
}


/**************************************************************************/

//  Break up a long text string into lines no longer than cc2 chars.             //  6.0
//  If fake newlines ("\n") are found, replace them with real newlines.
//  Break unconditionally where newlines are found and remove them.
//  Break at last char = blank between cc1 and cc2 if present.
//  Break at last delimiter char between cc1 and cc2 if present.
//  Break unconditionally at cc2 if none of the above.
//  Returns text lines in txout[*] with count as returned function value.
//  txout and txout[*] are subjects for zfree().

int breakup_text(cchar *txin0, char **&txout, cchar *delims, int cc1, int cc2)
{
   char     *txin;
   uchar    ch;
   int      p1, p2, cc3, Nout;
   int      Np, Bp, Sp;

   txin = zstrdup(txin0);
   txout = (char **) zmalloc(100 * sizeof(char *));

   if (strstr(txin0,"\\n"))                                                      //  replace "\n" with real newline chars
      repl_1str(txin0,txin,"\\n","\n");

   Nout = p1 = 0;

   while (true)
   {
      p2 = p1;                                                                   //  input line position
      cc3 = 0;                                                                   //  output line cc

      Np = Bp = Sp = 0;

      while (txin[p2]) {                                                         //  scan further up to cc2 chars
         ch = txin[p2];
         if (ch == '\n') { Np = p2; break; }                                     //  break out if newline found
         if (cc3 >= cc1) {
            if (ch == ' ') Bp = p2;                                              //  remember last ' ' found after cc1 chars
            if (strchr(delims,ch))  Sp = p2;                                     //  remember last delimiter found after cc1
         }
         if (ch < 0)
            while ((ch = txin[p2+1] < 0xC0)) p2++;
         p2++;
         cc3++;
         if (cc3 == cc2) break;
      }

      if (! cc3 && ! Np) break;
      if (Np) cc3 = Np - p1;
      else if (Bp) cc3 = Bp - p1 + 1;
      else if (Sp) cc3 = Sp - p1 + 1;
      else cc3 = p2 - p1;
      txout[Nout] = (char *) zmalloc(cc3+1);
      strncpy0(txout[Nout],txin+p1,cc3+1);
      Nout++;
      p2 = p1 + cc3;
      if (Np) p2++;
      p1 = p2;
      if (Nout == 100) break;
   }

   zfree(txin);
   return Nout;
}


/**************************************************************************/

//  Copy and convert string to hex string.
//  Each input character 'A' >> 3 output characters "41 "

void strncpyx(char *out, cchar *in, int ccin)
{
   int      ii, jj, c1, c2;
   char     cx[] = "0123456789ABCDEF";

   if (! ccin) ccin = strlen(in);

   for (ii = 0, jj = 0; ii < ccin; ii++, jj += 3)
   {
      c1 = (uchar) in[ii] >> 4;
      c2 = in[ii] & 15;
      out[jj] = cx[c1];
      out[jj+1] = cx[c2];
      out[jj+2] = ' ';
   }
   out[jj] = 0;
   return;
}


/**************************************************************************/

//  Strip trailing zeros from ascii floating numbers
//    (e.g. 1.230000e+02  -->  1.23e+02)

void StripZeros(char *pNum)
{
   int     ii, cc;
   int     pp, k1, k2;
   char    work[20];

   cc = strlen(pNum);
   if (cc >= 20) return;

   for (ii = 0; ii < cc; ii++)
   {
      if (pNum[ii] == '.')
      {
         pp = ii;
         k1 = k2 = 0;
         for (++ii; ii < cc; ii++)
         {
            if (pNum[ii] == '0')
            {
               if (! k1) k1 = k2 = ii;
               else k2 = ii;
               continue;
            }

            if ((pNum[ii] >= '1') && (pNum[ii] <= '9'))
            {
               k1 = 0;
               continue;
            }

            break;
         }

         if (! k1) return;

         if (k1 == pp + 1) k1++;
         if (k2 < k1) return;
         strcpy(work,pNum);
         strcpy(work+k1,pNum+k2+1);
         strcpy(pNum,work);
         return;
      }
   }
}


/**************************************************************************/

//  test for blank/null string
//  Returns status depending on input string:
//    0 not a blank or null string
//    1 argument string is NULL
//    2 string has zero length (*string == 0)
//    3 string is all blanks

int blank_null(cchar *string)
{
   if (! string) return 1;                                                       //  null string
   if (! *string) return 2;                                                      //  zero length string
   int cc = strlen(string);
   for (int ii = 0; ii < cc; ii++)
      if (string[ii] != ' ') return 0;                                           //  non-blank string
   return 3;                                                                     //  blank string
}


/**************************************************************************/

//  clean \x escape sequences and replace them with the escaped character
//    \n >> newline  \" >> doublequote  \\ >> backslash   etc.
//  see  $ man ascii  for the complete list

int clean_escapes(char *string)
{
   char     *pp1 = string, *pp2 = string, *pp;
   char     char1;
   char     escapes[] = "abtnvfr";
   int      count = 0;

   while (true)
   {
      char1 = *pp1++;

      if (char1 == 0) {
         *pp2 = 0;
         return count;
      }

      else if (char1 == '\\')  {
         char1 = *pp1++;
         pp = strchr(escapes,char1);
         if (pp) char1 = pp - escapes + 7;
         count++;
      }

      *pp2++ = char1;
   }
}


/**************************************************************************/

//  Compute the graphic character count for a UTF8 character string.
//  Depends on UTF8 rules:
//    - ascii characters are positive (0x00 to 0x7F)
//    - 1st char of multichar sequence is negative (0xC0 to 0xFD)
//    - subsequent multichars are in the range 0x80 to 0xBF

int utf8len(cchar *utf8string)
{
   int      ii, cc;
   char     xlimit = 0xC0;

   for (ii = cc = 0; utf8string[ii]; ii++)
   {
      if (utf8string[ii] < 0)                                                    //  multibyte character
         while (utf8string[ii+1] < xlimit) ii++;                                 //  skip extra bytes
      cc++;
   }

   return cc;
}


/**************************************************************************/

//  Extract a UTF8 substring with a specified count of graphic characters.
//    utf8in     input UTF8 string
//    utf8out    output UTF8 string, which must be long enough
//    pos        initial graphic character position to get (0 = first)
//    cc         max. count of graphic characters to get
//    returns    number of graphic characters extracted, <= cc
//  Output string is null terminated after last extracted character.

int utf8substring(char *utf8out, cchar *utf8in, int pos, int cc)
{
   int      ii, jj, kk, posx, ccx;
   char     xlimit = 0xC0;

   for (ii = posx = 0; posx < pos && utf8in[ii]; ii++)
   {
      if (utf8in[ii] < 0)
         while (utf8in[ii+1] < xlimit) ii++;
      posx++;
   }

   jj = ii;

   for (ccx = 0; ccx < cc && utf8in[jj]; jj++)
   {
      if (utf8in[jj] < 0)
         while (utf8in[jj+1] < xlimit) jj++;
      ccx++;
   }

   kk = jj - ii;

   strncpy(utf8out,utf8in+ii,kk);
   utf8out[kk] = 0;

   return   ccx;
}


/**************************************************************************/

//  check a string for valid utf8 encoding
//  returns:  0 = OK,  1 = bad string

int utf8_check(cchar *string)
{
   cchar             *pp;
   unsigned char     ch1, ch2, nch;

   for (pp = string; *pp; pp++)
   {
      ch1 = *pp;
      if (ch1 < 0x7F) continue;
      if (ch1 > 0xBF && ch1 < 0xE0) nch = 1;
      else if (ch1 < 0xF0) nch = 2;
      else if (ch1 < 0xF8) nch = 3;
      else if (ch1 < 0xFC) nch = 4;
      else if (ch1 < 0xFE) nch = 5;
      else return 1;
      while (nch) {
         pp++;
         ch2 = *pp;
         if (ch2 < 0x80 || ch2 > 0xBF) return 1;
         nch--;
      }
   }

   return 0;
}


/**************************************************************************/

//  Find the Nth graphic character position within a UTF8 string
//    utf8in      input UTF8 string
//    Nth         graphic character position, zero based
//  returns starting character (byte) position of Nth graphic character
//  returns -1 if Nth is beyond the string length                                //  5.5

int utf8_position(cchar *utf8in, int Nth)
{
   int      ii, posx;
   char     xlimit = 0xC0;

   for (ii = posx = 0; posx < Nth && utf8in[ii]; ii++)
   {
      if (utf8in[ii] < 0)                                                        //  multi-byte character
         while (utf8in[ii+1] && utf8in[ii+1] < xlimit) ii++;                     //  traverse member bytes
      posx++;
   }

   if (utf8in[ii]) return ii;
   return -1;                                                                    //  5.5
}


/**************************************************************************

   Conversion Utilities

   convSI(string, inum, delim)                     string to int
   convSI(string, inum, low, high, delim)          string to int with range check

   convSD(string, dnum, delim)                     string to double
   convSD(string, dnum, low, high, delim)          string to double with range check

   convSF(string, fnum, delim)                     string to float
   convSF(string, fnum, low, high, delim)          string to float with range check

   convIS(inum, string, cc)                        int to string with returned cc

   convDS(fnum, digits, string, cc)                double to string with specified
                                                     digits of precision and returned cc

   string      input (cchar *) or output (char *)
   inum        input (int) or output (int &)
   dnum        input (double) or output (double &)
   delim       optional returned delimiter (null or cchar **)
   low, high   input range check (int or double)
   cc          output string length (int &)
   digits      input digits of precision (int) to be used for output string

   NOTE: decimal point may be comma or period.
         1000's separators must NOT be present.

   convIS and convDS also return the length cc of the string output.
   convDS accepts same formats as atof. Decimal point can be comma or period.
   convDS will use whatever format (f/e) gives the shortest result.
   Outputs like "e03" or "e+03" will be shortened to "e3".

   function status returned:
       0    normal conversion, no invalid digits, blank/null termination
       1    successful converstion, but trailing non-numeric found
       2    conversion OK, but outside specified limits
       3    null or blank string, converted to zero
       4    conversion error, invalid data in string
   overlapping statuses have following precedence: 4 3 2 1 0

***************************************************************************/

#define  max10  (0x7fffffff / 10)


//  Convert string to integer

int convSI(cchar *string, int &inum, cchar **delim)
{
   char        ch;
   int         sign = 0, digits = 0, tnb = 0;
   cchar       *pch = string;

   inum = 0;

   while ((ch = *pch) == ' ') pch++;                                             //  skip leading blanks

   if (ch == '-') sign = -1;                                                     //  process leading +/- sign
   if (ch == '+') sign = 1;                                                      //  (at most one sign character)
   if (sign) pch++;

   while ((*pch >= '0') && (*pch <= '9'))                                        //  process digits 0 - 9
   {
      if (inum > max10) goto conv_err;                                           //  value too big
      inum = 10 * inum + *pch - '0';
      digits++;
      pch++;
   }

   if (delim) *delim = pch;                                                      //  terminating delimiter
   if (*pch && (*pch != ' ')) tnb++;                                             //  not null or blank

   if (! digits)                                                                 //  no digits found
   {
      if (tnb) return 4;                                                         //  non-numeric (invalid) string
      else return 3;                                                             //  null or blank string
   }

   if (sign == -1) inum = -inum;                                                 //  negate if - sign

   if (! tnb) return 0;                                                          //  no trailing non-numerics
   else return 1;                                                                //  trailing non-numerics

conv_err:
   inum = 0;
   return 4;
}


int convSI(cchar *string, int & inum, int lolim, int hilim, cchar **delim)
{
   int   stat = convSI(string,inum,delim);

   if (stat > 2) return stat;                                                    //  invalid or null/blank
   if (inum < lolim) return 2;                                                   //  return 2 if out of limits
   if (inum > hilim) return 2;                                                   //  (has precedence over status 1)
   return stat;                                                                  //  limits OK, return 0 or 1
}


//  Convert string to double.

int convSD(cchar *string, double &dnum, cchar **delim)
{
   char        ch;
   int         ii, sign = 0, digits = 0, ndec = 0;
   int         exp = 0, esign = 0, edigits = 0, tnb = 0;
   cchar       *pch = string;

   static int  first = 1;
   static double  decimals[21], exponents[74];

   if (first)                                                                    //  first-time called
   {
      first = 0;                                                                 //  pre-calculate constants
      for (ii = 1; ii <= 20; ii++) decimals[ii] = pow(10.0,-ii);
      for (ii = -36; ii <= 36; ii++) exponents[ii+37] = pow(10.0,ii);
   }

   dnum = 0.0;

   while ((ch = *pch) == ' ') pch++;                                             //  skip leading blanks

   if (ch == '-') sign = -1;                                                     //  process leading +/- sign
   if (ch == '+') sign = 1;                                                      //  (at most one sign character)
   if (sign) pch++;

get_digits:

   while ((*pch >= '0') && (*pch <= '9'))                                        //  process digits 0 - 9
   {
      dnum = 10.0 * dnum + (*pch - '0');
      pch++;
      digits++;
      if (ndec) ndec++;
   }

   if ((*pch == '.') || (*pch == ','))                                           //  process decimal point
   {                                                                             //  (allow comma or period)
      if (ndec) goto conv_err;
      ndec++;
      pch++;
      goto get_digits;
   }

   if ((*pch == 'e') || (*pch == 'E'))                                           //  process optional exponent
   {
      pch++;
      if (*pch == '+') esign = 1;                                                //  optional +/- sign
      if (*pch == '-') esign = -1;
      if (esign) pch++;

      if ((*pch < '0') || (*pch > '9')) goto conv_err;                           //  1st digit
      exp = *pch - '0';
      edigits++;
      pch++;

      if ((*pch >= '0') && (*pch <= '9'))                                        //  optional 2nd digit
      {
         exp = 10 * exp + (*pch - '0');
         edigits++;
         pch++;
      }

      if ((exp < -36) || (exp > 36)) goto conv_err;                              //  exponent too big
   }

   if (delim) *delim = pch;                                                      //  terminating delimiter
   if (*pch && (*pch != ' ')) tnb++;                                             //  not null or blank

   if (!(digits + edigits))                                                      //  no digits found
   {
      if (tnb) return 4;                                                         //  non-numeric (invalid) string
      else return 3;                                                             //  null or blank string
   }

   if (ndec > 1) dnum = dnum * decimals[ndec-1];                                 //  compensate for decimal places

   if (sign == -1) dnum = - dnum;                                                //  negate if negative

   if (exp)
   {
      if (esign == -1) exp = -exp;                                               //  process exponent
      dnum = dnum * exponents[exp+37];
   }

   if (! tnb) return 0;                                                          //  no trailing non-numerics
   else return 1;                                                                //  trailing non-numerics

conv_err:
   dnum = 0.0;
   return 4;
}


int convSD(cchar *string, double &dnum, double lolim, double hilim, cchar **delim)
{
   int stat = convSD(string,dnum,delim);

   if (stat > 2) return stat;                                                    //  invalid or null/blank
   if (dnum < lolim) return 2;                                                   //  return 2 if out of limits
   if (dnum > hilim) return 2;                                                   //  (has precedence over status 1)
   return stat;                                                                  //  limits OK, return 0 or 1
}


int convSF(cchar *string, float &fnum, cchar **delim)                            //  5.2
{
   double   dnum;
   int      err;
   err = convSD(string,dnum,delim);
   fnum = dnum;
   return err;
}


int convSF(cchar *string, float &fnum, float lolim, float hilim, cchar **delim)     //  5.2
{
   double   dnum, dlolim = lolim, dhilim = hilim;
   int      err;
   err = convSD(string,dnum,dlolim,dhilim,delim);
   fnum = dnum;
   return err;
}


//  Convert int to string with returned length.
//  (will never exceed 12 characters)

int convIS(int inum, char *string, int *cc)
{
   int   ccc;

   ccc = snprintf(string,12,"%d",inum);
   if (cc) *cc = ccc;
   return 0;
}


//  Convert double to string with specified digits of precision.
//  Shortest length format (f/e) will be used.
//  Output length is returned in optional argument cc.
//  (will never exceed 20 characters)

int convDS(double dnum, int digits, char *string, int *cc)
{
   char     *pstr;

   snprintf(string,20,"%.*g",digits,dnum);

   pstr = strstr(string,"e+");                                                   //  1.23e+12  >  1.23e12
   if (pstr) strcpy(pstr+1,pstr+2);

   pstr = strstr(string,"e0");                                                   //  1.23e02  >  1.23e2
   if (pstr) strcpy(pstr+1,pstr+2);

   pstr = strstr(string,"e0");
   if (pstr) strcpy(pstr+1,pstr+2);

   pstr = strstr(string,"e-0");                                                  //  1.23e-02  >  1.23e-2
   if (pstr) strcpy(pstr+2,pstr+3);

   pstr = strstr(string,"e-0");
   if (pstr) strcpy(pstr+2,pstr+3);

   if (cc) *cc = strlen(string);

   return 0;
}


//  format a number as "123 B" or "12.3 KB" or "1.23 MB" etc.
//  prec is the desired digits of precision to output.
//  WARNING: only the last 100 conversions remain available in memory.
//  Example formats for 3 digits of precision:
//    123 B,  999 B,  1.23 KB,  98.7 KB,  456 KB,  2.34 MB,  45.6 GB,  1.23 GB

char * formatKBMB(double fnum, int prec)
{
   #define kilo 1000
   #define bmega (kilo*kilo)
   #define bgiga (kilo*kilo*kilo)

   cchar          *units;
   static char    *output[100];
   static int     ftf = 1, ii;
   double         gnum;

   if (ftf) {                                                                    //  keep last 100 conversions
      ftf = 0;
      for (ii = 0; ii < 100; ii++)
         output[ii] = (char *) zmalloc(20);
   }

   gnum = fabs(fnum);

   if (gnum > bgiga) {
      fnum = fnum / bgiga;
      units = "GB";
   }
   else if (gnum > bmega) {
      fnum = fnum / bmega;
      units = "MB";
   }
   else if (gnum > kilo) {
      fnum = fnum / kilo;
      units = "KB";
   }
   else units = "B ";

   gnum = fabs(fnum);
   if (prec == 2 && gnum >= 99.5) prec++;                                        //  avoid e+nn formats
   if (prec == 3 && gnum >= 999.5) prec++;
   if (prec == 4 && gnum >= 9999.5) prec++;
   if (prec == 5 && gnum >= 99999.5) prec++;
   if (prec == 6 && gnum >= 999999.5) prec++;

   if (++ii > 99) ii = 0;
   snprintf(output[ii],20,"%.*g %s",prec,fnum,units);
   return output[ii];
}


/**************************************************************************

    Wildcard string match

    Match candidate string to wildcard string containing any number of
    '*' or '?' wildcard characters. '*' matches any number of characters,
    including zero characters. '?' matches any one character.
    Returns 0 if match, 1 if no match.

    Benchmark: 0.032 usec.       wild = *asdf*qwer?yxc
               3.3 GHz Core i5   match = XXXasdfXXXXqwerXyxc

***************************************************************************/

int MatchWild(cchar *pWild, cchar *pString)
{
   int   ii, star;

new_segment:

   star = 0;
   while (pWild[0] == '*')
   {
      star = 1;
      pWild++;
   }

test_match:

   for (ii = 0; pWild[ii] && (pWild[ii] != '*'); ii++)
   {
      if (pWild[ii] != pString[ii])
      {
         if (! pString[ii]) return 1;
         if (pWild[ii] == '?') continue;
         if (! star) return 1;
         pString++;
         goto test_match;
      }
   }

   if (pWild[ii] == '*')
   {
      pString += ii;
      pWild += ii;
      goto new_segment;
   }

   if (! pString[ii]) return 0;
   if (ii && pWild[ii-1] == '*') return 0;
   if (! star) return 1;
   pString++;
   goto test_match;
}


/**************************************************************************

    Wildcard string match - ignoring case
    Works like MatchWild() above, but case is ignored.

***/

int MatchWildIgnoreCase(cchar *pWild, cchar *pString)                            //  5.5
{
   int   ii, star;

new_segment:

   star = 0;
   while (pWild[0] == '*')
   {
      star = 1;
      pWild++;
   }

test_match:

   for (ii = 0; pWild[ii] && (pWild[ii] != '*'); ii++)
   {
      if (! strmatchcaseN(pWild+ii,pString+ii,1))                                //  the only difference
      {
         if (! pString[ii]) return 1;
         if (pWild[ii] == '?') continue;
         if (! star) return 1;
         pString++;
         goto test_match;
      }
   }

   if (pWild[ii] == '*')
   {
      pString += ii;
      pWild += ii;
      goto new_segment;
   }

   if (! pString[ii]) return 0;
   if (ii && pWild[ii-1] == '*') return 0;
   if (! star) return 1;
   pString++;
   goto test_match;
}


/**************************************************************************

   SearchWild  - wildcard file search

   Find all files with total /pathname/filename matching a pattern,
   which may have any number of the wildcard characters '*' and '?'
   in either or both the pathname and filename.

   cchar * SearchWild(cchar *wfilespec, int &flag)

   inputs:  flag = 1 to start a new search
            flag = 2 abort a running search
            *** do not modify flag within a search ***

            wfilespec = filespec to search with optional wildcards
               e.g. "/name1/na*me2/nam??e3/name4*.ext?"

   return:  a pointer to one matching file is returned per call,
            or null when there are no more matching files.

   The search may be aborted before completion, but make a final
   call with flag = 2 to clean up temp file. A new search with
   flag = 1 will also finish the cleanup.

   NOT THREAD SAFE - do not use in parallel threads

   shell find command is used for the initial search because this
   is much faster than recursive use of readdir() (why?).

   (#) is used in place of (*) in comments below to prevent
       the compiler from interpreting (#/) as end of comments

   GNU find peculiarities:
     find /path/#      omits "." files
     find /path/       includes "." files
     find /path/#      recurses directories under /path/
     find /path/#.txt  does not recurse directories
     find /path/#/     finds all files under /path/
     find /path/#/#    finds files >= 1 directory level under /path/
     find /path/xxx#   never finds anything

   SearchWild uses simpler rules:
     '/' and '.' are treated like all other characters and match '#' and '?'
     no files are excluded except pure directories
     /path/#.txt finds all xxx.txt files under /path/ at all levels
     (because #.txt matches aaa.txt, /aaa/bbb.txt, etc.)

   Benchmark: search for /usr/share/#/README, find 457 from 101K files
              1.9 secs.   3.3 GHz Core i5  SSD disk

***/

cchar * SearchWild(cchar *wpath, int &uflag)
{
   static FILE    *fid = 0;
   static char    matchfile[XFCC];
   char           searchpath[XFCC];
   char           command[XFCC];
   int            cc, err;
   char           *pp;

   if ((uflag == 1) || (uflag == 2)) {                                           //  first call or stop flag
      if (fid) {
         pclose(fid);                                                            //  if file open, close it
         fid = 0;
      }
   }

   if (uflag == 2) return 0;                                                     //  kill flag, done

   if (uflag == 1)                                                               //  first call flag
   {
      cc = strlen(wpath);
      if (cc == 0) return 0;
      if (cc > XFCC-20) zappcrash("SearchWild: wpath > XFCC");

      pp = (char *) wpath;
      repl_Nstrs(pp,searchpath,"$","\\$","\"","\\\"",null);                      //  init. search path, escape $ and "

      pp = strchr(searchpath,'*');
      if (pp) {                                                                  //  not efficient but foolproof
         while ((*pp != '/') && (pp > searchpath)) pp--;                         //  /aaa/bbb/cc*cc... >>> /aaa/bbb/
         if (pp > searchpath) *(pp+1) = 0;
      }

      snprintf(command,XFCC,"find \"%s\" -type f -or -type l",searchpath);       //  find files (ordinary, symlink)
      fid = popen(command,"r");
      if (! fid) zappcrash(strerror(errno));
      uflag = 763568954;                                                         //  begin search
   }

   if (uflag != 763568954) zappcrash("SearchWild, uflag invalid");

   while (true)
   {
      pp = fgets(matchfile,XFCC-2,fid);                                          //  next matching file
      if (! pp) {
         pclose(fid);                                                            //  no more
         fid = 0;
         return 0;
      }

      cc = strlen(matchfile);                                                    //  get rid of trailing \n
      matchfile[cc-1] = 0;

      err = MatchWild(wpath,matchfile);                                          //  wildcard match?
      if (err) continue;                                                         //  no

      return matchfile;                                                          //  return file
   }
}


/**************************************************************************

    SearchWildIgnoreCase  - wildcard file search - ignoring case
    Works like SearchWild() above, but case of file name is ignored.

    Actually, the trailing part of the path name is also case-insensitive,
    meaning that it is possible to get more matches than technically correct
    if directories like this are present:
       /AAA/BBB/.../filename
       /AAA/bbb/.../filename

***/

cchar * SearchWildIgnoreCase(cchar *wpath, int &uflag)                           //  5.5
{
   static FILE    *fid = 0;
   static char    matchfile[XFCC];
   char           searchpath[XFCC];
   char           command[XFCC];
   int            cc, err;
   char           *pp;

   if ((uflag == 1) || (uflag == 2)) {                                           //  first call or stop flag
      if (fid) {
         pclose(fid);                                                            //  if file open, close it
         fid = 0;
      }
   }

   if (uflag == 2) return 0;                                                     //  kill flag, done

   if (uflag == 1)                                                               //  first call flag
   {
      cc = strlen(wpath);
      if (cc == 0) return 0;
      if (cc > XFCC-20) zappcrash("SearchWild: wpath > XFCC");

      pp = (char *) wpath;
      repl_Nstrs(pp,searchpath,"$","\\$","\"","\\\"",null);                      //  init. search path, escape $ and "

      pp = strchr(searchpath,'*');
      if (pp) {                                                                  //  not efficient but foolproof
         while ((*pp != '/') && (pp > searchpath)) pp--;                         //  /aaa/bbb/cc*cc... >>> /aaa/bbb/
         if (pp > searchpath) *(pp+1) = 0;
      }

      snprintf(command,XFCC,"find \"%s\" -type f -or -type l",searchpath);       //  find files (ordinary, symlink)
      fid = popen(command,"r");
      if (! fid) zappcrash(strerror(errno));
      uflag = 763568954;                                                         //  begin search
   }

   if (uflag != 763568954) zappcrash("SearchWild, uflag invalid");

   while (true)
   {
      pp = fgets(matchfile,XFCC-2,fid);                                          //  next matching file
      if (! pp) {
         pclose(fid);                                                            //  no more
         fid = 0;
         return 0;
      }

      cc = strlen(matchfile);                                                    //  get rid of trailing \n
      matchfile[cc-1] = 0;

      err = MatchWildIgnoreCase(wpath,matchfile);                                //  wildcard match?
      if (err) continue;                                                         //  no

      return matchfile;                                                          //  return file
   }
}


/**************************************************************************/

//  perform a binary search on sorted list of integers
//  return matching element or -1 if not found
//  Benchmark: search a list of 10 million sorted integers
//             0.35 usecs.  3.3 GHz Core i5

int bsearch(int seekint, int nn, int list[])
{
   int      ii, jj, kk, rkk;

   ii = nn / 2;                                                                  //  next element to search
   jj = (ii + 1) / 2;                                                            //  next increment
   nn--;                                                                         //  last element
   rkk = 0;

   while (true)
   {
      kk = list[ii] - seekint;                                                   //  check element

      if (kk > 0)
      {
         ii -= jj;                                                               //  too high, go down
         if (ii < 0) return -1;
      }

      else if (kk < 0)
      {
         ii += jj;                                                               //  too low, go up
         if (ii > nn) return -1;
      }

      else if (kk == 0) return ii;                                               //  matched

      jj = jj / 2;                                                               //  reduce increment

      if (jj == 0)
      {
         jj = 1;                                                                 //  step by 1 element
         if (! rkk) rkk = kk;                                                    //  save direction
         else
         {
            if (rkk > 0) { if (kk < 0) return -1; }                              //  if change direction, fail
            else if (kk > 0) return -1;
         }
      }
   }
}


//  Perform a binary search on sorted set of records in memory.
//  Return matching record number (0 based) or -1 if not found.
//  Benchmark: search 10 million sorted records of 20 chars.
//             0.61 usecs.  3.3 GHz Core i5

int bsearch(cchar *seekrec, cchar *allrecs, int recl, int nrecs)
{
   int      ii, jj, kk, rkk;

   ii = nrecs / 2;                                                               //  next element to search
   jj = (ii + 1) / 2;                                                            //  next increment
   nrecs--;                                                                      //  last element
   rkk = 0;

   while (true)
   {
      kk = strcmp(allrecs+ii*recl,seekrec);                                      //  compare member rec to seek rec

      if (kk > 0)
      {
         ii -= jj;                                                               //  too high, go down in set
         if (ii < 0) return -1;
      }

      else if (kk < 0)
      {
         ii += jj;                                                               //  too low, go up in set
         if (ii > nrecs) return -1;
      }

      else if (kk == 0) return ii;                                               //  matched

      jj = jj / 2;                                                               //  reduce increment

      if (jj == 0)
      {
         jj = 1;                                                                 //  step by 1 element
         if (! rkk) rkk = kk;                                                    //  save direction
         else
         {
            if (rkk > 0) { if (kk < 0) return -1; }                              //  if change direction, fail
            else if (kk > 0) return -1;
         }
      }
   }
}


//  Perform a binary search on sorted set of pointers to records in memory.
//  Return matching record number (0 based) or -1 if not found.
//  The pointers are sorted in the order of the records starting at char N.
//  The records need not be sorted.
//  The string length of seekrec is compared.

int bsearch(cchar *seekrec, cchar **allrecs, int N, int nrecs)
{
   int      ii, jj, kk, rkk;

   ii = nrecs / 2;                                                               //  next element to search
   jj = (ii + 1) / 2;                                                            //  next increment
   nrecs--;                                                                      //  last element
   rkk = 0;

   while (true)
   {
      kk = strcmp(allrecs[ii]+N,seekrec);                                        //  compare member rec to seek rec  6.0

      if (kk > 0)
      {
         ii -= jj;                                                               //  too high, go down in set
         if (ii < 0) return -1;
      }

      else if (kk < 0)
      {
         ii += jj;                                                               //  too low, go up in set
         if (ii > nrecs) return -1;
      }

      else if (kk == 0) return ii;                                               //  matched

      jj = jj / 2;                                                               //  reduce increment

      if (jj == 0)
      {
         jj = 1;                                                                 //  step by 1 element
         if (! rkk) rkk = kk;                                                    //  save direction
         else
         {
            if (rkk > 0) { if (kk < 0) return -1; }                              //  if change direction, fail
            else if (kk > 0) return -1;
         }
      }
   }
}


/**************************************************************************
   heap sort functions

   void HeapSort(int list[], int nn)
   void HeapSort(float flist[], int nn)
   void HeapSort(double dlist[], int nn)
   -------------------------------------
   Sort list of nn integers, floats, or doubles.
   Numbers are sorted in ascending order. 

   void HeapSort(char *plist[], int nn)
   ------------------------------------
   Pointers are sorted in order of the strings they point to. 
   The strings are not changed.

   void HeapSort(char *plist1[], char *plist2[], int nn)
   -----------------------------------------------------
   Sort two lists of pointers to two sets of strings.
   Both lists are sorted in order of the first set of strings.

   void HeapSort(char *plist[], int nn, compfunc)
   ----------------------------------------------
   Sort list of pointers to strings in user-defined order.
   Pointers are sorted, strings are not changed.

   void HeapSort(char *recs, int RL, int NR, compfunc)
   ---------------------------------------------------
   Sort an array of records in memory using a caller-supplied compare function.
      recs  pointer to 1st record in array
      RL    record length
      NR    no. of records

   int compfunc(cchar *rec1, cchar *rec2)
   --------------------------------------
   compare rec1 to rec2, return -1 0 +1  if rec1 < = > rec2  in sort order.

   Benchmarks: (3.3 GHz Core i5)
      10 million integers: 1.5 secs
      10 million doubles: 2.4 secs
      2 million pointers to 100 character recs: 1.8 secs

***************************************************************************/

#define SWAP(x,y) (temp = (x), (x) = (y), (y) = temp)


//  heapsort for array of integers

static void adjust(int vv[], int n1, int n2)
{
   int   *bb, jj, kk, temp;

   bb = vv - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && bb[kk] < bb[kk+1]) kk++;
      if (bb[jj] < bb[kk]) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(int vv[], int nn)
{
   int   *bb, jj, temp;

   for (jj = nn/2; jj > 0; jj--) adjust(vv,jj,nn);

   bb = vv - 1;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb[1], bb[jj+1]);
      adjust(vv,1,jj);
   }
}


//  heapsort for array of floats

static void adjust(float vv[], int n1, int n2)
{
   float    *bb, temp;
   int      jj, kk;

   bb = vv - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && bb[kk] < bb[kk+1]) kk++;
      if (bb[jj] < bb[kk]) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(float vv[], int nn)
{
   float    *bb, temp;
   int      jj;

   for (jj = nn/2; jj > 0; jj--) adjust(vv,jj,nn);

   bb = vv - 1;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb[1], bb[jj+1]);
      adjust(vv,1,jj);
   }
}


//  heapsort for array of doubles

static void adjust(double vv[], int n1, int n2)
{
   double   *bb, temp;
   int      jj, kk;

   bb = vv - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && bb[kk] < bb[kk+1]) kk++;
      if (bb[jj] < bb[kk]) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(double vv[], int nn)
{
   double   *bb, temp;
   int      jj;

   for (jj = nn/2; jj > 0; jj--) adjust(vv,jj,nn);

   bb = vv - 1;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb[1], bb[jj+1]);
      adjust(vv,1,jj);
   }
}


//  heapsort array of pointers to strings in ascending order of strings
//  pointers are sorted, strings are not changed.

static void adjust(char *vv[], int n1, int n2)
{
   char     **bb, *temp;
   int      jj, kk;

   bb = vv - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && strcmp(bb[kk],bb[kk+1]) < 0) kk++;
      if (strcmp(bb[jj],bb[kk]) < 0) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(char *vv[], int nn)
{
   char     **bb, *temp;
   int      jj;

   for (jj = nn/2; jj > 0; jj--) adjust(vv,jj,nn);

   bb = vv;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb[0], bb[jj]);
      adjust(vv,1,jj);
   }
}


//  Heapsort 2 lists of pointers to 2 parallel sets of strings
//    in ascending order of the first set of strings.
//  Both lists of pointers are sorted together in tandem.
//  Pointers are sorted, strings are not changed.

static void adjust(char *vv1[], char *vv2[], int n1, int n2)                     //  6.0
{
   char     **bb1, **bb2, *temp;
   int      jj, kk;

   bb1 = vv1 - 1;
   bb2 = vv2 - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && strcmp(bb1[kk],bb1[kk+1]) < 0) kk++;
      if (strcmp(bb1[jj],bb1[kk]) < 0) {
         SWAP(bb1[jj],bb1[kk]);
         SWAP(bb2[jj],bb2[kk]);
      }
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(char *vv1[], char *vv2[], int nn)
{
   char     **bb1, **bb2, *temp;
   int      jj;

   for (jj = nn/2; jj > 0; jj--) adjust(vv1,vv2,jj,nn);

   bb1 = vv1;
   bb2 = vv2;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb1[0], bb1[jj]);
      SWAP(bb2[0], bb2[jj]);
      adjust(vv1,vv2,1,jj);
   }
}


//  heapsort array of pointers to strings in user-defined order.
//  pointers are sorted, strings are not changed.

static void adjust(char *vv[], int n1, int n2, HeapSortUcomp fcomp)
{
   char     **bb, *temp;
   int      jj, kk;

   bb = vv - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      if (kk < n2 && fcomp(bb[kk],bb[kk+1]) < 0) kk++;
      if (fcomp(bb[jj],bb[kk]) < 0) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(char *vv[], int nn, HeapSortUcomp fcomp)
{
   char     **bb, *temp;
   int      jj;

   for (jj = nn/2; jj > 0; jj--) adjust(vv,jj,nn,fcomp);

   bb = vv;

   for (jj = nn-1; jj > 0; jj--)
   {
      SWAP(bb[0], bb[jj]);
      adjust(vv,1,jj,fcomp);
   }
}


//  heapsort for array of records,
//  using caller-supplied record compare function.
//  HeapSortUcomp returns [ -1 0 +1 ]  for  rec1 [ < = > ] rec2
//  method: build array of pointers and sort these, then
//  use this sorted array to re-order the records at the end.

static int     *vv1, *vv2;

static void adjust(char *recs, int RL, int n1, int n2, HeapSortUcomp fcomp)
{
   int      *bb, jj, kk, temp;
   char     *rec1, *rec2;

   bb = vv1 - 1;
   jj = n1;
   kk = n1 * 2;

   while (kk <= n2)
   {
      rec1 = recs + RL * bb[kk];
      rec2 = recs + RL * bb[kk+1];
      if (kk < n2 && fcomp(rec1,rec2) < 0) kk++;
      rec1 = recs + RL * bb[jj];
      rec2 = recs + RL * bb[kk];
      if (fcomp(rec1,rec2) < 0) SWAP(bb[jj],bb[kk]);
      jj = kk;
      kk *= 2;
   }
}

void HeapSort(char *recs, int RL, int NR, HeapSortUcomp fcomp)
{
   int      *bb, jj, kk, temp, flag;
   char     *vvrec;

   vv1 = new int[NR];
   for (jj = 0; jj < NR; jj++) vv1[jj] = jj;

   for (jj = NR/2; jj > 0; jj--) adjust(recs,RL,jj,NR,fcomp);

   bb = vv1 - 1;

   for (jj = NR-1; jj > 0; jj--)
   {
      SWAP(bb[1], bb[jj+1]);
      adjust(recs,RL,1,jj,fcomp);
   }

   vv2 = new int[NR];
   for (jj = 0; jj < NR; jj++) vv2[vv1[jj]] = jj;

   vvrec = new char[RL];
   flag = 1;
   while (flag)
   {
      flag = 0;
      for (jj = 0; jj < NR; jj++)
      {
         kk = vv2[jj];
         if (kk == jj) continue;
         memmove(vvrec,recs+jj*RL,RL);
         memmove(recs+jj*RL,recs+kk*RL,RL);
         memmove(recs+kk*RL,vvrec,RL);
         SWAP(vv2[jj],vv2[kk]);
         flag = 1;
      }
   }

   delete vv1;
   delete vv2;
   delete vvrec;
}


/**************************************************************************

   int MemSort (char *RECS, int RL, int NR, int KEYS[][3], int NK)

   RECS is an array of records, to be sorted in-place.
   (record length = RL, record count = NR)

   KEYS[NK][3]  is an integer array defined as follows:
        [N][0]    starting position of Nth key field in RECS
        [N][1]    length of Nth key field in RECS
        [N][2]    type of sort for Nth key:
                    1 = char ascending
                    2 = char descending
                    3 = int*4 ascending
                    4 = int*4 descending
                    5 = float*4 ascending
                    6 = float*4 descending
                    7 = float*8 ascending (double)
                    8 = float*8 descending
   Benchmark: 2 million recs of 40 bytes with 4 sort keys:
              2.5 secs (3.3 GHz Core i5).

***/

int MemSortComp(cchar *rec1, cchar *rec2);
int MemSortKeys[10][3], MemSortNK;

int MemSort(char *RECS, int RL, int NR, int KEYS[][3], int NK)
{
   int   ii;

   if (NR < 2) return 1;

   if (NK > 10) zappcrash("MemSort, bad NK");
   if (NK < 1) zappcrash("MemSort, bad NK");

   MemSortNK = NK;

   for (ii = 0; ii < NK; ii++)
   {
      MemSortKeys[ii][0] = KEYS[ii][0];
      MemSortKeys[ii][1] = KEYS[ii][1];
      MemSortKeys[ii][2] = KEYS[ii][2];
   }

   HeapSort(RECS,RL,NR,MemSortComp);
   return 1;
}

int MemSortComp(cchar *rec1, cchar *rec2)
{
   int            ii, stat, kpos, ktype, kleng;
   int            inum1, inum2;
   float          rnum1, rnum2;
   double         dnum1, dnum2;
   cchar          *p1, *p2;

   for (ii = 0; ii < MemSortNK; ii++)                                            //  loop each key
   {
      kpos = MemSortKeys[ii][0];                                                 //  relative position
      kleng = MemSortKeys[ii][1];                                                //  length
      ktype = MemSortKeys[ii][2];                                                //  type

      p1 = rec1 + kpos;                                                          //  absolute position
      p2 = rec2 + kpos;

      switch (ktype)
      {
         case 1:                                                                 //  char ascending
            stat = strncmp(p1,p2,kleng);                                         //  compare 2 key values
            if (stat) return stat;                                               //  + if rec1 > rec2, - if <
            break;                                                               //  2 keys are equal, check next key

         case 2:                                                                 //  char descending
            stat = strncmp(p1,p2,kleng);
            if (stat) return -stat;
            break;

         case 3:                                                                 //  int ascending
            memmove(&inum1,p1,4);
            memmove(&inum2,p2,4);
            if (inum1 > inum2) return 1;
            if (inum1 < inum2) return -1;
            break;

         case 4:                                                                 //  int descending
            memmove(&inum1,p1,4);
            memmove(&inum2,p2,4);
            if (inum1 > inum2) return -1;
            if (inum1 < inum2) return 1;
            break;

         case 5:                                                                 //  float ascending
            memmove(&rnum1,p1,4);
            memmove(&rnum2,p2,4);
            if (rnum1 > rnum2) return 1;
            if (rnum1 < rnum2) return -1;
            break;

         case 6:                                                                 //  float descending
            memmove(&rnum1,p1,4);
            memmove(&rnum2,p2,4);
            if (rnum1 > rnum2) return -1;
            if (rnum1 < rnum2) return 1;
            break;

         case 7:                                                                 //  double ascending
            memmove(&dnum1,p1,8);
            memmove(&dnum2,p2,8);
            if (dnum1 > dnum2) return 1;
            if (dnum1 < dnum2) return -1;
            break;

         case 8:                                                                 //  double descending
            memmove(&dnum1,p1,8);
            memmove(&dnum2,p2,8);
            if (dnum1 > dnum2) return -1;
            if (dnum1 < dnum2) return 1;
            break;

         default:                                                                //  key type not 1-8
            zappcrash("MemSort, bad KEYS sort type");
      }
   }

   return 0;                                                                     //  records match on all keys
}


/**************************************************************************
   variable string list functions - array / list of strings

   pvlist * pvlist_create(int max)
   void pvlist_free(pvlist *pv)
   int pvlist_append(pvlist *pv, cchar *entry, int unique)
   int pvlist_prepend(pvlist *pv, cchar *entry, int unique)
   int pvlist_find(pvlist *pv, cchar *entry)
   int pvlist_remove(pvlist *pv, cchar *entry)
   int pvlist_remove(pvlist *pv, int Nth)
   int pvlist_count(pvlist *pv)
   int pvlist_replace(pvlist *pv, int Nth, char *entry)
   cchar * pvlist_get(pvlist *pv, int Nth)
   int pvlist_sort(pvlist *pv)

   These functions manage a variable length list of variable length strings.
   Declare such a list as: pvlist *pv;

***************************************************************************/

//  Creates a pvlist with a capacity of max strings and returns a pointer.
//  String lengths are unlimited, but the count of strings is limited to max.
//  Memory is allocated for max pointers at first. Memory for the strings is
//  allocated and freed as the strings are added or removed.

//  variable string list functions ========================================


static pvlist * pvlist_create(int max);                                                 //  create pvlist
static void pvlist_free(pvlist *pv);                                                    //  free pvlist
static int pvlist_append(pvlist *pv, cchar *entry, int unique = 0);                     //  append new entry (opt. if unique)
static int pvlist_prepend(pvlist *pv, cchar *entry, int unique = 0);                    //  prepend new entry (opt. if unique)
static int pvlist_find(pvlist *pv, cchar *entry);                                       //  find entry by name
static int pvlist_remove(pvlist *pv, int Nth);                                          //  remove entry by number (0...)
static int pvlist_count(pvlist *pv);                                                    //  return entry count
static char * pvlist_get(pvlist *pv, int Nth);                                          //  return Nth entry (0...)

static pvlist * pvlist_create(int max)
{
   pvlist      *pv;

   pv = (pvlist *) zmalloc(sizeof(pvlist));
   pv->max = max;
   pv->act = 0;
   pv->list = (char **) zmalloc(max * sizeof(char *));
   return pv;
}


//  free memory for variable list and contained strings

static void pvlist_free(pvlist *pv)
{
   int      ii;

   for (ii = 0; ii < pv->act; ii++)
      zfree(pv->list[ii]);
   zfree(pv->list);
   zfree(pv);
}


//  append new entry to end of list (optional if unique)
//  if list if full, first entry is removed and rest are packed down
//  return: N >= 0: new entry added at position N
//          N = -1: not unique, not added

static int pvlist_append(pvlist *pv, cchar *entry, int unique)
{
   int      ii;

   if (unique && pvlist_find(pv,entry) >= 0) return -1;                          //  not unique

   if (pv->act == pv->max) pvlist_remove(pv,0);                                  //  if list full, remove 1st entry

   ii = pv->act;
   pv->list[ii] = zstrdup(entry);                                                //  add to end of list
   pv->act++;
   return ii;
}


//  prepend new entry to list (optional if unique)
//  prior list entries are pushed down to make room
//  if list is full, last entry is removed first
//  return: N = 0: new entry added at position 0
//          N = -1: not unique, not added

static int pvlist_prepend(pvlist *pv, cchar *entry, int unique)
{
   int      ii;

   if (unique && pvlist_find(pv,entry) >= 0) return -1;                          //  not unique

   if (pv->act == pv->max) pvlist_remove(pv,pv->act-1);                          //  if list full, remove last entry

   for (ii = pv->act; ii > 0; ii--)                                              //  push all list entries down
      pv->list[ii] = pv->list[ii-1];
   pv->list[0] = zstrdup(entry);                                                 //  add to start of list
   pv->act++;
   return 0;
}


//  find list entry by name, return entry (0 based)
//  return -1 if not found

static int pvlist_find(pvlist *pv, cchar *entry)
{
   int      ii;

   for (ii = 0; ii < pv->act; ii++)
      if (strmatch(entry,pv->list[ii])) break;
   if (ii < pv->act) return ii;
   return -1;
}



//  remove an entry by number and repack list
//  returns -1 if entry is beyond list end

static int pvlist_remove(pvlist *pv, int ii)
{
   if (ii < 0 || ii >= pv->act) return -1;
   zfree(pv->list[ii]);
   for (++ii; ii < pv->act; ii++) {                                              //  pre-increment                   5.6
      if (! pv->act) printz("meaningless reference %d",ii);                      //  stop g++ optimization bug  ///
      pv->list[ii-1] = pv->list[ii];
   }
   pv->act--;
   return 0;
}


//  return entry count

static int pvlist_count(pvlist *pv)
{
   return pv->act;
}


//  return Nth entry or null

static char * pvlist_get(pvlist *pv, int Nth)
{
   if (Nth >= pv->act) return 0;
   return pv->list[Nth];
}


/**************************************************************************/

//  Random number generators with explicit context
//  and improved randomness over a small series.
//  Benchmark: lrandz 0.012 usec  drandz 0.014 usec  3.3 GHz Core i5

int lrandz(int64 *seed)                                                          //  returns 0 to 0x7fffffff
{
   *seed = *seed ^ (*seed << 17);
   *seed = *seed ^ (*seed << 20);
   return nrand48((unsigned int16 *) seed);
}

int lrandz()                                                                     //  implicit seed, repeatable sequence
{
   static int64   seed = 12345678;
   return lrandz(&seed);
}

double drandz(int64 *seed)                                                       //  returns 0.0 to 0.99999...
{
   *seed = *seed ^ (*seed << 17);
   *seed = *seed ^ (*seed << 20);
   return erand48((unsigned int16 *) seed);
}

double drandz()                                                                  //  implicit seed, repeatable sequence
{
   static int64   seed = 23459876;
   return drandz(&seed);
}


/**************************************************************************

   spline1: define a curve using a set of data points (x and y values)
   spline2: for a given x-value, return a y-value fitting the curve

   For spline1, the no. of curve-defining points must be < 100.
   For spline2, the given x-value must be within the range defined in spline1.

   The algorithm was taken from the book "Numerical Recipes"
   (Cambridge University Press) and converted from Fortran to C++.

***/

namespace splinedata
{
   int      nn;
   float    px1[100], py1[100], py2[100];
}


void spline1(int dnn, float *dx1, float *dy1)
{
   using namespace splinedata;

   float    sig, p, u[100];
   int      ii;

   nn = dnn;
   if (nn > 100) zappcrash("spline1(), > 100 data points");

   for (ii = 0; ii < nn; ii++)
   {
      px1[ii] = dx1[ii];
      py1[ii] = dy1[ii];
      if (ii && px1[ii] <= px1[ii-1])
         zappcrash("spline1(), x-value not increasing");
   }

   py2[0] = u[0] = 0;

   for (ii = 1; ii < nn-1; ii++)
   {
      sig = (px1[ii] - px1[ii-1]) / (px1[ii+1] - px1[ii-1]);
      p = sig * py2[ii-1] + 2;
      py2[ii] = (sig - 1) / p;
      u[ii] = (6 * ((py1[ii+1] - py1[ii]) / (px1[ii+1] - px1[ii]) - (py1[ii] - py1[ii-1])
            / (px1[ii] - px1[ii-1])) / (px1[ii+1] - px1[ii-1]) - sig * u[ii-1]) / p;
   }

   py2[nn-1] = 0;

   for (ii = nn-2; ii >= 0; ii--)
      py2[ii] = py2[ii] * py2[ii+1] + u[ii];

   return;
}


float  spline2(float x)
{
   using namespace splinedata;

   int      kk, klo = 0, khi = nn-1;
   float    h, a, b, y;

   while (khi - klo > 1)
   {
      kk = (khi + klo) / 2;
      if (px1[kk] > x) khi = kk;
      else klo = kk;
   }

   h = px1[khi] - px1[klo];
   a = (px1[khi] - x) / h;
   b = (x - px1[klo]) / h;
   y = a * py1[klo] + b * py1[khi] + ((a*a*a - a) * py2[klo]
                                   + (b*b*b - b) * py2[khi]) * (h*h) / 6;

   return y;
}

/**************************************************************************

   Initialize application files according to following conventions:              //  new version
     + binary executable is at:  /prefix/bin/appname                             //  = PREFIX/bin/appname
     + other application directories are derived as follows:
         /prefix/share/appname/data/            desktop, parameters ...
         /prefix/share/doc/appname/             README, changelog, userguide-xx.html ...
         /prefix/share/appname/icons/           icon files: filename.png
         /prefix/share/appname/locales/         translate-xx.po ... (original)
         /home/user/.appname/                   some installation files are copied here
         /home/user/.appname/logfile            log file with error messages

   zprefix        install location              normally /usr, subdirs /bin /share /doc
   zdatadir       installed data files          /prefix/share/appname/data/
   zdocdir        documentation files           /prefix/share/doc/appname/
   zicondir       icons                         /prefix/share/appname/icons/
   zlocalesdir    translation files             /prefix/share/appname/locales/
   zuserdir       local app files               /home/<user>/.appname

   If it does not already exist, an application directory for the current user is
   created at /home/username/.appname (following common Linux convention).
   If this directory was created for the first time, copy specified files
   (following the 1st argument) from the install directory into the newly created
   user-specific directory. The assumption is that all initial data files for the
   application (e.g. parameters) will be in the install data directory, and these are
   copied to the user directory where the user or application can modify them.

   If the running program is not connected to a terminal device, stdout and stderr are
   redirected to the log file at /home/user/.appname/logfile

   char * get_zprefix()      returns install top directory (has /bin and /share under it)
   char * get_zuserdir()     returns /home/user/.appname (or /root/.appname)
   char * get_zdatadir()     returns directory where application data files reside
   char * get_zdocdir()      returns directory for application documentation files
   char * get_zicondir()     returns directory for application icons
   char * get_zlocalesdir()  returns directory for translation files

***/

const std::string& get_zprefix() { return zfuncs::zprefix; }                                //  /usr or /home/<userid>
cchar * get_zuserdir() { return zfuncs::zuserdir; }                              //  /home/user/.local/share/appname
cchar * get_zdatadir() { return zfuncs::zdatadir; }                              //  parameters, icons
cchar * get_zdocdir()  { return zfuncs::zdocdir;  }                              //  documentation files
cchar * get_zicondir()  { return zfuncs::zicondir;  }                            //  icon files
cchar * get_zlocalesdir()  { return zfuncs::zlocalesdir;  }                      //  translation files


int zinitapp(cchar *appname)
{
   using namespace zfuncs;

   std::string work;
   char        logfile[200], oldlog[200];
   int         cc, secs, err;
   time_t      Tnow;
   char        *chTnow;
   STATB       statdat;
   FILE        *fid;

   zappname = appname;                                                     //  save app name                   5.6

   #ifndef PREFIX                                                                //  install location
      #define PREFIX "/usr"
   #endif

   work = PREFIX;                                                    //  /usr or /home/<userid>
   zprefix = work;                                                         //  /prefix
   strncatv(zdatadir,199,work.c_str(),"/share/",zappname.c_str(),"/data",null);                  //  /prefix/share/appname/data
   strncatv(zicondir,199,work.c_str(),"/share/",zappname.c_str(),"/icons",null);                 //  /prefix/share/appname/icons
   strncatv(zlocalesdir,199,work.c_str(),"/share/",zappname.c_str(),"/locales",null);            //  /prefix/share/appname/locales
   strncatv(zdocdir,199,work.c_str(),"/share/doc/",zappname.c_str(),null);                       //  /prefix/share/doc/appname

   #ifdef DOCDIR
      strncpy0(zdocdir,DOCDIR,199);                                              //  flexible DOCDIR location
   #endif

   if (!getenv("HOME")) {
      zappcrash("The environment variable \"HOME\" must be set");
   }

   const char *xdg_data_dir = getenv("XDG_DATA_HOME"); 
	  
   if (xdg_data_dir) {
      snprintf(zuserdir,sizeof(zuserdir),"%s/%s",xdg_data_dir,zappname.c_str());
   }
   else {
      snprintf(zuserdir,sizeof(zuserdir),"%s/.local/share/%s",getenv("HOME"),zappname.c_str());  //  /home/<username>/.local/share/appname/
   }
   cc = strlen(zuserdir);                                                        //  stop humongous username         5.3
   if (cc > 160) zappcrash("too big: %s",zuserdir);

   err = stat(zuserdir,&statdat);                                                //  does it exist already?
   if (err) {
      err = mkdir(zuserdir,0750);                                                //  no, create and initialize
      if (err) zappcrash("cannot create %s",zuserdir);
      shell_quiet("cp -R %s/* %s",zdatadir,zuserdir);                            //  copy initial data files         6.0
   }

   tid_main = pthread_self();                                                    //  thread ID of main() process
   Tnow = time(0);
   chTnow = ctime(&Tnow);
   chTnow[19] = 0;

   if (! isatty(fileno(stdin))) {                                                //  not attached to a terminal      5.7
      snprintf(logfile,199,"%s/logfile",zuserdir);                               //  /home/<user>/logfile            5.5
      snprintf(oldlog,199,"%s/logfile.old",zuserdir);
      err = stat(logfile,&statdat);
      if (! err) {
         secs = Tnow - statdat.st_mtime;                                         //  if log file age > 1 hour        5.0
         if (secs > 3600) rename(logfile,oldlog);                                //    rename to *.old
      }
      fid = freopen(logfile,"a",stdout);                                         //  redirect output to log file
      fid = freopen(logfile,"a",stderr);
      if (! fid) printz("*** cannot redirect stdout and stderr \n");
   }

   printz("\n =========== start %s %s \n",zappname.c_str(),chTnow);
   fflush(0);                                                                    //  5.2

   return 1;
}


//  Find a locale-dependent installation file or user file.
//    file type: doc, data, locale, user  [ userlocale removed v.6.1 ]
//    file name: README, changelog, userguide.html, parameters, translate.po ...
//  Returns complete file name, e.g. /usr/share/appname/locales/translate-de.po
//  Output filespec should be 200 bytes (limit for all installation files).
//  Returns 0 if OK, +N if not found.

int locale_filespec(cchar *filetype, cchar *filename, char *filespec)
{
   using namespace zfuncs;

   char     *pp, fname[20], fext[8];
   char     lc_RC[8];                                                            // -lc or -lc_RC
   int      cc, err;
   STATB    statb;

   filespec[0] = '/';
   strcat(filespec,filetype);                                                    //  leave /type as default

   if (strmatch(filetype,"doc")) strcpy(filespec,zdocdir);                       //  /usr/share/doc/appname
   if (strmatch(filetype,"data")) strcpy(filespec,zdatadir);                     //  /usr/share/appname/data
   if (strmatch(filetype,"locale")) strcpy(filespec,zlocalesdir);                //  /usr/share/appname/locales
   if (strmatch(filetype,"user")) strcpy(filespec,zuserdir);                     //  /home/<user>/.appname

   strncpy0(fname,filename,20);
   pp = strchr(fname,'.');
   if (pp) {
      strcpy(fext,pp);                                                           //  file type  .fext
      *pp = 0;
   }
   else *fext = 0;                                                               //  no type

   lc_RC[0] = '-';
   strncpy0(lc_RC+1,zlang,6);                                                    //  locale with region code: -lc_RC

tryextras:

   cc = strlen(filespec);
   filespec[cc] = '/';                                                           //  /directories.../
   strcpy(filespec+cc+1,fname);                                                  //  /directories.../fname
   cc = strlen(filespec);                                                        //                       |
   pp = filespec + cc;                                                           //                       pp

   strcpy(pp,lc_RC);                                                             //  /directories.../fname-lc_RC.fext
   strcat(pp,fext);
   err = stat(filespec,&statb);
   if (! err) return 0;

   strcpy(pp+3,fext);                                                            //  /directories.../fname-lc.fext
   err = stat(filespec,&statb);
   if (! err) return 0;

   strcpy(pp,"-en");                                                             //  /directories.../fname-en.fext
   strcat(pp,fext);
   err = stat(filespec,&statb);
   if (! err) return 0;

   strcpy(pp,fext);                                                              //  /directories.../fname.fext
   err = stat(filespec,&statb);
   if (! err) return 0;

   if (strmatch(filetype,"doc")) {                                               //  these files may be placed in
      strcpy(filespec,zdocdir);                                                  //    /usr/share/doc/appname/extras
      strcat(filespec,"/extras");                                                //       due to Linux chaos
      filetype = "";
      goto tryextras;                                                            //  try again using /extras
   }

   return 1;                                                                     //  not found
}


/**************************************************************************/

//  Display help file in a separate process so application is not blocked.
//  help file: /zdatadir/userguide-lc_RC.html (or) *-lc.html (or) *-en.html
//  context: optional arg. show file starting at internal link = context
//  look for user guide file in  /usr/share/data/appname/ [ extras/ ]

void showz_userguide(cchar *context)                                             //  5.5
{
   char     filespec[200], url[200];
   int      err;

   err = locale_filespec("data","userguide.html",filespec);

   if (err) {
      zmessageACK(0,ZTX("user guide not found"));
      return;
   }

   snprintf(url,199,"file://%s",filespec);

   if (context && *context)                                                      //  specific topic wanted
      strncatv(url,199,"#",context,null);                                        //  file://.../userguide-xx.html#context

   showz_html(url);
   return;
}


/**************************************************************************/

//  display application log file in a popup window
//  The log file is /home/<user>/.appname/logfile

void showz_logfile()                                                             //  log file      5.2
{
   using namespace zfuncs;

   char buff[200];

   fflush(0);                                                                    //  5.2
   snprintf(buff,199,"cat %s/logfile",zuserdir);
   popup_command(buff,800,600);
   return;
}


//  find and show a text file in /usr/share/doc/appname/
//                            or /usr/share/appname/data
//  the text file may also be a compressed .gz file
//  type is "doc" or "data"

void showz_textfile(const char *type, const char *file)                          //  5.5
{
   char     filex[40], filespec[200], command[200];
   int      err;

   strncpy0(filex,file,36);                                                      //  look for gzip file first      5.7
   strcat(filex,".gz");

   err = locale_filespec(type,filex,filespec);
   if (! err) {
      snprintf(command,200,"zcat %s",filespec);
      popup_command(command,600,400,0,1);
      return;
   }

   strncpy0(filex,file,36);                                                      //  look for uncompressed file

   err = locale_filespec(type,filex,filespec);
   if (! err) {
      snprintf(command,200,"cat %s",filespec);
      popup_command(command,600,400,0,1);
      return;
   }

   zmessageACK(0,"file not found: %s %s",type,file);
   return;
}


//  show a local or remote html file using the user's preferred browser
//  to show a local file starting at an internal live link location:
//    url = "file://directory/.../filename#livelink

void showz_html(cchar *url)
{
   static char    prog[20];
   static int     ftf = 1, err;

   if (ftf) {
      ftf = 0;
      *prog = 0;
      err = system("which firefox");                                             //  use xdg-open only as last resort
      if (! err) strcpy(prog,"firefox");                                         //  5.2
      else {
         err = system("which chromium-browser");
         if (! err) strcpy(prog,"chromium-browser");
         else {
            err = system("which xdg-open");
            if (! err) strcpy(prog,"xdg-open");
         }
      }
   }

   if (! *prog) {
      zmessLogACK(null,"html file reader not found");
      return;
   }

   shell_ack("%s %s &",prog,url);
   return;
}


/**************************************************************************/

//  Creates a desktop icon / launcher and a system menu entry.
//  The menu name is taken from the input command, without options.
//  A command like "mycom -optA -optB" would generate a menu name of "mycom".
//  The categories should be separated by semicolons and conform to LSB categories.
//  The generic name is free text to describe the application, e.g. "Image Editor".
//  If the target system is not LSB compliant this function will not work.

void zmake_menu_launcher(cchar *command, cchar *categories, cchar *genericname)
{
   using namespace zfuncs;

   char     appname[20], dtdir[200], dtfile[200], work[200];
   cchar    *xdgcomm = "xdg-user-dir DESKTOP";
   cchar    *pp;
   FILE     *fid;

   pp = strField(command,' ',1);
   if (! pp) pp = "?";
   strncpy0(appname,pp,20);

   fid = popen(xdgcomm,"r");                                                     //  get desktop directory for user
   if (! fid) {                                                                  //  5.8
      zmessageACK(0,"%s \n %s",xdgcomm,strerror(errno));
      return;
   }
   int nn = fscanf(fid,"%s",dtdir);
   pclose(fid);
   if (nn != 1) {
      zmessageACK(0,"xdg-user-dir DESKTOP failed");
      return;
   }

   snprintf(dtfile,200,"%s/%s.desktop",dtdir,appname);

   fid = fopen(dtfile,"w");
   if (! fid) {
      zmessageACK(0,"%s \n %s",dtfile,strerror(errno));
      return;
   }

   fputs("[Desktop Entry]\n",fid);                                               //  [Desktop Entry]
   snprintf(work,199,"Name=%s\n",appname);                                       //  Name=appname
   fputs(work,fid);
   snprintf(work,199,"Categories=%s\n",categories);                              //  Categories=Cat1;Cat2; ...
   fputs(work,fid);
   snprintf(work,199,"GenericName=%s\n",genericname);                            //  GenericName=generic app name
   fputs(work,fid);
   fputs("Type=Application\n",fid);                                              //  Type=Application
   fputs("Terminal=false\n",fid);                                                //  Terminal=false
   snprintf(work,199,"Exec=%s/bin/%s\n",zprefix.c_str(),command);                        //  Exec=/usr/bin/appname -options
   fputs(work,fid);
   snprintf(work,199,"Icon=%s/%s.png\n",zicondir,appname);                       //  /usr/share/appname/icons/appname.png
   fputs(work,fid);
   fclose(fid);

   shell_ack("chmod 0750 %s",dtfile);                                            //  make executable
   shell_ack("xdg-desktop-menu install --novendor %s",dtfile);                   //  add menu entry
   return;
}


/**************************************************************************

   Translation Functions

   Translation files are standard .po files as used in the Gnu gettext
   system. However the .po files are used directly, and there is no need
   to merge and compile them into a binary format (.mo file).

   A translation file is one of: <zlocalesdir>/translate-lc.po  or  *-lc_RC.po
   where "lc" is a standard language code and "lc_RC" a language and region code.
   The file may also be compressed with the file type .po.gz

   Translation files contain two record types:
      msgid "english text"
      msgstr "translation text"

   The text strings may continue on multiple lines, each such segment enclosed in quotes.
   The strings may contain C-format codes (%s %d etc.) and may contain escaped special
   characters (\n etc.).

   A text string may have a context part "context::string", where "context" is any short
   string, "::" is a separator, and "string" is the string to translate or the translation
   of a string. This is to handle the case where a single English string may need multiple
   translations, depending on context. The English string may be present multiple times in
   a .po file, each one marked with a different context and having a different translation.
   The context part is optional in the translation strings and is not displayed in the GUI.

   Initialize translations:  int ZTXinit(cchar *lang)
      lang is "lc" or "lc_RC" or null (current locale will be used)

   Initializes the running application for the translation of text messages. It reads the
   translation file for the user's locale and builds a translation table for use by ZTX().

   lang:   2-character language code 'lc' ("de" "fr" "es" etc.)
           or 5-character language and region code 'lc_RC' ("de_AT" etc.)
           or null to use the current locale

   Status returned: 0 = OK, 1 = unable to find or process translation file.

   Translate a text string:  cchar *translation = ZTX(cchar *english)
      english: text string which may have printf formats (%d %s ...)
      translation: the returned equivalent translation

   If the user language is English or if no translation is found,
   the input string is returned, else the translated string.

   example:

      program code:

         printf(ZTX("answer: %d %s \n next line"), 123, "qwerty");

      A German .po file (translate-de.po) would have the following:

         msgid ""
         "answer: %d %s \n"
         " next line"

         msgstr ""
         "Antwort: %d %s \n"
         " nächste Zeile"

***************************************************************************/

namespace ZTXnames
{
   FILE        *fidr, *fidw;
   char        buff[ZTXmaxcc], *ppq1, *ppq2;
   char        *porec, *wporec;
   char        Etext[ZTXmaxcc], Ttext[ZTXmaxcc];                                 //  .po text: "line 1 %s \n" "line 2"
   char        **etext, **ttext;                                                 //  arrays, english and translations
   char        **estring, **tstring;                                             //  merged, un-quoted, un-escaped
   int         Ntext = 0;                                                        //  array counts
   void        ZTXgettext(char *text);
   char        *ZTXmergetext(cchar *text);
}


//  read and process .po file at application startup
//  prepare english strings and translations for quick access

void ZTXinit(cchar *lang)                                                        //  initialize translations
{
   using namespace zfuncs;
   using namespace ZTXnames;

   int      ii, err;
   char     installpo[200], localpo[200];
   char     *pp, poname[20];

   if (Ntext) {                                                                  //  free prior translation
      for (ii = 0; ii < Ntext; ii++) {
         zfree(etext[ii]);
         zfree(ttext[ii]);
         zfree(estring[ii]);
         zfree(tstring[ii]);
      }
      zfree(etext);
      zfree(ttext);
      zfree(estring);
      zfree(tstring);
      Ntext = 0;
   }

   etext = (char **) zmalloc(ZTXmaxent * sizeof(char *));                        //  english text and translations
   ttext = (char **) zmalloc(ZTXmaxent * sizeof(char *));                        //  (segmented, quoted, escaped)
   estring = (char **) zmalloc(ZTXmaxent * sizeof(char *));                      //  english strings and translations
   tstring = (char **) zmalloc(ZTXmaxent * sizeof(char *));                      //  (merged, un-quoted, un-escaped)

   if (lang && *lang) strncpy0(zlang,lang,6);                                    //  use language from caller
   else {                                                                        //  help Linux chaos
      pp = getenv("LANG");                                                       //  use $LANG if defined
      if (! pp) pp = getenv("LANGUAGE");                                         //  use $LANGUAGE if defined
      if (! pp) pp = setlocale(LC_MESSAGES,"");                                  //  use locale if defined
      if (pp) strncpy0(zlang,pp,6);                                              //  "lc_RC" language/region code
      else strcpy(zlang,"en");                                                   //  use English
   }

   if (*zlang < 'a') strcpy(zlang,"en");                                         //  use English if garbage
   printz("language: %s \n",zlang);

   if (strmatchN(zlang,"en",2)) return;                                          //  English, do nothing

   err = locale_filespec("locale","translate.po",installpo);                     //  look for installed .po file
   if (err) err = locale_filespec("locale","translate.po.gz",installpo);
   if (err) {
      printz("*** no translate-%s.po file found \n",zlang);
      return;
   }

   pp = strrchr(installpo,'/');                                                  //  extract filename from full path
   strcpy(poname,pp+1);
   pp = strstr(poname,".gz");                                                    //  uncompress or copy installed .po file
   if (pp) {
      *pp = 0;
      err = shell_ack("gunzip -c %s > %s/%s",installpo,zuserdir,poname);
   }
   else err = shell_ack("cp %s %s/%s",installpo,zuserdir,poname);
   if (err) return;

   snprintf(localpo,200,"%s/%s",zuserdir,poname);                                //  final local .po file name

   fidr = fopen(localpo,"r");                                                    //  open .po file
   if (! fidr) {
      printz("*** cannot open .po file: %s \n",localpo);
      return;
   }

   porec = 0;                                                                    //  no .po record yet
   *Etext = *Ttext = 0;                                                          //  no text yet

   while (true)
   {
      if (! porec) porec = fgets_trim(buff,ZTXmaxcc,fidr);                       //  get next .po record
      if (! porec) break;                                                        //  EOF

      if (blank_null(porec)) {                                                   //  blank record
         porec = 0;
         continue;
      }
      if (*porec == '#') {                                                       //  comment
         porec = 0;
         continue;
      }

      if (strmatchN(porec,"msgid",5))                                            //  start new english string
      {
         if (*Etext) {                                                           //  two in a row
            printz("no translation: %s \n",Etext);
            *Etext = 0;
         }

         if (*Ttext) {
            printz("orphan translation: %s \n",Ttext);
            *Ttext = 0;
         }

         porec += 5;                                                             //  get segmented text string
         ZTXgettext(Etext);                                                      //  "segment1 %s \n" "segment2" ...
      }

      else if (strmatchN(porec,"msgstr",6))                                      //  start new translation
      {
         porec += 6;                                                             //  get segmented string
         ZTXgettext(Ttext);

         if (! *Etext) {
            printz("orphan translation: %s \n",Ttext);
            *Ttext = 0;
            continue;
         }

         if (strlen(Ttext) < 3)                                                  //  translation is "" (quotes included)
            printz("no translation: %s \n",Etext);                               //  leave as ""
      }

      else
      {
         printz("unrecognized .po record: %s \n",porec);
         porec = 0;
         continue;
      }

      if (*Etext && *Ttext)                                                      //  have an english/translation pair
      {
         etext[Ntext] = zstrdup(Etext);                                          //  add to translation tables
         ttext[Ntext] = zstrdup(Ttext);
         *Etext = *Ttext = 0;
         Ntext++;
         if (Ntext == ZTXmaxent)                                                 //  cannot continue
            zappcrash("more than %d translations",ZTXmaxent);
      }
   }

   fclose(fidr);
   remove(localpo);                                                              //  remove local .po file              6.1

   printz(".po file has %d entries \n",Ntext);

   for (ii = 0; ii < Ntext; ii++)
   {
      pp = ZTXmergetext(etext[ii]);                                              //  merge segmented text strings
      estring[ii] = zstrdup(pp);
      pp = ZTXmergetext(ttext[ii]);
      tstring[ii] = zstrdup(pp);
   }

   HeapSort(estring, tstring, Ntext);                                            //  sort both strings, english order
   return;
}


//  private function
//  read and combine multiple 'msgid' or 'msgstr' quoted strings
//  output is one string with one or more quoted segments:
//    "text line 1 %s \n" "text line 2" ...
//  each segment comes from a different line in the input .po file

void ZTXnames::ZTXgettext(char *pstring)
{
   using namespace ZTXnames;

   int      cc, scc = 0;

   while (true)                                                                  //  join multiple quoted strings
   {
      while (*porec && *porec != '"') porec++;                                   //  find opening string quote
      if (! *porec) {
         porec = fgets_trim(buff,ZTXmaxcc,fidr);                                 //  get next .po record
         if (! porec) return;
         if (strmatchN(porec,"msgid",5)) return;                                 //  end of this string
         if (strmatchN(porec,"msgstr",6)) return;
      }
      ppq1 = porec;                                                              //  opening quote
      ppq2 = ppq1 + 1;
      while ((*ppq2 && *ppq2 != '"') ||                                          //  find closing (non-escaped) quote
             (*ppq2 == '"' && *(ppq2-1) == '\\')) ppq2++;
      if (! *ppq2) return;
      cc = ppq2 - ppq1 + 1;                                                      //  min. case is ""
      if (cc + 1 + scc >= ZTXmaxcc)
         printz("*** string is too long %s \n",pstring);
      else {
         strncpy0(pstring+scc,ppq1,cc+1);                                        //  accum. substrings, minus quotes
         scc += cc;
      }
      porec = ppq2 + 1;
   }

   return;
}


//  private function
//  convert quoted string segments into binary form that
//  matches the compiled string in the source program
//  (remove quotes, merge strings, un-escape \n \" etc.)

char * ZTXnames::ZTXmergetext(cchar *dirtystring)
{
   static char    cleanstring[ZTXmaxcc];
   int            ii, jj;

   strncpy0(cleanstring,dirtystring,ZTXmaxcc);
   clean_escapes(cleanstring);

   for (ii = jj = 0; cleanstring[ii]; ii++)
      if (cleanstring[ii] != '"')
         cleanstring[jj++] = cleanstring[ii];
   cleanstring[jj] = 0;
   return cleanstring;
}


//  Translate the input english string or return the input string.
//  Look for "context::string" and return "string" only if context found.
//  This function may need a few microseconds if thousands of strings must be searched.

cchar * ZTX(cchar *english)
{
   using namespace ZTXnames;

   cchar    *pp, *pp2;
   int      ii;

   if (! Ntext) return english;                                                  //  no translations

   ii = bsearch(english,(cchar **) estring,0,Ntext);
   if (ii < 0) pp = english;
   else pp = tstring[ii];

   if (strlen(pp) == 0) pp = english;                                            //  translation is ""      5.6

   for (pp2 = pp; *pp2 && pp2 < pp+30; pp2++)                                    //  remove context if present
      if (*pp2 == ':' && *(pp2+1) == ':') return pp2+2;

   return pp;
}


//  Find all untranslated strings and return them one per call.
//  Set ftf = 1 for first call, will be returned = 0.
//  Returns null after last untranslated string.

cchar * ZTX_missing(int &ftf)                                                    //  5.6
{
   using namespace ZTXnames;

   int         ii;
   static int  next;

   if (ftf) ftf = next = 0;

   for (ii = next; ii < Ntext; ii++)
      if (strlen(tstring[ii]) == 0) break;                                       //  translation is ""      5.6

   next = ii + 1;
   if (ii < Ntext) return estring[ii];                                           //  return english

   return 0;                                                                     //  EOL
}


/**************************************************************************
   GTK utility functions
**************************************************************************/

//  Iterate main loop every "skip" calls.
//  If called within the main() thread, does a GTK main loop to process menu events, etc.
//  You must do this periodically within long-running main() thread tasks if you wish to
//  keep menus, buttons, output windows, etc. alive and working. The skip argument will
//  cause the function to do nothing for skip calls, then perform the normal function.
//  This allows it to be imbedded in loops with little execution time penalty.
//  If skip = 100, zmainloop() will do nothing for 100 calls, execute normally, etc.
//  If called from a thread, zmainloop() does nothing.

void zmainloop(int skip)
{
   static int  xskip = 0;

   if (skip) {
      if (++xskip < skip) return;
      xskip = 0;
   }

   if (! pthread_equal(pthread_self(),zfuncs::tid_main)) return;                 //  thread caller, do nothing    5.0

   while (gtk_events_pending())                                                  //  gdk_flush() removed          5.2
      gtk_main_iteration_do(0);                                                  //  use gtk_main_iteration_do    5.2

   return;
}


/**************************************************************************/

//  crash if current execution is not the main() thread

void zthreadcrash()
{
   if (pthread_equal(pthread_self(),zfuncs::tid_main)) return;
   zappcrash("forbidden function called from thread");
   return;
}


//  get hardware information via GTK
//  failure results in null pointers

void get_hardware_info(GtkWidget *widget)                                        //  5.9
{
   using namespace zfuncs;

   GdkWindow   *window;

   if (mouse) return;                                                            //  already done

   zthreadcrash();
   
   if (! widget) {                                                               //  6.2
      display = gdk_display_get_default();
      if (display) screen = gdk_display_get_default_screen(display);
   }
   else {
      window = gtk_widget_get_window(widget);
      display = gdk_window_get_display(window);
      screen = gdk_window_get_screen(window);
   }

   if (screen) manager = gdk_display_get_device_manager(display);
   if (screen) settings = gtk_settings_get_for_screen(screen);                   //  6.2
   if (manager) mouse = gdk_device_manager_get_client_pointer(manager);
   if (! mouse) printz("*** GTK cannot get hardware info \n");
   return;
}


//  move the mouse pointer to given position in given window

int move_pointer(GtkWidget *widget, int px, int py)                              //  5.9
{
   using namespace zfuncs;

   int         rpx, rpy;
   GdkWindow   *window;

   zthreadcrash();

   get_hardware_info(widget);
   if (! mouse) return 0;
   window = gtk_widget_get_window(widget);
   gdk_window_get_root_coords(window,px,py,&rpx,&rpy);
   gdk_device_warp(mouse,screen,rpx,rpy);
   return 1;
}


/**************************************************************************/

//  write message to GTK text view window
//  line:   +N    existing lines from top (replace)
//          -N    existing lines from bottom (replace)
//           0    next line (add new line at bottom)
//  scroll logic assumes only one \n per message

void wprintx(GtkWidget *mLog, int line, cchar *message, int bold)
{
   GtkTextMark          *endMark;
   GtkTextBuffer        *textBuff;
   GtkTextIter          iter1, iter2;
   GtkTextTag           *fontag = 0;
   int                  nlines, scroll = 0;
   cchar                *normfont = "monospace 9";
   cchar                *boldfont = "monospace bold 9";

   if (! mLog) {                                                                 //  if no GUI use STDOUT
      printz("%s",message);
      return;
   }

   zthreadcrash();                                                               //  thread usage not allowed

   textBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mLog));
   if (! textBuff) return;                                                       //  5.5

   endMark = gtk_text_buffer_get_mark(textBuff,"wpxend");                        //  get my end mark

   if (! endMark) {
      gtk_text_buffer_get_end_iter(textBuff,&iter1);                             //  new buffer, set my end mark
      endMark = gtk_text_buffer_create_mark(textBuff,"wpxend",&iter1,0);
   }

   nlines = gtk_text_buffer_get_line_count(textBuff);                            //  lines now in buffer

   if (line == 0) scroll++;                                                      //  auto scroll is on

   if (line < 0) {
      line = nlines + line + 1;                                                  //  last lines: -1, -2 ...
      if (line < 1) line = 1;                                                    //  above top, use line 1
   }

   if (line > nlines) line = 0;                                                  //  below bottom, treat as append

   if (line == 0) gtk_text_buffer_get_end_iter(textBuff,&iter1);                 //  append new line

   if (line > 0) {
      gtk_text_buffer_get_iter_at_line(textBuff,&iter1,line-1);                  //  old line start
      if (line < nlines)
          gtk_text_buffer_get_iter_at_line(textBuff,&iter2,line);                //  old line end
      if (line == nlines)                                                        //    or buffer end
          gtk_text_buffer_get_end_iter(textBuff,&iter2);
      gtk_text_buffer_delete(textBuff,&iter1,&iter2);                            //  delete old line
   }
   
   if (bold)
      fontag = gtk_text_buffer_create_tag(textBuff,0,"font",boldfont,0);
   else
      fontag = gtk_text_buffer_create_tag(textBuff,0,"font",normfont,0);
   gtk_text_buffer_insert_with_tags(textBuff,&iter1,message,-1,fontag,null);

   if (scroll)                                                                   //  scroll line into view
      gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(mLog),endMark,0,0,1,1);

   zmainloop();
   return;
}

void wprintf(GtkWidget *mLog, int line, cchar *format, ... )                     //  "printf" version
{
   va_list  arglist;
   char     message[1000];

   va_start(arglist,format);
   vsnprintf(message,999,format,arglist);
   va_end(arglist);

   wprintx(mLog,line,message);
   return;
}

void wprintf(GtkWidget *mLog, cchar *format, ... )                               //  "printf", scrolling output
{
   va_list  arglist;
   char     message[1000];

   va_start(arglist,format);
   vsnprintf(message,999,format,arglist);                                        //  stop overflow, remove warning
   va_end(arglist);

   wprintx(mLog,0,message);
   return;
}


/**************************************************************************/

//  scroll a text view window to put a given line on screen
//  1st line = 1.  for last line use line = 0.

void wscroll(GtkWidget *mLog, int line)
{
   GtkTextBuffer  *textbuff;
   GtkTextIter    iter;

   if (! mLog) return;

   zthreadcrash();                                                               //  thread usage not allowed

   for (int ii = 0; ii < 10; ii++) {                                             //  improve reliability                6.2
      zmainloop();                                                               //  what a croc, GTK
      zsleep(0.003);
   }

   textbuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mLog));
   if (! textbuff) return;                                                       //  5.6
   if (line <= 0) line = gtk_text_buffer_get_line_count(textbuff);
   line = line - 1;
   gtk_text_buffer_get_iter_at_line(textbuff,&iter,line);
   gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(mLog),&iter,0,1,0,0);              //  unreliable, also *_scroll_to_mark()
   return;
}


/**************************************************************************/

//  clear a text view window and get a new buffer (a kind of defrag)

void wclear(GtkWidget *mLog)
{
   GtkTextBuffer  *buff;

   if (! mLog) return;

   zthreadcrash();                                                               //  thread usage not allowed

   buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mLog));
   if (! buff) return;                                                           //  5.6
   gtk_text_buffer_set_text(buff,"",-1);
   return;
}


//  clear a text view window from designated line to end of buffer

void wclear(GtkWidget *mLog, int line)
{
   GtkTextBuffer           *textBuff;
   GtkTextIter             iter1, iter2;

   if (! mLog) return;

   zthreadcrash();                                                               //  thread usage not allowed

   textBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mLog));
   if (! textBuff) return;                                                       //  5.6
   gtk_text_buffer_get_iter_at_line(textBuff,&iter1,line-1);                     //  iter at line start
   gtk_text_buffer_get_end_iter(textBuff,&iter2);
   gtk_text_buffer_delete(textBuff,&iter1,&iter2);                               //  delete existing line
   return;
}


/**************************************************************************/

//  Read from a text widget, one line at a time.
//  Set ftf = 1 for first call (will be returned = 0).
//  The next line of text is returned, or null if no more text.
//  A final \n character is removed if present.
//  Only one line is remembered, so it must be copied before the next call.

char * wscanf(GtkWidget *mLog, int & ftf)
{
   GtkTextBuffer  *textBuff;
   GtkTextIter    iter1, iter2;
   static char    *precs = 0, *prec1, *pret;
   static int     cc;

   if (! mLog) return 0;

   zthreadcrash();                                                               //  thread usage not allowed

   if (ftf)
   {                                                                             //  get all window text
      ftf = 0;
      if (precs) g_free(precs);                                                  //  free prior memory if there
      textBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(mLog));                  //  get all text
      if (! textBuff) return 0;                                                  //  5.6
      gtk_text_buffer_get_bounds(textBuff,&iter1,&iter2);
      precs = gtk_text_buffer_get_text(textBuff,&iter1,&iter2,0);
      prec1 = precs;                                                             //  1st record
   }

   if (! precs || (*prec1 == 0))                                                 //  no more records
   {
      if (precs) g_free(precs);
      precs = 0;
      return 0;
   }

   cc = 0;
   while ((prec1[cc] != 0) && (prec1[cc] != '\n')) cc++;                         //  scan for terminator
   pret = prec1;
   prec1 = prec1 + cc;                                                           //  next record
   if (*prec1 == '\n') prec1++;
   pret[cc] = 0;                                                                 //  replace \n with 0
   return pret;
}


/**************************************************************************/

//  dump text window into file
//  return:  0: OK  +N: error

int   wfiledump_maxcc = 0;

int wfiledump(GtkWidget *mLog, const char *filespec)
{
   FILE        *fid;
   char        *prec;
   int         ftf, err, cc;

   if (! mLog) return 0;

   fid = fopen(filespec,"w");                                                    //  open file
   if (! fid) {
      zmessageACK(0,ZTX("cannot open file %s"),filespec);
      return 1;
   }

   wfiledump_maxcc = 0;

   ftf = 1;
   while (true)
   {
      prec = wscanf(mLog,ftf);                                                   //  get text line
      if (! prec) break;
      fprintf(fid,"%s\n",prec);                                                  //  output with \n
      cc = strlen(prec);
      if (cc > wfiledump_maxcc) wfiledump_maxcc = cc;
   }

   err = fclose(fid);                                                            //  close file
   if (err) {
      zmessageACK(0,"file close error");
      return 2;
   }
   else return 0;
}


/**************************************************************************/

//  save text window to file, via file chooser dialog

void wfilesave(GtkWidget *mLog, GtkWindow *parent)
{
   int         err;
   std::string file;

   if (! mLog) return;

   file = zgetfile(ZTX("save screen to file"),parent,"save","screen-save.txt");
   if (file.empty()) return;
   err = wfiledump(mLog,file.c_str());
   if (err) zmessageACK(0,"save screen failed (%d)",err);
   return;
}


/**************************************************************************/

//  print text window to default printer
//  use landscape mode if max. print line > A4 width

void wprintp(GtkWidget *mLog)
{
   int            err;
   char           printfile[30];
   static int64   seed = 0;
   static int     pid;

   if (! seed) seed = pid = getpid();
   if (! mLog) return;
   snprintf(printfile,30,"/tmp/wprintp-%d-%d",pid,lrandz(&seed));
   err = wfiledump(mLog,printfile);
   if (err) return;

   if (wfiledump_maxcc < 97)
      err = shell_ack("lp -o %s -o %s -o %s -o %s -o %s -o %s %s",
                      "cpi=14","lpi=8","page-left=50","page-top=50",
                      "page-right=40","page-bottom=40",printfile);
   else
      err = shell_ack("lp -o %s -o %s -o %s -o %s -o %s -o %s -o %s %s",
                      "landscape","cpi=14","lpi=8","page-left=50","page-top=50",
                      "page-right=40","page-bottom=40",printfile);
   return;
}


/**************************************************************************/

//  Set a function to be called when a GTK text view widget is mouse-clicked.
//  Function returns the clicked line number and position, both zero based.
//  A wrapped line is still one logical line.
//  Note: cannot be used on text windows that are edited.
//  The called function looks like this:
//    void clickfunc(GtkWidget *widget, int line, int pos)

void textwidget_set_clickfunc(GtkWidget *widget, clickfunc_t clickfunc)
{
   int textwidget_mousefunc(GtkWidget *widget, GdkEventButton *event, clickfunc_t clickfunc);

   zthreadcrash();

   gtk_widget_add_events(widget,GDK_ALL_EVENTS_MASK);                            //  connect events      5.2
   G_SIGNAL(widget,"event",textwidget_mousefunc,clickfunc);
   return;
}

int textwidget_mousefunc(GtkWidget *widget, GdkEventButton *event, clickfunc_t clickfunc)
{
   using namespace zfuncs;

   static GdkCursor  *arrowcursor = 0;
   GdkWindow         *gdkwin;
   GtkTextIter       iter1;
   int               mpx, mpy, tbx, tby, line, pos;

   #define TEXT GTK_TEXT_WINDOW_TEXT
   #define VIEW GTK_TEXT_VIEW
   #define ARROW GDK_TOP_LEFT_ARROW

   if (! arrowcursor) arrowcursor = gdk_cursor_new_for_display(display,ARROW);
   gdkwin = gtk_text_view_get_window(VIEW(widget),TEXT);
   if (gdkwin) gdk_window_set_cursor(gdkwin,arrowcursor);                        //  why must this be repeated?      5.5

   if (event->type != GDK_BUTTON_RELEASE) return 0;                              //  5.7

   mpx = int(event->x);                                                          //  mouse click position
   mpy = int(event->y);
   gtk_text_view_window_to_buffer_coords(VIEW(widget),TEXT,mpx,mpy,&tbx,&tby);
   gtk_text_view_get_iter_at_location(VIEW(widget),&iter1,tbx,tby);
   line = gtk_text_iter_get_line(&iter1);                                        //  clicked line
   pos = gtk_text_iter_get_line_offset(&iter1);                                  //  clicked position
   clickfunc(widget,line,pos);                                                   //  call user function
   return 0;                                                                     //  5.6
}


//  get a given line of text from a GTK text view widget
//  returned text is subject for zfree()

char * textwidget_get_line(GtkWidget *widget, int line, int hilite)
{
   GtkTextBuffer     *textbuffer;
   GtkTextIter       iter1, iter2;
   char              *text, *ztext;
   int               cc;

   zthreadcrash();

   textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
   if (! textbuffer) return 0;                                                   //  5.6
   gtk_text_buffer_get_iter_at_line(textbuffer,&iter1,line);
   iter2 = iter1;
   gtk_text_iter_forward_line(&iter2);
   if (hilite)                                                                   //  highlight selected line         5.6
      gtk_text_buffer_select_range(textbuffer,&iter1,&iter2);
   text = gtk_text_buffer_get_text(textbuffer,&iter1,&iter2,0);
   if (! text) return 0;
   ztext = zstrdup(text);
   zfree(text);
   cc = strlen(ztext);
   while (cc && ztext[cc-1] < ' ') cc--;
   ztext[cc] = 0;
   if (cc == 0) {
      zfree(ztext);
      return 0;
   }
   return ztext;
}


//  get the clicked word within the line
//  words are defined by line starts and ends, and the given delimiter
//  returns word and delimiter (&end)

char * textwidget_get_word(const char *line, int pos, cchar *dlims, char &end)
{
   const char     *pp1, *pp2;
   char *ztext;
   int      cc;

   if (! line) return 0;
   pos = utf8_position(line,pos);                                                //  graphic position to byte position
   if (pos < 0) return 0;                                                        //  5.5

   pp1 = line + pos;
   if (! *pp1 || strchr(dlims,*pp1)) return 0;                                   //  reject edge position or delimiter
   while (pp1 > line && ! strchr(dlims,pp1[-1])) pp1--;                          //  find start of word
   pp2 = pp1;
   while (pp2[1] && ! strchr(dlims,pp2[1])) pp2++;                               //  find following delimiter or EOL
   end = pp2[1];                                                                 //  return delimiter
   while (*pp1 == ' ') pp1++;                                                    //  no leading or trailing blanks
   while (*pp2 == ' ') pp2--;
   cc = pp2 - pp1 + 1;
   if (cc < 1) return 0;                                                         //  all blanks?
   ztext = (char *) zmalloc(cc+1);
   strncpy0(ztext,pp1,cc+1);
   return ztext;
}


/**************************************************************************

   simplified GTK menu bar, tool bar, status bar functions

   These functions simplify the creation of GTK menus and toolbars.
   The functionality is limited but adequate for most purposes.

   mbar = create_menubar(vbox)                               create menubar
   mitem = add_menubar_item(mbar, label, func)               add menu item to menubar
   msub = add_submenu_item(mitem, label, func, tip)          add submenu item to menu or submenu

   tbar = create_toolbar(vbox, iconsize)                     create toolbar
   add_toolbar_button(tbar, label, tip, icon, func)          add button to toolbar

   stbar = create_stbar(vbox)                                create status bar
   stbar_message(stbar, message)                             display message in status bar

   These functions to the following:
      * create a menu bar and add to existing window verticle packing box
      * add menu item to menu bar
      * add submenu item to menu bar item or submenu item
      * create a toolbar and add to existing window
      * add button to toolbar, using stock icon or custom icon
      * create a status bar and add to existing window
      * display a message in the status bar

   argument definitions:
     vbox         GtkWidget *    a verticle packing box (in a window)
     mbar         GtkWidget *    reference for menu bar
     popup        GtkWidget *    reference for popup menu
     mitem        GtkWidget *    reference for menu item (in a menu bar)
     msub         GtkWidget *    reference for submenu item (in a menu)
     label        cchar *        menu or toolbar name or label
     tbar         GtkWidget *    reference for toolbar
     tip          cchar *        tool button tool tip (popup text via mouse-over)
     icon         cchar *        stock icon name or custom icon file name (see below)
     func         see below      menu or tool button response function
     arg          cchar *        argument to response function
     stbar        int            reference for status bar
     message      cchar *        message to display in status bar

   The icon argument for the function add_toolbar_button() has two forms.
   For a GTK stock item referenced with a macro like GTK_STOCK_OPEN, use the
   corresponding text name, like "gtk-open".

   For a custom icon, use the icon's file name like "my-icon.png".
   The file is expected to be in  get_zdatadir()/icons.
   The icon file may be any size, and is resized for use on the toolbar.
   If the file is not found, the stock icon "gtk-missing-image" is used
   (".png" and ".jpg" files both work).

   For a button with no icon (text label only), use 0 or null for the icon argument.
   For a menu separator, use the menu name "separator".
   For a toolbar separator, use the label "separator".
   For a title menu (no response function), set the response function to null.

   The response function for both menus and toolbar buttons looks like this:
       void func(GtkWidget *, cchar *)

   The following macro is also supplied to simplify the coding of response functions:
   G_SIGNAL(window,event,func,arg)   which expands to:
   g_signal_connect(G_OBJECT(window),event,G_CALLBACK(func),(void *) arg)

***************************************************************************/

//  create menu bar and add to vertical packing box

GtkWidget * create_menubar(GtkWidget *vbox)                                      //  icon size removed
{
   GtkWidget   *wmbar;

   zthreadcrash();

   wmbar = gtk_menu_bar_new();
   gtk_box_pack_start(GTK_BOX(vbox),wmbar,0,0,0);
   return wmbar;
}


//  add menu item to menu bar, with optional response function

GtkWidget * add_menubar_item(GtkWidget *wmbar, cchar *mname, cbFunc func)
{
   GtkWidget   *wmitem;

   zthreadcrash();

   wmitem = gtk_menu_item_new_with_label(mname);
   gtk_menu_shell_append(GTK_MENU_SHELL(wmbar),wmitem);
   if (func) G_SIGNAL(wmitem,"activate",func,mname);
   return  wmitem;
}


//  add submenu item to menu item, with optional response function

GtkWidget * add_submenu_item(GtkWidget *wmitem, cchar *mlab, cbFunc func, cchar *mtip)
{
   GtkWidget      *wmsub, *wmsubitem;

   zthreadcrash();

   wmsub = gtk_menu_item_get_submenu(GTK_MENU_ITEM(wmitem));                     //  add submenu if not already
   if (wmsub == null) {
      wmsub = gtk_menu_new();
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(wmitem),wmsub);
   }

   if (strmatch(mlab,"separator"))
      wmsubitem = gtk_separator_menu_item_new();
   else  wmsubitem = gtk_menu_item_new_with_label(mlab);                         //  add menu item with label only

   gtk_menu_shell_append(GTK_MENU_SHELL(wmsub),wmsubitem);                       //  append submenu item to submenu

   if (func) G_SIGNAL(wmsubitem,"activate",func,mlab);                           //  connect optional response function

   if (mtip) g_object_set(G_OBJECT(wmsubitem),"tooltip-text",mtip,null);         //  add optional popup menu tip        5.2

   return  wmsubitem;
}


/**************************************************************************/

//  create toolbar and add to vertical packing box

int      tbIconSize = 32;                                                        //  default if not supplied            6.2

GtkWidget * create_toolbar(GtkWidget *vbox, int iconsize)
{
   using namespace zfuncs;

   GtkWidget   *wtbar;

   zthreadcrash();

   wtbar = gtk_toolbar_new();
   gtk_box_pack_start(GTK_BOX(vbox),wtbar,0,0,0);
   tbIconSize = iconsize;
   return  wtbar;
}


//  add toolbar button with label and icon ("iconfile.png") and tool tip
//  at least one of label and icon should be present

GtkWidget * add_toolbar_button(GtkWidget *wtbar, cchar *blab, cchar *btip, cchar *icon, cbFunc func)
{
   using namespace zfuncs;

   GtkToolItem    *tbutton;
   GError         *gerror = 0;
   PIXBUF         *pixbuf;
   GtkWidget      *wicon = 0;
   char           iconpath[300], *pp;
   STATB          statb;
   int            err, cc;

   zthreadcrash();

   if (blab && strmatch(blab,"separator")) {
      tbutton = gtk_separator_tool_item_new();
      gtk_toolbar_insert(GTK_TOOLBAR(wtbar),GTK_TOOL_ITEM(tbutton),-1);
      return  (GtkWidget *) tbutton;
   }

   if (icon && *icon) {                                                          //  get icon pixbuf
      *iconpath = 0;
      strncatv(iconpath,199,zicondir,"/",icon,null);
      err = stat(iconpath,&statb);
      if (err) {                                                                 //  alternative path                   6.2
         cc = readlink("/proc/self/exe",iconpath,300);                           //  get own program path
         if (cc > 0) iconpath[cc] = 0;                                           //  readlink() quirk
         pp = strrchr(iconpath,'/');                                             //  directory of program
         if (pp) *pp = 0;
         strncatv(iconpath,300,"/icons/",icon,null);                             //  .../icons/iconfile.png
      }
      pixbuf = gdk_pixbuf_new_from_file_at_scale(iconpath,tbIconSize,tbIconSize,1,&gerror);
      if (pixbuf) wicon = gtk_image_new_from_pixbuf(pixbuf);
   }

   tbutton = gtk_tool_button_new(wicon,blab);                                    //  5.8
   if (! wicon) gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(tbutton),"gtk-missing-image");

   if (btip) gtk_tool_item_set_tooltip_text(tbutton,btip);
   gtk_tool_item_set_homogeneous(tbutton,0);
   gtk_toolbar_insert(GTK_TOOLBAR(wtbar),GTK_TOOL_ITEM(tbutton),-1);
   if (func) G_SIGNAL(tbutton,"clicked",func,blab);
   return  (GtkWidget *) tbutton;
}


/**************************************************************************/

//  create a status bar and add to the start of a packing box

GtkWidget * create_stbar(GtkWidget *pbox)
{
   GtkWidget      *stbar;

   zthreadcrash();

   stbar = gtk_statusbar_new();
   gtk_box_pack_start(GTK_BOX(pbox),stbar,0,0,0);
   gtk_widget_show(stbar);
   return  stbar;
}


//  display message in status bar

int stbar_message(GtkWidget *wstbar, cchar *message)
{
   static int     ctx = -1;

   zthreadcrash();

   if (ctx == -1)
      ctx = gtk_statusbar_get_context_id(GTK_STATUSBAR(wstbar),"all");
   gtk_statusbar_pop(GTK_STATUSBAR(wstbar),ctx);
   gtk_statusbar_push(GTK_STATUSBAR(wstbar),ctx,message);
   return 0;
}


/**************************************************************************

   Popup Menu

   GtkWidget   *popup, *mitem
   cchar       *label, *arg, *tip
   void func(GtkWidget *, cchar *arg)

   popup = create_popmenu()                                  create a popup menu
   mitem = add_popmenu_item(popup, label, func, arg, tip)    add menu item to popup menu
   popup_menu(GtkWidget *parent, popup)                      popup the menu at mouse position

   Call 'create_popmenu' and then 'add_popmenu_item' for each item in the menu.
   'label' is the menu name, 'func' the response function, 'arg' an argument
   for 'func', and 'tip' is a tool-tip. 'arg' and 'tip' may be null.
   A call to 'popup_menu' will show all menu entries at the mouse position.
   Clicking an entry will call the respective response function.
   Hovering on the entry will show the tool-tip.

   The response function looks like this:
       void func(GtkWidget *, cchar *menu)

***/

//  create a popup menu

GtkWidget * create_popmenu()
{
   int popmenu_event(GtkWidget *, GdkEvent *);

   GtkWidget   *popmenu;
   popmenu = gtk_menu_new();

   zthreadcrash();

   gtk_widget_add_events(popmenu,GDK_BUTTON_PRESS_MASK);                         //  5.5
   G_SIGNAL(popmenu,"button-press-event",popmenu_event,0);                       //  no event passed in Ubuntu 13.10    5.7
   return popmenu;
}


//  handle mouse button event in a popup menu

int popmenu_event(GtkWidget *popmenu, GdkEvent *event)                           //  not called in Ubuntu 13.10         5.7
{
   if (((GdkEventButton *) event)->button != 1)                                  //  if not left mouse, kill menu
      gtk_menu_popdown(GTK_MENU(popmenu));
   return 0;
}


//  add a menu item to a popup menu

GtkWidget * add_popmenu_item(GtkWidget *popmenu, cchar *mname, cbFunc func, cchar *arg, cchar *mtip)
{
   GtkWidget   *wmitem;

   zthreadcrash();

   wmitem = gtk_menu_item_new_with_label(mname);
   if (mtip) g_object_set(G_OBJECT(wmitem),"tooltip-text",mtip,null);            //  add optional popup menu tip

   gtk_menu_shell_append(GTK_MENU_SHELL(popmenu),wmitem);
   if (func) {
      if (arg) G_SIGNAL(wmitem,"activate",func,arg);                             //  call func with arg 
      else G_SIGNAL(wmitem,"activate",func,mname);                               //  call func with menu name
   }
   return  wmitem;
}


//  Show a popup menu at current mouse position
//
//  UGLY KLUDGE: If mouse is near screen bottom edge, popup menu will 
//  be partly below the screen and invisible. The following work-around
//  is implemented: If within 600 of screen bottom, show at bottom - 600
//  to insure entire popup is visible. Get the true height of the realized
//  widget. If within height of screen bottom, show at bottom - height.
//  Otherwise show at mouse position.

void popup_menu(GtkWidget *widget, GtkWidget *popmenu)
{
   void popup_moveloc(GtkMenu *, int *, int *, int *, void *);                   //  6.1

   int      int600 = 600;
   GtkRequisition req;

   zthreadcrash();

   gtk_menu_popup(GTK_MENU(popmenu),0,0,popup_moveloc,&int600,1,0);              //  show popup at screen bottom - 600
   gtk_widget_show_all(popmenu);
   gtk_widget_get_preferred_size((GtkWidget *) popmenu,null,&req);               //  get size of realized popup menu
   gtk_menu_popup(GTK_MENU(popmenu),0,0,popup_moveloc,&req.height,1,0);          //  show at bottom - height
   gtk_widget_show_all(popmenu);

   return;
}


//  Move popup menu 40 pixels to the right of the mouse.
//  If within 'vphh' of the screen bottom, move it up to bottom - 'vphh'.

void popup_moveloc(GtkMenu *menu, int *mx, int *my, int *, void *vphh)           //  6.1
{
   using namespace zfuncs;
   
   int      phh, scrhh;

   get_hardware_info(GTK_WIDGET(menu));
   if (! mouse) return;
   scrhh = gdk_screen_get_height(screen);
   gdk_device_get_position(mouse,&screen,mx,my);
   *mx += 40;
   phh = *((int *) vphh);
   if (scrhh - *my < phh) *my = scrhh - phh - 5;                                 //  if under bottom - height, move up 
   return;
}


/**************************************************************************

   Customizable Graphic Popup Menu

   void gmenuz(GtkWidget *parent, cchar *configfile, callbackfunc)

   Open a popup window with a customizable graphic menu.
      parent         parent window or null
      configfile     menu configuration file, will be created if missing
      callbackfunc   callback function to receive clicked menu entry:
                        typedef void callbackfunc(cchar *menu)

   This function allows an application to offer a customizable menu which a
   user can populate with frequently used (menu) functions, arranged as desired.
   A menu entry selected by the user is passed to the application for execution.

   The initial popup window is blank. Right click an empty space on the popup
   window to define a new menu entry. Right click an existing entry to modify it.
   Use the resulting dialog to define or change the menu entry.
      menu text     optional text appearing in the popup window
      menu func     text that is passed to the application callback function
      menu icon     optional menu icon: /directory.../filename.png
      icon size     rendered icon size in the popup window, 24x24 to 64x64 pixels
      close         checkbox: option to close the popup window when menu is used

   Left drag a menu entry to move it somewhere else on the popup window.
   The popup window can be resized to fit the contained menu entries.
   Left click a menu entry to select the menu.
   The callback function will be called to execute the menu function.
   If "close" was checked, the popup window will close.

   All menu settings are saved in the supplied configuration file whenever the
   popup window is closed, if any changes were made since it was opened.
   Icon files are copied into the same directory as the configuration file and
   these copies are used. The icon selected when the menu entry is created can
   disappear without consequence.

   If the [x] window kill button is pressed, the window is closed and the
   calling program is informed by passing "quit" to the callback function.

   layout of menu configuration file:

      popup xpos ypos width height     popup window position and size

      posn  xpos ypos ww hh            menu position in popup window
      menu  menu text                  menu text on popup window
      func  funcname                   argument for user callback function
      icon  /.../file.png              optional menu icon file
      size  NN                         optional icon size (default 24)
      kill                             optional flag, kill window when used

      menu  menu text                  next menu entry
      ...

***************************************************************************/

namespace gmenuznames
{
   #define maxME 200                                                             //  max. menu entries
   #define maxText 1000                                                          //  max. text size, all menu fields
   #define menuFont "sans 9"                                                     //  menu font
   #define iconSize 24                                                           //  menu icon size (default)

   typedef void callbackfunc(cchar *menu);                                       //  user callback function
   callbackfunc    *gmenuzcallback;

   char        *menuconfigfile = 0;                                              //  configuration file from user

   GtkWidget   *mWin, *layout;                                                   //  popup and drawing windows
   GtkWidget   *pWin;                                                            //  parent window

   int         winposx=100, winposy=100, winww=400, winhh=300;                   //  initial popup WRT parent window

   struct      menuent {                                                         //  menu entry on popup window
      int         xpos, ypos, ww, hh;                                            //  layout position, extent
      char        *menu;                                                         //  text on window on null
      char        *func;                                                         //  func name for user callback
      char        *icon;                                                         //  icon file or null
      PIXBUF      *pixbuf;                                                       //  icon pixbuf or null
      int         size;                                                          //  icon size or zero
      int         kill;                                                          //  kill popup window when menu used
   };
   menuent     menus[200];                                                       //  menu entries
   int         NME = 0;                                                          //  entry count

   zdialog     *zdedit = 0;                                                      //  active edit dialog
   int         mpx, mpy;                                                         //  mouse click/drag position
   int         me;                                                               //  current menu entry
   int         Fchanged = 0;                                                     //  flag, menu edited or resized
   int         Fpopquit = 0;                                                     //  popup is being closed
   int         Fpopbusy = 0;                                                     //  popup is active

   void  wpaint(GtkWidget *, cairo_t *);                                         //  window repaint - draw event
   void  resize();                                                               //  window resize event
   void  quit();                                                                 //  kill window and exit
   void  update_configfile();                                                    //  update menu configuration file
   void  mouse_event(GtkWidget *, GdkEventButton *, void *);                     //  mouse event function
   void  KB_event(GtkWidget *, GdkEventKey *, void *);                           //  KB event function
   void  draw_text(cairo_t *, char *, int px, int py, int &ww, int &hh);         //  draw text and return pixel extent
   void  edit_menu();                                                            //  dialog to create/edit menu entry
   int   edit_menu_event(zdialog *zd, cchar *event);                             //  dialog event function
   void  drag_drop_event(int mx, int my, const char *file);                            //  drag-drop event function
}


//  user callable function to build the menu from user's configuration file

void gmenuz(GtkWidget *parent, cchar *title, cchar *ufile, gmenuznames::callbackfunc ufunc)
{
   using namespace gmenuznames;

   FILE        *fid;
   int         nn, xx, yy, ww, hh, size;
   int         pposx, pposy;
   int         xpos, ypos;
   char        *pp, buff[maxText];
   PIXBUF      *pixbuf;
   GError      *gerror;

   zthreadcrash();

   if (Fpopbusy) return;                                                         //  don't allow multiple popups

   pWin = parent;                                                                //  get parent window

   if (menuconfigfile) zfree(menuconfigfile);
   menuconfigfile = zstrdup(ufile);                                              //  get menu configuration file

   gmenuzcallback = ufunc;                                                       //  get user callback function

   NME = 0;

   fid = fopen(menuconfigfile,"r");                                              //  read window geometry
   if (fid)
   {
      nn = fscanf(fid," popup %d %d %d %d",&xx,&yy,&ww,&hh);                     //  get popup window position and size
      if (nn == 4 && ww > 50 && ww < 1000 && hh > 50 && hh < 1000) {
         winposx = xx;                                                           //  OK to use
         winposy = yy;
         winww = ww;
         winhh = hh;
      }

      while (true)
      {
         pp = fgets_trim(buff,maxText-1,fid,1);                                  //  read next menu entry
         if (! pp) break;

         if (strmatchN(pp,"posn ",5)) {                                          //  position in popup window
            if (NME == maxME) {
               zmessageACK(mWin,"exceeded %d menu entries",maxME);
               break;
            }

            me = NME;                                                            //  new entry
            NME++;                                                               //  entry count
            memset(&menus[me],0,sizeof(menuent));                                //  clear all menu data

            nn = sscanf(pp+5," %d %d ",&xpos,&ypos);                             //  position in popup window
            if (nn != 2) xpos = ypos = 100;
            if (xpos > 1000) xpos = 1000;
            if (ypos > 1000) ypos = 1000;
            menus[me].xpos = xpos;
            menus[me].ypos = ypos;
         }

         if (strmatchN(pp,"menu ",5)) {                                          //  menu text
            if (strlen(pp+5) > 0)
               menus[me].menu = zstrdup(pp+5);                                   //  get menu text
            else menus[me].menu = 0;
         }

         if (strmatchN(pp,"func ",5)) {                                          //  function name
            if (strlen(pp+5)) menus[me].func = zstrdup(pp+5);
            else menus[me].func = 0;
         }

         if (strmatchN(pp,"icon ",5)) {                                          //  menu icon file
            if (strlen(pp+5)) {
               menus[me].icon = zstrdup(pp+5);
               gerror = 0;
               pixbuf = gdk_pixbuf_new_from_file(pp+5,&gerror);
               if (! pixbuf && gerror) printz("*** %s \n",gerror->message);
               menus[me].pixbuf = pixbuf;
            }
            else menus[me].pixbuf = 0;
         }

         if (strmatchN(pp,"size ",5)) {
            size = atoi(pp+5);
            if (size < 24) size = 24;
            if (size > 64) size = 64;
            menus[me].size = size;
         }

         if (strmatchN(pp,"kill",4))                                             //  kill window flag
            menus[me].kill = 1;
      }

      fclose(fid);
   }

   Fchanged = 0;                                                                 //  no changes yet
   Fpopquit = 0;                                                                 //  not being closed
   Fpopbusy = 1;

   mWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);                                   //  create popup window for menu entries

   if (! pWin) pposx = pposy = 0;                                                //  no parent window
   else {
      gtk_window_get_position(GTK_WINDOW(pWin),&pposx,&pposy);                   //  parent window position (NW corner)
      gtk_window_set_transient_for(GTK_WINDOW(mWin),GTK_WINDOW(pWin));           //  popup window belongs to parent
   }

   if (title) gtk_window_set_title(GTK_WINDOW(mWin),title);                      //  5.8

   winposx += pposx;                                                             //  popup position relative to parent
   winposy += pposy;

   gtk_window_set_default_size(GTK_WINDOW(mWin),winww,winhh);                    //  set size and position
   gtk_window_move(GTK_WINDOW(mWin),winposx,winposy);

   layout = gtk_layout_new(0,0);                                                 //  create drawing window
   gtk_container_add(GTK_CONTAINER(mWin),layout);                                //  add to popup window

   G_SIGNAL(mWin,"destroy",quit,0);                                              //  connect signals to windows
   G_SIGNAL(mWin,"delete_event",quit,0);
   G_SIGNAL(mWin,"notify",resize,0);
   G_SIGNAL(mWin,"key-release-event",KB_event,0);                                //  connect KB key release event
   G_SIGNAL(layout,"draw",wpaint,0);

   gtk_widget_add_events(layout,GDK_BUTTON_PRESS_MASK);                          //  connect mouse events
   gtk_widget_add_events(layout,GDK_BUTTON_RELEASE_MASK);
   gtk_widget_add_events(layout,GDK_BUTTON_MOTION_MASK);
   gtk_widget_add_events(layout,GDK_POINTER_MOTION_MASK);
   G_SIGNAL(layout,"button-press-event",mouse_event,0);
   G_SIGNAL(layout,"button-release-event",mouse_event,0);
   G_SIGNAL(layout,"motion-notify-event",mouse_event,0);
   
   drag_drop_connect(mWin,drag_drop_event);                                      //  v.6.2

   gtk_widget_show_all(mWin);                                                    //  show all widgets
   return;
}


//  paint window when created, exposed, resized

void gmenuznames::wpaint(GtkWidget *, cairo_t *cr)
{
   using namespace gmenuznames;

   PIXBUF      *pixbuf;
   char        *text, *text2;
   int         xpos, ypos, ww, hh, size, yadd;

   for (int me = 0; me < NME; me++)                                              //  loop all menu entries
   {
      xpos = menus[me].xpos;                                                     //  window position
      ypos = menus[me].ypos;
      text = menus[me].menu;                                                     //  menu text
      pixbuf = menus[me].pixbuf;                                                 //  icon
      size = menus[me].size;                                                     //  size

      if (pixbuf) {                                                              //  draw icon at window position
         gdk_cairo_set_source_pixbuf(cr,pixbuf,xpos,ypos);
         cairo_paint(cr);
         if (! size) size = iconSize;                                            //  use default if not specified
      }
      else size = 0;

      yadd = 0;
      if (pixbuf) yadd = size + 2;                                               //  extra space under icon

      if (text) {
         text2 = (char *) zmalloc(strlen(text)+2);                               //  replace "\n" with newline
         repl_1str(text,text2,"\\n","\n");
         draw_text(cr,text2,xpos,ypos+yadd,ww,hh);                               //  returns size of text written
         zfree(text2);
      }
      else ww = hh = 0;

      if (ww < size) ww = size;                                                  //  menu entry enclosing rectangle
      hh += yadd;
      menus[me].ww = ww;
      menus[me].hh = hh;
   }

   return;
}


//  resize event - save current window size

void gmenuznames::resize()
{
   using namespace gmenuznames;

   int            xx, yy, ww, hh;

   if (Fpopquit) return;                                                         //  ignore bogus call when killed

   gtk_window_get_position(GTK_WINDOW(mWin),&xx,&yy);
   gtk_window_get_size(GTK_WINDOW(mWin),&ww,&hh);

   if (xx == winposx && yy == winposy &&
       ww == winww && hh == winhh) return;                                       //  no change

   winposx = xx;
   winposy = yy;
   winww = ww;
   winhh = hh;

   Fchanged = 1;                                                                 //  mark window changed
   return;
}


//  [x] kill window
//  Save current menu status for next session.

void gmenuznames::quit()
{
   using namespace gmenuznames;

   zthreadcrash();

   Fpopquit = 1;
   Fpopbusy = 0;
   if (Fchanged) update_configfile();
   gtk_widget_destroy(mWin);
   gmenuzcallback("quit");                                                       //  inform host program          5.4
   return;
}


//  menu changed, save all menu data to menu config. file

void gmenuznames::update_configfile()
{
   using namespace gmenuznames;

   char           *pp, *pxbfile;
   int            pposx, pposy;
   FILE           *fid;
   GError         *gerror;

   zthreadcrash();

   if (pWin)
      gtk_window_get_position(GTK_WINDOW(pWin),&pposx,&pposy);                   //  parent window position (may have moved)
   else pposx = pposy = 0;

   winposx -= pposx;                                                             //  popup position relative to parent
   winposy -= pposy;

   fid = fopen(menuconfigfile,"w");                                              //  open for write
   if (! fid) {
      zmessageACK(mWin," %s \n %s",menuconfigfile,strerror(errno));              //  diagnose permissions error      5.7
      return;
   }

   fprintf(fid,"popup %d %d %d %d \n",winposx,winposy,winww,winhh);

   for (me = 0; me < NME; me++)                                                  //  write all menu entries to file
   {
      if (! menus[me].menu && ! menus[me].pixbuf) {                              //  no text and no icon
         printz("*** gmenuz: skip empty menu entry \n");
         continue;
      }

      fprintf(fid,"\n");                                                         //  blank line separator

      fprintf(fid,"posn %d %d \n",menus[me].xpos, menus[me].ypos);               //  menu position in window

      if (menus[me].menu)                                                        //  menu text
         fprintf(fid,"menu %s \n",menus[me].menu);

      if (menus[me].func)                                                        //  menu function (text)
         fprintf(fid,"func %s \n",menus[me].func);

      if (menus[me].pixbuf) {                                                    //  pixbuf image for menu icon
         pxbfile = zstrdup(menuconfigfile,20);
         pp = pxbfile + strlen(pxbfile);                                         //  create a local PNG file for pixbuf
         snprintf(pp,20,"-pixbuf-%03d.png",me);
         gerror = 0;
         gdk_pixbuf_save(menus[me].pixbuf,pxbfile,"png",&gerror,null);           //  write pixbuf to file
         if (gerror)
            printz("*** %s \n %s \n",menus[me].menu,gerror->message);
         else fprintf(fid,"icon %s \n",pxbfile);                                 //  pixbuf file name in menu file
         zfree(pxbfile);
      }

      if (menus[me].size)                                                        //  icon size
         fprintf(fid,"size %d \n",menus[me].size);

      if (menus[me].kill) fprintf(fid,"kill \n");                                //  kill window flag
   }

   fclose(fid);

   Fchanged = 0;
   return;
}


//  mouse event function - capture buttons and drag movements

void gmenuznames::mouse_event(GtkWidget *, GdkEventButton *event, void *)
{
   using namespace gmenuznames;

   static int     bdtime = 0, butime = 0;
   static int     Lmouse = 0, Rmouse = 0, Fdrag = 0;
   static int     elapsed, mpx0 = 0, mpy0 = 0;
   int            Fclick, dx, dy, xpos, ypos;

   mpx = int(event->x);                                                          //  mouse position in window
   mpy = int(event->y);

   if (event->type == GDK_BUTTON_PRESS)
   {
      Lmouse = Rmouse = Fdrag = 0;
      if (event->button == 1) Lmouse++;                                          //  left or right mouse button
      if (event->button == 3) Rmouse++;
      bdtime = event->time;

      for (me = 0; me < NME; me++)                                               //  look for clicked menu entry
      {
         if (mpx < menus[me].xpos) continue;
         if (mpy < menus[me].ypos) continue;
         if (mpx > menus[me].xpos + menus[me].ww) continue;
         if (mpy > menus[me].ypos + menus[me].hh) continue;
         break;
      }

      if (me < NME) {                                                            //  menu item clicked (selected)
         mpx0 = mpx;                                                             //  set new drag origin
         mpy0 = mpy;
      }
      else me = -1;                                                              //  indicate empty space clicked
   }

   if (event->type == GDK_BUTTON_RELEASE)
   {
      Fclick = 0;
      butime = event->time;
      elapsed = butime - bdtime;                                                 //  button down time, milliseconds
      if (elapsed < 500 && ! Fdrag) Fclick = 1;                                  //  mouse clicked

      if (me >= 0 && Fclick && Lmouse) {                                         //  menu entry was left-clicked
         if (menus[me].func)
            gmenuzcallback(menus[me].func);                                      //  caller user function(func)
         if (menus[me].kill) quit();                                             //  close menu after app launch
      }

      else if (Fclick && Rmouse)                                                 //  menu entry or empty space right-clicked
         edit_menu();                                                            //  edit menu

      else if (me >= 0 && Fdrag) {                                               //  menu entry drag ended
         xpos = menus[me].xpos;                                                  //  align to 16-pixel raster
         ypos = menus[me].ypos;
         xpos = 16 * ((xpos + 8) / 16) + 4;                                      //  min. 4 pixels from edge
         ypos = 16 * ((ypos + 8) / 16) + 4;
         menus[me].xpos = xpos;
         menus[me].ypos = ypos;
         gtk_widget_queue_draw(layout);                                          //  repaint window
         Fdrag = 0;
         Fchanged = 1;                                                           //  mark menu revised
      }

      Lmouse = Rmouse = 0;                                                       //  mouse click action completed
   }

   if (event->type == GDK_MOTION_NOTIFY)                                         //  mouse movement
   {
      if (me >= 0 && Lmouse) {                                                   //  menu drag underway
         dx = mpx - mpx0;
         dy = mpy - mpy0;
         if (Fdrag || (abs(dx) + abs(dy) > 15)) {                                //  ignore small drags              5.5
            Fdrag++;
            mpx0 = mpx;                                                          //  set new drag origin
            mpy0 = mpy;
            menus[me].xpos += dx;                                                //  add motion to image position
            menus[me].ypos += dy;
            gtk_widget_queue_draw(layout);                                       //  repaint window
         }
      }
   }

   return;
}


//  KB release event function - send KB F1 key to main app

void gmenuznames::KB_event(GtkWidget *, GdkEventKey *kbevent, void *)
{
   int   KBkey = kbevent->keyval;
   if (KBkey == GDK_KEY_F1) KBstate(kbevent,0);                                  //  5.6
   return;
}


//  draw text into layout and return pixel dimensions of enclosing rectangle

void gmenuznames::draw_text(cairo_t *cr, char *text, int x, int y, int &w, int &h)
{
   using namespace gmenuznames;

   static PangoFontDescription   *pfont = 0;
   static PangoLayout            *playout = 0;

   zthreadcrash();

   if (! pfont) {
      pfont = pango_font_description_from_string(menuFont);                      //  first call, get font sizing poop
      playout = gtk_widget_create_pango_layout(layout,0);
      pango_layout_set_font_description(playout,pfont);
   }

   pango_layout_set_text(playout,text,-1);                                       //  compute layout
   pango_layout_get_pixel_size(playout,&w,&h);                                   //  pixel width and height of layout

   cairo_move_to(cr,x,y);                                                        //  draw layout with text
   cairo_set_source_rgb(cr,0,0,0);
   pango_cairo_show_layout(cr,playout);

   return;
}


//  dialog to create a new menu entry from user inputs

void gmenuznames::edit_menu()
{
   using namespace gmenuznames;

   zthreadcrash();

   if (me < 0) {                                                                 //  new menu entry
      if (NME == maxME) {
         zmessageACK(mWin,"capacity limit exceeded");
         return;
      }
      me = NME;
      memset(&menus[me],0,sizeof(menuent));                                      //  clear all data
   }

/**
         menu text [________________________]
         menu func [________________________]
         menu icon [_______________] [browse]
         icon size [___|+-]  [x] close window

                    [apply] [delete] [cancel]
**/

   if (! zdedit)                                                                 //  create dialog if not already
   {
      zdedit = zdialog_new("edit menu entry",mWin,"apply","delete","cancel",null);
      zdialog_add_widget(zdedit,"hbox","hb1","dialog");
      zdialog_add_widget(zdedit,"vbox","vb1","hb1",0,"homog|space=3");
      zdialog_add_widget(zdedit,"vbox","vb2","hb1",0,"homog|expand");
      zdialog_add_widget(zdedit,"label","lab11","vb1","menu text");
      zdialog_add_widget(zdedit,"label","lab12","vb1","menu func");
      zdialog_add_widget(zdedit,"label","lab13","vb1","menu icon");
      zdialog_add_widget(zdedit,"label","lab14","vb1","icon size");
      zdialog_add_widget(zdedit,"entry","text","vb2",0,"size=40|space=2");
      zdialog_add_widget(zdedit,"entry","func","vb2",0,"size=40|space=2");
      zdialog_add_widget(zdedit,"hbox","hb2","vb2",0,"expand|space=2");
      zdialog_add_widget(zdedit,"entry","icon","hb2",0,"expand");
      zdialog_add_widget(zdedit,"button","browse","hb2","browse","space=5");
      zdialog_add_widget(zdedit,"hbox","hb3","vb2",0,"space=2");
      zdialog_add_widget(zdedit,"spin","size","hb3","24|64|1|24");
      zdialog_add_widget(zdedit,"check","kill","hb3","close window","space=30");
      zdialog_run(zdedit,edit_menu_event);
   }

   if (menus[me].menu)                                                           //  stuff menu text into dialog
      zdialog_stuff(zdedit,"text",menus[me].menu);
   else zdialog_stuff(zdedit,"text","");

   if (menus[me].func)                                                           //  stuff menu function
      zdialog_stuff(zdedit,"func",menus[me].func);
   else zdialog_stuff(zdedit,"func","");

   if (menus[me].icon)                                                           //  stuff icon file
      zdialog_stuff(zdedit,"icon",menus[me].icon);
   else zdialog_stuff(zdedit,"icon","");

   if (menus[me].size)                                                           //  stuff icon size
      zdialog_stuff(zdedit,"size",menus[me].size);

   if (menus[me].kill)                                                           //  stuff window kill flag
      zdialog_stuff(zdedit,"kill",1);
   else zdialog_stuff(zdedit,"kill",0);

   if (me == NME) {                                                              //  new menu entry
      menus[me].xpos = mpx;                                                      //  initial position from mouse
      menus[me].ypos = mpy;
   }

   return;
}


//  menu entry dialog event function

int gmenuznames::edit_menu_event(zdialog *zd, cchar *event)
{
   using namespace gmenuznames;

   char        text[maxText];
   std::string iconfile;
   int         size;
   PIXBUF      *pixbuf;
   GError      *gerror;

   if (strmatch(event,"browse")) {                                               //  browse for icon file
      iconfile = zgetfile("select icon",null,"file",0);
      if (iconfile.length() > 0) zdialog_stuff(zd,"icon",iconfile.c_str());
   }

   if (zd->zstat)                                                                //  dialog complete
   {
      if (zd->zstat == 2) {                                                      //  [delete] - delete menu entry
         if (me < 0 || me >= NME) return 0;
         for (int me2 = me; me2 < NME-1; me2++)                                  //  remove menu entry and close hole
            menus[me2] = menus[me2+1];
         NME--;
         Fchanged = 1;                                                           //  mark menu revised
         gtk_widget_queue_draw(layout);                                          //  repaint window
      }

      if (zd->zstat != 1) {                                                      //  not [apply] - kill dialog
         zdialog_free(zdedit);
         return 0;
      }
                                                                                 //  [apply] - update menu from dialog data
      zdialog_fetch(zd,"text",text,maxText);
      if (*text) menus[me].menu = zstrdup(text);                                 //  menu text, optional
      else menus[me].menu = 0;

      zdialog_fetch(zd,"func",text,maxText);                                     //  menu function name
      strTrim2(text);
      if (*text) menus[me].func = zstrdup(text);
      else menus[me].func = 0;

      zdialog_fetch(zd,"icon",text,maxText);                                     //  menu icon file, optional
      strTrim2(text);
      if (*text) {
         zdialog_fetch(zd,"size",size);                                          //  icon size
         gerror = 0;
         pixbuf = gdk_pixbuf_new_from_file_at_size(text,size,size,&gerror);
         if (! pixbuf) {
            if (gerror) zmessageACK(mWin,gerror->message);                       //  bad icon file
            zd->zstat = 0;                                                       //  keep dialog open
            return 0;                                                            //  do nothing
         }
         menus[me].icon = zstrdup(text);
         menus[me].pixbuf = pixbuf;
         menus[me].size = size;
      }
      else {
         menus[me].icon = 0;
         menus[me].pixbuf = 0;
         menus[me].size = 0;
      }

      zdialog_fetch(zd,"kill",menus[me].kill);                                   //  popup window kill flag

      if (me == NME) NME++;                                                      //  if new menu entry, incr. count

      Fchanged = 1;                                                              //  mark menu revised

      zdialog_free(zdedit);                                                      //  destroy dialog
      gtk_widget_queue_draw(layout);                                             //  repaint window
   }

   return 0;
}


//  function to accept drag-drop of a desktop application icon
//  process the associated text desktop file

void gmenuznames::drag_drop_event(int mpx, int mpy, const char *file)                  //  v.6.2
{
   using namespace gmenuznames;
   
   FILE     *fid;
   char     buff[100], *pp;
   char     name[100], exec[100], icon[100];
   GError   *gerror = 0;
   PIXBUF   *pixbuf = 0;
   STATB    statb;
   int      ii, jj, err;

   cchar    *icondir[3] = { "", 
                            "/usr/share/app-install/icons/",
                            "/usr/share/pixmaps/" }; 
   cchar    *iconext[3] = { "", ".png", ".svg" };
   
   if (NME == maxME) {
      zmessageACK(mWin,"capacity limit exceeded");
      return;
   }

   me = NME;
   memset(&menus[me],0,sizeof(menuent));                                         //  clear all data

   fid = fopen(file,"r");                                                        //  get .desktop file
   if (! fid) {
      zmessageACK(mWin,strerror(errno));
      return;
   }
   
   *name = *exec = *icon = 0;
   
   while (true) {
      pp = fgets_trim(buff,100,fid);
      if (! pp) break;
      if (strmatchN(buff,"Name=",5))                                             //  app name
         strncpy0(name,buff+5,100);
      if (strmatchN(buff,"Exec=",5))                                             //  executable
         strncpy0(exec,buff+5,100);
      if (strmatchN(buff,"Icon=",5))                                             //  icon (usr/share/app-install/icons/*.png)
         strncpy0(icon,buff+5,100);
   }
   
   fclose(fid);

   printz("drag-drop: %d %d %s %s %s \n",mpx,mpy,name,exec,icon);
   
   menus[me].xpos = mpx;                                                         //  position from mouse
   menus[me].ypos = mpy;

   menus[me].menu = zstrdup(name);                                               //  menu text
   menus[me].func = zstrdup(exec);                                               //  menu function
   
   if (*icon)
   {
      for (ii = 0; ii < 2; ii++)                                                 //  search icon locations
      {
         for (jj = 0; jj < 3; jj++)                                              //  try known icon file types
         {
            *buff = 0;
            strncatv(buff,100,icondir[ii],icon,iconext[jj],null);
            err = stat(buff,&statb);
            if (! err) break;
         }
         if (jj < 3) break;
      }
         
      if (err) zmessageACK(mWin,"icon file not found: %s \n",icon);
      else {
         pixbuf = gdk_pixbuf_new_from_file_at_size(buff,32,32,&gerror);
         if (! pixbuf) zmessageACK(mWin,"icon file error: %s \n",buff);
         else {
            menus[me].icon = zstrdup(buff);
            menus[me].pixbuf = pixbuf;
            menus[me].size = 32;
         }
      }
   }

   menus[me].kill = 1;                                                           //  popup window kill flag

   NME++;                                                                        //  incr. menu count
   Fchanged = 1;                                                                 //  mark menu revised
   gtk_widget_queue_draw(layout);                                                //  repaint window

   return;
}


/**************************************************************************

   Vertical Menu / Toolbar

   Build a custom vertical menu and/or toolbar in a vertical packing box

   vbm = Vmenu_new(GtkWidget *vbox)                         create base menu

   Vmenu_add(vbm, name, icon, iww, ihh, desc, func, arg)    add menu item or toolbar button
      Vmenu       *vbm
      cchar       *name, *icon, *desc, *arg
      int         iww, ihh 
      void func(GtkWidget *, cchar *name)

   Create a vertical menu / toolbar in a vertical packing box.
   Added items can have a menu name, icon, description, response function,
   and function argument. 'name' and 'icon' can be null but not both.

   name        menu name
   icon        menu icon, filespec for a .png file
   iww, ihh    size of icon in menu display
   desc        optional tool tip if mouse is hovered over displayed menu

   When 'name/icon' is clicked, 'func' is called with 'arg'.
   If 'arg' is null, 'name' is used instead.

   To create a menu entry that is a popup menu with multiple entries, do as follows:
        popup = create_popmenu();
        add_popup_menu_item(popup ...);         see create_popmenu()
        add_popup_menu_item(popup ...);
        ...
        Vmenu_add(vbm, name, icon, ww, hh, desc, create_popmenu, (cchar *) popup);

   i.e. use create_popmenu() as the response function and use the previously
        created menu 'popup' as the argument (cast to cchar *).

***/


namespace Vmenunames
{
   #define menufont1 "sans 10"                                                   //  menu font
   #define menufont2 "sans bold 10"                                              //  menu font (selected)
   #define fontheight 18                                                         //  menu text height in layout
   #define margin 5                                                              //  margins for menu text
   #define BGCOLOR 59000,59000,59000                                             //  = cairo 0.9,0.9,0.9

   PangoFontDescription    *pfont1, *pfont2;
   PangoAttrList           *pattrlist;
   PangoAttribute          *pbackground;

   void  wpaint(GtkWidget *, cairo_t *, Vmenu *);                                //  window repaint - draw event
   void  mouse_event(GtkWidget *, GdkEventButton *, Vmenu *);                    //  mouse event function
   void  paint_menu(cairo_t *cr, Vmenu *vbm, int me, int hilite);                //  paint menu entry, opt. highlight
}


Vmenu *Vmenu_new(GtkWidget *vbox)                                                //  revised 5.7
{
   using namespace Vmenunames;

   int      cc;

   zthreadcrash();

   cc = sizeof(Vmenu);
   Vmenu *vbm = (Vmenu *) zmalloc(cc);
   memset(vbm,0,cc);
   vbm->vbox = vbox;
   vbm->layout = gtk_layout_new(0,0);
   vbm->mcount = 0;
   gtk_box_pack_start(GTK_BOX(vbox),vbm->layout,1,1,0);
   vbm->xmax = vbm->ymax = 10;                                                   //  layout size                        5.8

   pattrlist = pango_attr_list_new();
   pbackground = pango_attr_background_new(BGCOLOR);
   pango_attr_list_change(pattrlist,pbackground);

   pfont1 = pango_font_description_from_string(menufont1);
   pfont2 = pango_font_description_from_string(menufont2);

   gtk_widget_add_events(vbm->layout,GDK_BUTTON_PRESS_MASK);
   gtk_widget_add_events(vbm->layout,GDK_BUTTON_RELEASE_MASK);
   gtk_widget_add_events(vbm->layout,GDK_POINTER_MOTION_MASK);
   gtk_widget_add_events(vbm->layout,GDK_LEAVE_NOTIFY_MASK);
   G_SIGNAL(vbm->layout,"button-press-event",mouse_event,vbm);
   G_SIGNAL(vbm->layout,"button-release-event",mouse_event,vbm);
   G_SIGNAL(vbm->layout,"motion-notify-event",mouse_event,vbm);
   G_SIGNAL(vbm->layout,"leave-notify-event",mouse_event,vbm);
   G_SIGNAL(vbm->layout,"draw",wpaint,vbm);

   return vbm;
}


void Vmenu_add(Vmenu *vbm, cchar *name, cchar *icon, int iconww, int iconhh, cchar *desc, cbFunc func, cchar *arg)
{
   using namespace Vmenunames;

   int         me, cc1, cc2, xpos, ww, hh;
   char        iconpath[200], *mdesc, *name__;
   cchar       *blanks = "                    ";                                 //  20 blanks
   PIXBUF      *pixbuf;
   GError      *gerror = 0;

   PangoLayout             *playout;
   PangoFontDescription    *pfont;

   zthreadcrash();

   if (! name && ! icon) return;

   me = vbm->mcount++;                                                           //  track no. menu entries

   if (name) vbm->menu[me].name = zstrdup(name);                                 //  create new menu entry from caller data

   if (icon) {
      vbm->menu[me].icon = zstrdup(icon);
      vbm->menu[me].iconww = iconww;                                             //  5.8
      vbm->menu[me].iconhh = iconhh;
   }

   if (desc) {                                                                   //  pad description with blanks for looks
      cc1 = strlen(desc);                                                        //  5.6
      mdesc = (char *) zmalloc(cc1+3);
      mdesc[0] = ' ';
      strcpy(mdesc+1,desc);
      strcpy(mdesc+cc1+1," ");
      vbm->menu[me].desc = mdesc;
   }

   vbm->menu[me].func = func;
   vbm->menu[me].arg = name;                                                     //  argument is menu name or arg if avail.
   if (arg) vbm->menu[me].arg = arg;

   vbm->menu[me].pixbuf = 0;

   if (icon) {                                                                   //  if icon is named, get pixbuf
      *iconpath = 0;
      strncatv(iconpath,199,zfuncs::zicondir,"/",icon,null);
      pixbuf = gdk_pixbuf_new_from_file_at_scale(iconpath,iconww,iconhh,1,&gerror);
      if (pixbuf) vbm->menu[me].pixbuf = pixbuf;
   }

   if (me == 0) vbm->ymax = margin;                                              //  first menu, top position

   vbm->menu[me].iconx = 0;
   vbm->menu[me].icony = 0;
   vbm->menu[me].namex = 0;
   vbm->menu[me].namey = 0;

   if (icon) {
      vbm->menu[me].iconx = margin;                                              //     ______
      vbm->menu[me].icony = vbm->ymax;                                           //    |      |
      if (name) {                                                                //    | icon | menu name
         vbm->menu[me].namex = margin + iconww + margin;                         //    |______|
         vbm->menu[me].namey = vbm->ymax + (iconhh - fontheight) / 2 + 2;
      }
      vbm->menu[me].ylo = vbm->ymax;
      vbm->ymax += iconhh + iconhh / 8;                                          //  position for next menu entry
      vbm->menu[me].yhi = vbm->ymax;
      if (margin + iconww > vbm->xmax) vbm->xmax = margin + iconww;              //  keep track of max. layout width
   }

   else if (name) {
      vbm->menu[me].namex = margin;                                              //  menu name
      vbm->menu[me].namey = vbm->ymax;
      vbm->menu[me].ylo = vbm->ymax;
      vbm->ymax += fontheight;
      vbm->menu[me].yhi = vbm->ymax;
   }

   vbm->menu[me].playout1 = gtk_widget_create_pango_layout(vbm->layout,0);
   vbm->menu[me].playout2 = gtk_widget_create_pango_layout(vbm->layout,0);

   if (name) {
      xpos = vbm->menu[me].namex;

      cc1 = strlen(name);                                                        //  prepare menu name with trailing blanks
      cc2 = 0.3 * cc1 + 3;                                                       //    so normal name longer than bold name
      if (cc2 > 20) cc2 = 20;                                                    //      otherwise bold not 100% overwritten
      name__ = zstrdup(name,cc2);
      strncpy0(name__+cc1,blanks,cc2);

      playout = vbm->menu[me].playout1;                                          //  normal font
      pfont = pfont1;
      pango_layout_set_attributes(playout,pattrlist);
      pango_layout_set_font_description(playout,pfont);
      pango_layout_set_text(playout,name__,-1);                                  //  compute layout
      pango_layout_get_pixel_size(playout,&ww,&hh);                              //  pixel width and height of layout

      playout = vbm->menu[me].playout2;                                          //  bold font
      pfont = pfont2;
      pango_layout_set_attributes(playout,pattrlist);
      pango_layout_set_font_description(playout,pfont);
      pango_layout_set_text(playout,name,-1);                                    //  compute layout
      pango_layout_get_pixel_size(playout,&ww,&hh);                              //  pixel width and height of layout
      if (xpos + ww > vbm->xmax) vbm->xmax = xpos + ww;                          //  keep track of max. layout width
   }

   gtk_widget_set_size_request(vbm->layout,vbm->xmax+margin,0);                  //  add right margin to layout width

   return;
}


//  paint window when created, exposed, resized

void Vmenunames::wpaint(GtkWidget *widget, cairo_t *cr, Vmenu *vbm)
{
   using namespace Vmenunames;

   cairo_set_source_rgb(cr,0.9,0.9,0.9);                                         //  white background
   cairo_paint(cr);

   for (int me = 0; me < vbm->mcount; me++)                                      //  paint all menu entries
      paint_menu(cr,vbm,me,0);

   return;
}


//  draw menu icon and text into layout

void Vmenunames::paint_menu(cairo_t *cr, Vmenu *vbm, int me, int hilite)
{
   using namespace Vmenunames;

   PIXBUF         *pixbuf;
   PangoLayout    *playout;
   int            xpos, ypos;
   int            iconww, iconhh;
   cchar          *name;

   pixbuf = vbm->menu[me].pixbuf;                                                //  icon
   if (pixbuf) {                                                                 //  draw menu icon at menu position
      xpos = vbm->menu[me].iconx;
      ypos = vbm->menu[me].icony;
      iconww = vbm->menu[me].iconww;
      iconhh = vbm->menu[me].iconhh;

      if (! hilite) {                                                            //  erase box around icon
         cairo_set_source_rgb(cr,0.9,0.9,0.9);
         cairo_rectangle(cr,xpos-1,ypos-1,iconww+2,iconhh+2);
         cairo_fill(cr);
      }

      gdk_cairo_set_source_pixbuf(cr,pixbuf,xpos,ypos);                          //  draw icon
      cairo_paint(cr);

      if (hilite) {
         cairo_set_source_rgb(cr,0.5,0.5,0.5);                                   //  draw box around icon
         cairo_set_line_width(cr,1);
         cairo_set_line_join(cr,CAIRO_LINE_JOIN_ROUND);
         cairo_rectangle(cr,xpos,ypos,iconww,iconhh);
         cairo_stroke(cr);
      }
   }

   name = vbm->menu[me].name;                                                    //  menu text
   if (name) {                                                                   //  draw menu text at menu position
      xpos = vbm->menu[me].namex;
      ypos = vbm->menu[me].namey;
      cairo_move_to(cr,xpos,ypos);                                               //  draw layout with text
      cairo_set_source_rgb(cr,0,0,0);
      if (hilite) playout = vbm->menu[me].playout2;
      else playout = vbm->menu[me].playout1;
      pango_cairo_show_layout(cr,playout);
   }

   return;
}


//  mouse event function - capture buttons and drag movements

void Vmenunames::mouse_event(GtkWidget *widget, GdkEventButton *event, Vmenu *vbm)
{
   using namespace Vmenunames;

   GdkWindow   *gdkwin;
   cchar       *desc;
   int         me, mpx, mpy, button, ww, ylo, yhi;
   static int  me0 = -1;
   cairo_t     *cr;

   gdkwin = gtk_layout_get_bin_window(GTK_LAYOUT(widget));
   cr = gdk_cairo_create(gdkwin);

   mpx = int(event->x);                                                          //  mouse position
   mpy = int(event->y);
   button = event->button;

   if (event->type == GDK_MOTION_NOTIFY)                                         //  mouse inside layout
   {
      for (me = 0; me < vbm->mcount; me++) {                                     //  find menu where mouse is
         ylo = vbm->menu[me].ylo;
         yhi = vbm->menu[me].yhi;
         if (mpy < ylo) continue;
         if (mpy < yhi) break;
      }

      if (me != me0 && me0 >= 0) {                                               //  unhilight prior
         paint_menu(cr,vbm,me0,0);
         desc = vbm->menu[me0].desc;                                             //  remove prior tool tip
         if (desc) poptext_mouse(0,0,0,0,0);
         me0 = -1;
      }

      if (me == me0) goto MEreturn;                                              //  same as before
      if (me == vbm->mcount) goto MEreturn;                                      //  no new menu match

      paint_menu(cr,vbm,me,1);                                                   //  hilight menu entry at mouse
      desc = vbm->menu[me].desc;                                                 //  show tool tip
      if (desc) poptext_mouse(desc,30,20,0,10);

      me0 = me;                                                                  //  remember last match
      goto MEreturn;
   }

   if (me0 >= 0)                                                                 //  mouse left layout
   {
      paint_menu(cr,vbm,me0,0);                                                  //  unhilight prior
      desc = vbm->menu[me0].desc;                                                //  remove prior tool tip
      if (desc) poptext_mouse(0,0,0,0,0);
      me0 = -1;
   }

   if (event->type == GDK_BUTTON_RELEASE)                                        //  menu entry clicked
   {
      for (me = 0; me < vbm->mcount; me++) {                                     //  look for clicked menu entry
         ylo = vbm->menu[me].ylo;
         yhi = vbm->menu[me].yhi;
         if (mpy < ylo) continue;
         if (mpy < yhi) break;
      }

      if (me == vbm->mcount) goto MEreturn;                                      //  no menu match

      zfuncs::vmenuclickbutton = button;                                         //  1/2/3 = left/mid/right button      5.9

      ww = vbm->menu[me].iconww;                                                 //  get horiz. click posn. on menu icon
      if (ww) mpx = 100 * (mpx - margin) / ww;                                   //    scale 0-100
      else mpx = 0;                                                              //                                     5.8
      if (mpx < 0) mpx = 0;
      if (mpx > 100) mpx = 100;
      zfuncs::vmenuclickposn = mpx;

      paint_menu(cr,vbm,me,0);                                                   //  unhighlight menu

      if (vbm->menu[me].func)
         (vbm->menu[me].func)(widget,vbm->menu[me].arg);                         //  caller function(widget,arg)
   }

MEreturn:
   cairo_destroy(cr);
   return;
}


/**************************************************************************
   simplified GTK dialog functions

   create a dialog with response buttons (OK, cancel ...)
   add widgets to dialog (button, text entry, combo box ...)
   put data into widgets (text or numeric data)
   run the dialog, get user inputs (button press, text entry, checkbox selection ...)
   run caller event function when widgets change from user inputs
   run caller event function when dialog is completed or canceled
   get dialog completion status (OK, cancel, destroyed ...)
   get data from dialog widgets (the user inputs)
   destroy the dialog and free memory

***************************************************************************/


//  private functions for widget events and dialog completion

int  zdialog_widget_event(GtkWidget *, zdialog *zd);
int  zdialog_delete_event(GtkWidget *, GdkEvent *, zdialog *zd);


//  create a new zdialog dialog
//  The title and parent arguments may be null.
//  optional arguments: up to zdmaxbutts button labels followed by null
//  returned dialog status: +N = button N (1 to zdmaxbutts)
//                          <0 = [x] button or other GTK destroy action
//  completion buttons are also events like other widgets
//  all dialogs run parallel, use zdialog_wait() if needed
//  The status returned by zdialog_wait() corresponds to the button
//  (1-10) used to end the dialog. Status < 0 indicates the [x] button
//  was used or the dialog was killed by the program itself.

zdialog * zdialog_new(cchar *title, GtkWidget *parent, ...)                      //  parent added
{
   using namespace zfuncs;

   zdialog        *zd;
   GtkWidget      *dialog, *hbox, *vbox, *butt, *hsep;
   cchar          *bulab[zdmaxbutts];
   int            cc, ii, nbu;
   va_list        arglist;

   zthreadcrash();                                                               //  thread usage not allowed

   va_start(arglist,parent);
   for (nbu = 0; nbu < zdmaxbutts; nbu++)                                        //  get completion buttons
   {
      bulab[nbu] = va_arg(arglist, cchar *);
      if (! bulab[nbu]) break;
   }
   va_end(arglist);

   if (! title) title = " ";                                                     //  5.0

   dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);                                 //  use window, not dialog          5.5

   gtk_window_set_title(GTK_WINDOW(dialog),title);
   vbox = gtk_box_new(VERTICAL,0);                                               //  vertical packing box
   gtk_container_add(GTK_CONTAINER(dialog),vbox);                                //  add to main window
   gtk_box_set_spacing(GTK_BOX(vbox),2);
   gtk_container_set_border_width(GTK_CONTAINER(vbox),5);                        //  6.0

   cc = sizeof(zdialog);                                                         //  allocate zdialog
   zd = (zdialog *) zmalloc(cc);

   if (zdialog_count == zdialog_max) {                                           //  add to active list              5.9
      for (ii = 0; ii < zdialog_count; ii++)
         printz("dialog: %s \n",zdialog_list[ii]->widget[0].data);
      zappcrash("max. zdialogs exceeded");
   }

   zdialog_list[zdialog_count] = zd;
   zdialog_count++;

   if (parent)
      gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(parent));

   if (nbu)                                                                      //  completion buttons
   {
      hbox = gtk_box_new(HORIZONTAL,2);                                          //  add hbox for buttons at bottom
      gtk_box_pack_end(GTK_BOX(vbox),hbox,0,0,2);
      hsep = gtk_separator_new(HORIZONTAL);                                      //  add separator line
      gtk_box_pack_end(GTK_BOX(vbox),hsep,0,0,3);

      for (ii = nbu-1; ii >= 0; ii--) {                                          //  add buttons to hbox
         butt = gtk_button_new_with_label(bulab[ii]);                            //  reverse order nbu-1...0
         gtk_box_pack_end(GTK_BOX(hbox),butt,0,0,2);
         G_SIGNAL(butt,"clicked",zdialog_widget_event,zd);                       //  connect to event function
         zd->compwidget[ii] = butt;                                              //  save button widgets
         zd->compbutton[ii] = bulab[ii];                                         //  and button labels               5.9
      }
   }
   zd->compbutton[nbu] = 0;                                                      //  mark EOL                        5.8

   ////  compensate GTK broken API - entry widgets automatically 150 pixels wide even if expand=0                    6.2
   gtk_window_set_default_size(GTK_WINDOW(dialog),10,10);

   zd->parent = parent;                                                          //  parent window or null
   zd->sentinel1 = zdsentinel | (lrandz() & 0x0000FFFF);                         //  validity sentinels              5.8
   zd->sentinel2 = zd->sentinel1;                                                //  fixed part + random part
   zd->eventCB = 0;                                                              //  no user event callback function
   zd->zstat = 0;                                                                //  no zdialog status
   zd->disabled = 1;                                                             //  widget signals disabled
   zd->saveposn = 0;                                                             //  position not saved

   zd->widget[0].type = "dialog";                                                //  set up 1st widget = dialog
   zd->widget[0].name = "dialog";
   zd->widget[0].pname = 0;
   zd->widget[0].data = zstrdup(title);
   zd->widget[0].cblist = 0;
   zd->widget[0].widget = dialog;

   zd->widget[1].type = 0;                                                       //  eof - no contained widgets yet
   return zd;
}


//  change a zdialog title

void zdialog_set_title(zdialog *zd, cchar *title)
{
   gtk_window_set_title(GTK_WINDOW(zd->widget[0].widget),title);
   return;
}


//  set a zdialog to be modal

void zdialog_set_modal(zdialog *zd)                                              //  6.1
{
   GtkWidget  *widget = zdialog_widget(zd,"dialog");
   gtk_window_set_modal(GTK_WINDOW(widget),1);
   gtk_window_set_keep_above(GTK_WINDOW(widget),1);
   return;
}


//  set a zdialog to be decorated or not

void zdialog_set_decorated(zdialog *zd, int decorated)                           //  6.1
{
   void zdialog_drag(GtkWidget *widget, GdkEventButton *event, void *);

   GtkWidget  *widget;

   widget = zdialog_widget(zd,"dialog");
   gtk_window_set_decorated(GTK_WINDOW(widget),decorated);
   if (decorated) return;
   gtk_widget_add_events(widget,GDK_BUTTON_PRESS_MASK);
   gtk_widget_add_events(widget,GDK_BUTTON_RELEASE_MASK);
   gtk_widget_add_events(widget,GDK_POINTER_MOTION_MASK);
   G_SIGNAL(widget,"button-press-event",zdialog_drag,0);                         //  connect mouse events to drag
   G_SIGNAL(widget,"button-release-event",zdialog_drag,0);                       //    undecorated window
   G_SIGNAL(widget,"motion-notify-event",zdialog_drag,0);
   return;
}

void zdialog_drag(GtkWidget *widget, GdkEventButton *event, void *)              //  6.1
{
   static int  bdown = 0, type;
   static int  mx0, my0, mx, my;
   static int  wx0, wy0, wx, wy;

   type = event->type;
   gdk_device_get_position(zfuncs::mouse,0,&mx,&my);                             //  mouse position in monitor

   if (type == GDK_BUTTON_PRESS) {
      bdown = 1;
      mx0 = mx;                                                                  //  drag start
      my0 = my;
      gtk_window_get_position(GTK_WINDOW(widget),&wx0,&wy0);                     //  initial window position
   }

   if (type == GDK_BUTTON_RELEASE)
      bdown = 0;

   if (type == GDK_MOTION_NOTIFY) {
      if (! bdown) return;
      wx = wx0 + mx - mx0;
      wy = wy0 + my - my0;
      gtk_window_move(GTK_WINDOW(widget),wx,wy);
   }
   
   return;
}


//  present a zdialog (visible and on top)

void zdialog_present(zdialog *zd)
{
   GtkWidget  *widget = zdialog_widget(zd,"dialog");
   gtk_window_present(GTK_WINDOW(widget));
   return;
}


//  add widget to existing zdialog
//
//   Arguments after parent are optional and default to 0.
//   zd         zdialog *, created with zdialog_new()
//   type       string, one of the widget types listed below
//   name       string, widget name, used to stuff or fetch widget data
//   parent     string, parent name: "dialog" or a previously added container widget
//   data       string, initial data for widget (label name, entry string, spin value, etc.)
//   size       cc for text entry or pixel size for image widget
//   homog      for hbox or vbox to make even space allocation for contained widgets
//   expand     widget should expand with dialog box expansion
//   space      extra space between this widget and neighbors, pixels
//   wrap       allow text to wrap at right margin
//
//   data can be a string ("initial widget data") or a number in string form ("12.345")
//   data for togbutt / check / radio: use "0" or "1" for OFF or ON
//   data for spin / hscale / vscale: use format "min|max|step|value" (default: 0 | 100 | 1 | 50)
//   data for colorbutt: use "rrr|ggg|bbb" with values 0-255 for each RGB color.
//   This format is used to initialize the control and read back when user selects a color.
//   Multiple radio buttons with same parent are a group: pressing one turns the others OFF.

int zdialog_add_widget (
     zdialog *zd, cchar *type, cchar *name, cchar *pname,                        //  mandatory args
     cchar *data, int size, int homog, int expand, int space, int wrap)          //  optional args (default = 0)
{
   using namespace zfuncs;

   GtkWidget      *widget = 0, *pwidget = 0;
   GtkWidget      *entry, *image, *vbox;
   GtkTextBuffer  *editBuff = 0;
   PIXBUF         *pixbuf = 0;
   GdkRGBA        gdkrgba;
   GError         *gerror = 0;
   cchar          *pp, *ptype = 0;
   char           vdata[30], iconpath[200];
   double         min, max, step, val;
   double         f256 = 1.0 / 256.0;
   int            iiw, iip, kk, err;

   zthreadcrash();

   if (! zdialog_valid(zd)) zappcrash("zdialog invalid");

   for (iiw = 1; zd->widget[iiw].type; iiw++);                                   //  find next avail. slot
   if (iiw > zdmaxwidgets-2) zappcrash("too many widgets: %d",iiw);

   zd->widget[iiw].type = zstrdup(type);                                         //  initz. widget struct
   zd->widget[iiw].name = zstrdup(name);                                         //  all strings in nonvolatile mem
   zd->widget[iiw].pname = zstrdup(pname);
   zd->widget[iiw].data = 0;
   zd->widget[iiw].cblist = 0;
   zd->widget[iiw].size = size;
   zd->widget[iiw].homog = homog;
   zd->widget[iiw].expand = expand;
   zd->widget[iiw].space = space;
   zd->widget[iiw].wrap = wrap;
   zd->widget[iiw].widget = 0;
   zd->widget[iiw].stopKB = 0;

   zd->widget[iiw+1].type = 0;                                                   //  set new EOF marker

   if (strmatchV(type,"dialog","hbox","vbox","hsep","vsep","frame","scrwin",
                      "label","link","entry","edit","text","button","togbutt",
                      "imagebutt","colorbutt","check","combo","comboE","radio",
                      "spin","hscale","vscale","image",null) == 0) {
      printz("*** zdialog, bad widget type: %s \n",type);
      return 0;
   }

   for (iip = iiw-1; iip >= 0; iip--)                                            //  find parent (container) widget
      if (strmatch(pname,zd->widget[iip].name)) break;
   if (iip < 0) zappcrash("zdialog, no parent for widget: %s",name);

   pwidget = zd->widget[iip].widget;                                             //  parent widget, type
   ptype = zd->widget[iip].type;

   if (strmatchV(ptype,"dialog","hbox","vbox","frame","scrwin",null) == 0)
      zappcrash("zdialog, bad widget parent type: %s",ptype);

   if (strmatch(type,"hbox")) widget = gtk_box_new(HORIZONTAL,space);            //  expandable container boxes   5.4
   if (strmatch(type,"vbox")) widget = gtk_box_new(VERTICAL,space);
   if (strstr("hbox vbox",type))
      gtk_box_set_homogeneous(GTK_BOX(widget),homog);

   if (strmatch(type,"hsep")) widget = gtk_separator_new(HORIZONTAL);            //  horiz. & vert. separators
   if (strmatch(type,"vsep")) widget = gtk_separator_new(VERTICAL);

   if (strmatch(type,"frame")) {                                                 //  frame around contained widgets
      widget = gtk_frame_new(data);
      gtk_frame_set_shadow_type(GTK_FRAME(widget),GTK_SHADOW_IN);
      data = 0;                                                                  //  5.3 data not further used
   }

   if (strmatch(type,"scrwin")) {                                                //  scrolled window container
      widget = gtk_scrolled_window_new(0,0);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget),
                        GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
      data = 0;                                                                  //  5.3 data not further used
   }

   if (strmatch(type,"label")) {                                                 //  label (static text)
      widget = gtk_label_new(data);
      data = 0;                                                                  //  5.3 data not further used
   }

   if (strmatch(type,"link")) {                                                  //  label is clickable
      if (strmatch(name,"nolabel"))
         widget = gtk_link_button_new(data);                                     //  link is also label
      else                                                                       //  5.6
         widget = gtk_link_button_new_with_label(data,name);                     //  label >> link
      G_SIGNAL(widget,"clicked",zdialog_widget_event,zd);
      data = 0;                                                                  //  5.3 data not further used
   }

   if (strmatch(type,"entry")) {                                                 //  1-line text entry
      widget = gtk_entry_new();
      if (data) gtk_entry_set_text(GTK_ENTRY(widget),data);
      if (size) gtk_entry_set_width_chars(GTK_ENTRY(widget),size);
      /// fix huge default entry widgets but not buildable with gtk 3.10 (ubuntu 14.04)      //////
      /// if (! expand) gtk_entry_set_max_width_chars(GTK_ENTRY(widget),size);               //////
      G_SIGNAL(widget,"changed",zdialog_widget_event,zd);
   }

   if (strmatch(type,"edit")) {                                                  //  multiline text input 
      widget = gtk_text_view_new();
      gtk_text_view_set_left_margin(GTK_TEXT_VIEW(widget),3);                    //  6.1
      editBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
      if (data) gtk_text_buffer_set_text(editBuff,data,-1);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(widget),1);
      if (wrap) gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widget),GTK_WRAP_WORD);
      G_SIGNAL(editBuff,"changed",zdialog_widget_event,zd);                      //  buffer signals, not widget
   }

   if (strmatch(type,"text")) {                                                  //  text output (not editable)
      widget = gtk_text_view_new();
      gtk_text_view_set_left_margin(GTK_TEXT_VIEW(widget),3);                    //  6.1
      editBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
      if (data) gtk_text_buffer_set_text(editBuff,data,-1);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(widget),0);
      if (wrap) gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widget),GTK_WRAP_WORD);
   }

   if (strmatch(type,"button")) {                                                //  button
      widget = gtk_button_new_with_label(data);
      G_SIGNAL(widget,"clicked",zdialog_widget_event,zd);
      data = 0;                                                                  //  5.3 data not further used
   }

   if (strmatch(type,"togbutt")) {                                               //  toggle button
      widget = gtk_toggle_button_new_with_label(data);
      G_SIGNAL(widget,"toggled",zdialog_widget_event,zd);
      data = "0";                                                                //  5.3 default data
   }

   if (strmatch(type,"imagebutt")) {                                             //  button with image                  6.0
      snprintf(iconpath,200,"%s/%s",get_zicondir(),data);
      data = 0;
      pixbuf = gdk_pixbuf_new_from_file_at_scale(iconpath,size,size,1,&gerror);
      if (pixbuf) {
         image = gtk_image_new_from_pixbuf(pixbuf);
         g_object_unref(pixbuf);
      }
      else image = gtk_image_new_from_icon_name("missing",GTK_ICON_SIZE_BUTTON);
      widget = gtk_button_new_with_label(data);
      gtk_button_set_image(GTK_BUTTON(widget),image);
      G_SIGNAL(widget,"clicked",zdialog_widget_event,zd);
   }

   if (strmatch(type,"colorbutt")) {                                             //  color edit button
      if (! data) data = "0|0|0";                                                //  data format: "nnn|nnn|nnn" = RGB
      pp = strField(data,'|',1); gdkrgba.red = f256 * atoi(pp);                  //  RGB values are 0-1                 5.8
      pp = strField(data,'|',2); gdkrgba.green = f256 * atoi(pp);
      pp = strField(data,'|',3); gdkrgba.blue = f256 * atoi(pp);
      gdkrgba.alpha = 1.0;                                                       //  5.8
      widget = gtk_color_button_new_with_rgba(&gdkrgba);
      G_SIGNAL(widget,"color-set",zdialog_widget_event,zd);
   }

   if (strmatch(type,"check")) {                                                 //  checkbox
      if (data) widget = gtk_check_button_new_with_label(data);
      else  widget = gtk_check_button_new();
      G_SIGNAL(widget,"toggled",zdialog_widget_event,zd);
      data = "0";                                                                //  5.3 default data
   }

   if (strmatch(type,"combo")) {                                                 //  combo box
      widget = gtk_combo_box_text_new();
      zd->widget[iiw].cblist = pvlist_create(zdcbmax);                           //  for drop-down list
      if (! blank_null(data)) {
         pvlist_append(zd->widget[iiw].cblist,data);                             //  add data to drop-down list
         gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget),data);
         gtk_combo_box_set_active(GTK_COMBO_BOX(widget),0);
      }
      G_SIGNAL(widget,"changed",zdialog_widget_event,zd);
   }

   if (strmatch(type,"comboE")) {                                                //  combo box with entry box
      widget = gtk_combo_box_text_new_with_entry();
      zd->widget[iiw].cblist = pvlist_create(zdcbmax);                           //  for drop-down list
      if (! blank_null(data)) {
         entry = gtk_bin_get_child(GTK_BIN(widget));
         gtk_entry_set_text(GTK_ENTRY(entry),data);                              //  entry = initial data
         pvlist_append(zd->widget[iiw].cblist,data);                             //  add data to drop-down list
         gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget),data);
      }
      G_SIGNAL(widget,"changed",zdialog_widget_event,zd);
   }

   if (strmatch(type,"radio")) {                                                 //  radio button
      for (kk = iip+1; kk <= iiw; kk++)
         if (strmatch(zd->widget[kk].pname,pname) &&                             //  find first radio button
             strmatch(zd->widget[kk].type,"radio")) break;                       //    with same container
      if (kk == iiw)
         widget = gtk_radio_button_new_with_label(null,data);                    //  this one is first
      else
         widget = gtk_radio_button_new_with_label_from_widget                    //  not first, add to group
              (GTK_RADIO_BUTTON(zd->widget[kk].widget),data);
      G_SIGNAL(widget,"toggled",zdialog_widget_event,zd);
      data = "0";                                                                //  5.3 default data
   }

   if (strmatchV(type,"spin","hscale","vscale",null)) {                          //  spin button or sliding scale
      if (! data) zappcrash("zdialog_add_widget(): data missing");               //  5.8
      pp = strField(data,'|',1); err = convSD(pp,min);
      pp = strField(data,'|',2); err += convSD(pp,max);
      pp = strField(data,'|',3); err += convSD(pp,step);
      pp = strField(data,'|',4); err += convSD(pp,val);
      if (err) { min = 0; max = 100; step = 1; val = 50; }

      if (*type == 's') {
         widget = gtk_spin_button_new_with_range(min,max,step);
         gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),val);
      }
      if (*type == 'h') {
         widget = gtk_scale_new_with_range(HORIZONTAL,min,max,step);
         gtk_range_set_value(GTK_RANGE(widget),val);
         gtk_scale_set_draw_value(GTK_SCALE(widget),0);
      }
      if (*type == 'v') {
         widget = gtk_scale_new_with_range(VERTICAL,min,max,step);
         gtk_range_set_value(GTK_RANGE(widget),val);
         gtk_scale_set_draw_value(GTK_SCALE(widget),0);
      }
      G_SIGNAL(widget,"value-changed",zdialog_widget_event,zd);
      snprintf(vdata,30,"%g",val);
      data = vdata;
   }

   if (strmatch(type,"image")) {                                                 //  image widget                       5.7
      snprintf(iconpath,200,"%s/%s",get_zicondir(),data);
      data = 0;                                                                  //  data not further used
      pixbuf = gdk_pixbuf_new_from_file_at_scale(iconpath,size,size,1,&gerror);
      if (pixbuf) {
         widget = gtk_image_new_from_pixbuf(pixbuf);
         g_object_unref(pixbuf);
      }
      else widget = gtk_image_new_from_icon_name("missing",GTK_ICON_SIZE_BUTTON);
   }

   //  all widget types come here

   zd->widget[iiw].widget = widget;                                              //  set widget in zdialog

   if (strmatchV(type,"entry","edit","comboE",null) > 0)                         //  for data entry widgets, stop
      zd->widget[iiw].stopKB = 1;                                                //    propagation of KB events

   if (strmatch(ptype,"hbox") || strmatch(ptype,"vbox"))                         //  add to hbox/vbox
      gtk_box_pack_start(GTK_BOX(pwidget),widget,expand,expand,space);
   if (strmatch(ptype,"frame"))                                                  //  add to frame
      gtk_container_add(GTK_CONTAINER(pwidget),widget);
   if (strmatch(ptype,"scrwin"))                                                 //  add to scroll window
      gtk_container_add(GTK_CONTAINER(pwidget),widget);
   if (strmatch(ptype,"dialog")) {                                               //  add to dialog box
      vbox = gtk_bin_get_child(GTK_BIN(pwidget));                                //  dialog is a gtkwindow              5.5
      gtk_box_pack_start(GTK_BOX(vbox),widget,expand,expand,space);
   }
   if (data) zd->widget[iiw].data = zstrdup(data);                               //  use heap memory

   return 0;
}


//  add widget to existing zdialog - alternative form (clearer and easier code)
//  options: "size=nn | homog | expand | space=nn | wrap"  (all optional, any order)

int zdialog_add_widget(zdialog *zd, cchar *type, cchar *name,
                       cchar *parent, cchar *data, cchar *options)
{
   int      stat, size = 0, homog = 0, expand = 0, space = 0, wrap = 0, begin = 1;
   char     pname[8];
   double   pval;

   while (true)
   {
      stat = strParms(begin,options,pname,8,pval);
      if (stat == -1) break;
      if (stat == 1) zappcrash("bad zdialog options: %s",options);
      if (strmatch(pname,"size")) size = (int(pval));
      else if (strmatch(pname,"homog")) homog = 1;
      else if (strmatch(pname,"expand")) expand = 1;
      else if (strmatch(pname,"space")) space = (int(pval));
      else if (strmatch(pname,"wrap")) wrap = 1;
      else zappcrash("bad zdialog options: %s",options);
   }

   stat = zdialog_add_widget(zd,type,name,parent,data,size,homog,expand,space,wrap);
   return stat;
}


//  return 1/0 if zdialog is valid/invalid

int zdialog_valid(zdialog *zd, int errmess)                                      //  6.0
{
   using namespace zfuncs;

   int      ok, ii;

   if (! zd) return 0;

   for (ii = 0; ii < zdialog_count; ii++)                                        //  find in valid zdialog list      5.9
      if (zd == zdialog_list[ii]) break;
   if (ii == zdialog_count) {
      if (errmess) printz("*** zdialog invalid %p \n",__builtin_return_address(0));
      return 0;
   }

   ok = 1;
   if ((zd->sentinel1 & 0xFFFF0000) != zdsentinel) ok = 0;
   if (zd->sentinel2 != zd->sentinel1) ok = 0;
   if (! ok) {
      printz("*** zdialog sentinel invalid %p \n",zd);
      return 0;
   }

   return 1;
}


//  get GTK widget from zdialog and widget name

GtkWidget * zdialog_widget(zdialog *zd, cchar *name)
{
   if (! zdialog_valid(zd)) return 0;
   for (int ii = 0; zd->widget[ii].type; ii++)
      if (strmatch(zd->widget[ii].name,name))
         return zd->widget[ii].widget;
   return 0;
}


//  add a popup tool tip to a zdialog widget

int zdialog_add_ttip(zdialog *zd, cchar *wname, cchar *ttip)                     //  5.9
{
   GtkWidget   *widget;
   int         ii;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (ii = 0; zd->compwidget[ii]; ii++)                                        //  search completion buttons
      if (strmatch(zd->compbutton[ii],wname)) {                                  //    for matching name
         gtk_widget_set_tooltip_text(zd->compwidget[ii],ttip);
         return 1;
      }

   widget = zdialog_widget(zd,wname);                                            //  search zdialog widgets
   if (! widget) {
      printz(" *** zdialog widget invalid %s \n",wname);
      return 0;
   }

   gtk_widget_set_tooltip_text(widget,ttip);
   return 1;
}


//  set a common group for a set of radio buttons
//  (GTK, this does not work)

int zdialog_set_group(zdialog *zd, cchar *radio1, ...)
{
   va_list        arglist;
   cchar          *radio2;
   GtkWidget      *gwidget, *widget;
   GSList         *glist;

   zthreadcrash();

   gwidget = zdialog_widget(zd,radio1);
   glist = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gwidget));
   if (! glist) zappcrash("no radio button group");

   va_start(arglist,radio1);

   while (true)
   {
      radio2 = va_arg(arglist,cchar *);
      if (! radio2) break;
      widget = zdialog_widget(zd,radio2);
      gtk_radio_button_set_group(GTK_RADIO_BUTTON(widget),glist);
   }

   va_end(arglist);

   return 0;
}


//  resize dialog to a size greater than initial size
//  (as determined by the included widgets)

int zdialog_resize(zdialog *zd, int width, int height)
{
   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   if (! width) width = 10;                                                      //  stop spurious GTK warnings         6.1
   if (! height) height = 10;
   GtkWidget *window = zd->widget[0].widget;
   gtk_window_set_default_size(GTK_WINDOW(window),width,height);
   return 1;
}


//  put data into a zdialog widget
//  private function

int zdialog_put_data(zdialog *zd, cchar *name, cchar *data)
{
   GtkWidget      *widget, *entry;
   GtkTextBuffer  *textBuff;
   GdkRGBA        gdkrgba;
   int            iiw, nn, kk;
   cchar          *type, *pp;
   char           *wdata;
   double         dval;
   double         f256 = 1.0 / 256.0;
   float          lval, hval, nval, F, F2;

   zthreadcrash();                                                               //  thread usage not allowed

   if (! zdialog_valid(zd)) return 0;

   if (! name || ! *name) zappcrash("zdialog_put_data(), name null");

   for (iiw = 1; zd->widget[iiw].type; iiw++)                                    //  find widget
      if (strmatch(zd->widget[iiw].name,name)) break;
   if (! zd->widget[iiw].type) {
      printz("*** zdialog_put_data(), bad name %s \n",name);
      return 0;
   }

   type = zd->widget[iiw].type;
   widget = zd->widget[iiw].widget;

   wdata = zd->widget[iiw].data;
   if (wdata) zfree(wdata);                                                      //  free prior data memory
   zd->widget[iiw].data = 0;

   if (data) {
      wdata = zstrdup(data);                                                     //  set new data for widget
      zd->widget[iiw].data = wdata;
      if (utf8_check(wdata))
         printz("*** zdialog: bad UTF8 encoding %s \n",wdata);
   }

   zd->disabled++;                                                               //  disable for widget stuffing

   if (strmatch(type,"label"))
      gtk_label_set_text(GTK_LABEL(widget),data);

   if (strmatch(type,"link"))
      gtk_label_set_text(GTK_LABEL(widget),data);

   if (strmatch(type,"entry"))
      gtk_entry_set_text(GTK_ENTRY(widget),data);

   if (strmatch(type,"button"))                                                  //  change button label
      gtk_button_set_label(GTK_BUTTON(widget),data);

   if (strmatch(type,"edit")) {                                                  //  text input to editable text
      textBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
      gtk_text_buffer_set_text(textBuff,data,-1);                                //  generates GTK error messages ****
   }

   if (strmatch(type,"text")) {                                                  //  text output         5.5
      textBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
      gtk_text_buffer_set_text(textBuff,data,-1);
   }

   if (strmatchV(type,"togbutt","check","radio",null))
   {
      if (! data) kk = nn = 0;
      else kk = convSI(data,nn);
      if (kk != 0) nn = 0;                                                       //  data not integer, force zero
      if (nn <= 0) nn = 0; else nn = 1;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),nn);                //  set gtk widget value
   }

   if (strmatch(type,"spin")) {
      kk = convSD(data,dval);
      if (kk != 0) dval = 0.0;
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),dval);
   }

   if (strmatch(type,"colorbutt")) {                                             //  color button data is nnn|nnn|nnn
      pp = strField(data,'|',1);
      if (pp) gdkrgba.red = f256 * atoi(pp);                                     //  RGB range is 0-1                   5.8
      pp = strField(data,'|',2);
      if (pp) gdkrgba.green = f256 * atoi(pp);
      pp = strField(data,'|',3);
      if (pp) gdkrgba.blue = f256 * atoi(pp);
      gdkrgba.alpha = 1.0;
      gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(widget),&gdkrgba);            //  5.8
   }

   if (strmatchV(type,"hscale","vscale",null))                                   //  slider widget
   {
      kk = convSD(data,dval);                                                    //  zdialog widget value
      if (kk != 0) dval = 0.0;

      if (zd->widget[iiw].rescale)                                               //  widget value --> slider value      6.2
      {
         lval = zd->widget[iiw].lval;                                            //  rescaled for more sensitivity
         nval = zd->widget[iiw].nval;                                            //    around neutral value
         hval = zd->widget[iiw].hval;

         if (dval >= lval && dval <= nval) {
            F2 = (nval - dval) / (nval - lval);
            F = sqrtf(F2);
            dval = nval - F * (nval - lval);
         }
         
         else if (dval >= nval && dval <= hval) {
            F2 = (dval - nval) / (hval - nval);
            F = sqrtf(F2);
            dval = nval + F * (hval - nval);
         }
      }

      gtk_range_set_value(GTK_RANGE(widget),dval);
   }

   if (strmatch(type,"combo")) {
      if (! blank_null(data)) {
         kk = pvlist_prepend(zd->widget[iiw].cblist,data,1);                     //  add to drop-down list
         if (kk == 0)                                                            //  (only if unique)
            gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(widget),data);
         kk = pvlist_find(zd->widget[iiw].cblist,data);
         gtk_combo_box_set_active(GTK_COMBO_BOX(widget),kk);                     //  make the active entry
      }
      else gtk_combo_box_set_active(GTK_COMBO_BOX(widget),-1);                   //  make no active entry
   }

   if (strmatch(type,"comboE")) {
      entry = gtk_bin_get_child(GTK_BIN(widget));
      if (! blank_null(data)) {
         kk = pvlist_prepend(zd->widget[iiw].cblist,data,1);                     //  add to drop-down list
         if (kk == 0)                                                            //  (only if unique)
            gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(widget),data);
         gtk_entry_set_text(GTK_ENTRY(entry),data);                              //  stuff entry box with new data
      }
      else gtk_entry_set_text(GTK_ENTRY(entry),"");                              //  stuff entry box with nothing
   }

   zd->disabled--;                                                               //  re-enable dialog
   return iiw;
}


//  get data from a dialog widget based on its name
//  private function

cchar * zdialog_get_data(zdialog *zd, cchar *name)
{
   if (! zdialog_valid(zd)) return 0;

   for (int ii = 1; zd->widget[ii].type; ii++)
      if (strmatch(zd->widget[ii].name,name))
            return zd->widget[ii].data;
   return 0;
}


//  set new limits for a numeric data entry widget (spin, hscale, vscale)

int zdialog_set_limits(zdialog *zd, cchar *name, double min, double max)
{
   GtkWidget   *widget;
   cchar       *type;
   int         iiw;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (iiw = 1; zd->widget[iiw].type; iiw++)
      if (strmatch(name,zd->widget[iiw].name)) break;
   if (! zd->widget[iiw].type) {
      printz("*** zdialog_set_limits, %s not found \n",name);
      return 0;
   }

   widget = zd->widget[iiw].widget;
   type = zd->widget[iiw].type;

   if (*type == 's')
      gtk_spin_button_set_range(GTK_SPIN_BUTTON(widget),min,max);

   if (*type == 'h' || *type == 'v')
      gtk_range_set_range(GTK_RANGE(widget),min,max);

   return 1;
}


//  Expand a widget scale in the region around the neutral value.                //  6.0
//  Control small adjustments near the neutral value more precisely.
//  lval and hval: the range of values to be rescaled.
//  nval: the neutral value where the scale will be expanded the most.
//        lval <= nval <= hval

int zdialog_rescale(zdialog *zd, cchar *name, float lval, float nval, float hval)
{
   int         iiw;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (iiw = 1; zd->widget[iiw].type; iiw++)
      if (strmatch(name,zd->widget[iiw].name)) break;
   if (! zd->widget[iiw].type) {
      printz("*** zdialog_rescale, %s not found \n",name);
      return 0;
   }

   zd->widget[iiw].rescale = 1;
   zd->widget[iiw].lval = lval;
   zd->widget[iiw].nval = nval;
   zd->widget[iiw].hval = hval;

   return 1;
}


//  run the dialog and send events to the event function
//
//  evfunc: int func(zdialog *zd, cchar *event)
//  If present, eventFunc is called when a dialog widget is changed or the dialog
//  is completed. If a widget was changed, event is the widget name.
//  Get the new widget data with zdialog_fetch().
//  If a completion button was pressed, event is "zstat" and zd->zstat will be
//  the button number 1-N.
//  If the dialog was destroyed, event is "zstat" and zd->zstat is negative.
//
//  posn: optional dialog box position:
//    "mouse" = position at mouse
//    "desktop" = center on desktop
//    "parent" = center on parent window
//    "nn/nn" = position NW corner at relative x/y position in parent window,
//     where nn/nn is a percent 0-100 of the parent window x/y dimensions.
//    "save" = save last user-set position and use this whenever the dialog is repeated,
//             also across sessions.
//
//  KBstate: extern void KBstate(GdkEventKey *event, int state)
//  This function must be supplied by the caller of zdialog.
//  It is called when Ctrl|Shift|Alt|F1 is pressed or released.

int zdialog_run(zdialog *zd, zdialog_event evfunc, cchar *posn)
{
   int   zdialog_KBpress(GtkWidget *, GdkEventKey *event, zdialog *zd);
   int   zdialog_KBrelease(GtkWidget *, GdkEventKey *event, zdialog *zd);
   int   zdialog_focus_event_signal(GtkWidget *, GdkEvent *event, zdialog *zd);

   int         ii;
   GtkWidget   *widget, *dialog, *entry;

   zthreadcrash();

   if (! zdialog_valid(zd)) zappcrash("zdialog invalid");

   dialog = zd->widget[0].widget;

   if (posn) zdialog_set_position(zd,posn);                                      //  put dialog at remembered position

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  *** stop auto-selection
   {                                                                             //  (GTK "feature")
      if (strmatch(zd->widget[ii].type,"entry")) {
         widget = zd->widget[ii].widget;
         if (gtk_editable_get_editable(GTK_EDITABLE(widget)))
            gtk_editable_set_position(GTK_EDITABLE(widget),-1);
         break;
      }

      if (strmatch(zd->widget[ii].type,"comboE")) {                              //  also combo edit box
         widget = zd->widget[ii].widget;
         entry = gtk_bin_get_child(GTK_BIN(widget));
         gtk_editable_set_position(GTK_EDITABLE(entry),-1);
         break;
      }
   }

   if (evfunc) zd->eventCB = (void *) evfunc;                                    //  link to dialog event callback

   dialog = zd->widget[0].widget;
   gtk_widget_show_all(dialog);                                                  //  activate dialog
   gtk_window_present(GTK_WINDOW(dialog));
   if (! zd->parent)                                                             //  if no parent, force to top
      gtk_window_set_keep_above(GTK_WINDOW(dialog),1);

   G_SIGNAL(dialog,"focus-in-event",zdialog_focus_event_signal,zd);              //  connect focus event function       5.0
   G_SIGNAL(dialog,"key-press-event",zdialog_KBpress,zd);                        //  connect key press event function
   G_SIGNAL(dialog,"key-release-event",zdialog_KBrelease,zd);                    //  connect key release event function
   G_SIGNAL(dialog,"delete-event",zdialog_delete_event,zd);                      //  connect delete event function      5.5

   zd->zstat = 0;                                                                //  dialog not complete                5.7
   zd->disabled = 0;                                                             //  enable widget events

   zfuncs::zdialog_busy++;                                                       //  count open zdialogs
   return 0;                                                                     //  return now, dialog is non-modal
}


//  zdialog event handler - called for dialog events.
//  Updates data in zdialog, calls user callback function (if present).
//  private function

int zdialog_widget_event(GtkWidget *widget, zdialog *zd)
{
   zdialog_event  *evfunc = 0;                                                   //  dialog event callback function

   GtkTextView       *textView = 0;
   GtkTextBuffer     *textBuff = 0;
   GtkTextIter       iter1, iter2;
   GdkRGBA           gdkrgba;
   GtkWidget         *entry;
   static GtkWidget  *lastwidget = 0;
   int               ii, nn;
   cchar             *wname, *type, *wdata;
   static char       event[40];                                                  //  preserve from dialog destroy    5.9
   char              sdata[20];
   double            dval;
   float             lval, nval, hval, F;
   static int        cbadded = 0;

   if (! zdialog_valid(zd)) return 0;

   if (zd->disabled) return 1;                                                   //  re-entry from zdialog_put_data()
   zd->disabled = 1;                                                             //    or user event function        5.8

   for (ii = 0; ii < zdmaxbutts; ii++) {                                         //  check completion buttons
      if (zd->compwidget[ii] == null) break;                                     //  EOL                             5.8
      if (zd->compwidget[ii] != widget) continue;
      zd->zstat = ii+1;                                                          //  zdialog status = button no.
      strncpy0(event,"zstat",40);
      goto call_evfunc;                                                          //  call zdialog event function     5.7
   }

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget in zdialog
      if (zd->widget[ii].widget == widget) goto found_widget;

   for (ii = 1; zd->widget[ii].type; ii++) {                                     //  failed, test if buffer
      if (strmatch(zd->widget[ii].type,"edit")) {                                //    of text view widget
         textView = GTK_TEXT_VIEW(zd->widget[ii].widget);
         textBuff = gtk_text_view_get_buffer(textView);
         if (widget == (GtkWidget *) textBuff) goto found_widget;
      }
   }

   printz("*** zdialog event ignored \n");                                       //  not found, ignore event
   zd->disabled = 0;
   return 1;

found_widget:

   wname = zd->widget[ii].name;
   type = zd->widget[ii].type;
   wdata = 0;

   if (strmatch(type,"button")) wdata = "clicked";

   if (strmatch(type,"entry"))
         wdata = gtk_entry_get_text(GTK_ENTRY(widget));

   if (strmatch(type,"edit")) {
      gtk_text_buffer_get_bounds(textBuff,&iter1,&iter2);
      wdata = gtk_text_buffer_get_text(textBuff,&iter1,&iter2,0);
   }

   if (strmatchV(type,"radio","check","togbutt",null))
   {
      nn = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
      if (nn == 0) wdata = "0";
      else wdata = "1";
   }

   if (strmatch(type,"combo"))
         wdata = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

   if (strmatch(type,"comboE"))
   {
      if (widget == lastwidget && cbadded) {
         pvlist_remove(zd->widget[ii].cblist,0);                                 //  detect multiple edits (keystrokes)
         gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(widget),0);                //    amd replace with new entry
      }
      entry = gtk_bin_get_child(GTK_BIN(widget));
      wdata = gtk_entry_get_text(GTK_ENTRY(entry));
      cbadded = 0;
      if (! blank_null(wdata)) {
         nn = pvlist_prepend(zd->widget[ii].cblist,wdata,1);                     //  add entry to drop-down list
         if (nn == 0) {                                                          //  (only if unique)
            gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(widget),wdata);
            cbadded = 1;
         }
      }
   }

   if (strmatch(type,"spin"))
   {
      dval = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
      snprintf(sdata,20,"%g",dval);
      wdata = sdata;
   }

   if (strmatch(type,"colorbutt"))                                               //  color button                    5.8
   {
      gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(widget),&gdkrgba);
      snprintf(sdata,20,"%.0f|%.0f|%.0f",gdkrgba.red*255,gdkrgba.green*255,gdkrgba.blue*255);
      wdata = sdata;
   }

   if (strmatchV(type,"hscale","vscale",null))
   {
      dval = gtk_range_get_value(GTK_RANGE(widget));

      if (zd->widget[ii].rescale)                                                //  slider value --> widget value      6.0
      {
         lval = zd->widget[ii].lval;
         nval = zd->widget[ii].nval;
         hval = zd->widget[ii].hval;

         if (dval >= lval && dval <= nval) {                                     //  lval ... nval
            F = (nval - dval) / (nval - lval);                                   //  1 ... 0
            dval = (1.0 - F * F) * (nval - lval) + lval;                         //  lval ... nval
         }

         else if (dval >= nval && dval <= hval) {                                //  nval ... hval
            F = (dval - nval) / (hval - nval);                                   //  0 ... 1
            dval = F * F * (hval - nval) + nval;                                 //  nval ... hval
         }
      }

      snprintf(sdata,20,"%g",dval);
      wdata = sdata;
   }

   //  all widgets come here

   if (zd->widget[ii].data) zfree(zd->widget[ii].data);                          //  clear prior data
   zd->widget[ii].data = 0;
   if (wdata) zd->widget[ii].data = zstrdup(wdata);                              //  set new data

   lastwidget = widget;                                                          //  remember last widget updated
   strncpy0(event,wname,40);                                                     //  event = widget name

call_evfunc:                                                                     //  call zdialog event function     5.7

   if (zd->eventCB) {
      evfunc = (zdialog_event *) zd->eventCB;                                    //  do callback function
      evfunc(zd,event);
   }

   zd->stopKB = zd->widget[ii].stopKB;                                           //  stop KB propagation if req.     5.1
   zd->disabled = 0;                                                             //  re-enable widgets
   return 1;
}


//  zdialog response handler for "focus-in-event" signal                         //  5.0
//  private function

zdialog  *zdialog_focus_zd;                                                      //  5.8  current zdialog

int zdialog_focus_event_signal(GtkWidget *, GdkEvent *event, zdialog *zd)
{
   if (! zdialog_valid(zd)) return 0;
   if (zd->zstat) return 0;                                                      //  already complete
   zdialog_focus_zd = zd;
   zdialog_send_event(zd,"focus");                                               //  notify dialog event function
   return 0;                                                                     //  must be 0
}


//  zdialog response handler for keyboard events
//  key symbols can be found at /usr/include/gtk-3.0/gdk/gdkkeysyms.h
//  main app must provide: extern void KBstate(GdkEventKey *event, int state)
//  private function

int      KB_Ctrl = 0;                                                            //  track state of KB Ctrl
int      KB_Shift = 0;                                                           //    and Shift and Alt keys
int      KB_Alt = 0;

int zdialog_KBpress(GtkWidget *, GdkEventKey *kbevent, zdialog *zd)
{
   KB_Ctrl = KB_Shift = KB_Alt = 0;
   if (kbevent->state & GDK_CONTROL_MASK) KB_Ctrl = 1;                           //  5.6
   if (kbevent->state & GDK_SHIFT_MASK) KB_Shift = 1;
   if (kbevent->state & GDK_MOD1_MASK) KB_Alt = 1;

   if (KB_Ctrl || KB_Alt) {                                                      //  if Ctrl or Alt pressed,         5.6
      KBstate(kbevent,1);                                                        //    send next key to main app
      return 1;
   }

   return 0;                                                                     //  send to dialog widget
}


int zdialog_KBrelease(GtkWidget *widget, GdkEventKey *kbevent, zdialog *zd)
{
   void  zdialog_copyfunc(GtkWidget *, GtkClipboard *);
   void  zdialog_pastefunc(GtkClipboard *, cchar *, void *);

   GtkClipboard   *clipboard = 0;
   int      KBkey = kbevent->keyval;

   KB_Ctrl = KB_Shift = KB_Alt = 0;
   if (kbevent->state & GDK_CONTROL_MASK) KB_Ctrl = 1;                           //  5.6
   if (kbevent->state & GDK_SHIFT_MASK) KB_Shift = 1;
   if (kbevent->state & GDK_MOD1_MASK) KB_Alt = 1;

   if (KB_Ctrl && KBkey == GDK_KEY_c)                                            //  Ctrl+C = copy to clipboard      6.0
   {
      clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      zdialog_copyfunc(widget,clipboard);
      return 1;
   }

   if (KB_Ctrl && KBkey == GDK_KEY_v)                                            //  Ctrl+V = paste clipboard        5.8
   {
      clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_clipboard_request_text(clipboard,zdialog_pastefunc,widget);
      return 1;
   }

   if (KB_Ctrl || KB_Alt) {                                                      //  if Ctrl or Alt pressed,
      KBstate(kbevent,0);                                                        //    send next key to main app     5.6
      return 1;
   }

   if (KBkey == GDK_KEY_F1) {                                                    //  send F1 (help) to main app      5.6
      KBstate(kbevent,0);
      return 1;
   }

   if (KBkey == GDK_KEY_F11) {                                                   //  F11 (toggle full screen) to main app
      KBstate(kbevent,0);                                                        //  5.6
      return 1;
   }

   if (KBkey == GDK_KEY_Escape) {                                                //  ESC key                         6.1
      zdialog_send_event(zd,"escape");
      return 1;
   }

   if (KBkey == GDK_KEY_Return) {                                                //  Return (Enter) key              5.8
      zdialog_send_event(zd,"enter");
      return 1;
   }

   if (zd->stopKB) return 1;                                                     //  ignore KB events                5.1

   return 0;                                                                     //  send KB input to dialog widget
}


//  process Ctrl+C (copy text from widget to clipboard)                          //  6.0
//  private function

void zdialog_copyfunc(GtkWidget *widget, GtkClipboard *clipboard)
{
   GtkTextView    *textView = 0;
   GtkTextBuffer  *textBuff = 0;
   zdialog        *zd;
   int            ii, cc = 0;
   cchar          *wname;
   char           text[1000];

   widget = gtk_window_get_focus(GTK_WINDOW(widget));
   if (! widget) return;

   zd = zdialog_focus_zd;
   if (! zdialog_valid(zd)) return;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget in zdialog
      if (zd->widget[ii].widget == widget) goto found_widget;
   for (ii = 1; zd->widget[ii].type; ii++) {                                     //  failed, test if buffer
      if (strmatch(zd->widget[ii].type,"edit")) {                                //    of text view widget
         textView = GTK_TEXT_VIEW(zd->widget[ii].widget);
         textBuff = gtk_text_view_get_buffer(textView);
         if (widget == (GtkWidget *) textBuff) goto found_widget;
      }
   }
   return;                                                                       //  not found

found_widget:
   wname = zd->widget[ii].name;
   zdialog_fetch(zd,wname,text,999);                                             //  current text in widget
   cc = strlen(text);
   gtk_clipboard_set_text(clipboard,text,cc);
   return;
}


//  process Ctrl+V (paste text from clipboard to widget)                         //  5.8
//  private function

void zdialog_pastefunc(GtkClipboard *clipboard, cchar *cliptext, void *arg)
{
   GtkWindow      *window;
   GtkWidget      *widget;
   GtkTextView    *textView = 0;
   GtkTextBuffer  *textBuff = 0;
   zdialog        *zd;
   int            ii, cc = 0;
   cchar          *wname;
   char           text[1000];

   window = (GtkWindow *) arg;
   widget = gtk_window_get_focus(window);
   if (! widget) return;                                                         //  widget for pasted text
   if (! cliptext || ! *cliptext) return;                                        //  clipboard text pasted

   zd = zdialog_focus_zd;
   if (! zdialog_valid(zd)) return;

   if (zd->zstat) return;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget in zdialog
      if (zd->widget[ii].widget == widget) goto found_widget;
   for (ii = 1; zd->widget[ii].type; ii++) {                                     //  failed, test if buffer
      if (strmatch(zd->widget[ii].type,"edit")) {                                //    of text view widget
         textView = GTK_TEXT_VIEW(zd->widget[ii].widget);
         textBuff = gtk_text_view_get_buffer(textView);
         if (widget == (GtkWidget *) textBuff) goto found_widget;
      }
   }
   return;                                                                       //  not found

found_widget:
   wname = zd->widget[ii].name;
   zdialog_fetch(zd,wname,text,999);                                             //  current text in widget
   cc = strlen(text);
   if (cc > 995) return;
   strncpy(text+cc,cliptext,999-cc);                                             //  add clipboard text
   text[999] = 0;
   zdialog_stuff(zd,wname,text);
   return;
}


//  private function called when zdialog is completed.
//  called when dialog is canceled via [x] button or destroyed by GTK (zstat < 0).

int zdialog_delete_event(GtkWidget *, GdkEvent *, zdialog *zd)
{
   zdialog_event  *evfunc = 0;                                                   //  dialog event callback function

   if (! zdialog_valid(zd)) return 1;
   if (zd->zstat) return 1;                                                      //  already complete
   if (zd->disabled) return 1;                                                   //  in process                      5.9

   zd->zstat = -1;                                                               //  set zdialog cancel status

   if (zd->eventCB) {
      evfunc = (zdialog_event *) zd->eventCB;                                    //  do callback function
      zd->disabled = 1;                                                          //                                  6.1
      evfunc(zd,"zstat");
      zd->disabled = 0;
   }

   else zdialog_destroy(zd);                                                     //  caller should zdialog_free()
   return 0;
}


//  Send the event "name" to an active zdialog.
//  The response function eventFunc() will be called with this event.

int zdialog_send_event(zdialog *zd, cchar *event)
{
   zdialog_event * evfunc = 0;                                                   //  dialog event callback function

   if (! zdialog_valid(zd)) return 0;
   if (zd->disabled) return 0;                                                   //  busy                            6.1
   evfunc = (zdialog_event *) zd->eventCB;
   if (! evfunc) return 0;
   zd->disabled = 1;                                                             //                                  6.1
   evfunc(zd,event);                                                             //  call dialog event function
   zd->disabled = 0;
   return 1;
}


//  Complete an active dialog and assign a status.
//  Equivalent to the user pressing a dialog completion button.
//  The dialog completion function is called if defined,
//  and zdialog_wait() is unblocked.
//  returns:  0 = no active dialog or completion function, 1 = OK

int zdialog_send_response(zdialog *zd, int zstat)
{
   zdialog_event  *evfunc = 0;                                                   //  dialog event callback function

   if (! zdialog_valid(zd)) return 0;
   if (zd->disabled) return 0;                                                   //                                  6.1
   zd->zstat = zstat;                                                            //  set status
   evfunc = (zdialog_event *) zd->eventCB;
   if (! evfunc) return 0;
   zd->disabled = 1;                                                             //                                  6.1
   evfunc(zd,"zstat");
   zd->disabled = 0;
   return 1;
}


//  show or hide a zdialog window
//  returns 1 if successful, 0 if zd does not exist.

int zdialog_show(zdialog *zd, int show)
{
   static GtkWidget  *widget, *pwidget = 0;
   static int        posx, posy;

   zthreadcrash();

   widget = zdialog_widget(zd,"dialog");
   if (! widget) return 0;
   if (show) {                                                                   //  show window
      if (widget == pwidget) {                                                   //  restore prior position       5.4
         gtk_window_move(GTK_WINDOW(widget),posx,posy);
         pwidget = 0;
      }
      gtk_widget_show_all(widget);                                               //  GTK bug?? window can no longer  ////
   }                                                                             //            receive focus events
   else {                                                                        //  hide window
      pwidget = widget;
      gtk_window_get_position(GTK_WINDOW(widget),&posx,&posy);                   //  save position                5.4
      gtk_widget_hide(widget);
   }
   return 1;
}


//  Destroy the zdialog - must be done by zdialog_run() caller
//  (else dialog continues active even after completion button).
//  Data in widgets remains valid until zdialog_free() is called.

int zdialog_destroy(zdialog *zd)
{
   using namespace zfuncs;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   if (zd->zstat < 0) {                                                          //  destroyed by [x] button or GTK
      if (zd->widget[0].widget) zdialog_busy--;                                  //  bugfix                             5.8
      zd->widget[0].widget = 0;                                                  //  assume GTK dialog is gone
   }

   if (zd->saveposn) zdialog_save_position(zd);                                  //  save position for next use   moved 6.0

   if (zd->widget[0].widget) {                                                   //  multiple destroys OK
      gtk_widget_destroy(zd->widget[0].widget);                                  //  destroy GTK dialog
      zd->widget[0].widget = 0;
      zdialog_busy--;
   }

   if (! zd->zstat) zd->zstat = -1;                                              //  status = destroyed

// if (zd->parent)
//    gtk_window_present(GTK_WINDOW(zd->parent));                                //  focus on parent window     removed 6.2

   return 1;
}


//  free zdialog memory (will destroy first, if not already)
//  zd is set to null

int zdialog_free(zdialog *&zd)                                                   //  reference
{
   using namespace zfuncs;

   int      ii;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;                                            //  validate zd pointer

   zdialog_save_inputs(zd);                                                      //  save user inputs for next use   5.3

   zdialog_destroy(zd);                                                          //  destroy GTK dialog if there
   zd->sentinel1 = zd->sentinel2 = 0;                                            //  mark sentinels invalid          5.8
   zfree(zd->widget[0].data);                                                    //  bugfix memory leak

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  loop through widgets
   {
      if (strmatchV(zd->widget[ii].type,"combo","comboE",null))                  //  free combo list
         pvlist_free(zd->widget[ii].cblist);
      zfree((char *) zd->widget[ii].type);                                       //  free strings
      zfree((char *) zd->widget[ii].name);
      zfree((char *) zd->widget[ii].pname);
      if (zd->widget[ii].data) zfree(zd->widget[ii].data);                       //  free data
   }

   for (ii = 0; ii < zdialog_count; ii++)                                        //  remove from valid zdialog list  5.9
      if (zd == zdialog_list[ii]) break;
   zdialog_count--;
   for ( ; ii < zdialog_count; ii++)                                             //  pack down list
      zdialog_list[ii] = zdialog_list[ii+1];

   zfree(zd);                                                                    //  free zdialog memory
   zd = 0;                                                                       //  caller pointer = null
   return 1;
}


//  Wait for a dialog to complete or be destroyed. This is a zmainloop() loop.
//  The returned status is the button 1-N used to complete the dialog, or negative
//  if the dialog was destroyed with [x] or otherwise by GTK. If the status was 1-N and
//  the dialog will be kept active, set zd->zstat = 0 to restore the active state.

int zdialog_wait(zdialog *zd)
{
   while (true)
   {
      zmainloop();
      if (! zd) return -1;                                                       //  6.0
      if (! zdialog_valid(zd,0)) return -1;                                      //  no error message
      if (zd->zstat) return zd->zstat;
      zsleep(0.01);
   }
}


//  put cursor at named widget

int zdialog_goto(zdialog *zd, cchar *name)
{
   GtkWidget   *widget;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   widget = zdialog_widget(zd, name);
   if (! widget) return 0;
   gtk_widget_grab_focus(widget);

   return 1;
}


//  set cursor for zdialog (e.g. a busy cursor)

void zdialog_set_cursor(zdialog *zd, GdkCursor *cursor)
{
   GtkWidget   *dialog;
   GdkWindow   *window;

   zthreadcrash();

   if (! zdialog_valid(zd)) return;
   dialog = zd->widget[0].widget;
   if (! dialog) return;
   window = gtk_widget_get_window(dialog);
   gdk_window_set_cursor(window,cursor);
   return;
}


//  insert data into a zdialog widget

int zdialog_stuff(zdialog *zd, cchar *name, cchar *data)                         //  stuff a string
{
   if (data) zdialog_put_data(zd, name, data);
   else zdialog_put_data(zd, name, "");                                          //  null > ""     5.2
   return 1;
}

int zdialog_stuff(zdialog *zd, cchar *name, int idata)                           //  stuff an integer
{
   char  string[16];

   snprintf(string,16,"%d",idata);
   zdialog_put_data(zd,name,string);
   return 1;
}

int zdialog_stuff(zdialog *zd, cchar *name, double ddata)                        //  stuff a double
{
   char  string[32];

   snprintf(string,32,"%g",ddata);                                               //  outputs decimal point or comma
   zdialog_put_data(zd,name,string);                                             //  (per locale)
   return 1;
}

int zdialog_stuff(zdialog *zd, cchar *name, double ddata, cchar *format)         //  stuff a double, formatted          6.1
{
   char  string[32];

   snprintf(string,32,format,ddata);                                             //  use ".2g" etc. for
   zdialog_put_data(zd,name,string);                                             //    locale dependent point/comma
   return 1;
}

int zdialog_labelfont(zdialog *zd, cchar *labl, cchar *font, cchar *txt)         //  set label text with font           6.2
{
   GtkWidget   *widget;
   cchar       *format =  "<span font=\"%s\" >%s</span>";
   char        txt2[1000];

   snprintf(txt2,1000,format,font,txt);
   widget = zdialog_widget(zd,labl);
   gtk_label_set_markup(GTK_LABEL(widget),txt2);
   return 1;
}


//  get data from a zdialog widget

int zdialog_fetch(zdialog *zd, cchar *name, char *data, int maxcc)               //  fetch string data
{
   cchar  *zdata;

   zdata = zdialog_get_data(zd,name);
   if (! zdata) {
      *data = 0;
      return 0;
   }

   return strncpy0(data,zdata,maxcc);                                            //  0 = OK, 1 = truncation
}

int zdialog_fetch(zdialog *zd, cchar *name, int &idata)                          //  fetch an integer
{
   cchar  *zdata;

   zdata = zdialog_get_data(zd,name);
   if (! zdata) {
      idata = 0;
      return 0;
   }

   idata = atoi(zdata);
   return 1;
}

int zdialog_fetch(zdialog *zd, cchar *name, double &ddata)                       //  fetch a double
{
   int         stat;
   cchar       *zdata;

   zdata = zdialog_get_data(zd,name);
   if (! zdata) {
      ddata = 0;
      return 0;
   }

   stat = convSD(zdata,ddata);                                                   //  period or comma decimal point OK
   if (stat < 4) return 1;
   return 0;
}

int zdialog_fetch(zdialog *zd, cchar *name, float &fdata)                        //  fetch a float
{
   int         stat;
   cchar       *zdata;
   double      ddata;

   zdata = zdialog_get_data(zd,name);
   if (! zdata) {
      fdata = 0;
      return 0;
   }

   stat = convSD(zdata,ddata);                                                   //  period or comma decimal point OK
   fdata = ddata;
   if (stat < 4) return 1;
   return 0;
}


/**************************************************************************

   Widget type Combo Box with Entry
   --------------------------------
   stat = zdialog_cb_app(zd, name, data)      append to combo box list
   stat = zdialog_cb_prep(zd, name, data)     prepend to combo box list
   data = zdialog_cb_get(zd, name, Nth)       get combo box list item
   data = zdialog_cb_delete(zd, name, data)   delete entry matching "data"
   data = zdialog_cb_clear(zd, name)          clear all entries
   zdialog_cb_popup(zd, name)                 open the combo box entries

   These functions map and track a combo box drop-down list, by maintaining a
   parallel list in memory. The function zdialog-stuff, when called for a comboE
   widget, automatically prepends the stuffed data to the drop-down list.
   The private function zdialog_event(), when processing user input to the edit
   box of a comboE widget, adds the updated entry to the drop-down list.
   The drop-down list is kept free of redundant and blank entries.

***************************************************************************/


//  append new item to combo box list without changing entry box

int zdialog_cb_app(zdialog *zd, cchar *name, cchar *data)
{
   int      ii, nn;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   if (blank_null(data)) return 0;                                               //  find widget
   for (ii = 1; zd->widget[ii].type; ii++)
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box

   nn = pvlist_append(zd->widget[ii].cblist,data,1);                             //  append unique
   if (nn >= 0)
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(zd->widget[ii].widget),data);

   return 1;
}


//  prepend new item to combo box list without changing entry box

int zdialog_cb_prep(zdialog *zd, cchar *name, cchar *data)
{
   int      ii, nn;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   if (blank_null(data)) return 0;                                               //  find widget
   for (ii = 1; zd->widget[ii].type; ii++)
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box

   nn = pvlist_prepend(zd->widget[ii].cblist,data,1);                            //  prepend unique
   if (nn == 0)
      gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(zd->widget[ii].widget),data);

   return 1;
}


//  get combo box drop-down list entry
//  Nth = 0 = first list entry (not comboE edit box)

char * zdialog_cb_get(zdialog *zd, cchar *name, int Nth)
{
   int      ii;

   if (! zdialog_valid(zd)) return 0;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box
   return pvlist_get(zd->widget[ii].cblist,Nth);
}


//  delete entry by name from combo drop down list

int zdialog_cb_delete(zdialog *zd, cchar *name, cchar *data)
{
   int      ii, nn;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box

   nn = pvlist_find(zd->widget[ii].cblist,data);                                 //  find entry by name
   if (nn < 0) return -1;

   pvlist_remove(zd->widget[ii].cblist,nn);                                      //  remove from memory list
   gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(zd->widget[ii].widget),nn);      //    and from widget
   gtk_combo_box_set_active(GTK_COMBO_BOX(zd->widget[ii].widget),-1);            //  set no active entry

   return 0;
}


//  delete all entries from combo drop down list

int zdialog_cb_clear(zdialog *zd, cchar *name)
{
   int         ii, jj, nn;
   GtkWidget   *widget, *entry;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box

   nn = pvlist_count(zd->widget[ii].cblist);                                     //  entry count
   for (jj = nn-1; jj >= 0; jj--) {
      pvlist_remove(zd->widget[ii].cblist,jj);                                   //  remove from memory list
      gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(zd->widget[ii].widget),jj);   //  remove from widget
   }

   widget = zd->widget[ii].widget;
   gtk_combo_box_set_active(GTK_COMBO_BOX(widget),-1);                           //  set no active entry
   if (strmatch(zd->widget[ii].type,"comboE")) {                                 //  stuff entry box with nothing
      entry = gtk_bin_get_child(GTK_BIN(widget));
      gtk_entry_set_text(GTK_ENTRY(entry),"");
   }

   return 1;
}


//  make a combo box drop down to show all entries

int zdialog_cb_popup(zdialog *zd, cchar *name)
{
   int      ii;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 0;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 0;                                          //  not found
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 0;         //  not combo box

   gtk_combo_box_popup(GTK_COMBO_BOX(zd->widget[ii].widget));
   gtk_combo_box_set_active(GTK_COMBO_BOX(zd->widget[ii].widget),-1);            //  no active entry                    6.2

   return 1;
}


//  save all combo box entries to a file in /home/<user>/.appname/filename
//  returns 0 = OK, +N = error

int zdialog_cb_save(zdialog *zd, cchar *name, cchar *filename)                   //  6.2
{
   char     file[200], *pp;
   int      ii, jj, nn;
   FILE     *fid;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 1;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 2;
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 3;         //  not combo box

   nn = pvlist_count(zd->widget[ii].cblist);                                     //  entry count
   if (! nn) return 0;

   snprintf(file,200,"%s/%s",get_zuserdir(),filename);
   fid = fopen(file,"w");
   if (! fid) return 4;

   for (jj = 0; jj < nn; jj++) {
      pp = pvlist_get(zd->widget[ii].cblist,jj);
      fprintf(fid,"%s\n",pp);
   }
   
   fclose(fid);
   return 0;
}


//  load combo box entries from a file in /home/<user>/.appname/filename
//  returns 0 = OK, +N = error

int zdialog_cb_load(zdialog *zd, cchar *name, cchar *filename)                   //  6.2
{
   char     file[200], data[100], *pp;
   int      ii;
   FILE     *fid;

   zthreadcrash();

   if (! zdialog_valid(zd)) return 1;

   for (ii = 1; zd->widget[ii].type; ii++)                                       //  find widget
      if (strmatch(zd->widget[ii].name,name)) break;
   if (! zd->widget[ii].type) return 2;
   if (! strmatchV(zd->widget[ii].type,"combo","comboE",null)) return 3;         //  not combo box

   zdialog_cb_clear(zd,name);

   snprintf(file,200,"%s/%s",get_zuserdir(),filename);
   fid = fopen(file,"r");
   if (! fid) return 0;
   
   while (true) {
      pp = fgets_trim(data,99,fid);
      if (! pp) break;
      zdialog_cb_app(zd,name,data);
   }
   
   fclose(fid);
   return 0;
}


/**************************************************************************/

//  functions to save and recall zdialog window positions

namespace zdposn_names
{
   struct zdposn_t {
      float    xpos, ypos;                                                       //  window position WRT parent or desktop
      char     wintitle[64];                                                     //  window title (ID)
   }  zdposn[200];                                                               //  space for 200 windows

   int      Nzdposn;                                                             //  no. in use
   int      Nzdpmax = 200;                                                       //  table size
}


//  Load zdialog positions table from its file (application startup)
//  or save zdialog positions table to its file (application exit).
//  Action is "load" or "save". Number of table entries is returned.

int zdialog_positions(cchar *action)
{
   using namespace zfuncs;
   using namespace zdposn_names;

   char     posfile[200], buff[100], wintitle[64], *pp;
   float    xpos, ypos;
   int      nn, ii;
   FILE     *fid;

   snprintf(posfile,199,"%s/zdialog_positions",zuserdir);                        //  /home/<user>/.appname/zdialog_positions

   if (strmatch(action,"load"))                                                  //  load dialog positions table from file
   {
      fid = fopen(posfile,"r");
      if (! fid) {
         Nzdposn = 0;
         return 0;
      }

      for (nn = 0; nn < Nzdpmax; nn++)
      {
         pp = fgets(buff,100,fid);
         if (! pp) break;
         if (strlen(pp) < 64) continue;
         strncpy0(wintitle,buff,64);
         strTrim(wintitle);
         if (strlen(wintitle) < 3) continue;
         ii = sscanf(buff + 64," %f %f ",&xpos,&ypos);
         if (ii != 2) continue;
         strcpy(zdposn[nn].wintitle,wintitle);
         zdposn[nn].xpos = xpos;
         zdposn[nn].ypos = ypos;
      }

      fclose(fid);
      Nzdposn = nn;
      return Nzdposn;
   }

   if (strmatch(action,"save"))                                                  //  save dialog positions table to file
   {
      fid = fopen(posfile,"w");
      if (! fid) {
         printz("*** cannot write zdialog_positions file \n");
         return 0;
      }

      for (nn = 0; nn < Nzdposn; nn++)
         fprintf(fid,"%-64s %0.1f %0.1f \n",zdposn[nn].wintitle,
                                 zdposn[nn].xpos, zdposn[nn].ypos);
      fclose(fid);
      return Nzdposn;
   }

   printz("*** zdialog_positions bad action: %s \n",action);
   return 0;
}


//  Set the initial or new zdialog window position from "posn".
//  Called by zdialog_run().  Private function.
//     null:      window manager decides
//    "mouse"     put dialog at mouse position
//    "desktop"   center dialog in desktop window
//    "parent"    center dialog in parent window
//    "save"      use the same position last set by the user
//    "nn/nn"     put NW corner of dialog in parent window at % size
//                (e.g. "50/50" puts NW corner at center of parent)

void zdialog_set_position(zdialog *zd, cchar *posn)
{
   using namespace zdposn_names;

   int         ii, ppx, ppy, zdpx, zdpy, pww, phh;
   float       xpos, ypos;
   char        wintitle[64], *pp;
   GtkWidget   *parent, *dialog;

   zthreadcrash();

   parent = zd->parent;
   dialog = zd->widget[0].widget;

   if (strmatch(posn,"mouse")) {
      gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_MOUSE);
      return;
   }

   if (strmatch(posn,"desktop")) {
      gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
      return;
   }

   if (strmatch(posn,"parent")) {
      gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER_ON_PARENT);
      return;
   }

   if (! parent) {                                                               //  no parent window
      ppx = ppy = 0;                                                             //  use desktop
      pww = gdk_screen_width();
      phh = gdk_screen_height();
   }
   else {
      gtk_window_get_position(GTK_WINDOW(parent),&ppx,&ppy);                     //  parent window NW corner
      gtk_window_get_size(GTK_WINDOW(parent),&pww,&phh);                         //  parent window size
   }

   if (strmatch(posn,"save"))                                                    //  use last saved window position
   {
      zd->saveposn = 1;                                                          //  set flag for zdialog_free()

      pp = (char *) gtk_window_get_title(GTK_WINDOW(dialog));                    //  get window title, used as ID
      if (! pp) return;
      if (strlen(pp) < 2) return;
      strncpy0(wintitle,pp,64);                                                  //  window title, < 64 chars.
      for (ii = 0; ii < Nzdposn; ii++)                                           //  search table for title
         if (strmatch(wintitle,zdposn[ii].wintitle)) break;
      if (ii == Nzdposn) return;                                                 //  not found - zdialog_destroy() will add

      zdpx = ppx + 0.01 * zdposn[ii].xpos * pww;                                 //  position for dialog window
      zdpy = ppy + 0.01 * zdposn[ii].ypos * phh;
      gtk_window_move(GTK_WINDOW(dialog),zdpx,zdpy);
      return;
   }

   else     //  "nn/nn"                                                          //  position from caller
   {
      ii = sscanf(posn,"%f/%f",&xpos,&ypos);                                     //  parse "nn/nn"
      if (ii != 2) return;
      zdpx = ppx + 0.01 * xpos * pww;                                            //  position for dialog window
      zdpy = ppy + 0.01 * ypos * phh;
      gtk_window_move(GTK_WINDOW(dialog),zdpx,zdpy);
      return;
   }
}


//  If the dialog window position is "save" then save
//  its position WRT parent or desktop for next use.
//  called by zdialog_destroy().  Private function.

void zdialog_save_position(zdialog *zd)
{
   using namespace zdposn_names;

   int         ii, ppx, ppy, pww, phh, zdpx, zdpy;
   float       xpos, ypos;
   char        wintitle[64], *pp;
   GtkWidget   *parent, *dialog;

   zthreadcrash();

   dialog = zd->widget[0].widget;
   if (! dialog) return;                                                         //  destroyed

   gtk_window_get_position(GTK_WINDOW(dialog),&zdpx,&zdpy);                      //  dialog window NW corner
   if (! zdpx && ! zdpy) return;                                                 //  (0,0) ignore                    5.7

   parent = zd->parent;                                                          //  parent window

   if (! parent) {                                                               //  no parent window
      ppx = ppy = 0;                                                             //  use desktop
      pww = gdk_screen_width();
      phh = gdk_screen_height();
   }
   else {
      gtk_window_get_position(GTK_WINDOW(parent),&ppx,&ppy);                     //  parent window NW corner
      gtk_window_get_size(GTK_WINDOW(parent),&pww,&phh);                         //  parent window size
   }

   xpos = 100.0 * (zdpx - ppx) / pww;                                            //  dialog window relative position
   ypos = 100.0 * (zdpy - ppy) / phh;                                            //  (as percent of parent size)

   pp = (char *) gtk_window_get_title(GTK_WINDOW(dialog));
   if (! pp) return;
   if (strlen(pp) < 2) return;
   strncpy0(wintitle,pp,64);                                                     //  window title, < 64 chars.

   for (ii = 0; ii < Nzdposn; ii++)                                              //  search table for window
      if (strmatch(wintitle,zdposn[ii].wintitle)) break;
   if (ii == Nzdposn) {                                                          //  not found
      if (ii == Nzdpmax) return;                                                 //  table full
      strcpy(zdposn[ii].wintitle,wintitle);                                      //  add window to table
      Nzdposn++;
   }

   zdposn[ii].xpos = xpos;                                                       //  save window position
   zdposn[ii].ypos = ypos;
   return;
}


/**************************************************************************/

//  Functions to save and restore zdialog user inputs
//    within an app session or across app sessions.

namespace zdinputs_names
{
   struct zdinputs_t {
      char     *zdtitle;                                                         //  zdialog title
      int      Nw;                                                               //  no. of widgets
      char     **wname;                                                          //  list of widget names
      char     **wdata;                                                          //  list of widget data
   }  zdinputs[200];                                                             //  space for 200 dialogs

   #define  Nwmax zdmaxwidgets                                                   //  max. widgets in a dialog
   int      Nzd = 0;                                                             //  no. zdialogs in use
   int      Nzdmax = 200;                                                        //  max. zdialogs
   int      ccmax1 = 99;                                                         //  max. widget name length
   int      ccmax2 = 199;                                                        //  max. widget data length
}


//  Load zdialog input fields from its file (app startup)
//  or save zdialog input fields to its file (app shutdown).
//  Action is "load" or "save".
//  Number of zdialogs is returned.

int zdialog_inputs(cchar *action)                                                //  5.3
{
   using namespace zfuncs;
   using namespace zdinputs_names;

   char     zdinputsfile[200], buff[200];
   char     zdtitle[100], wname[Nwmax][100], wdata[Nwmax][200];
   char     *pp, *pp1, *pp2, wdata2[250];                                        //  ccmax2 + 50
   FILE     *fid;
   int      Nw, ii, jj, cc, cc1, cc2;

   snprintf(zdinputsfile,199,"%s/zdialog_inputs",zuserdir);                      //  /home/<user>/.appname/zdialog_inputs

   if (strmatch(action,"load"))                                                  //  load dialog input fields from its file
   {
      Nzd = 0;

      fid = fopen(zdinputsfile,"r");                                             //  no file
      if (! fid) return 0;

      while (true)
      {
         pp = fgets_trim(buff,200,fid,1);                                        //  read next zdialog title record
         if (! pp) break;
         if (! strmatchN(pp,"zdialog == ",11)) continue;

         strncpy0(zdtitle,pp+11,ccmax1);                                         //  save new zdialog title

         pp = fgets_trim(buff,200,fid,1);                                        //  read widget count
         if (! pp) break;
         Nw = atoi(pp);
         if (Nw < 1 || Nw > Nwmax) {
            printz("*** zdialog_inputs() bad data: %s \n",zdtitle);
            continue;
         }

         for (ii = 0; ii < Nw; ii++)                                             //  read widget data recs
         {
            pp = fgets_trim(buff,200,fid,1);
            if (! pp) break;
            pp1 = pp;
            pp2 = strstr(pp1," ==");
            if (! pp2) break;                                                    //  widget has no data
            cc1 = pp2 - pp1;
            pp1[cc1] = 0;
            pp2 += 3;
            if (*pp2 == ' ') pp2++;
            cc2 = strlen(pp2);
            if (cc1 < 1 || cc1 > ccmax1) break;
            if (cc2 < 1) pp2 = (char *) "";
            if (cc2 > ccmax2) break;                                             //  do not copy large inputs
            strcpy(wname[ii],pp1);                                               //  save widget name and data
            strcpy(wdata2,pp2);
            repl_1str(wdata2,wdata[ii],"\\n","\n");                              //  replace "\n" with newline chars.
         }

         if (ii < Nw) {
            printz("*** zdialog_inputs() bad data: %s \n",zdtitle);
            continue;
         }

         if (Nzd == Nzdmax) {
            printz("*** zdialog_inputs() too many dialogs \n");
            break;
         }

         zdinputs[Nzd].zdtitle = zstrdup(zdtitle);                               //  save acculumated zdialog data
         zdinputs[Nzd].Nw = Nw;
         cc = Nw * sizeof(char *);
         zdinputs[Nzd].wname = (char **) zmalloc(cc);
         zdinputs[Nzd].wdata = (char **) zmalloc(cc);
         for (ii = 0; ii < Nw; ii++) {
            zdinputs[Nzd].wname[ii] = zstrdup(wname[ii]);
            zdinputs[Nzd].wdata[ii] = zstrdup(wdata[ii]);
         }

         Nzd++;
      }

      fclose(fid);
      return Nzd;
   }

   if (strmatch(action,"save"))                                                  //  save dialog input fields to its file
   {
      fid = fopen(zdinputsfile,"w");
      if (! fid) {
         printz("*** zdialog_inputs() cannot write file \n");
         return 0;
      }

      for (ii = 0; ii < Nzd; ii++)
      {
         fprintf(fid,"zdialog == %s \n",zdinputs[ii].zdtitle);                   //  zdialog == zdialog title
         Nw = zdinputs[ii].Nw;
         fprintf(fid,"%d \n",Nw);                                                //  widget count
         for (jj = 0; jj < Nw; jj++) {
            pp1 = zdinputs[ii].wname[jj];                                        //  widget name == widget data
            pp2 = zdinputs[ii].wdata[jj];
            repl_1str(pp2,wdata2,"\n","\\n");                                    //  replace newline chars. with "\n"
            fprintf(fid,"%s == %s \n",pp1,wdata2);
         }
         fprintf(fid,"\n");
      }

      fclose(fid);
      return Nzd;
   }

   printz("*** zdialog_inputs bad action: %s \n",action);
   return 0;
}


//  Save dialog user input fields when a dialog is finished.
//  Called automatically by zdialog_free(). Private function.

int zdialog_save_inputs(zdialog *zd)                                             //  5.3
{
   using namespace zdinputs_names;

   char     zdtitle[100], wname[100], wdata[200], *type;
   int      ii, jj, Nw, cc;

   if (! zdialog_valid(zd)) return 0;

   if (! zd->saveinputs) return 0;                                               //  zdialog does not use this service

   strncpy0(zdtitle,zd->widget[0].data,ccmax1);                                  //  zdialog title is widget[0].data

   for (ii = 0; ii < Nzd; ii++)                                                  //  find zdialog in zdinputs table
      if (strmatch(zdtitle,zdinputs[ii].zdtitle)) break;

   if (ii < Nzd) {                                                               //  found
      zfree(zdinputs[ii].zdtitle);                                               //  delete obsolete zdinputs data
      for (jj = 0; jj < zdinputs[ii].Nw; jj++) {
         zfree(zdinputs[ii].wname[jj]);
         zfree(zdinputs[ii].wdata[jj]);
      }
      zfree(zdinputs[ii].wname);
      zfree(zdinputs[ii].wdata);
      Nzd--;                                                                     //  decr. zdialog count
      for (ii = ii; ii < Nzd; ii++)                                              //  pack down the rest
         zdinputs[ii] = zdinputs[ii+1];
   }

   if (Nzd == Nzdmax) {
      printz("*** zdialog_save_inputs, too many zdialogs \n");
      return 0;
   }

   ii = Nzd;                                                                     //  next zdinputs table entry

   for (Nw = 0, jj = 1; zd->widget[jj].type; jj++)                               //  count zdialog widgets
   {
      type = (char *) zd->widget[jj].type;
      if (strstr("dialog hbox vbox hsep vsep frame "                             //  skip non-input widgets
                 "scrwin label link button",type)) continue;
      Nw++;
   }

   if (! Nw) return 0;                                                           //  no input widgets
   if (Nw > Nwmax) {
      printz("*** zdialog_inputs() bad data: %s \n",zdtitle);
      return 0;
   }

   zdinputs[ii].zdtitle = zstrdup(zdtitle);                                      //  set zdialog title

   cc = Nw * sizeof(char *);                                                     //  allocate pointers for widgets
   zdinputs[ii].wname = (char **) zmalloc(cc);
   zdinputs[ii].wdata = (char **) zmalloc(cc);

   for (Nw = 0, jj = 1; zd->widget[jj].type; jj++)                               //  add widget names and data
   {
      type = (char *) zd->widget[jj].type;
      if (strstr("dialog hbox vbox hsep vsep frame "
                 "scrwin label link button",type)) continue;
      strncpy0(wname,zd->widget[jj].name,ccmax1);
      if (zd->widget[jj].data)
         strncpy0(wdata,zd->widget[jj].data,ccmax2);
      else strcpy(wdata,"");
      zdinputs[ii].wname[Nw] = zstrdup(wname);
      zdinputs[ii].wdata[Nw] = zstrdup(wdata);
      Nw++;
   }

   zdinputs[ii].Nw = Nw;                                                         //  set widget count
   Nzd++;                                                                        //  add zdialog to end of zdinputs

   return 1;
}


//  Restore user input fields from prior use of the same dialog.
//  Call this if wanted after zdialog is built and before it is run.
//  Override old user inputs with zdialog_stuff() where needed.

int zdialog_restore_inputs(zdialog *zd)                                          //  5.3
{
   using namespace zdinputs_names;

   char     *zdtitle, *wname, *wdata;
   int      ii, jj;

   zd->saveinputs = 1;                                                           //  flag, save data at zdialog_free()

   zdtitle = (char *) zd->widget[0].data;                                        //  zdialog title

   for (ii = 0; ii < Nzd; ii++)                                                  //  find zdialog in zdinputs
      if (strmatch(zdtitle,zdinputs[ii].zdtitle)) break;
   if (ii == Nzd) return 0;                                                      //  not found

   for (jj = 0; jj < zdinputs[ii].Nw; jj++) {                                    //  stuff all saved widget data
      wname = zdinputs[ii].wname[jj];
      wdata = zdinputs[ii].wdata[jj];
      zdialog_put_data(zd,wname,wdata);
   }

   return 1;
}


/**************************************************************************/

//  Output text to a popup window.
//  Window is left on screen until user destroys it with [x] button or
//  caller closes it with "close" action.
//     action:  open: create window with title = text and size = ww, hh
//              write: write text to next line in window
//              writebold: write bold text to next line in window
//              top: scroll back to line 1 after writing last line
//              close: close window now
//              close N: close window after N seconds
//              move: move the window by (ww,hh) pixels                          //                                     6.2
//  The parent argument is optional and will cause the popup window to center
//  on the parent window. The GtkTextView window is returned for possible
//  use with textwidget_set_clickfunc() etc.

GtkWidget * write_popup_text(cchar *action, cchar *text, int ww, int hh, GtkWidget *parent)
{
   int popup_text_timeout(GtkWidget **mWin);

   static GtkWidget  *mWin = 0, *mVbox, *mScroll;
   static GtkWidget  *mLog = 0;
   static GtkAdjustment  *vscroll;
   double      upper;
   int         secs, posx, posy;
   char        buff[200];

   zthreadcrash();                                                               //  thread usage not allowed

   if (strmatch(action,"open"))
   {
      if (mWin) gtk_widget_destroy(mWin);                                        //  only one at a time

      if (! ww) ww = 400;
      if (! hh) hh = 300;

      mWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);                                //  create main window
      gtk_window_set_title(GTK_WINDOW(mWin),text);
      gtk_window_set_default_size(GTK_WINDOW(mWin),ww,hh);

      if (parent) {                                                              //  parent added
         gtk_window_set_transient_for(GTK_WINDOW(mWin),GTK_WINDOW(parent));
         gtk_window_set_position(GTK_WINDOW(mWin),GTK_WIN_POS_CENTER_ON_PARENT);
      }
      else
         gtk_window_set_position(GTK_WINDOW(mWin),GTK_WIN_POS_MOUSE);

      mVbox = gtk_box_new(VERTICAL,0);                                           //  vertical packing box
      gtk_container_add(GTK_CONTAINER(mWin),mVbox);                              //  add to main window
      mScroll = gtk_scrolled_window_new(0,0);                                    //  scrolled window
      gtk_box_pack_end(GTK_BOX(mVbox),mScroll,1,1,0);                            //  add to main window mVbox
      mLog = gtk_text_view_new();                                                //  text edit window
      gtk_container_add(GTK_CONTAINER(mScroll),mLog);                            //  add to scrolled window
      G_SIGNAL(mWin,"destroy",write_popup_text,"destroypop");                    //  connect window destroy event
      gtk_widget_show_all(mWin);                                                 //  show window
      if (! parent) gtk_window_set_keep_above(GTK_WINDOW(mWin),1);
      vscroll = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(mScroll));
      zfuncs::open_popup_windows++;
   }
   
   if (! mWin) return 0;

   if (strmatch(action,"write")) {                                               //  add text to window
      snprintf(buff,200,"%s\n",text);
      wprintx(mLog,0,buff);
      upper = gtk_adjustment_get_upper(vscroll);                                 //  scroll down                        6.1
      gtk_adjustment_set_value(vscroll,upper);                                   //  (GTK did not need this earlier)
   }

   if (strmatch(action,"writebold")) {                                           //  add bold text to window
      snprintf(buff,200,"%s\n",text);
      wprintx(mLog,0,buff,1);
      upper = gtk_adjustment_get_upper(vscroll);                                 //  scroll down                        6.1
      gtk_adjustment_set_value(vscroll,upper);                                   //  (GTK did not need this earlier)
   }

   if (strmatch(action,"top"))                                                   //  scroll to top line
      wscroll(mLog,1);

   if (strmatchN(action,"close",5)) {                                            //  close window
      secs = atoi(action+5);
      if (secs < 1) {
         gtk_widget_destroy(mWin);                                               //  immediate destroy
         mWin = 0;   
      }
      else g_timeout_add_seconds(secs,(GSourceFunc) popup_text_timeout,&mWin);
   }
   
   if (strmatch(action,"move")) {                                                //  shift the text window              6.2
      gtk_window_get_position(GTK_WINDOW(mWin),&posx,&posy);
      gtk_window_move(GTK_WINDOW(mWin),posx+ww,posy+hh);
   }

   if (text && strmatch(text,"destroypop")) {                                    //  "destroy" signal from [x]
      zfuncs::open_popup_windows--;
      mWin = 0;
   }

   zmainloop();
   return mLog;
}


//  private function for timeout

int popup_text_timeout(GtkWidget **mWin)
{
   if (*mWin) gtk_widget_destroy(*mWin);
   return 0;
}


/**************************************************************************/

//  execute a command and show the output in a scrolling popup window

int popup_command(cchar *command, int ww, int hh, GtkWidget *parent, int top)
{
   char        *buff;
   int         err, contx = 0;

   write_popup_text("open",command,ww,hh,parent);                                //  bugfix

   while ((buff = command_output(contx,command)))
   {
      write_popup_text("write",buff);
      zfree(buff);
   }

   if (top) write_popup_text("top",0);                                           //  back to top of window     6.0

   err = command_status(contx);
   return err;
}


/**************************************************************************/

//  display message box and wait for user acknowledgement
//  may be called from a thread (uses xterm message).

void zmessageACK(GtkWidget *parent, cchar *format, ... )
{
   va_list     arglist;
   char        message[1000];
   cchar       *posn = "mouse";
   zdialog     *zd;

   va_start(arglist,format);
   vsnprintf(message,999,format,arglist);
   va_end(arglist);

   if (! pthread_equal(pthread_self(),zfuncs::tid_main)) {                       //  from a thread, no GTK allowed
      zpopup_message(0,message);                                                 //  alternative popup
      return;
   }

   zd = zdialog_new(0,parent,"OK",null);
   zdialog_add_widget(zd,"hbox","hb1","dialog",0,"space=3");
   zdialog_add_widget(zd,"label","lab1","hb1",message,"space=5");
   zdialog_resize(zd,200,0);
   zdialog_set_modal(zd);
   zdialog_set_decorated(zd,0);
   if (parent) posn = "parent";                                                  //  6.1
   zdialog_run(zd,0,posn);
   zdialog_wait(zd);
   zdialog_free(zd);
   return;
}


/**************************************************************************/

//  log error message to STDOUT as well as message box and user OK

void zmessLogACK(GtkWidget *parent, cchar *format, ...)
{
   va_list        arglist;
   char           message[1000];

   va_start(arglist,format);
   vsnprintf(message,999,format,arglist);
   va_end(arglist);

   printz("%s \n",message);
   zmessageACK(parent,message);
   return;
}


/**************************************************************************/

//  display message box and wait for user Yes or No response
//  returns 1 or 0

int zmessageYN(GtkWidget *parent, cchar *format, ... )
{
   va_list        arglist;
   char           message[400];
   zdialog        *zd;
   int            zstat;

   zthreadcrash();                                                               //  thread usage not allowed

   va_start(arglist,format);
   vsnprintf(message,400,format,arglist);
   va_end(arglist);

   zd = zdialog_new("message",parent,ZTX("Yes"),ZTX("No"),null);
   zdialog_add_widget(zd,"hbox","hb1","dialog",0,"space=3");
   zdialog_add_widget(zd,"label","lab1","hb1",message,"space=5");
   zdialog_resize(zd,200,0);
   zdialog_set_modal(zd);
   zdialog_set_decorated(zd,0);
   zdialog_run(zd,0,"mouse");                                                    //  5.5
   zstat = zdialog_wait(zd);
   zdialog_free(zd);
   if (zstat == 1) return 1;
   return 0;
}


/**************************************************************************/

//  display message until timeout (can be forever) or user cancel
//  or caller kills it with zdialog_free()

zdialog * zmessage_post(GtkWidget *parent, int seconds, cchar *format, ... )
{
   int  zmessage_post_timeout(zdialog *zd);
   int  zmessage_post_event(zdialog *zd, cchar *event);

   va_list           arglist;
   char              message[400];
   static zdialog    *zd;

   zthreadcrash();                                                               //  thread usage not allowed

   va_start(arglist,format);
   vsnprintf(message,400,format,arglist);
   va_end(arglist);

   zd = zdialog_new(0,parent,null);
   zdialog_add_widget(zd,"hbox","hb1","dialog",0,"space=3");
   zdialog_add_widget(zd,"label","lab1","hb1",message,"space=5");
   zdialog_set_decorated(zd,0);
   zdialog_run(zd,zmessage_post_event);

   if (seconds)
      g_timeout_add_seconds(seconds,(GSourceFunc) zmessage_post_timeout,zd);

   zmainloop();                                                                  //  5.2
   return zd;
}

int zmessage_post_timeout(zdialog *zd)
{
   if (! zdialog_valid(zd)) return 0;
   if (zd->zstat == 0) zdialog_free(zd);
   return 0;
}

int zmessage_post_event(zdialog *zd, cchar *event)
{
   if (! zdialog_valid(zd)) return 0;
   if (zd->zstat) zdialog_free(zd);
   return 0;
}


/**************************************************************************/

//  get text input from a popup dialog
//  returned text is subject for zfree()
//  null is returned if user presses [cancel] button.

char * zdialog_text(GtkWidget *parent, cchar *title, cchar *inittext)
{
   zdialog     *zd;
   int         zstat;
   char        *text;

   zthreadcrash();                                                               //  thread usage not allowed

   zd = zdialog_new(title,parent,"OK",ZTX("cancel"),null);
   if (! title) zdialog_set_decorated(zd,0);                                     //  6.1
   zdialog_add_widget(zd,"frame","fred","dialog");
   zdialog_add_widget(zd,"edit","edit","fred");
   if (inittext) zdialog_stuff(zd,"edit",inittext);

   zdialog_resize(zd,200,0);
   zdialog_set_modal(zd);
   zdialog_run(zd,0,"mouse");                                                    //  5.5
   zstat = zdialog_wait(zd);
   if (zstat == 1)
      text = (char *) zdialog_get_data(zd,"edit");
   else text = 0;
   if (text) text = zstrdup(text);
   zdialog_free(zd);
   return text;
}


/**************************************************************************/

//  Display a dialog with a message and 1-5 choice buttons.
//  Returns choice 1-N corresponding to button selected.
//  nn = zdialog_choose(parent,message,butt1,butt2, ...null)

int zdialog_choose(GtkWidget *parent, cchar *message, ...)
{
   zdialog     *zd;
   va_list     arglist;
   int         ii, zstat, Nbutts;
   cchar       *butts[6];

   zthreadcrash();                                                               //  thread usage not allowed

   va_start(arglist,message);

   for (ii = 0; ii < 5; ii++)
   {
      butts[ii] = va_arg(arglist,cchar *);
      if (! butts[ii]) break;
   }

   Nbutts = ii;
   if (! Nbutts) zappcrash("zdialog_choose(), no buttons");

repeat:

   zd = zdialog_new(0,parent,butts[0],butts[1],butts[2],butts[3],butts[4],null);
   zdialog_add_widget(zd,"hbox","hbmess","dialog","space=3");
   zdialog_add_widget(zd,"label","labmess","hbmess",message,"space=5");
   zdialog_set_modal(zd);
   zdialog_set_decorated(zd,0);
   zdialog_run(zd,0,"mouse");                                                    //  5.5
   zstat = zdialog_wait(zd);
   zdialog_free(zd);

   if (zstat < 1) goto repeat;
   return zstat;
}


/**************************************************************************/

//  functions to show popup text windows

namespace poptext {
   char        *ptext = 0;
   GtkWidget   *popwin = 0;
   char        *pcurrent = 0;
   #define GSFNORMAL GTK_STATE_FLAG_NORMAL
}


//  timer function to show popup window after a given time

int poptext_show(char *current)
{
   using namespace poptext;

   zthreadcrash();

   if (current != pcurrent) return 0;
   if (popwin) gtk_widget_show_all(popwin);
   return 0;
}


//  timer function to kill popup window after a given time

int poptext_kill(char *current)
{
   using namespace poptext;

   zthreadcrash();

   if (current != pcurrent) return 0;
   if (popwin) gtk_widget_destroy(popwin);
   if (ptext) zfree(ptext);
   popwin = 0;
   ptext = 0;
   return 0;
}


//  kill popup window unconditionally                                            //  5.8

int poptext_killnow()
{
   using namespace poptext;

   zthreadcrash();

   if (popwin) gtk_widget_destroy(popwin);
   if (ptext) zfree(ptext);
   popwin = 0;
   ptext = 0;
   return 0;
}


//  Show a popup text message at current mouse position + offsets.               //  5.6
//  Any prior popup will be killed and replaced.
//  If text == null, kill without replacement.
//  secs1 is time to delay before showing the popup.
//  secs2 is time to kill the popup after it is shown.
//  This function returns immediately.

void poptext_mouse(cchar *text, int dx, int dy, float secs1, float secs2)        //  5.9
{
   using namespace poptext;

   GtkWidget   *label;
   int         cc, mx, my;
   int         millisec1, millisec2;

   zthreadcrash();                                                               //  thread usage not allowed

   if (popwin) gtk_widget_destroy(popwin);                                       //  kill prior popup
   if (ptext) zfree(ptext);
   popwin = 0;
   ptext = 0;
   pcurrent++;                                                                   //  make current != pcurrent

   if (! text) return;

   cc = strlen(text);                                                            //  construct popup window
   ptext = (char *) zmalloc(cc+1);                                               //    with caller's text
   strcpy(ptext,text);
   popwin = gtk_window_new(GTK_WINDOW_POPUP);
   label = gtk_label_new(ptext);
   gtk_container_add(GTK_CONTAINER(popwin),label);

   gdk_device_get_position(zfuncs::mouse,0,&mx,&my);                             //  position popup window
   gtk_window_move(GTK_WINDOW(popwin),mx+dx,my+dy);

   if (secs1 > 0) {                                                              //  delayed popup display
      millisec1 = secs1 * 1000;
      g_timeout_add(millisec1,(GSourceFunc) poptext_show,pcurrent);
   }
   else gtk_widget_show_all(popwin);                                             //  immediate display

   if (secs2 > 0) {                                                              //  popup kill timer
      millisec2 = (secs1 + secs2) * 1000;
      g_timeout_add(millisec2,(GSourceFunc) poptext_kill,pcurrent);
   }

   return;
}


//  Show a popup text message at the given window position.                      //  5.6
//  Any prior popup will be killed and replaced.
//  If text == null, kill without replacement.
//  secs1 is time to delay before showing the popup.
//  secs2 is time to kill the popup after it is shown (-1 = never).
//  This function returns immediately.

void poptext_window(cchar *text, GtkWindow *pwin, int mx, int my, float secs1, float secs2)
{
   using namespace poptext;

   GtkWidget   *label;
   int         cc, pmx, pmy;
   int         millisec1, millisec2;

   zthreadcrash();                                                               //  thread usage not allowed

   if (popwin) gtk_widget_destroy(popwin);                                       //  kill prior popup
   if (ptext) zfree(ptext);
   popwin = 0;
   ptext = 0;
   pcurrent++;                                                                   //  make current != pcurrent

   if (! text) return;

   cc = strlen(text);                                                            //  construct popup window
   ptext = (char *) zmalloc(cc+1);                                               //    with caller's text
   strcpy(ptext,text);
   popwin = gtk_window_new(GTK_WINDOW_POPUP);
   label = gtk_label_new(ptext);
   gtk_container_set_border_width(GTK_CONTAINER(popwin),2);
   gtk_container_add(GTK_CONTAINER(popwin),label);

   if (pwin) {                                                                   //  position relative to pwin
      gtk_window_set_transient_for(GTK_WINDOW(popwin),pwin);
      gtk_window_get_position(pwin,&pmx,&pmy);
      mx += pmx;
      my += pmy;
   }
   gtk_window_move(GTK_WINDOW(popwin),mx,my);

   if (secs1 > 0) {                                                              //  delayed popup display
      millisec1 = secs1 * 1000;
      g_timeout_add(millisec1,(GSourceFunc) poptext_show,pcurrent);
   }
   else gtk_widget_show_all(popwin);                                             //  immediate display

   if (secs2 > 0) {                                                              //  popup kill timer
      millisec2 = (secs1 + secs2) * 1000;
      g_timeout_add(millisec2,(GSourceFunc) poptext_kill,pcurrent);
   }

   return;
}


//  Show an image file in a popup window at mouse position.
//  Re-use most recent window or create a new one if Fnewin != 0.
//  Returns 0 if OK, +N otherwise.

int popup_image(cchar *file, GtkWindow *parent, int Fnewin, int size)            //  6.0
{
   int popup_image_paint(GtkWidget *, cairo_t *, cchar *);
   int popup_image_scroll(GtkWidget *, GdkEventButton *event, cchar *);
   int popup_image_KBevent(GtkWidget *, GdkEventKey *event, cchar *);

   static GtkWidget  *window[10], *drawarea[10];                                 //  up to 10 popup windows open
   static char       *filex[10];
   static int        Nw = 0;

   zthreadcrash();

   if (Fnewin)
      if (++Nw == 10) Nw = 0;                                                    //  new window wanted, re-use oldest up to 10
   if (! Fnewin)
      while (Nw > 0 && window[Nw] == 0) Nw--;                                    //  else re-use latest still active

   if (window[Nw]) {
      gtk_widget_destroy(drawarea[Nw]);
      drawarea[Nw] = 0;
      zfree(filex[Nw]);
      filex[Nw] = 0;
   }
   else {
      window[Nw] = gtk_window_new(GTK_WINDOW_TOPLEVEL);                          //  create new popup window
      if (! window[Nw]) return 1;
      if (! size) size = 512;
      gtk_window_set_default_size(GTK_WINDOW(window[Nw]),size,size);
      if (parent) gtk_window_set_transient_for(GTK_WINDOW(window[Nw]),parent);
      gtk_window_set_position(GTK_WINDOW(window[Nw]),GTK_WIN_POS_MOUSE);
      gtk_widget_freeze_child_notify(window[Nw]);                                //  improves resize performance
      G_SIGNAL(window[Nw],"destroy",gtk_widget_destroyed,&window[Nw]);           //  set window = null if destroyed
   }

   filex[Nw] = zstrdup(file);
   drawarea[Nw] = gtk_drawing_area_new();                                        //  new drawing area always required
   if (! drawarea[Nw]) return 2;
   gtk_container_add(GTK_CONTAINER(window[Nw]),drawarea[Nw]);
   G_SIGNAL(drawarea[Nw],"draw",popup_image_paint,filex[Nw]);                    //  connect paint function & image file
   gtk_widget_add_events(drawarea[Nw],GDK_SCROLL_MASK);
   G_SIGNAL(drawarea[Nw],"scroll-event",popup_image_scroll,filex[Nw]);           //  connect mouse wheel scroll event
   gtk_widget_add_events(drawarea[Nw],GDK_SCROLL_MASK);
   G_SIGNAL(window[Nw],"key-release-event",popup_image_KBevent,filex[Nw]);
   gtk_widget_show_all(window[Nw]);

   return 0;
}


int popup_image_paint(GtkWidget *drawarea, cairo_t *cr, cchar *file)
{
   GtkWidget   *window;
   PIXBUF      *pixb1, *pixb2;
   GError      *gerror;
   int         ww1, hh1, ww2, hh2;
   int         sww, shh;
   double      area;
   cchar       *pp;

   if (! file) return 1;

   window = gtk_widget_get_parent(drawarea);                                     //  parent window
   if (! window) return 1;

   pp = strrchr(file,'/');                                                       //  window title = file name
   gtk_window_set_title(GTK_WINDOW(window),pp+1);

   gerror = 0;
   pixb1 = gdk_pixbuf_new_from_file(file,&gerror);                               //  load image file into pixbuf
   if (! pixb1) {
      printz("*** file: %s \n %s \n",file,gerror->message);
      return 1;
   }

   ww1 = gdk_pixbuf_get_width(pixb1);                                            //  image dimensions
   hh1 = gdk_pixbuf_get_height(pixb1);

   sww = gdk_screen_get_width(zfuncs::screen);                                   //  screen dimensions
   shh = gdk_screen_get_height(zfuncs::screen);

   gtk_window_get_size(GTK_WINDOW(window),&ww2,&hh2);                            //  current window dimensions
   area = ww2 * hh2;
   ww2 = sqrt(area * ww1 / hh1);                                                 //  fit window to image, keeping same area
   hh2 = area / ww2;
   if (ww2 < sww && hh2 < shh)                                                   //  prevent GTK resize event loop
      gtk_window_resize(GTK_WINDOW(window),ww2,hh2);

   pixb2 = gdk_pixbuf_scale_simple(pixb1,ww2,hh2,GDK_INTERP_BILINEAR);           //  rescale pixbuf to window
   if (! pixb2) return 1;

   gdk_cairo_set_source_pixbuf(cr,pixb2,0,0);                                    //  paint image
   cairo_paint(cr);

   g_object_unref(pixb1);
   g_object_unref(pixb2);
   return 1;
}


int popup_image_scroll(GtkWidget *drawarea, GdkEventButton *event, cchar *file)
{
   GtkWidget   *window;
   int         scroll, ww, hh;
   int         sww, shh;
   double      ff = 1.0;

   if (event->type == GDK_SCROLL) {                                              //  mouse whell event
      scroll = ((GdkEventScroll *) event)->direction;
      if (scroll == GDK_SCROLL_UP) ff = 1.33333;
      if (scroll == GDK_SCROLL_DOWN) ff = 0.75;
   }

   window = gtk_widget_get_parent(drawarea);                                     //  parent window
   if (! window) return 1;

   sww = gdk_screen_get_width(zfuncs::screen);                                   //  screen dimensions
   shh = gdk_screen_get_height(zfuncs::screen);

   gtk_window_get_size(GTK_WINDOW(window),&ww,&hh);                              //  current window dimensions
   ww *= ff;
   hh *= ff;

   if (ww > sww || hh > shh) {                                                   //  if request > screen size
      gtk_window_fullscreen(GTK_WINDOW(window));                                 //    use fullscreen mode
      return 1;
   }
   else gtk_window_unfullscreen(GTK_WINDOW(window));                             //  else no fullscreen mode

   if (ww + hh > 500)
      gtk_window_resize(GTK_WINDOW(window),ww,hh);                               //  rescale up or down
   else
      gtk_widget_destroy(window);                                                //  if very small, delete window

   return 1;
}


int popup_image_KBevent(GtkWidget *window, GdkEventKey *event, cchar *file)
{
   int         ww, hh, sww, shh;

   int KBkey = event->keyval;

   if (KBkey == GDK_KEY_Escape) gtk_widget_destroy(window);
   if (KBkey != GDK_KEY_F11) return 1;

   sww = gdk_screen_get_width(zfuncs::screen);                                   //  screen dimensions
   shh = gdk_screen_get_height(zfuncs::screen);

   gtk_window_get_size(GTK_WINDOW(window),&ww,&hh);                              //  current window dimensions

   if (ww < sww && hh < shh) {
      gtk_window_fullscreen(GTK_WINDOW(window));                                 //  make fullscreen mode
      return 1;
   }
   else gtk_window_unfullscreen(GTK_WINDOW(window));                             //  no fullscreen mode

   return 1;
}


/**************************************************************************

    File chooser dialog for one or more files

    Action:  "file"            select an existing file
             "files"           select multiple existing files
             "save"            select an existing or new file
             "folder"          select existing folder
             "folders"         select multiple existing folders
             "create folder"   select existing or new folder

    hidden   if > 0, add button to toggle display of hidden files
             optional, default = 0

    Returns a list of filespecs terminated with null.
    Memory for returned list and returned files are subjects for zfree();

***************************************************************************/


//  version for 1 file only: file, save, folder, create folder
//  returns one filespec or null
//  returned file is subject for zfree()

std::string zgetfile(cchar *title, GtkWindow *parent, cchar *action, cchar *initfile, int hidden)
{
   if (! strmatchV(action,"file","save","folder","create folder",null))
      zappcrash("zgetfile() call error: %s",action);

   char **flist = zgetfiles(title,parent,action,initfile,hidden);                //  parent added   6.1
   if (! flist) return 0;
   std::string file = *flist;
   zfree(flist);
   return file;
}


//  version for 1 or more files
//  returns a list of filespecs (char **) terminated with null

char ** zgetfiles(cchar *title, GtkWindow *parent, cchar *action, cchar *initfile, int hidden)
{
   using namespace zfuncs;

   void zgetfile_preview(GtkWidget *dialog, GtkWidget *pvwidget);                //  private functions
   int  zgetfile_KBkey(GtkWidget *dialog, GdkEventKey *event);
   void zgetfile_newfolder(GtkFileChooser *dialog, void *);

   GtkFileChooserAction fcact = GTK_FILE_CHOOSER_ACTION_OPEN;

   GtkWidget   *dialog;
   PIXBUF      *thumbnail;
   GtkWidget   *pvwidget = gtk_image_new();
   GSList      *gslist = 0;
   cchar       *button1 = 0, *buttxx = 0;
   char        *pdir, *pfile;
   int         ii, err, NF, setfname = 0;
   int         fcstat, bcode = 0, hide = 1;
   char        *file1, *file2, **flist = 0;
   STATB       fstat;

   zthreadcrash();                                                               //  thread usage not allowed

   if (strmatch(action,"file")) {
      fcact = GTK_FILE_CHOOSER_ACTION_OPEN;
      button1 = ZTX("choose file");
   }

   else if (strmatch(action,"files")) {
      fcact = GTK_FILE_CHOOSER_ACTION_OPEN;
      button1 = ZTX("choose files");
   }

   else if (strmatch(action,"save")) {
      fcact = GTK_FILE_CHOOSER_ACTION_SAVE;
      button1 = ZTX("save");
      setfname = 1;
   }

   else if (strmatch(action,"folder")) {
      fcact = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
      button1 = ZTX("choose folder");
   }

   else if (strmatch(action,"folders")) {
      fcact = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
      button1 = ZTX("choose folders");
   }

   else if (strmatch(action,"create folder")) {
      fcact = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
      button1 = ZTX("create folder");
      setfname = 1;
   }

   else zappcrash("zgetfiles() call error: %s",action);

   if (hidden) {
      buttxx = ZTX("hidden");
      bcode = 103;
   }

   dialog = gtk_file_chooser_dialog_new(title, parent, fcact,                    //  create file selection dialog
                              button1, GTK_RESPONSE_ACCEPT,                      //  parent added                       6.1
                              ZTX("cancel"), GTK_RESPONSE_CANCEL,
                              buttxx, bcode, null);

   gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog),pvwidget);

   G_SIGNAL(dialog,"update-preview",zgetfile_preview,pvwidget);                  //  create preview for selected file
   G_SIGNAL(dialog,"key-release-event",zgetfile_KBkey,0);                        //  respond to F1 help key

   gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_MOUSE);                //  put dialog at mouse position
   gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog),0);                 //  default: no show hidden

   if (strmatch(action,"save"))                                                  //  overwrite confirmation
      gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),1);

   if (strmatch(action,"files") || strmatch(action,"folders"))
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),1);          //  select multiple files or folders   5.8

   if (initfile) {                                                               //  pre-select filespec
      err = stat(initfile,&fstat);
      if (err) {
         pdir = zstrdup(initfile);                                               //  non-existent file
         pfile = strrchr(pdir,'/');
         if (pfile && pfile > pdir) {
            *pfile++ = 0;                                                        //  set folder name
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),pdir);
         }
         if (setfname) {                                                         //  set new file name
            if (! pfile) pfile = (char *) initfile;                              //  6.0
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),pfile);
         }
         zfree(pdir);
      }
      else if (S_ISREG(fstat.st_mode))                                           //  select given file
         gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),initfile);
      else if (S_ISDIR(fstat.st_mode))                                           //  select given folder
         gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),initfile);
   }

   if (initfile) {
      thumbnail = get_thumbnail(initfile,256);                                   //  preview for initial file           6.0
      if (thumbnail) {
         gtk_image_set_from_pixbuf(GTK_IMAGE(pvwidget),thumbnail);
         gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(dialog),1);
         g_object_unref(thumbnail);
      }
      else gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(dialog),0);
   }

   gtk_widget_show_all(dialog);

   while (true)
   {
      fcstat = gtk_dialog_run(GTK_DIALOG(dialog));                               //  run dialog, get status button

      if (fcstat == 103) {                                                       //  show/hide hidden files
         hide = 1 - hide;
         gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(dialog),hide);
         continue;
      }

      else if (fcstat == GTK_RESPONSE_ACCEPT)
      {
         gslist = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
         if (! gslist) continue;

         NF = g_slist_length(gslist);                                            //  no. selected files
         flist = (char **) zmalloc((NF+1)*sizeof(char *));                       //  allocate returned list

         for (ii = 0; ii < NF; ii++)
         {                                                                       //  process selected files
            file1 = (char *) g_slist_nth_data(gslist,ii);
            file2 = zstrdup(file1);                                              //  re-allocate memory
            flist[ii] = file2;
            g_free(file1);
         }
         flist[ii] = 0;                                                          //  EOL marker
         break;
      }

      else break;                                                                //  user bailout
   }

   if (gslist) g_slist_free(gslist);                                             //  return selected file(s)
   gtk_widget_destroy(dialog);
   return flist;
}


//  zgetfile private function - get preview images for image files

void zgetfile_preview(GtkWidget *dialog, GtkWidget *pvwidget)
{
   using namespace zfuncs;

   PIXBUF      *thumbnail;
   char        *filename;

   filename = gtk_file_chooser_get_preview_filename(GTK_FILE_CHOOSER(dialog));

   if (! filename) {
      gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(dialog),0);
      return;
   }

   thumbnail = get_thumbnail(filename,256);                                      //  256x256 pixels
   g_free(filename);

   if (thumbnail) {
      gtk_image_set_from_pixbuf(GTK_IMAGE(pvwidget),thumbnail);
      gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(dialog),1);
      g_object_unref(thumbnail);
   }
   else gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(dialog),0);

   return;
}


//  zgetfile private function - send F1 key (help) to main app

int zgetfile_KBkey(GtkWidget *dialog, GdkEventKey *event)
{
   int KBkey = event->keyval;
   if (KBkey != GDK_KEY_F1) return 0;                                            //  5.6
   KBstate(event,0);
   return 1;
}


/**************************************************************************

   print_image_file(GtkWidget *parent, cchar *imagefile)

   Print an image file using the printer, paper, orientation,
   margins, and scale set by the user.

   HPLIP problem: Setting paper size was made less flexible.
   GtkPrintSettings paper size must agree with the one in the current
   printer setup. This can only be set in the printer setup dialog, not
   in the application. Also the print size (width, height) comes from
   the chosen paper size and cannot be changed in the application.
   Print margins can be changed to effect printing a smaller or shifted
   image on a larger paper size.

***************************************************************************/

namespace print_image
{
   #define MM GTK_UNIT_MM
   #define INCH GTK_UNIT_INCH
   #define PRINTOP   GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG
   #define PORTRAIT  GTK_PAGE_ORIENTATION_PORTRAIT
   #define LANDSCAPE GTK_PAGE_ORIENTATION_LANDSCAPE
   #define QUALITY   GTK_PRINT_QUALITY_HIGH

   GtkWidget                  *parent = 0;
   GtkPageSetup               *priorpagesetup = 0;
   GtkPageSetup               *pagesetup;
   GtkPrintSettings           *printsettings = 0;
   GtkPrintOperation          *printop;
   GtkPageOrientation         orientation = PORTRAIT;
   PIXBUF                     *pixbuf;
   cchar                      *printer = 0;
   int                        landscape = 0;                                     //  true if landscape
   double                     width = 21.0, height = 29.7;                       //  paper size, CM (default A4 portrait)
   double                     margins[4] = { 0.5, 0.5, 0.5, 0.5 };               //  margins, CM (default 0.5)    5.5
   double                     imagescale = 100;                                  //  image print scale, percent
   double                     pwidth, pheight;                                   //  printed image size

   int   page_setup();
   int   margins_setup();
   int   margins_dialog_event(zdialog *zd, cchar *event);
   void  get_printed_image_size();
   void  print_page(GtkPrintOperation *, GtkPrintContext *, int page);
}


//  user callable function to set paper, margins, scale, and then print

void print_image_file(GtkWidget *pwin, cchar *imagefile)                         //  overhauled  5.2
{
   using namespace print_image;

   GtkPrintOperationResult  printstat;
   GError   *gerror = 0;
   int      err;

   zthreadcrash();

   parent = pwin;                                                                //  save parent window

   pixbuf = gdk_pixbuf_new_from_file(imagefile,&gerror);                         //  read image file
   if (! pixbuf) {
      zmessageACK(null,gerror->message);
      return;
   }

   err = page_setup();                                                           //  select size and orientation
   if (err) return;                                                              //  5.5

   err = margins_setup();                                                        //  set margins and scale
   if (err) return;                                                              //  5.5

   printop = gtk_print_operation_new();                                          //  print operation
   gtk_print_operation_set_default_page_setup(printop,pagesetup);
   gtk_print_operation_set_print_settings(printop,printsettings);
   gtk_print_operation_set_n_pages(printop,1);

   g_signal_connect(printop,"draw-page",G_CALLBACK(print_page),0);               //  start print
   printstat = gtk_print_operation_run(printop,PRINTOP,0,0);

   if (printstat == GTK_PRINT_OPERATION_RESULT_ERROR) {
      gtk_print_operation_get_error(printop,&gerror);
      zmessageACK(null,gerror->message);
   }

   g_object_unref(printop);
   return;
}


//  draw the graphics for the print page
//  rescale with cairo

void print_image::print_page(GtkPrintOperation *printop, GtkPrintContext *printcontext, int page)
{
   using namespace print_image;

   cairo_t           *cairocontext;
   double            iww, ihh, pww, phh, scale;

   pww = gtk_print_context_get_width(printcontext);                              //  print context size, pixels
   phh = gtk_print_context_get_height(printcontext);

   iww = gdk_pixbuf_get_width(pixbuf);                                           //  original image size
   ihh = gdk_pixbuf_get_height(pixbuf);

   scale = pww / iww;                                                            //  rescale to fit page
   if (phh / ihh < scale) scale = phh / ihh;

   cairocontext = gtk_print_context_get_cairo_context(printcontext);             //  use cairo to rescale
   cairo_translate(cairocontext,0,0);
   cairo_scale(cairocontext,scale,scale);
   gdk_cairo_set_source_pixbuf(cairocontext,pixbuf,0,0);
   cairo_paint(cairocontext);

   return;
}


//   Do a print paper format selection, after which the page width, height
//   and orientation are available to the caller. Units are CM.
//   (paper width and height are reversed for landscape orientation)

int print_image::page_setup()
{
   using namespace zfuncs;
   using namespace print_image;

   char     printsettingsfile[200], pagesetupfile[200];

   snprintf(printsettingsfile,200,"%s/printsettings",zuserdir);
   snprintf(pagesetupfile,200,"%s/pagesetup",zuserdir);

   if (! printsettings) {                                                        //  start with prior print settings
      printsettings = gtk_print_settings_new_from_file(printsettingsfile,0);
      if (! printsettings)
         printsettings = gtk_print_settings_new();
   }

   if (! priorpagesetup) {                                                       //  start with prior page setup
      priorpagesetup = gtk_page_setup_new_from_file(pagesetupfile,0);
      if (! priorpagesetup)
         priorpagesetup = gtk_page_setup_new();
   }

   pagesetup = gtk_print_run_page_setup_dialog                                   //  select printer, paper, orientation
            (GTK_WINDOW(parent),priorpagesetup,printsettings);                   //  user cancel cannot be detected     5.5

   g_object_unref(priorpagesetup);                                               //  save for next call
   priorpagesetup = pagesetup;

   orientation = gtk_print_settings_get_orientation(printsettings);              //  save orientation
   if (orientation == LANDSCAPE) landscape = 1;
   else landscape = 0;

   gtk_print_settings_set_quality(printsettings,QUALITY);                        //  set high quality 300 dpi
   gtk_print_settings_set_resolution(printsettings,300);

   gtk_print_settings_to_file(printsettings,printsettingsfile,0);                //  save print settings to file
   gtk_page_setup_to_file(pagesetup,pagesetupfile,0);                            //  save print settings to file

   return 0;
}


//   Optionally set the print margins and print scale.
//   If canceled the margins are zero (or printer-dependent minimum)
//   and the scale is 100% (fitting the paper and margins).

int print_image::margins_setup()
{
   using namespace print_image;

   zdialog     *zd;
   int         zstat;

/***
          margins  top  bottom  left   right
             CM   [___]  [___]  [___]  [___]
            Inch  [___]  [___]  [___]  [___]

         image scale [80|-+] percent

              image  width  height
                CM    xx.x   xx.x
               Inch   xx.x   xx.x

***/

   zd = zdialog_new(ZTX("margins"),parent,ZTX("done"),ZTX("cancel"),null);
   zdialog_add_widget(zd,"hbox","hbmlab","dialog");

   zdialog_add_widget(zd,"vbox","vbmarg","hbmlab",0,"homog|space=3");
   zdialog_add_widget(zd,"vbox","vbtop","hbmlab",0,"homog|space=3");
   zdialog_add_widget(zd,"vbox","vbbottom","hbmlab",0,"homog|space=3");
   zdialog_add_widget(zd,"vbox","vbleft","hbmlab",0,"homog|space=3");
   zdialog_add_widget(zd,"vbox","vbright","hbmlab",0,"homog|space=3");

   zdialog_add_widget(zd,"label","labmarg","vbmarg",ZTX("margins"),"space=5");
   zdialog_add_widget(zd,"label","labcm","vbmarg","CM","space=5");
   zdialog_add_widget(zd,"label","labinch","vbmarg","Inch","space=5");

   zdialog_add_widget(zd,"label","labtop","vbtop",ZTX("top"));
   zdialog_add_widget(zd,"spin","mtopcm","vbtop","0|10|0.01|0");
   zdialog_add_widget(zd,"spin","mtopin","vbtop","0|4|0.01|0");

   zdialog_add_widget(zd,"label","labbot","vbbottom",ZTX("bottom"));
   zdialog_add_widget(zd,"spin","mbottcm","vbbottom","0|10|0.01|0");
   zdialog_add_widget(zd,"spin","mbottin","vbbottom","0|4|0.01|0");

   zdialog_add_widget(zd,"label","lableft","vbleft",ZTX("left"));
   zdialog_add_widget(zd,"spin","mleftcm","vbleft","0|10|0.01|0");
   zdialog_add_widget(zd,"spin","mleftin","vbleft","0|4|0.01|0");

   zdialog_add_widget(zd,"label","labright","vbright",ZTX("right"));
   zdialog_add_widget(zd,"spin","mrightcm","vbright","0|10|0.01|0");
   zdialog_add_widget(zd,"spin","mrightin","vbright","0|4|0.01|0");

   zdialog_add_widget(zd,"hbox","hbscale","dialog",0,"space=5");
   zdialog_add_widget(zd,"label","labscale","hbscale",ZTX("image scale"),"space=5");
   zdialog_add_widget(zd,"spin","scale","hbscale","5|100|1|100");
   zdialog_add_widget(zd,"label","labpct","hbscale",ZTX("percent"),"space=5");

   zdialog_add_widget(zd,"hbox","hbsize","dialog",0,"space=3");
   zdialog_add_widget(zd,"vbox","vbunit","hbsize",0,"space=5");
   zdialog_add_widget(zd,"vbox","vbwidth","hbsize",0,"space=5");
   zdialog_add_widget(zd,"vbox","vbheight","hbsize",0,"space=5");

   zdialog_add_widget(zd,"label","space","vbunit",ZTX("image"));
   zdialog_add_widget(zd,"label","labcm","vbunit","CM");
   zdialog_add_widget(zd,"label","labinch","vbunit","Inch");

   zdialog_add_widget(zd,"label","labwidth","vbwidth",ZTX("width"));
   zdialog_add_widget(zd,"label","labwcm","vbwidth","xx.x");
   zdialog_add_widget(zd,"label","labwin","vbwidth","xx.x");

   zdialog_add_widget(zd,"label","labheight","vbheight",ZTX("height"));
   zdialog_add_widget(zd,"label","labhcm","vbheight","xx.x");
   zdialog_add_widget(zd,"label","labhin","vbheight","xx.x");

   zdialog_restore_inputs(zd);                                                   //  recall prior settings

   zdialog_fetch(zd,"mtopcm",margins[0]);
   zdialog_fetch(zd,"mbottcm",margins[1]);
   zdialog_fetch(zd,"mleftcm",margins[2]);
   zdialog_fetch(zd,"mrightcm",margins[3]);
   zdialog_fetch(zd,"scale",imagescale);

   get_printed_image_size();
   zdialog_stuff(zd,"labwcm",pwidth,"%.2f");                                     //  update image size in dialog
   zdialog_stuff(zd,"labhcm",pheight,"%.2f");
   zdialog_stuff(zd,"labwin",pwidth/2.54,"%.2f");
   zdialog_stuff(zd,"labhin",pheight/2.54,"%.2f");

   gtk_page_setup_set_top_margin(pagesetup,10*margins[0],MM);                    //  set page margins
   gtk_page_setup_set_bottom_margin(pagesetup,10*margins[1],MM);                 //  (cm to mm units)
   gtk_page_setup_set_left_margin(pagesetup,10*margins[2],MM);
   gtk_page_setup_set_right_margin(pagesetup,10*margins[3],MM);
   gtk_print_settings_set_scale(printsettings,imagescale);                       //  set image print scale %

   zdialog_run(zd,margins_dialog_event);                                         //  run dialog
   zstat = zdialog_wait(zd);                                                     //  wait for completion
   zdialog_free(zd);                                                             //  kill dialog

   if (zstat == 1) return 0;
   return 1;
}


//  dialog event function
//  save user margin and scale changes
//  recompute print image size

int print_image::margins_dialog_event(zdialog *zd, cchar *event)
{
   using namespace print_image;

   double   temp;

   if (strmatch(event,"mtopcm")) {                                               //  get cm inputs and set inch values
      zdialog_fetch(zd,"mtopcm",margins[0]);
      zdialog_stuff(zd,"mtopin",margins[0]/2.54);
   }

   if (strmatch(event,"mbottcm")) {
      zdialog_fetch(zd,"mbottcm",margins[1]);
      zdialog_stuff(zd,"mbottin",margins[1]/2.54);
   }

   if (strmatch(event,"mleftcm")) {
      zdialog_fetch(zd,"mleftcm",margins[2]);
      zdialog_stuff(zd,"mleftin",margins[2]/2.54);
   }

   if (strmatch(event,"mrightcm")) {
      zdialog_fetch(zd,"mrightcm",margins[3]);
      zdialog_stuff(zd,"mrightin",margins[3]/2.54);
   }

   if (strmatch(event,"mtopin")) {                                               //  get inch inputs and set cm values
      zdialog_fetch(zd,"mtopin",temp);
      margins[0] = temp * 2.54;
      zdialog_stuff(zd,"mtopcm",margins[0]);
   }

   if (strmatch(event,"mbottin")) {
      zdialog_fetch(zd,"mbottin",temp);
      margins[1] = temp * 2.54;
      zdialog_stuff(zd,"mbottcm",margins[1]);
   }

   if (strmatch(event,"mleftin")) {
      zdialog_fetch(zd,"mleftin",temp);
      margins[2] = temp * 2.54;
      zdialog_stuff(zd,"mleftcm",margins[2]);
   }

   if (strmatch(event,"mrightin")) {
      zdialog_fetch(zd,"mrightin",temp);
      margins[3] = temp * 2.54;
      zdialog_stuff(zd,"mrightcm",margins[3]);
   }

   zdialog_fetch(zd,"scale",imagescale);                                         //  get image scale

   get_printed_image_size();
   zdialog_stuff(zd,"labwcm",pwidth,"%.2f");                                     //  update image size in dialog
   zdialog_stuff(zd,"labhcm",pheight,"%.2f");
   zdialog_stuff(zd,"labwin",pwidth/2.54,"%.2f");
   zdialog_stuff(zd,"labhin",pheight/2.54,"%.2f");

   gtk_page_setup_set_top_margin(pagesetup,10*margins[0],MM);                    //  set page margins
   gtk_page_setup_set_bottom_margin(pagesetup,10*margins[1],MM);                 //  (cm to mm units)
   gtk_page_setup_set_left_margin(pagesetup,10*margins[2],MM);
   gtk_page_setup_set_right_margin(pagesetup,10*margins[3],MM);
   gtk_print_settings_set_scale(printsettings,imagescale);                       //  set image print scale %

   return 1;
}


//  compute printed image size based on paper size,
//    orientation, margins, and scale (percent)

void print_image::get_printed_image_size()
{
   using namespace print_image;

   double   iww, ihh, pww, phh, scale;

   pww = 0.1 * gtk_page_setup_get_paper_width(pagesetup,MM);                     //  get paper size
   phh = 0.1 * gtk_page_setup_get_paper_height(pagesetup,MM);                    //  (mm to cm units)

   pww = pww - margins[2] - margins[3];                                          //  reduce for margins
   phh = phh - margins[0] - margins[1];                                          //  bugfix                             6.2

   pww = pww / 2.54 * 300;                                                       //  convert to dots @ 300 dpi
   phh = phh / 2.54 * 300;

   iww = gdk_pixbuf_get_width(pixbuf);                                           //  original image size, pixels
   ihh = gdk_pixbuf_get_height(pixbuf);

   scale = pww / iww;                                                            //  rescale image to fit page
   if (phh / ihh < scale) scale = phh / ihh;

   scale = scale * 0.01 * imagescale;                                            //  adjust for user scale setting

   pwidth = iww * scale / 300 * 2.54;                                            //  dots to cm
   pheight = ihh * scale / 300 * 2.54;

   return;
}


/**************************************************************************/

//  connect a user callback function to a window drag-drop event

void drag_drop_connect(GtkWidget *window, drag_drop_func *ufunc)
{
   int  drag_drop_connect2(GtkWidget *, void *, int, int, void *, int, int, void *);
   char     string[] = "STRING";
   GtkTargetEntry  file_drop_target = { string, 0, 0 };

   zthreadcrash();

   gtk_drag_dest_set(window, GTK_DEST_DEFAULT_ALL, &file_drop_target, 1, GDK_ACTION_COPY);
   G_SIGNAL(window, "drag-data-received", drag_drop_connect2, ufunc);
   gtk_drag_dest_add_uri_targets(window);                                        //  accept URI (file) drop

   return;
}


//  private function
//  get dropped file, clean escapes, pass to user function
//  passed filespec is subject for zfree()

int drag_drop_connect2(GtkWidget *, void *, int mpx, int mpy, void *sdata, int, int, void *ufunc)
{
   char  * drag_drop_unescape(cchar *escaped_string);
   drag_drop_func  *ufunc2;

   char     *text, *text2, *file, *file2;
   int      cc;

   text = (char *) gtk_selection_data_get_data((GtkSelectionData *) sdata);

   ufunc2 = (drag_drop_func *) ufunc;

   if (strstr(text,"file://"))                                                   //  text is a filespec
   {
      file = zstrdup(text+7);                                                    //  get rid of junk added by GTK
      cc = strlen(file);
      while (file[cc-1] < ' ') cc--;
      file[cc] = 0;
      file2 = drag_drop_unescape(file);                                          //  clean %xx escapes from Nautilus
      zfree(file);
      ufunc2(mpx,mpy,file2);                                                     //  pass file to user function
   }

   else
   {
      text2 = zstrdup(text);
      ufunc2(mpx,mpy,text2);
   }

   return 1;
}


//  private function
//  Clean %xx escapes from strange Nautilus drag-drop file names

char * drag_drop_unescape(cchar *inp)
{
   int  drag_drop_convhex(char ch);

   char     inch, *out, *outp;
   int      nib1, nib2;

   out = (char *) zmalloc(strlen(inp)+1);
   outp = out;

   while ((inch = *inp++))
   {
      if (inch == '%')
      {
         nib1 = drag_drop_convhex(*inp++);
         nib2 = drag_drop_convhex(*inp++);
         *outp++ = nib1 << 4 | nib2;
      }
      else *outp++ = inch;
   }

   *outp = 0;
   return out;
}


//  private function - convert character 0-F to number 0-15

int drag_drop_convhex(char ch)
{
   if (ch >= '0' && ch <= '9') return  ch - '0';
   if (ch >= 'A' && ch <= 'F') return  ch - 'A' + 10;
   if (ch >= 'a' && ch <= 'f') return  ch - 'a' + 10;
   return ch;
}


/**************************************************************************
   Miscellaneous GDK/GTK functions
***************************************************************************/

//  Get thumbnail image for given image file.
//  Returned thumbnail belongs to caller: g_object_unref() is necessary.

PIXBUF * get_thumbnail(cchar *fpath, int size)                                   //  5.0
{
   using namespace zfuncs;

   PIXBUF      *thumbpxb;
   GError      *gerror = 0;
   int         err;
   char        *bpath;
   STATB       statf;

   zthreadcrash();                                                               //  thread usage not allowed

   err = stat(fpath,&statf);                                                     //  fpath status info
   if (err) return 0;

   if (S_ISDIR(statf.st_mode)) {                                                 //  if directory, return folder image
      bpath = (char *) zmalloc(500);
      *bpath = 0;                                                                //  5.8
      strncatv(bpath,499,zicondir,"/folder.png",null);
      thumbpxb = gdk_pixbuf_new_from_file_at_size(bpath,size,size,&gerror);
      zfree(bpath);
      return thumbpxb;
   }

   thumbpxb = gdk_pixbuf_new_from_file_at_size(fpath,size,size,&gerror);
   return thumbpxb;                                                              //  return pixbuf to caller
}


//  make a cursor from a graphic file in application's icon directory
//  (see initz_appfiles()).

GdkCursor * zmakecursor(cchar *iconfile)
{
   using namespace zfuncs;

   GError         *gerror = 0;
   PIXBUF         *pixbuf;
   GdkDisplay     *display;
   GdkCursor      *cursor = 0;
   char           iconpath[200];

   zthreadcrash();                                                               //  thread usage not allowed

   display = gdk_display_get_default();
   *iconpath = 0;
   strncatv(iconpath,199,zicondir,"/",iconfile,null);
   pixbuf = gdk_pixbuf_new_from_file(iconpath,&gerror);
   if (pixbuf && display)
      cursor = gdk_cursor_new_from_pixbuf(display,pixbuf,0,0);
   else printz("*** %s \n",gerror->message);
   return cursor;
}


/**************************************************************************

   PIXBUF * gdk_pixbuf_rotate(PIXBUF *pixbuf, float angle, int acolor)

   Rotate a pixbuf through an arbitrary angle (degrees).

   The returned image has the same size as the original, but the
   pixbuf envelope is increased to accomodate the rotated original
   (e.g. a 100x100 pixbuf rotated 45 deg. needs a 142x142 pixbuf).

   Pixels added around the rotated image have all RGB values = acolor.
   Angle is in degrees. Positive direction is clockwise.
   Pixbuf must have 8 bits per channel and 3 or 4 channels.
   Loss of resolution is about 1/2 pixel.
   Speed is about 28 million pixels/sec. on 3.3 GHz CPU.
   (e.g. a 10 megapix image needs about 0.36 seconds)

   NULL is returned if the function fails for one of the following:
      - pixbuf not 8 bits/channel or < 3 channels
      - unable to create output pixbuf (lack of memory?)

   Algorithm:
      create output pixbuf big enough for rotated input pixbuf
      compute coefficients for affine transform
      loop all output pixels (px2,py2)
         get corresp. input pixel (px1,py1) using affine transform
         if outside of pixmap
            output pixel = black
            continue
         for 4 input pixels based at (px0,py0) = (int(px1),int(py1))
            compute overlap (0 to 1) with (px1,py1)
            sum RGB values * overlap
         output aggregate RGB to pixel (px2,py2)

   Benchmark: rotate 7 megapixel image 10 degrees
               0.31 secs.  3.3 GHz Core i5

***/

PIXBUF * gdk_pixbuf_rotate(PIXBUF *pixbuf1, float angle, int acolor)
{
   typedef unsigned char  *pixel;                                                //  3 RGB values, 0-255 each

   PIXBUF      *pixbuf2;
   GDKCOLOR    color;

   int      nch, nbits, alpha;
   int      ww1, hh1, rs1, ww2, hh2, rs2;
   int      px2, py2, px0, py0;
   pixel    ppix1, ppix2, pix0, pix1, pix2, pix3;
   float    px1, py1;
   float    f0, f1, f2, f3, red, green, blue, tran = 0;
   float    a, b, d, e, ww15, hh15, ww25, hh25;
   float    PI = 3.141593;

   zthreadcrash();                                                               //  thread usage not allowed

   nch = gdk_pixbuf_get_n_channels(pixbuf1);
   nbits = gdk_pixbuf_get_bits_per_sample(pixbuf1);
   if (nch < 3) return 0;                                                        //  must have 3+ channels (colors)
   if (nbits != 8) return 0;                                                     //  must be 8 bits per channel

   color = gdk_pixbuf_get_colorspace(pixbuf1);                                   //  get input pixbuf1 attributes
   alpha = gdk_pixbuf_get_has_alpha(pixbuf1);
   ww1 = gdk_pixbuf_get_width(pixbuf1);
   hh1 = gdk_pixbuf_get_height(pixbuf1);
   rs1 = gdk_pixbuf_get_rowstride(pixbuf1);

   while (angle < -180) angle += 360;                                            //  normalize, -180 to +180
   while (angle > 180) angle -= 360;
   angle = angle * PI / 180;                                                     //  radians, -PI to +PI

   if (fabsf(angle) < 0.001) {                                                   //  bugfix 0.01 >> 0.001
      pixbuf2 = gdk_pixbuf_copy(pixbuf1);                                        //  angle is zero within my precision
      return pixbuf2;
   }

   ww2 = int(ww1*fabsf(cosf(angle)) + hh1*fabsf(sinf(angle)));                   //  rectangle containing rotated image
   hh2 = int(ww1*fabsf(sinf(angle)) + hh1*fabsf(cosf(angle)));

   pixbuf2 = gdk_pixbuf_new(color,alpha,nbits,ww2,hh2);                          //  create output pixbuf2
   if (! pixbuf2) return 0;
   rs2 = gdk_pixbuf_get_rowstride(pixbuf2);

   ppix1 = gdk_pixbuf_get_pixels(pixbuf1);                                       //  input pixel array
   ppix2 = gdk_pixbuf_get_pixels(pixbuf2);                                       //  output pixel array

   ww15 = 0.5 * ww1;
   hh15 = 0.5 * hh1;
   ww25 = 0.5 * ww2;
   hh25 = 0.5 * hh2;

   a = cosf(angle);                                                              //  affine transform coefficients
   b = sinf(angle);
   d = - sinf(angle);
   e = cosf(angle);

   for (py2 = 0; py2 < hh2; py2++)                                               //  loop through output pixels
   for (px2 = 0; px2 < ww2; px2++)
   {
      px1 = a * (px2 - ww25) + b * (py2 - hh25) + ww15;                          //  (px1,py1) = corresponding
      py1 = d * (px2 - ww25) + e * (py2 - hh25) + hh15;                          //    point within input pixels

      px0 = int(px1);                                                            //  pixel containing (px1,py1)
      py0 = int(py1);

      if (px1 < 0 || px0 >= ww1-1 || py1 < 0 || py0 >= hh1-1) {                  //  if outside input pixel array
         pix2 = ppix2 + py2 * rs2 + px2 * nch;                                   //    output is acolor
         pix2[0] = pix2[1] = pix2[2] = acolor;
         continue;
      }

      pix0 = ppix1 + py0 * rs1 + px0 * nch;                                      //  4 input pixels based at (px0,py0)
      pix1 = pix0 + rs1;
      pix2 = pix0 + nch;
      pix3 = pix0 + rs1 + nch;

      f0 = (px0+1 - px1) * (py0+1 - py1);                                        //  overlap of (px1,py1)
      f1 = (px0+1 - px1) * (py1 - py0);                                          //    in each of the 4 pixels
      f2 = (px1 - px0) * (py0+1 - py1);
      f3 = (px1 - px0) * (py1 - py0);

      red =   f0 * pix0[0] + f1 * pix1[0] + f2 * pix2[0] + f3 * pix3[0];         //  sum the weighted inputs
      green = f0 * pix0[1] + f1 * pix1[1] + f2 * pix2[1] + f3 * pix3[1];
      blue =  f0 * pix0[2] + f1 * pix1[2] + f2 * pix2[2] + f3 * pix3[2];
      if (alpha)
       tran = f0 * pix0[3] + f1 * pix1[3] + f2 * pix2[3] + f3 * pix3[3];         //  4th color = alpha

      if (red == acolor && green == acolor && blue == acolor) {                  //  avoid acolor in image
         if (blue == 0) blue = 1;
         else blue--;
      }

      pix2 = ppix2 + py2 * rs2 + px2 * nch;                                      //  output pixel
      pix2[0] = int(red);
      pix2[1] = int(green);
      pix2[2] = int(blue);
      if (alpha) pix2[3] = int(tran);                                            //  bugfix
   }

   return pixbuf2;
}


/**************************************************************************/

//  strip the alpha channel from a pixbuf
//  returns 0 if no alpha channel or fatal error

PIXBUF * gdk_pixbuf_stripalpha(const PIXBUF *pixbuf1)                                  //  6.2
{
   PIXBUF      *pixbuf2;
   GDKCOLOR    color;
   int         ww, hh, rs1, rs2;
   uint8       *ppix1, *ppix2, *pix1, *pix2;
   int         nch, ac;
   int         px, py;

   zthreadcrash();                                                               //  thread usage not allowed

   ac = gdk_pixbuf_get_has_alpha(pixbuf1);
   if (! ac) return 0;
   nch = gdk_pixbuf_get_n_channels(pixbuf1);
   color = gdk_pixbuf_get_colorspace(pixbuf1);
   ww = gdk_pixbuf_get_width(pixbuf1);
   hh = gdk_pixbuf_get_height(pixbuf1);
   
   pixbuf2 = gdk_pixbuf_new(color,0,8,ww,hh);                                    //  create output pixbuf2
   if (! pixbuf2) return 0;

   ppix1 = gdk_pixbuf_get_pixels(pixbuf1);                                       //  input pixel array
   ppix2 = gdk_pixbuf_get_pixels(pixbuf2);                                       //  output pixel array
   rs1 = gdk_pixbuf_get_rowstride(pixbuf1);
   rs2 = gdk_pixbuf_get_rowstride(pixbuf2);
   
   for (py = 0; py < hh; py++)
   {
      pix1 = ppix1 + py * rs1;
      pix2 = ppix2 + py * rs2;

      for (px = 0; px < ww; px++)
      {
         memcpy(pix2,pix1,nch-1);
         pix1 += nch;
         pix2 += nch-1;
      }
   }
   
   return pixbuf2;
}


/**************************************************************************/

//  Create a pixbuf containing text with designated font and attributes.
//  Text is white on black. Widget is ultimate display destination.                                               6.2

PIXBUF * text_pixbuf(cchar *text, cchar *font, int fontsize, GtkWidget *widget)
{
   char                    font2[60];
   PangoFontDescription    *pfont;
   PangoLayout             *playout;
   cairo_surface_t         *surface;
   cairo_t                 *cr;
   PIXBUF                  *pixbuf;
   uint8                   *pixels, *cairo_data, *cpix, *pix2;
   int                     ww, hh, rs, px, py;

   snprintf(font2,60,"%s %d",font,fontsize);                                     //  combine font and size

   pfont = pango_font_description_from_string(font2);                            //  make layout with text
   playout = gtk_widget_create_pango_layout(widget,text);
   pango_layout_set_font_description(playout,pfont);
   
   pango_layout_get_pixel_size(playout,&ww,&hh);
   ww += 2 + 0.2 * fontsize;                                                     //  compensate bad font metrics
   hh += 2 + 0.1 * fontsize;

   surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,ww,hh);               //  cairo output image
   cr = cairo_create(surface);
   pango_cairo_show_layout(cr,playout);                                          //  write text layout to image

   cairo_data = cairo_image_surface_get_data(surface);                           //  get text image pixels
   
   pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,0,8,ww,hh);
   rs = gdk_pixbuf_get_rowstride(pixbuf);
   pixels = gdk_pixbuf_get_pixels(pixbuf);

   for (py = 0; py < hh; py++)                                                   //  copy text image to PXB
   for (px = 0; px < ww; px++)
   {
      cpix = cairo_data + 4 * (ww * py + px);
      pix2 = pixels + py * rs + px * 3;
      pix2[0] = pix2[1] = pix2[2] = cpix[3];
   }

   pango_font_description_free(pfont);                                           //  free resources
   g_object_unref(playout);
   cairo_destroy(cr);
   cairo_surface_destroy(surface);

   return pixbuf;
}
