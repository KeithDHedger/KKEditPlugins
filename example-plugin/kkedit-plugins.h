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

#define GLOBALPLUGS			1
#define LOCALPLUGS			0

#define TOOL_ASYNC			8
#define TOOL_IGNORE_OP		0
#define TOOL_PASTE_OP		1
#define TOOL_REPLACE_OP		2
#define TOOL_SHOW_DOC		4
#define	TOOL_INSERT_MASK	0b10111
#define	TOOL_VIEW_OP 		16
#define USEFILE				-2
#define USEURI				-1


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
	GList*				userDataList;
	gpointer			reserved1;
	gpointer			reserved2;
	gpointer			reserved3;
	gpointer			reserved4;
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
//	GList*			plugins;
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
	GtkWidget*		mainWindowHPane;
//right
	GtkWidget*		rightUserBox;
	GtkWidget*		secondWindowHPane;
//bottom
	GtkWidget*		bottomUserBox;
//kkedit main window
	GtkWidget*		mainWindow;
//tool output window buffer;
	GtkTextBuffer*	toolOutBuffer;
//tool output window
	GtkWidget*		toolOutWindow;
//tab popup menu
	GtkWidget*		tabPopUpMenu;
//right click popup menu
	GtkWidget*		contextPopUpMenu;

//leftright user box visiblity ref
	int				leftShow;
	int				rightShow;
};

void		showDocView(int howtodisplay,char* text,const char* title);
pageStruct*	getPageStructPtr(int pagenum);
void		showToolOutput(bool immediate);
void		hideToolOutput(bool immediate);
void		showSide(bool left);
void		hideSide(bool left);
void		runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot,char* title);
void		debugFree(gpointer ptr,const char* message);
bool		openFile(const gchar *filepath,int linenumber,bool warn);
bool		saveFile(GtkWidget* widget,gpointer data);
void		newFile(GtkWidget* widget,gpointer data);

#endif