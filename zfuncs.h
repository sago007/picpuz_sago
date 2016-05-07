/**************************************************************************
   zfuncs.h      include file for zfuncs functions

   Copyright 2006-2016 Michael Cornelison
   source URL:  kornelic.net
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

***************************************************************************/

//     zfuncs.h   version  v.6.2

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <string>
#include <vector>

#define VERTICAL GTK_ORIENTATION_VERTICAL                                        //  GTK shortcuts
#define HORIZONTAL GTK_ORIENTATION_HORIZONTAL
#define PIXBUF GdkPixbuf
#define GDKCOLOR GdkColorspace

#define  int8   char                                                             //  number types
#define  int16  short
#define  int32  int
#define  int64  long long                                                        //  long long is always 64 bits
#define  uint8   unsigned char
#define  uint16  unsigned short
#define  uint32  unsigned int
#define  uint64  unsigned long long
#define  uchar  unsigned char
#define  cchar  const char

#define  wstrerror(err) strerror(WEXITSTATUS(err))                               //  get text status for child process

#define  STATB  struct stat                                                      //  stat() file status buffer

#define  mutex_t        pthread_mutex_t                                          //  abbreviations
#define  mutex_init     pthread_mutex_init
#define  mutex_lock     pthread_mutex_lock
#define  mutex_trylock  pthread_mutex_trylock
#define  mutex_unlock   pthread_mutex_unlock
#define  mutex_destroy  pthread_mutex_destroy

#define  XFCC 1000                                                               //  max. file pathname cc tolerated

//  trace execution: source file, function, line no, caller address

#define TRACE trace(__FILE__,__FUNCTION__,__LINE__,__builtin_return_address(0));

//  system functions ======================================================


                                             //  get disk temp, e.g. "/dev/sda"     v.5.9
void zsleep(double dsecs);                                                       //  sleep specified seconds


int shell_ack(cchar *command, ...);                                              //   ""  + popup an error message if error
char * fgets_trim(char * buff, int maxcc, FILE *, int bf = 0);                   //  fgets + trim trailing \n \r (blanks)


//  string macros and functions ===========================================

#define strmatch(str1,str2) (! strcmp((str1),(str2)))                            //  TRUE if strings equal
#define strmatchN(str1,str2,cc) (! strncmp((str1),(str2),(cc)))                  //  TRUE if strings[cc] equal
#define strmatchcase(str1,str2) (! strcasecmp((str1),(str2)))                    //  TRUE if strings equal, ignoring case
#define strmatchcaseN(str1,str2,cc) (! strncasecmp((str1),(str2),(cc)))          //  TRUE if strings[cc] equal, ignoring case






struct  pvlist {
   int      max;                          //  max. entries
   int      act;                          //  actual entries
   char   **list;                         //  entries
};

//  random number functions ===============================================

int lrandz(int64 * seed);                                                        //  returns 0 to 0x7fffffff
double drandz(int64 * seed);                                                     //  returns 0.0 to 0.99999...

//  spline curve-fitting functions ========================================

void spline1(int nn, float *dx, float *dy);                                      //  define a curve using nn data points
float spline2(float x);                                                          //  return y-value for given x-value

//  application initialization and administration =========================

int zinitapp(cchar *appname);                                                    //  initz. app directories and files   v.4.1
const std::string& get_zprefix();                                                           //  get /usr or /usr/local  ...        v.4.1
cchar * get_zuserdir();                                                          //  get /home/user/.appname/
const std::string& get_zdatadir();                                                          //  get data directory
cchar * get_zdocdir();                                                           //  get document directory
cchar * get_zicondir();                                                          //  get icon directory                 v.5.7
int  locale_filespec(cchar *ftype, cchar *fname, std::string& filespec);                //  get a locale dependent file        v.5.5

void showz_userguide(cchar *context = 0);                                        //  show user guide in new process
void showz_logfile();                                                            //  show log file in popup window      v.5.2
void showz_textfile(cchar *type, cchar *file);                                   //  show text file [.gz] in popup window
void showz_html(cchar *url);                                                     //  show html via preferred browser
void zmake_menu_launcher(cchar *command, cchar *cats, cchar *generic);           //  add desktop menu and launcher      v.4.1

/**************************************************************************/

//  translation functions

#define ZTXmaxent 2000                                                           //  max. translation strings
#define ZTXmaxcc 4000                                                            //  max. cc per string

void ZTXinit(cchar *lang);                                                       //  setup for message translation
cchar * ZTX(cchar *english);                                                     //  get translation for English message
cchar * ZTX_missing(int &ftf);                                                   //  get missing translations, one per call

/**************************************************************************
   GTK utility functions
***************************************************************************/

void zmainloop(int skip = 0);                                                    //  do main loop, process menu events
void zthreadcrash();                                                             //  crash if thread is not main() thread

void get_hardware_info(GtkWidget *widget = 0);                                   //  get display, screen, mouse
int  move_pointer(GtkWidget *, int px, int py);                                  //  move the mouse pointer to px, py

//  text window print and read utilities

void wprintx(GtkWidget *Win, int line, cchar *mess, int bold = 0);               //  write text to line, optional bold  6.2
void wprintf(GtkWidget *Win, int line, cchar *format, ...);                      //  "printf" version
void wprintf(GtkWidget *Win, cchar *format, ... );                               //  "printf" to next line, scroll up
void wscroll(GtkWidget *mLog, int line);                                         //  scroll window to put line on screen
void wclear(GtkWidget *Win);                                                     //  clear window
void wclear(GtkWidget *Win, int line);                                           //  clear from line to end
char * wscanf(GtkWidget *Win, int &ftf);                                         //  get text lines from edit widget
int  wfiledump(GtkWidget *Win, const char *filespec);                                  //  dump text window to file
void wfilesave(GtkWidget *Win, GtkWindow *parent);                               //  wfiledump() via file-chooser dialog
void wprintp(GtkWidget *Win);                                                    //  print text window to default printer

//  intercept text window mouse click functions

typedef void clickfunc_t(GtkWidget *widget, int line, int pos);                  //  function to get clicked text position
void textwidget_set_clickfunc(GtkWidget *widget, clickfunc_t clickfunc);         //  (wrapped lines are one logical line)
char * textwidget_get_line(GtkWidget *widget, int line, int hilite);             //  get entire line at clicked position
char * textwidget_get_word(const char *line, int pos, cchar *dlims, char &end);        //  get delimited word at clicked position

//  functions to simplify building menus, tool bars, status bars

#define G_SIGNAL(window,event,func,arg) \
        g_signal_connect(G_OBJECT(window),event,G_CALLBACK(func),(void *) arg)

#define  zdcbmax 100                                                             //  max. combo box drop-down list

typedef void cbFunc(GtkWidget *, cchar *mname);                                  //  menu or button response function

GtkWidget * create_menubar(GtkWidget *vbox);                                     //  create menubar in packing box
GtkWidget * add_menubar_item(GtkWidget *mbar, cchar *mname, cbFunc func = 0);    //  add menu item to menubar
GtkWidget * add_submenu_item(GtkWidget *mitem, cchar *subname,                   //  add submenu item to menu item
                             cbFunc func = 0, cchar *mtip = 0);                  //    with opt. function and popup tip

GtkWidget * create_toolbar(GtkWidget *vbox, int iconsize = 24);                  //  toolbar in packing box (no vert gtk3)
GtkWidget * add_toolbar_button(GtkWidget *tbar, cchar *lab, cchar *tip,          //  add button with label, tool-tip, icon
                                 cchar *icon, cbFunc func);

GtkWidget * create_stbar(GtkWidget *vbox);                                       //  create status bar in packing box
int stbar_message(GtkWidget *stbar, cchar *message);                             //  display message in status bar

/**************************************************************************/

GtkWidget * create_popmenu();                                                    //  create an empty popup menu
GtkWidget * add_popmenu_item(GtkWidget *popmenu, cchar *mname,                   //  add menu item to popup menu
                           cbFunc func, cchar *arg, cchar *mtip = 0);
void popup_menu(GtkWidget *, GtkWidget *popmenu);                                //  pop-up menu at current mouse posn.

/**************************************************************************/

//  user editable graphic menu in popup window
//  menus can be added and arranged using the mouse

typedef void gmenuz_cbfunc(cchar *menu);                                         //  caller-supplied callback function
void gmenuz(GtkWidget *parent, cchar *title, cchar *ufile, gmenuz_cbfunc);       //  show window, handle mouse drag/click

/**************************************************************************/

//  create vertical menu/toolbar in vertical packing box                         //  v.5.5

struct vmenuent {                                                                //  menu data from caller
   cchar       *name;                                                            //  menu name, text
   cchar       *icon;                                                            //  opt. icon file name
   cchar       *desc;                                                            //  description (mouse hover popup)
   cbFunc      *func;                                                            //  callback func (GtkWidget *, cchar *arg)
   cchar       *arg;                                                             //  callback arg
   PIXBUF      *pixbuf;                                                          //  icon pixbuf or null
   PangoLayout *playout1, *playout2;                                             //  normal and bold menu text
   int         namex, namey;                                                     //  menu name position in layout
   int         iconx, icony;                                                     //  menu icon position
   int         ylo, yhi;                                                         //  menu height limits
   int         iconww, iconhh;                                                   //  icon width and height
};

struct Vmenu {
   GtkWidget   *vbox;                                                            //  parent window
   GtkWidget   *layout;                                                          //  drawing window
   int         xmax, ymax;                                                       //  layout size                     v.5.8
   int         mcount;                                                           //  menu entry count
   vmenuent    menu[100];
};

Vmenu *Vmenu_new(GtkWidget *vbox);                                               //  create new menu in parent vbox
void Vmenu_add(Vmenu *vbm, cchar *name, cchar *icon,                             //  add menu item with response function
               int iconww, int iconhh, cchar *desc,
               cbFunc func, cchar *arg);                                         //  function may be popup_menu()

/**************************************************************************/

//   functions to implement GTK dialogs with less complexity
//   widget types: dialog, hbox, vbox, hsep, vsep, frame, scrwin, label, entry, edit, radio,
//                 check, button, togbutt, spin, combo, comboE, hscale, vscale, colorbutt

#define zdmaxwidgets 300                           //  v.5.8
#define zdmaxbutts 10
#define zdsentinel 0x97530000

struct zwidget  {
      cchar       *type;                           //  dialog, hbox, vbox, label, entry ...
      cchar       *name;                           //  widget name
      cchar       *pname;                          //  parent (container) name
      char        *data;                           //  widget data, initial / returned
      pvlist      *cblist;                         //  combo box drop-down list
      int         size;                            //  text entry cc or image pixel size
      int         homog;                           //  hbox/vbox: equal spacing flag
      int         expand;                          //  widget is expandable flag
      int         space;                           //  extra padding space (pixels)
      int         wrap;                            //  wrap mode for edit widget
      int         stopKB;                          //  stop propagation of KB events to parent
      int         rescale;                         //  widget is rescaled for more resolution                     v.6.0
      float       lval, nval, hval;                //  scale range and neutral value
      GtkWidget   *widget;                         //  GTK widget pointer
};

struct zdialog  {
      int         sentinel1, sentinel2;            //  validity sentinels
      void        *eventCB;                        //  widget event user callback function
      int         zstat;                           //  dialog status (from completion button)
      int         disabled;                        //  widget signals/events are disabled
      int         saveposn;                        //  save and recall window position each use
      int         saveinputs;                      //  save and recall user inputs each use
      int         stopKB;                          //  flag, next KB event will be ignored
      GtkWidget   *parent;                         //  parent window or null
      cchar       *compbutton[zdmaxbutts];         //  dialog completion button labels                            v.5.9
      GtkWidget   *compwidget[zdmaxbutts];         //  dialog completion button widgets
      zwidget     widget[zdmaxwidgets];            //  dialog widgets (EOF = type = 0)
};

zdialog *zdialog_new(cchar *title, GtkWidget *parent, ...);                      //  create a zdialog with opt. buttons
void zdialog_set_title(zdialog *zd, cchar *title);                               //  change zdialog title               v.6.0
void zdialog_set_modal(zdialog *zd);                                             //  set zdialog modal                  v.6.1
void zdialog_set_decorated(zdialog *zd, int decorated);                          //  set zdialog decorated or not       v.6.1
void zdialog_present(zdialog *zd);                                               //  zdialog visible and on top         v.6.1

int zdialog_add_widget(zdialog *zd,                                              //  add widget to zdialog
      cchar *type, cchar *name, cchar *pname,                                    //  required args
      cchar *data = 0, int size = 0, int homog = 0,                              //  optional args
      int expand = 0, int space = 0, int wrap = 0);

int zdialog_add_widget(zdialog *zd,                                              //  add widget to zdialog
      cchar *type, cchar *name, cchar *pname,                                    //  (alternative form)
      cchar *data, cchar *options);                                              //  "size=nn|homog|expand|space=nn|wrap"

int zdialog_valid(zdialog *zd, int errmess = 1);                                 //  return 1/0 if zdialog valid/invalid
GtkWidget * zdialog_widget(zdialog *zd, cchar *name);                            //  GTK widget from zdialog widget name
int zdialog_add_ttip(zdialog *zd, cchar *wname, cchar *ttip);                    //  add popup tool tip to a widget
int zdialog_resize(zdialog *zd, int width, int height);                          //  set size > widget sizes
int zdialog_put_data(zdialog *zd, cchar *name, cchar *data);                     //  put data in widget (entry, spin ...)
cchar * zdialog_get_data(zdialog *zd, cchar *name);                              //  get widget data
int zdialog_set_limits(zdialog *, cchar *name, double min, double max);          //  set new widget limits (spin, scale)
int zdialog_rescale(zdialog *zd, cchar *wname, float, float, float);             //  rescale widget, lo/neut/hi vals    v.6.0

typedef int zdialog_event(zdialog *zd, cchar *name);                             //  widget event callback function
int zdialog_run(zdialog *zd, zdialog_event = 0, cchar *posn = 0);                //  run dialog, handle events
void KBstate(GdkEventKey *event, int state);                                     //  extern: pass KB events to main app

int zdialog_send_event(zdialog *zd, cchar *event);                               //  send an event to an active dialog
int zdialog_send_response(zdialog *zd, int zstat);                               //  complete a dialog, set status
int zdialog_show(zdialog *zd, int flag);                                         //  show or hide a dialog              v.5.2
int zdialog_destroy(zdialog *zd);                                                //  destroy dialog (caller resp.)
int zdialog_free(zdialog *&zd);                                                  //  free zdialog memory
int zdialog_wait(zdialog *zd);                                                   //  wait for dialog completion
int zdialog_goto(zdialog *zd, cchar *name);                                      //  put cursor at named widget
void zdialog_set_cursor(zdialog *zd, GdkCursor *cursor);                         //  set cursor for dialog window

int zdialog_stuff(zdialog *zd, cchar *name, cchar *data);                        //  stuff string data into widget
int zdialog_stuff(zdialog *zd, cchar *name, int  data);                          //  stuff int data
int zdialog_stuff(zdialog *zd, cchar *name, double data);                        //  stuff double data
int zdialog_stuff(zdialog *zd, cchar *name, double data, cchar *format);         //  stuff double data, formatted
int zdialog_labelfont(zdialog *zd, cchar *lab, cchar *font, cchar *txt);         //  stuff label text with font

int zdialog_fetch(zdialog *zd, cchar *name, char *data, int maxcc);              //  get string data from widget
int zdialog_fetch(zdialog *zd, cchar *name, int  &data);                         //  get int data
int zdialog_fetch(zdialog *zd, cchar *name, double &data);                       //  get double data
int zdialog_fetch(zdialog *zd, cchar *name, float &data);                        //  get float data

int zdialog_cb_app(zdialog *zd, cchar *name, cchar *data);                       //  append entry to combo drop down list
int zdialog_cb_prep(zdialog *zd, cchar *name, cchar *data);                      //  prepend entry to combo drop down list
char * zdialog_cb_get(zdialog *zd, cchar *name, int Nth);                        //  get combo drop down list Nth entry
int zdialog_cb_delete(zdialog *zd, cchar *name, cchar *data);                    //  delete combo drop down list entry
int zdialog_cb_clear(zdialog *zd, cchar *name);                                  //  clear all combo box entries
int zdialog_cb_popup(zdialog *zd, cchar *name);                                  //  show all combo box list entries
int zdialog_cb_save(zdialog *zd, cchar *name, cchar *file);                      //  save combo box list to a file
int zdialog_cb_load(zdialog *zd, cchar *name, cchar *file);                      //  load combo box list from a file

int  zdialog_positions(cchar *action);                                           //  load or save zdialog window positions
void zdialog_set_position(zdialog *zd, cchar *posn);                             //  set initial/new zdialog window position
void zdialog_save_position(zdialog *zd);                                         //  save zdialog window position

int zdialog_inputs(cchar *action);                                               //  load or save zdialog input fields
int zdialog_save_inputs(zdialog *zd);                                            //  save zdialog input fields when finished
int zdialog_restore_inputs(zdialog *zd);                                         //  restore zdialog inputs from prior use


//  popup message dialogs

void zmessageACK(GtkWidget *parent, cchar *format, ... );                           //  display message, wait for OK
void zmessLogACK(GtkWidget *parent, cchar *format, ... );                           //  same, with log to STDOUT
int  zmessageYN(GtkWidget *parent, cchar *format, ... );                            //  display message, wait for YES/NO
zdialog * zmessage_post(GtkWidget *parent, int secs, cchar *format, ...);           //  show message, timeout or cancel
char * zdialog_text(GtkWidget *parent, cchar *title, cchar *inittext);              //  get short text input from user
int  zdialog_choose(GtkWidget *parent, cchar *message, ...);                        //  show message, return choice
void poptext_mouse(cchar *text, int dx, int dy, float s1, float s2);                //  show popup text at mouse posn
void poptext_window(cchar *text, GtkWindow *, int mx, int my, float s1, float s2);  //  show popup text at window posn
int  poptext_killnow();                                                             //  kill current popup window
int  popup_image(cchar *imagefile, GtkWindow *parent, int Fnewin, int size);        //  popup window with image

//  file chooser dialogs for one file or multiple files

std::string zgetfile(cchar *title, GtkWindow *parent, cchar *action, cchar *file, int hidden = 0);

//  print an image file, choosing printer, paper, orientation, margins, and scale

void print_image_file(GtkWidget *parent, cchar *imagefile);

//  drag and drop functions

typedef void drag_drop_func(int x, int y, const char *text);                           //  user function, get drag_drop text
void drag_drop_connect(GtkWidget *window, drag_drop_func);                       //  connect window to user function

//  miscellaneous GDK/GTK functions

PIXBUF * get_thumbnail(cchar *fpath, int size);                                  //  get sized thumbnail for image file
GdkCursor * zmakecursor(cchar *iconfile);                                        //  make a cursor from an image file
PIXBUF * gdk_pixbuf_rotate(PIXBUF *, float deg, int alfa = 0);                   //  rotate pixbuf through any angle
PIXBUF * gdk_pixbuf_stripalpha(const PIXBUF *pixbuf);                                  //  strip alpha channel from pixbuf
PIXBUF * text_pixbuf(cchar *text, cchar *font, int fsize, GtkWidget *);          //  create pixbuf with text using font
