
# include "my_viewer.h"
# include "sn_mynode.h"

# include <sigogl/gl_tools.h>
# include <sigogl/ui_button.h>
#include <vector>
#include <iostream>
static std::vector<SnMyNode*> nodes; 
static int numTriangles; 
static float littleR;
static float bigR; 
MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	add_ui ();

	//add_mynode (4); //the code provided for the sample code
	add_node(); 

	littleR = 0.1f; 
	bigR = 0.5f; 
	numTriangles = 10; 

	torus_node(littleR, bigR, numTriangles);
}

void MyViewer::add_ui ()
{
	UiPanel *p;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "Add", EvAdd ) );
	p->add ( new UiButton ( "Info", EvInfo ) );
	p->add ( new UiButton ( "Exit", EvExit ) );
	p->add(new UiButton("Update", Update)); 
	

}

void MyViewer::add_mynode ( int n )
{
	SnMyNode* c;

	float r=0.15f; // position range
	
	while ( n-->0 )
	{	c = new SnMyNode;
		c->init.set(gs_random(-r, r), gs_random(-r, r), gs_random(-r, r));
		c->width = gs_random(0.001f, r);
		c->height = gs_random(0.001f, r * 2);
		c->color(GsColor::random());

		// Example how to print/debug your generated data:
		// gsout<<n<<": "<<c->color()<<gsnl;
		rootg()->add(c);
	}
}

GsVec torusFunction(int phi, int theta, float r, float R) {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float alpha = GS_TORAD(float(phi));
	float beta = GS_TORAD(float(theta));

	x = float(R + r * cosf(alpha))*cosf(beta);
	y = float(R + r * cosf(alpha)) * sinf(beta);
	z = float(r * sinf(alpha));

	return GsVec(x, y, z);
}

void MyViewer::torus_node(float r, float R, int n) {
	rootg()->remove_all();
	int prevPhi = 0;
	int prevTheta = 0;

	int nextPhi = 0;
	int nextTheta = 0;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	SnMyNode *c;





	//c->GsVecArray = new std::vector<std::vector<GsVec>*>(360); 
	for (nextPhi = n; prevPhi <= 360; nextPhi+=n) {
		for (nextTheta = n; nextTheta <= 360; nextTheta+=n) {

			c = new SnMyNode();

			c->A00 = torusFunction(prevPhi, prevTheta, r, R);
			c->A10 = torusFunction(nextPhi, prevTheta, r, R);
			c->A01 = torusFunction(prevPhi, nextTheta, r, R);
			c->A11 = torusFunction(nextPhi, nextTheta, r, R);

			c->color(GsColor::lightblue);

			rootg()->add(c);

			prevTheta = nextTheta; 
		}

		prevPhi = nextPhi;
	}



	//c->init.set(0.2f, 0.2f, 0.2f);



	return;

}

void MyViewer::add_node() { //The function I implemented in lab

	SnMyNode * c; 

	std::vector<char*> name = { "L", "E", "O", "N" }; 

	float r = 0.15f; 
	float x = 0.0f; 
	float y = 0.0f; 
	float z = 0.0f; 
	

	for (int i = 0; i < 4; ++i) {
		c = new SnMyNode;
		c->init.set(x, y, z);
		c->width = r;
		c->height= 2*r;
		c->color(GsColor::black);
		c->shape = name.at(i);

		nodes.push_back(c);

		rootg()->add(nodes.at(i));
		
		x = x + 0.5f; 	
	}

	return; 
}

void MyViewer:: update_node(float x, float y) { //The function I implemented in lab
	
	std::vector<SnMyNode *>::iterator it; 

	for (it = nodes.begin(); it != nodes.end(); ++it) {
		(*it)->init.x = x; 
		(*it)->init.y = y; 

		(*it)->touch();
		
	
		render(); 

		x = x + 0.5f;
	}

	return;
}




int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	switch ( e.key )
	{	case GsEvent::KeyEsc : 
			gs_exit(); 
			return 1;
		case GsEvent::KeyLeft: 
		{
			//gsout << "Key pressed: " << e.key << gsnl;
			
			std::vector<SnMyNode*>::iterator it;
			for (it = nodes.begin(); it != nodes.end(); ++it) {
				
				(*it)->init.x -= 0.1f; 

				(*it)->touch(); 
				
				render();
				
			}
		
			return 1;
		}
		case GsEvent::KeyRight:
		{
			//gsout << "Key pressed: " << e.key << gsnl;

			std::vector<SnMyNode*>::iterator it;
			for (it = nodes.begin(); it != nodes.end(); ++it) {
				
				(*it)->init.x += 0.1f;

				(*it)->touch();

				render();

			}

			return 1;
		}
		case GsEvent::KeyDown:
		{
			//gsout << "Key pressed: " << e.key << gsnl;

			std::vector<SnMyNode*>::iterator it;
			for (it = nodes.begin(); it != nodes.end(); ++it) {
				
				(*it)->init.y -= 0.1f;

				(*it)->touch();

				render();

			}

			return 1;
		}
		case GsEvent::KeyUp:
		{
			//gsout << "Key pressed: " << e.key << gsnl;

			std::vector<SnMyNode*>::iterator it;
			for (it = nodes.begin(); it != nodes.end(); ++it) {
				
				(*it)->init.y += 0.1f;

				(*it)->touch();

				render();

			}

			return 1;
		}
		case GsEvent::KeyEnter: 
		{
			std::vector<SnMyNode*>::iterator it;
			for (it = nodes.begin(); it != nodes.end(); ++it) {
				(*it)->color(GsColor::random());
				render();

			}
			return 1;
		}
		case 'q':
		{
			++numTriangles; 
			torus_node(littleR, bigR, numTriangles);
			redraw();
			return 1; 
		}
		case 'a':
		{
			if (numTriangles > 1){
				--numTriangles;
			}
			else {
				gsout << "Reached the limit to decrease" << gsnl;
				return 1; 
			}
			torus_node(littleR, bigR, numTriangles);
			redraw(); 
			return 1; 
		}
		case 'w':
		{
			littleR += 0.1f; 
			torus_node(littleR, bigR, numTriangles); 
			redraw();
			return 1;
		}
		case 's': 
		{

			littleR -= 0.1f;
			torus_node(littleR, bigR, numTriangles);
			redraw();
			return 1; 
		}
		case 'e':
		{
			bigR += 0.1f;
			torus_node(littleR, bigR, numTriangles);
			redraw();
			return 1; 
		}
		case 'd':
		{
			bigR -= 0.1f; 
			torus_node(littleR, bigR, numTriangles);
			redraw();
			return 1; 
		}
		// etc
		default: 
			gsout<<"Key pressed: "<<e.key<<gsnl;

			return 1; 
	}

	return 0;
}


int MyViewer::uievent ( int e )
{

	switch ( e )
	{	case EvAdd: add_mynode(1); return 1;

		case EvInfo:
		{	if ( output().len()>0 ) { output(""); return 1; }
			output_pos(0,30);
			activate_ogl_context(); // we need an active context
			GsOutput o; o.init(output()); gl_print_info(&o); // print info to viewer
			return 1;
		}
		case Update:
		{
			float x = 0.0f; 
			float y = 0.0f; 
			gsout << "Enter desired origin" << gsnl; 
			gsout << "X coordinate: "; 
			std::cin >> x; 
			gsout << "Y coordinate: "; 
			std::cin >> y; 



			update_node(x, y); 
			
			return 1; 
		}

		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
