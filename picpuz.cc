/**************************************************************************
   picpuz         computer graphics jigsaw puzzle

   Copyright 2006-2016 Michael Cornelison
   source URL:  kornelix.net
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

#include "zfuncs.h"
#include <gtk/gtk.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

#define gtitle "Picpuz v.2.7"                                                    //  version
#define Tindex(row,col) row * Ncols + col                                        //  map row/col to linear index
#define drand(seed,range) (drandz(&seed) * range)                                //  random double, 0.0 to 0.9999 * range
#define lrand(seed,range) (lrandz(&seed) % range)                                //  random integer, 0 to range-1
#define interp GDK_INTERP_BILINEAR
#define GDKRGB GDK_COLORSPACE_RGB

GtkWidget      *win1, *vbox1, *dwin1;                                            //  main and drawing window
GtkWidget      *win2, *dwin2;                                                    //  reference and drawing window
GtkWidget      *stbar;
PIXBUF         *iPixbuf;                                                         //  full size image pixbuff
PIXBUF         *wPixbuf;                                                         //  image pixbuf for window
cairo_t        *mwcr;                                                            //  main window cairo context

#define MWIN GTK_WINDOW(win1)

timeval     rtime;
int64       rseed;                                                               //  random number seed
int         debug = 0;                                                           //  from command line: -d

string        imagedirk = "";                                                //  image directory
string        clfile = "";                                                   //  command line file
string        imagefile = "";                                                      //  image file pathname
string        pname;                                                          //  puzzle name
int         winW = 900, winH = 600;                                              //  window size
int         imageW, imageH;                                                      //  image size
int         tileU = 80;                                                          //  tile size, user setpoint
int         tileW, tileH;                                                        //  actual tile size used
int         Nrows, Ncols;                                                        //  puzzle tile rows, cols
int         Ntiles = 0;                                                          //  puzzle tile count
int         Nmoves = 0;                                                          //  tiles moved count
int         Nhome = 0;                                                           //  tiles at home position
int         Mstate = 0;                                                          //  mouse, tile select state
int         linecolor = 0;                                                       //  0/1/2/3 = white/black/red/green

struct tileposn_t {
   int      row, col;                                                            //  map tile position
};
tileposn_t   *wposn = 0;                                                         //  window position of home tile
tileposn_t   *hposn = 0;                                                         //  home position of window tile

void m_open(const char *file);                                                         //  open image for new puzzle
void m_tile();                                                                   //  set new tile size
void m_mix();                                                                    //  mix-up pizzle tiles
void m_show();                                                                   //  show reference image
void m_save();                                                                   //  save puzzle for later
void m_resume();                                                                 //  resume saved puzzle
void m_doN(int N);                                                               //  move tiles home
void m_line();                                                                   //  change tile border lines
void m_quit();                                                                   //  exit application
void m_help();                                                                   //  display help file

int  gtkinitfunc(void *data);
void winpaint(GtkWidget *, cairo_t *);                                           //  window paint function
void menufunc(GtkWidget *, const char *menu);                                    //  menu processor
void destroyfunc();                                                              //  window destroy signal function
int  puzzle_status();                                                            //  test puzzle status
void drag_drop(int x, int y, const char *file);                                        //  drag/drop event handler   v.2.4
void init_puzzle(int init);                                                      //  initialize puzzle
void clear_puzzle();                                                             //  release memory, set no puzzle
void tile_window(int init);                                                      //  paint tiles to window
void mouse_event(GtkWidget *, GdkEventButton *);                                 //  handle mouse clicks
void swap_tiles(int row1, int col1, int row2, int col2);                         //  swap two tiles
void draw_tile(int row, int col);                                                //  draw tile at window position
void stbar_update();                                                             //  update status bar
void save_imagedirk();                                                           //  save image directory on exit
void load_imagedirk();                                                           //  reload upon next startup


//  main program

int main(int argc, char *argv[])
{
   int         ii;
   GtkWidget   *tbar;
   string        lang;

   gtk_init(&argc, &argv);                                                       //  GTK command line options

   zinitapp("picpuz");                                                           //  set up app directories

   for (ii = 1; ii < argc; ii++)                                                 //  command line args
   {
      if (strmatch(argv[ii],"-d"))                                               //  -d (debug flag)
            debug = 1;
      else if (strmatch(argv[ii],"-l") && argc > ii+1)                           //  -l language code
            lang = argv[++ii];
      else if (strmatch(argv[ii],"-i") && argc > ii+1)                           //  -i imageDirectory
            imagedirk = argv[++ii];
      else if (strmatch(argv[ii],"-f") && argc > ii+1)                           //  -f imageFile
            clfile = argv[++ii];
      else clfile = argv[ii];                                              //  assume imageFile
   }

   ZTXinit(lang.c_str());                                                                //  setup translations   v.1.9

   win1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);                                   //  create main window
   gtk_window_set_title(GTK_WINDOW(win1),gtitle);
   gtk_window_set_position(GTK_WINDOW(win1),GTK_WIN_POS_CENTER);
   gtk_window_set_default_size(GTK_WINDOW(win1),winW,winH);
   gtk_window_maximize(GTK_WINDOW(win1));

   vbox1 = gtk_box_new(VERTICAL,0);                                              //  add vertical packing box
   gtk_container_add(GTK_CONTAINER(win1),vbox1);

   tbar = create_toolbar(vbox1,32);                                              //  add toolbar and buttons
   add_toolbar_button(tbar,ZTX("open"),ZTX("open new image file"),"open.png",menufunc);
   add_toolbar_button(tbar,ZTX("tile"),ZTX("set tile size"),"tile.png",menufunc);
   add_toolbar_button(tbar,ZTX("mix"),ZTX("randomize tiles"),"refresh.png",menufunc);
   add_toolbar_button(tbar,ZTX("show"),ZTX("show image file"),"show.png",menufunc);
   add_toolbar_button(tbar,ZTX("toolbar::save"),ZTX("save puzzle to a file"),"save.png",menufunc);
   add_toolbar_button(tbar,ZTX("resume"),ZTX("resume saved puzzle"),"open.png",menufunc);
   add_toolbar_button(tbar,ZTX("do 1"),ZTX("move one tile home"),"piece.png",menufunc);
   add_toolbar_button(tbar,ZTX("do 8"),ZTX("move eight tiles home"),"piece.png",menufunc);
   add_toolbar_button(tbar,ZTX("line"),ZTX("change tile border line"),"line.png",menufunc);
   add_toolbar_button(tbar,ZTX("quit"),ZTX("quit picpuz"),"quit.png",menufunc);
   add_toolbar_button(tbar,ZTX("help"),ZTX("view help document"),"help.png",menufunc);

   dwin1 = gtk_drawing_area_new();                                               //  add drawing window
   gtk_box_pack_start(GTK_BOX(vbox1),dwin1,1,1,0);

   stbar = create_stbar(vbox1);                                                  //  add status bar

   gtk_widget_add_events(dwin1,GDK_BUTTON_PRESS_MASK);                           //  connect signals
   gtk_widget_add_events(dwin1,GDK_BUTTON_RELEASE_MASK);
   G_SIGNAL(dwin1,"button-press-event",mouse_event,0);
   G_SIGNAL(dwin1,"button-release-event",mouse_event,0);
   G_SIGNAL(dwin1,"draw",winpaint,0);                                            //  gtk3
   G_SIGNAL(win1,"destroy",destroyfunc,0);

   drag_drop_connect(dwin1,drag_drop);                                           //  connect drag-drop event      v.2.4

   gtk_widget_show_all(win1);                                                    //  show all widgets
   get_hardware_info();                                                          //  2.7

   g_timeout_add(0,gtkinitfunc,0);                                               //  setup initz. call         gtk3
   gtk_main();                                                                   //  process window events
   return 0;
}


//  initial function called from gtk_main() at startup
//  (must be done after gtk_main() loop has started)

int gtkinitfunc(void *)
{
   gettimeofday(&rtime,0);                                                       //  random number seed
   rseed = rtime.tv_sec;

   if (imagedirk.length() == 0) load_imagedirk();                                           //  get image directory   v.1.8

   if (clfile.length()) {                                                                //  command line image file
      string p;
      if (clfile[0] != '/') {
		 p = string(imagedirk) + "/" + clfile;
      }
      else {
		  p = clfile;                                                 //  use clfile
	  } 

      m_open(p.c_str());                                                                //  open command line file
   }

   return 0;
}


//  paint window when created, exposed, or resized

void winpaint(GtkWidget *, cairo_t *cr)
{
   mwcr = cr;                                                                    //  gtk3
   cairo_set_line_width(mwcr,1);
   if (Ntiles) tile_window(0);                                                   //  paint tiles
   mwcr = 0;
   return;
}


//  process toolbar button selection events

void menufunc(GtkWidget *, const char *menu)
{
   if (strmatch(menu,ZTX("open"))) m_open(0);
   if (strmatch(menu,ZTX("tile"))) m_tile();
   if (strmatch(menu,ZTX("mix"))) m_mix();
   if (strmatch(menu,ZTX("show"))) m_show();
   if (strmatch(menu,ZTX("toolbar::save"))) m_save();
   if (strmatch(menu,ZTX("resume"))) m_resume();
   if (strmatch(menu,ZTX("do 1"))) m_doN(1);
   if (strmatch(menu,ZTX("do 8"))) m_doN(8);
   if (strmatch(menu,ZTX("line"))) m_line();
   if (strmatch(menu,ZTX("quit"))) m_quit();
   if (strmatch(menu,ZTX("help"))) m_help();
}


//  create a new puzzle from an image file

void m_open(const char *file)
{
   std::string newfile;

   if (file && strmatch(file,"F1")) return;                                      //  context help     v.2.1

   if (puzzle_status()) return;                                                  //  do not discard
   clear_puzzle();

   if (file) {
	   newfile = file;
   }
   else {
      newfile = zgetfile(ZTX("select image file"),MWIN,"file",imagedirk.c_str());
      if (newfile.empty()) return;
   }

   imagefile = newfile;

   imagedirk = imagefile;                                                  //  set new image directory
   std::size_t found = imagedirk.find_last_of("/");
   if (found != string::npos) {
	   imagedirk.substr(0, found+1);
   }

   if (win2) gtk_widget_destroy(win2);                                           //  reference window obsolete    v.2.1.3
   init_puzzle(1);                                                               //  initz. main window
   return;
}


//  drag-drap enent. open the dropped file.

void drag_drop(int x, int y, const char *file)                                         //  v.2.4
{
   m_open(file);
   return;
}


//  re-tile puzzle according to user input

void m_tile()
{
   zdialog     *zd;
   int         zstat, nn;

   if (! Ntiles) return;
   if (puzzle_status()) return;                                                  //  do not discard

   zd = zdialog_new(ZTX("change tile size"),win1,"OK",ZTX("cancel"),null);
   zdialog_add_widget(zd,"hbox","hb1","dialog",0,"space=10");
   zdialog_add_widget(zd,"label","lb1","hb1",ZTX("new tile size (pixels)"));
   zdialog_add_widget(zd,"spin","spin","hb1","20|200|1|80");
   zdialog_stuff(zd,"spin",tileU);

   zdialog_run(zd);
   zstat = zdialog_wait(zd);

   zdialog_fetch(zd,"spin",nn);
   zdialog_free(zd);

   if (zstat != 1) return;                                                       //  cancel
   if (nn == tileU) return;                                                      //  no change

   tileU = nn;                                                                   //  new setpoint tile size
   tile_window(1);                                                               //  initialize puzzle

   return;
}


//  mix-up the tiles

void m_mix()
{
   int      row1, col1, row2, col2;

   if (puzzle_status()) return;                                                  //  do not discard

   for (row1 = 0; row1 < Nrows; row1++)                                          //  randomize tile positions
   for (col1 = 0; col1 < Ncols; col1++)
   {
      row2 = lrand(rseed,Nrows);
      col2 = lrand(rseed,Ncols);
      swap_tiles(row1,col1,row2,col2);
      zsleep(0.01);
      zmainloop();
   }

   stbar_update();                                                               //  update status bar
   return;
}


//  display a reference image in a new window

void m_show()
{
   void win2_paint(GtkWidget *, cairo_t *);
   void win2_destroy();

   if (! iPixbuf) return;                                                        //  no image

   if (win2) {
      gtk_widget_queue_draw(dwin2);
      return;
   }

   win2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);                                   //  create window
   gtk_window_set_position(GTK_WINDOW(win2),GTK_WIN_POS_CENTER);
   gtk_window_set_default_size(GTK_WINDOW(win2),400,300);

   dwin2 = gtk_drawing_area_new();                                               //  add drawing window
   gtk_container_add(GTK_CONTAINER(win2),dwin2);

   G_SIGNAL(win2,"destroy",win2_destroy,0);                                      //  connect events
   G_SIGNAL(dwin2,"draw",win2_paint,0);

   gtk_widget_show_all(win2);                                                    //  show all widgets
   return;
}

void win2_paint(GtkWidget *, cairo_t *cr)
{
   PIXBUF      *refPixbuf;
   int         x, y, winx, winy;

   if (! iPixbuf) return;

   gtk_window_set_title(GTK_WINDOW(win2),pname.c_str());

   winx = gtk_widget_get_allocated_width(dwin2);                                 //  curr. window size
   winy = gtk_widget_get_allocated_height(dwin2);

   x = int(1.0 * winy * imageW / imageH);                                        //  preserve X/Y ratio
   if (winx > x) winx = x;
   y = int(1.0 * winx * imageH / imageW);
   if (winy > y) winy = y;

   refPixbuf = gdk_pixbuf_scale_simple(iPixbuf,winx,winy,interp);                //  scale image to window
   gdk_cairo_set_source_pixbuf(cr,refPixbuf,0,0);                                //  gtk3
   cairo_paint(cr);
   g_object_unref(refPixbuf);

   return;
}

void win2_destroy()
{
   win2 = 0;
   return;
}


//  save puzzle to a file for later resumption

void m_save()
{
   FILE           *fid;
   std::string    sfile;
   int            ii, row, col;
   string           savefile;                                                //  saved puzzle file name

   if (! Ntiles) return;
                                                     //  /home/user/.local/share/picpuz/pname.puz
   savefile = string(get_zuserdir())+"/"+pname+".puz";

   sfile = zgetfile(ZTX("save puzzle to a file"),MWIN,"save",savefile.c_str());
   if (sfile.empty()) return;

   fid = fopen(sfile.c_str(), "w");
   if (! fid) {
      zmessageACK(win1,ZTX("cannot open: %s"),sfile.c_str());
      return;
   }

   fprintf(fid,"%s \n",imagefile.c_str());                                               //  save image file
   fprintf(fid," %d %d \n",Ntiles,Nhome);                                        //  save tile count and no. home
   fprintf(fid," %d %d \n",Nrows,Ncols);                                         //  save row and col counts

   for (row = 0; row < Nrows; row++) {                                           //  save tile position data
      for (col = 0; col < Ncols; col++) {
         ii = Tindex(row,col);
         fprintf(fid," %d,%d ",wposn[ii].row, wposn[ii].col);
      }
      fprintf(fid,"\n");
   }

   fclose(fid);

   Nmoves = 0;                                                                   //  reset move count
   return;
}


//  resume a saved puzzle

void m_resume()
{
   FILE        *fid;
   int         stat, row1, col1, row2, col2, ii;
   string      newfile;
   char        *pp;

   if (puzzle_status()) return;                                                  //  do not discard
   clear_puzzle();

   newfile = zgetfile(ZTX("load puzzle from file"),MWIN,"file",get_zuserdir());
   if (newfile.empty()) return;

   fid = fopen(newfile.c_str(),"r");
   if (! fid) {
      zmessageACK(win1,ZTX("cannot open: %s"),newfile.c_str());
      return;
   }

   newfile = (char *) malloc(XFCC);
   newfile.resize(XFCC);
   pp = fgets_trim(&newfile[0],newfile.length(), fid, 1);                                        //  read image file name
   if (! pp) goto badfile;

   imagefile = newfile;

   stat = fscanf(fid," %d %d ",&Ntiles,&Nhome);
   if (stat != 2) goto badfile;

   stat = fscanf(fid," %d %d ",&Nrows,&Ncols);                                   //  read row and col counts
   if (stat != 2) goto badfile;
   if (Ntiles != Nrows * Ncols) goto badfile;

   if (wposn) delete [] wposn;
   wposn = new tileposn_t[Ntiles];

   for (row1 = 0; row1 < Nrows; row1++) {                                        //  read tile position data
      for (col1 = 0; col1 < Ncols; col1++) {
         stat = fscanf(fid," %d,%d ",&row2,&col2);
         if (stat != 2) goto badfile;
         if (row2 < 0 || row2 >= Nrows) goto badfile;
         if (col2 < 0 || col2 >= Ncols) goto badfile;
         ii = Tindex(row1,col1);
         wposn[ii].row = row2;
         wposn[ii].col = col2;
      }
   }

   fclose(fid);

   init_puzzle(0);                                                               //  initialize, preserve tile data
   return;

badfile:
   fclose(fid);
   zmessageACK(win1,ZTX("saved puzzle file is not valid"));
   clear_puzzle();
   return;
}


//  move tiles into place automatically

void m_doN(int nn1)
{
   int      row1, col1, row2, col2, ii, nn2;

   if (! Ntiles) return;

   while (true)
   {
      row1 = lrand(rseed,Nrows);                                                 //  random starting tile
      col1 = lrand(rseed,Ncols);
      nn2 = Ntiles;

      while (true)                                                               //  scan for tile to move
      {
         ii = Tindex(row1,col1);
         row2 = wposn[ii].row;                                                   //  curr. position
         col2 = wposn[ii].col;
         if (row1 != row2 || col1 != col2) break;                                //  not at home position
         if (--nn2 == 0) goto mdone;
         if (++col1 < Ncols) continue;                                           //  look at next tile
         col1 = 0;
         if (++row1 < Nrows) continue;
         row1 = 0;
      }

      swap_tiles(row1,col1,row2,col2);                                           //  move tile home
      zsleep(0.02);
      zmainloop();
      if (--nn1 == 0) goto mdone;
   }

mdone:
   stbar_update();                                                               //  update status bar
   return;
}


//  chage tile borderline color                                                  //  gtk3

void m_line()
{
   linecolor++;
   if (linecolor > 3) linecolor = 0;
   gtk_widget_queue_draw(dwin1);
   return;
}


//  quit function with last chance to save unfinished puzzle

void m_quit()
{
   if (puzzle_status()) return;                                                  //  do not discard
   save_imagedirk();                                                             //  for next session  v.1.8
   gtk_main_quit();
   return;
}


//  signal function for window destroy signal

void destroyfunc()
{
   if (puzzle_status()) m_save();                                                //  do not discard
   gtk_main_quit();
   return;
}


//  test puzzle before discarding, give user a chance to save
//  returns  0 = completed, unchanged, or user says discard
//           1 = incomplete, modified, and user says keep

int puzzle_status()
{
   if (! Ntiles) return 0;                                                       //  no puzzle
   if (! Nmoves) return 0;                                                       //  no new moves
   if (Nhome == Ntiles) return 0;                                                //  puzzle done
   if (zmessageYN(win1,ZTX("DISCARD unfinished puzzle?")))                       //  OK to discard
      return 0;
   return 1;                                                                     //  keep
}


//  thread function to display help file

void m_help()                                                                    //  menu function
{
   showz_userguide();                                                            //  display help in new process
   return;
}


//  initialize puzzle from image file with optional preservation
//  of existing tile data (row and column counts, tile positions)
//  called by new_puzzle() and resume_puzzle()

void init_puzzle(int newp)
{
   GError      *gerror = 0;
   const char        *pp;

   if (imagefile.length() == 0) return;

   pp = strrchr(imagefile.c_str(),'/');                                         //  puzzle name = image file name
   if (! pp++) pp = imagefile.c_str();
   pname = pp;

   if (iPixbuf) g_object_unref(iPixbuf);
   iPixbuf = gdk_pixbuf_new_from_file(imagefile.c_str(),&gerror);                        //  create pixbuf from image file
   if (!iPixbuf) {
      zmessageACK(win1,ZTX("image type not recognized:\n %s"),imagefile.c_str());
      clear_puzzle();
      return;
   }

   imageW = gdk_pixbuf_get_width(iPixbuf);                                       //  get image size
   imageH = gdk_pixbuf_get_height(iPixbuf);

   if (imageW < 300 || imageH < 300) {
      zmessageACK(win1,ZTX("image too small, please select another"));
      clear_puzzle();
      return;
   }

   tile_window(newp);                                                            //  paint tiles on main window

   return;
}


//  release memory and clear all puzzle data

void clear_puzzle()
{
   if (iPixbuf) g_object_unref(iPixbuf);
   iPixbuf = 0;
   Ntiles = Nmoves = Nhome = 0;
   stbar_update();
   return;
}


//  create tile pixmaps and paint tiles on window
//  called by init_puzzle(), retile_puzzle(), winpaint()

void tile_window(int newp)
{
   int         x1, y1, ii, row, col;

   if (! iPixbuf) return;

   winW = gtk_widget_get_allocated_width(dwin1);                                 //  window size
   winH = gtk_widget_get_allocated_height(dwin1);

   winW = winW - 4;                                                              //  to keep margins visible
   winH = winH - 4;

   x1 = int(1.0 * winH * imageW / imageH);                                       //  preserve image X/Y ratio
   if (winW > x1) winW = x1;
   y1 = int(1.0 * winW * imageH / imageW);
   if (winH > y1) winH = y1;

   if (newp)                                                                     //  new puzzle
   {
      Ncols = int(1.0 * winW / tileU + 0.5);                                     //  best fit to user tile size
      Nrows = int(1.0 * winH / tileU + 0.5);
      Ntiles = Nrows * Ncols;
      Nhome = Ntiles;

      if (wposn) delete [] wposn;                                                //  new window position map
      wposn = new tileposn_t[Ntiles];

      for (row = 0; row < Nrows; row++)
      for (col = 0; col < Ncols; col++)
      {
         ii = Tindex(row,col);                                                   //  all window positions = home
         wposn[ii].row = row;
         wposn[ii].col = col;
      }
   }

   if (hposn) delete [] hposn;                                                   //  new home position map
   hposn = new tileposn_t[Ntiles];

   for (row = 0; row < Nrows; row++)                                             //  initialize home position map
   for (col = 0; col < Ncols; col++)                                             //    from window position map
   {
      ii = Tindex(row,col);
      ii = Tindex(wposn[ii].row,wposn[ii].col);
      hposn[ii].row = row;
      hposn[ii].col = col;
   }

   tileW = winW / Ncols;                                                         //  actual tile size to use
   tileH = winH / Nrows;

   winW = Ncols * tileW;                                                         //  synch. window to tile size
   winH = Nrows * tileH;

   if (wPixbuf) g_object_unref(wPixbuf);                                         //  gtk3
   wPixbuf = gdk_pixbuf_scale_simple(iPixbuf,winW,winH,interp);                  //  scale image to window size

   if (mwcr) {
      for (row = 0; row < Nrows; row++)                                          //  draw tile pixmaps on window
      for (col = 0; col < Ncols; col++)
         draw_tile(row,col);
   }
   else {
      mwcr = gdk_cairo_create(gtk_widget_get_window(dwin1));                     //  gtk3
      cairo_set_line_width(mwcr,1);
      for (row = 0; row < Nrows; row++)
      for (col = 0; col < Ncols; col++)
         draw_tile(row,col);
      cairo_destroy(mwcr);
      mwcr = 0;
   }

   Mstate = 0;                                                                   //  no tile selected
   stbar_update();                                                               //  update status bar
   return;
}


//  process mouse events (button down, button up)

void mouse_event(GtkWidget *, GdkEventButton *event)
{
   static int  row1, col1, row2, col2;
   int         button, ii, x, y;

   if (! Ntiles) return;

   button = event->button;                                                       //  1/2/3 = left/middle/right
   x = int(event->x);
   y = int(event->y);

   if (x < 1 || x >= (Ncols * tileW)) goto mret0;                                //  ignore if not within picture
   if (y < 1 || y >= (Nrows * tileH)) goto mret0;

   if (button == 3) {                                                            //  if right button,
      row1 = y / tileH;                                                          //    get tile belonging here
      col1 = x / tileW;
      ii = Tindex(row1,col1);
      row2 = wposn[ii].row;                                                      //  where it is now
      col2 = wposn[ii].col;
      swap_tiles(row1,col1,row2,col2);                                           //  swap
      goto mret0;
   }

   if (Mstate < 2) {                                                             //  1st tile selection
      row1 = y / tileH;
      col1 = x / tileW;
   }

   else {                                                                        //  2nd tile selection
      row2 = y / tileH;
      col2 = x / tileW;
   }

   Mstate++;

   if (Mstate < 4) goto mret;
   swap_tiles(row1,col1,row2,col2);                                              //  last button up

mret0:
   Mstate = 0;

mret:
   stbar_update();                                                               //  update status bar
   return;
}


//  swap two tile positions on window

void swap_tiles(int row1, int col1, int row2, int col2)
{
   void swap2(int, int, int, int);                                               //  private functions
   void swap3(int, int, int, int);

   swap2(row1,col1,row2,col2);                                                   //  swap the two tiles
   swap3(row1,col1,row2,col2);                                                   //  move adjacent tiles home
   swap3(row2,col2,row1,col1);                                                   //  move adjacent tiles home
   return;
}


//  perform the swap of the two tiles only

void swap2(int row1, int col1, int row2, int col2)
{
   #define  SWAP(x,y) { temp = (x); (x) = (y); (y) = temp; }

   int      ii1, ii2, temp;

   if (! Ntiles) return;
   if (row1 == row2 && col1 == col2) return;

   ii1 = Tindex(row1,col1);                                                      //  count tiles moved from home
   ii2 = Tindex(row2,col2);
   if (hposn[ii1].row == row1 && hposn[ii1].col == col1) Nhome--;
   if (hposn[ii2].row == row2 && hposn[ii2].col == col2) Nhome--;

   ii1 = Tindex(row1,col1);                                                      //  swap home positions for window tiles
   ii2 = Tindex(row2,col2);                                                      //    at (row1,col1) and (row2,col2)
   SWAP(hposn[ii1].row,hposn[ii2].row)
   SWAP(hposn[ii1].col,hposn[ii2].col)

   ii1 = Tindex(hposn[ii1].row,hposn[ii1].col);                                  //  swap window positions for home tiles
   ii2 = Tindex(hposn[ii2].row,hposn[ii2].col);                                  //    of these window positions
   SWAP(wposn[ii1].row,wposn[ii2].row)
   SWAP(wposn[ii1].col,wposn[ii2].col)

   mwcr = gdk_cairo_create(gtk_widget_get_window(dwin1));                        //  gtk3
   cairo_set_line_width(mwcr,1);
   draw_tile(row1,col1);                                                         //  draw tiles at new positions
   draw_tile(row2,col2);
   cairo_destroy(mwcr);
   mwcr = 0;

   Nmoves++;                                                                     //  incr. move count

   ii1 = Tindex(row1,col1);                                                      //  count tiles moved to home
   ii2 = Tindex(row2,col2);
   if (hposn[ii1].row == row1 && hposn[ii1].col == col1) Nhome++;
   if (hposn[ii2].row == row2 && hposn[ii2].col == col2) Nhome++;

   return;
}


//  When a tile is moved to its home position, find all adjoining tiles
//  that will also be home if moved in parallel with the first tile.
//  (move home a cluster of fitting tiles all of which are "off by N")

void swap3(int row1, int col1, int row2, int col2)
{
   int      ii1, iix, iiy, jj, kk, drow, dcol, change;
   int      Nstack, Estack;
   int      adjrow[4] = { -1, 0, 0, +1 };
   int      adjcol[4] = { 0, -1, +1, 0 };
   int      rowx, colx, rowy, coly, rowz, colz;

   ii1 = Tindex(row1,col1);
   if (hposn[ii1].row != row1 || hposn[ii1].col != col1)                         //  if tile1 not at home,
      return;                                                                    //    nothing to do

   drow = row2 - row1;
   dcol = col2 - col1;

   vector<int> Rstack(Ntiles);
   vector<int> Cstack(Ntiles);

   Rstack[0] = row1;
   Cstack[0] = col1;
   Nstack = 1;
   Estack = 0;

   while (Estack < Nstack)
   {
      row1 = Rstack[Estack];
      col1 = Cstack[Estack];
      Estack++;

      for (jj = 0; jj < 4; jj++)
      {
         rowx = row1 + adjrow[jj];                                               //  tilex is adjoining a tile
         colx = col1 + adjcol[jj];                                               //    that was moved to home
         if (rowx < 0 || colx < 0) continue;                                     //  skip if out of bounds
         if (rowx == Nrows || colx == Ncols) continue;

         rowy = rowx + drow;                                                     //  corresponding tiley where
         coly = colx + dcol;                                                     //    home tile was moved from
         if (rowy < 0 || coly < 0) continue;                                     //  skip if out of bounds
         if (rowy == Nrows || coly == Ncols) continue;

         iiy = Tindex(rowy,coly);
         rowz = hposn[iiy].row;                                                  //  get home position of tiley
         colz = hposn[iiy].col;
         if (rowz == rowx && colz == colx) {                                     //  tiley home = tilex position?
            for (kk = 0; kk < Nstack; kk++)                                      //  yes, check if tilex in stack
               if (rowx == Rstack[kk] && colx == Cstack[kk]) break;
            if (kk == Nstack) {                                                  //  no
               Rstack[kk] = rowx;                                                //  add tilex to stack
               Cstack[kk] = colx;
               Nstack++;
            }
         }
      }
   }

   change = 1;
   while (change)
   {
      change = 0;
      Estack = 0;
      while (Estack < Nstack)
      {
         rowx = Rstack[Estack];                                                  //  next tile position from stack,
         colx = Cstack[Estack];                                                  //    to be filled with home tile
         Estack++;
         iix = Tindex(rowx,colx);
         rowz = hposn[iix].row;                                                  //  home position of tile
         colz = hposn[iix].col;
         if (rowz != rowx || colz != colx) {                                     //  if not already home
            rowz = wposn[iix].row;                                               //  get position of home tile
            colz = wposn[iix].col;
            swap2(rowx,colx,rowz,colz);                                          //  swap
            change++;
         }
      }
   }
   return;
}


//  draw tile at window position (row, col)

void draw_tile(int row, int col)
{
   void  draw_body(int row, int col);
   void  draw_lobe(int row, int col);
   void  draw_hole(int row, int col);

   if (col > 0) {
      draw_body(row,col-1);                                                      //  refresh tile to left
      draw_lobe(row,col-1);
      draw_hole(row,col-1);
   }

   draw_body(row,col);                                                           //  refresh this tile
   draw_lobe(row,col);
   draw_hole(row,col);

   if (col < Ncols-1) draw_lobe(row,col+1);                                      //  refresh lobe from tile to right
   return;
}

//  draw tile body at window position (no lobes or holes)

void draw_body(int row2, int col2)
{
   int         ii, row1, col1;
   int         x1, y1, x2, y2, rs;
   uint8       *ptile;
   PIXBUF      *tPixbuf;                                                         //  tile pixbuf

   ii = Tindex(row2,col2);                                                       //  tile home position
   row1 = hposn[ii].row;
   col1 = hposn[ii].col;

   x1 = col1 * tileW;                                                            //  position in image
   y1 = row1 * tileH;
   x2 = col2 * tileW;                                                            //  position in window
   y2 = row2 * tileH;

   rs = gdk_pixbuf_get_rowstride(wPixbuf);
   ptile = gdk_pixbuf_get_pixels(wPixbuf) + y1 * rs + 3 * x1;                    //  draw tile        gtk3
   tPixbuf = gdk_pixbuf_new_from_data(ptile,GDKRGB,0,8,tileW,tileH,rs,0,0);
   gdk_cairo_set_source_pixbuf(mwcr,tPixbuf,x2,y2);
   cairo_paint(mwcr);
   g_object_unref(tPixbuf);

   if (linecolor == 0) cairo_set_source_rgb(mwcr,1,1,1);                         //  draw tile outline   gtk3
   if (linecolor == 1) cairo_set_source_rgb(mwcr,0,0,0);
   if (linecolor == 2) cairo_set_source_rgb(mwcr,1,0,0);
   if (linecolor == 3) cairo_set_source_rgb(mwcr,0,1,0);

   cairo_rectangle(mwcr,x2,y2,tileW,tileH);
   cairo_stroke(mwcr);

   return;
}

//  draw protruding lobe from left side of puzzle tile at window position

void draw_lobe(int row2, int col2)
{
   int         ii, row1, col1, rs;
   int         x1, y1, x2, y2, px, py, pw, ph;
   int64       seed;
   uint8       *plobe;
   PIXBUF      *lPixbuf;                                                         //  lobe pixbuf

   if (col2 == 0) return;                                                        //  window position on left edge

   ii = Tindex(row2,col2);                                                       //  get home position for tile
   row1 = hposn[ii].row;
   col1 = hposn[ii].col;
   if (col1 == 0) return;                                                        //  home position on left edge

   seed = row1 + col1;
   px = int(0.2 * tileW + 0.5);                                                  //  get lobe position, size
   py = int(drand(seed,0.6) * tileH + 0.1 * tileH);
   pw = int(0.2 * tileW + 0.5);
   ph = int(0.2 * tileH + 0.5);
   x1 = col1 * tileW - px;                                                       //  position in image
   y1 = row1 * tileH + py;
   x2 = col2 * tileW - px;                                                       //  position in window
   y2 = row2 * tileH + py;

   rs = gdk_pixbuf_get_rowstride(wPixbuf);
   plobe = gdk_pixbuf_get_pixels(wPixbuf) + y1 * rs + 3 * x1;                    //  draw lobe        gtk3
   lPixbuf = gdk_pixbuf_new_from_data(plobe,GDKRGB,0,8,pw,ph,rs,0,0);
   gdk_cairo_set_source_pixbuf(mwcr,lPixbuf,x2,y2);
   cairo_paint(mwcr);
   g_object_unref(lPixbuf);

   if (linecolor == 0) cairo_set_source_rgb(mwcr,1,1,1);
   if (linecolor == 1) cairo_set_source_rgb(mwcr,0,0,0);
   if (linecolor == 2) cairo_set_source_rgb(mwcr,1,0,0);
   if (linecolor == 3) cairo_set_source_rgb(mwcr,0,1,0);

   cairo_move_to(mwcr,x2+pw,y2);                                                 //  draw lobe outline    gtk3
   cairo_line_to(mwcr,x2,y2);
   cairo_line_to(mwcr,x2,y2+ph);
   cairo_line_to(mwcr,x2+pw,y2+ph);
   cairo_stroke(mwcr);

   return;
}

//  draw recess hole on right side where lobe from right fits in

void draw_hole(int row2, int col2)
{
   int      ii, row1, col1;
   int      x2, y2, px, py, pw, ph;
   int64    seed;

   ii = Tindex(row2,col2);                                                       //  get home position for tile
   row1 = hposn[ii].row;
   col1 = hposn[ii].col;
   if (++col1 == Ncols) return;                                                  //  tile to the right, unless on edge

   seed = row1 + col1;
   px = int(0.2 * tileW + 0.5);
   py = int(drand(seed,0.6) * tileH + 0.1 * tileH);
   pw = int(0.2 * tileW + 0.5);
   ph = int(0.2 * tileH + 0.5);
   x2 = col2 * tileW + tileW - px;                                               //  hole position in window
   y2 = row2 * tileH + py;

   cairo_set_source_rgb(mwcr,0,0,0);                                             //  draw hole rectangle       gtk3
   cairo_rectangle(mwcr,x2,y2,pw,ph);
   cairo_fill(mwcr);

   return;
}


//  update statistics on status bar

void stbar_update()
{
   char     message[50];

   sprintf(message,ZTX("tiles home: %d/%d"),Nhome,Ntiles);
   if (Mstate > 1) snprintf(message,49,"%s  %s",message,ZTX("1st tile selected"));
   stbar_message(stbar,message);
   return;
}


//  save current image directory upon exit, reload this upon startup             //  v.1.8
//  directory is saved in file $HOME/.picpuz/image_directory

void save_imagedirk()
{
   shell_ack("echo %s > %s/image_directory",imagedirk.c_str(), get_zuserdir());
   return;
}

void load_imagedirk()
{
   int      err;
   FILE     *fid;
   STATB    statdat;
   char     dirbuff[XFCC], *pp;

   pp = getcwd(dirbuff,XFCC-1);                                                //  default is current directory
   imagedirk = dirbuff;
   
   snprintf(dirbuff,XFCC-1,"%s/image_directory",get_zuserdir());                 //  read saved file
   fid = fopen(dirbuff,"r");
   if (! fid) return;
   pp = fgets_trim(dirbuff,XFCC-1,fid,1);
   fclose(fid);
   if (! pp) return;
   err = stat(dirbuff,&statdat);                                                 //  contains a valid directory name?
   if (err) return;
   if (! S_ISDIR(statdat.st_mode)) return;
   imagedirk = dirbuff;                                                    //  yes, use it
   return;
}


//  supply unused zdialog callback function

void KBstate(GdkEventKey *event, int state)
{ return; }



