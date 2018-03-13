# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvAdd, EvInfo, EvExit, Update };
   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void add_ui ();
	void torus_node(float r, float R, int n);
	void add_mynode ( int n );//the one given in lab; making 2 separate functions for modularity
	void add_node(); //my implementation for lab 
	void update_node(float x, float y);
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
	
};
