/*
 *
 * K.D.Hedger 2014 <kdhedger68713@gmail.com>
 *
 * plugins.h
 *
*/


#ifndef _PLUGINS_
#define _PLUGINS_

#include <gtksourceview/gtksourceview.h>

#ifndef _PAGESTRUCT_
#define _PAGESTRUCT_
struct pageStruct
{
	GtkWidget*			pane;
	GtkScrolledWindow*	pageWindow;
	GtkScrolledWindow*	pageWindow2;
	GtkSourceBuffer*	buffer;
	GtkSourceView*		view;
	GtkSourceView*		view2;
	char*				filePath;
	char*				realFilePath;
	char*				dirName;
	GtkMenuItem*		navSubMenu;
	bool				rebuildMenu;
	GtkWidget*			tabName;
	GtkTextIter			iter;
	GtkTextIter			match_start;
	GtkTextIter			match_end;
	bool				isFirst;
	char*				fileName;
	GFile*				gFile; 
	GFileMonitor*		monitor;
	bool				itsMe;
	GList*				markList;
	bool				inTop;
	bool				isSplit;
	const char*			lang;
	GtkWidget*			tabVbox;
	bool				showingChanged;
	GtkTextMark*		backMark;
	GtkTextTag*			highlightTag;
};
#endif

#ifndef _PLUGINDATA_
#define _PLUGINDATA_
struct moduleData
{
	char*		name;
	bool		enabled;
	GModule*	module;
	bool		loaded;
	char*		path;
	bool		unload;
};
#endif

//plugins
struct plugMenuList
{
	GtkWidget*		menuBar;
	GtkWidget*		menuFile;
	GtkWidget*		menuEdit;
	GtkWidget*		menuFunc;
	GtkWidget*		menuNav;
	GtkWidget*		menuTools;
	GtkWidget*		menuHelp;
	GtkWidget*		menuBookMark;
	GtkWidget*		menuView;
};

struct plugData
{
//menus
	plugMenuList	mlist;
	moduleData*		modData;
//install location of KKEdit data
	const char*		dataDir;
//global plugin folder defaults to DATADIR/plugins
	char*			gPlugFolder;
//local plugin folder defaults to ~/.KKEdit/plugins
	char*			lPlugFolder;
//unused
	char*			htmlFile;
//location of variable that holds the uri to be disp[layed by showDoc
	char**			thePage;
//current page MAYBE NULL!!
	pageStruct*		page;
//cuurent tab
	int				currentTab;
//KKEdits temp folder
	char*			tmpFolder;
//kkedits main window user box's
//top
	GtkWidget*		topUserBox;
//left
	GtkWidget*		leftUserBox;
//right
	GtkWidget*		rightUserBox;
//bottom
	GtkWidget*		bottomUserBox;
//kkedit main window
	GtkWidget*		mainWindow;
//tool output window buffer;
	GtkTextBuffer*	toolOutBuffer;
//tool output window
	GtkWidget*		toolOutWindow;
};

#define USEFILE			-2
#define USEURI			-1

void		showDocView(int howtodisplay,char* text,const char* title);
pageStruct*	getPageStructPtr(int pagenum);
void		showToolOutput(bool immediate);
void		hideToolOutput(bool immediate);

#endif