
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>
static bool _smooth = true;

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
GsVec smoothCalculation(int phi, int theta) {

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float alpha = GS_TORAD(float(phi));
	float beta = GS_TORAD(float(theta));

	x = cosf(alpha) * cosf(beta);
	y = cosf(alpha) * sinf(beta);
	z = sinf(alpha);

	return GsVec(x, y, z);
}

GsVec calculateSurfaceNormal(GsVec u, GsVec v) {
	float nx = 0.0f;
	float ny = 0.0f;
	float nz = 0.0f;

	nx = u.y * v.z - u.z*v.y;
	ny = u.z * v.x - u.x * v.z;
	nz = u.x * v.y - u.y * v.x;

	return GsVec(nx, ny, nz);
}

MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{
	_nbut = 0;
	_animating = false;
	build_ui();
	build_scene();
}

void MyViewer::build_ui()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel("", UiPanel::HorizLeft);
	p->add(new UiButton("View", sp = new UiPanel()));
	{	UiPanel* p = sp;
	p->add(_nbut = new UiCheckButton("Normals", EvNormals));
	}
	p->add(new UiButton("Animate", EvAnimate));
	p->add(new UiButton("Exit", EvExit)); p->top()->separate();
}

void MyViewer::add_model(SnShape* s, GsVec p)
{
	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation(p);
	manip->initial_mat(m);

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);

	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	GsModel * torus = new GsModel;
	SnModel * sn;
	int prevPhi = 0;
	int prevTheta = 0;

	int nextPhi = 0;
	int nextTheta = 0;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float r = 0.1f;
	float R = 0.5f;

	int a = 0;
	int b = 1;
	int c = 2;

	for (nextPhi = 10; prevPhi <= 360; nextPhi += 10) {
		for (nextTheta = 10; nextTheta <= 360; nextTheta += 10) {



			GsVec A00 = torusFunction(prevPhi, prevTheta, r, R);
			GsVec A10 = torusFunction(nextPhi, prevTheta, r, R);
			GsVec A01 = torusFunction(prevPhi, nextTheta, r, R);
			GsVec A11 = torusFunction(nextPhi, nextTheta, r, R);

			//0, 1, 2 index
			GsModel::Face f1 = GsModel::Face(a, b, c);
			//3, 4, 5 index 
			GsModel::Face f2 = GsModel::Face(a + 3, b + 3, c + 3);

			//Push the points that make one triangle
			torus->V.push() = A00;
			torus->V.push() = A10;
			torus->V.push() = A01;


			//Push the points that make the other triangle
			torus->V.push() = A10;
			torus->V.push() = A11;
			torus->V.push() = A01;

			//Push the faces of the triangles
			torus->F.push() = f1;
			torus->F.push() = f2;


			if (_smooth) {

				//Calculating normals
				A00 = smoothCalculation(prevPhi, prevTheta);
				A10 = smoothCalculation(nextPhi, prevTheta);
				A01 = smoothCalculation(prevPhi, nextTheta);
				A11 = smoothCalculation(nextPhi, nextTheta);

				//Pushing the normals as the triangles
				torus->N.push() = A00;
				torus->N.push() = A10;
				torus->N.push() = A01;

				torus->N.push() = A10;
				torus->N.push() = A11;
				torus->N.push() = A01;

				torus->set_mode(GsModel::Smooth, GsModel::NoMtl);
			}
			else {
				GsVec u = torus->V[f1.b] - torus->V[f1.a];
				GsVec v = torus->V[f1.c] - torus->V[f1.a];

				torus->N.push() = calculateSurfaceNormal(u, v);

				u = torus->V[f2.b] - torus->V[f2.a];
				v = torus->V[f2.c] - torus->V[f2.a];

				torus->N.push() = calculateSurfaceNormal(u, v);

				torus->set_mode(GsModel::Flat, GsModel::NoMtl);
			}
			a += 6;
			b += 6;
			c += 6;
			prevTheta = nextTheta;
		}

		prevPhi = nextPhi;
	}

	sn = new SnModel(torus);



	add_model(sn, GsVec(0, 0, 0));

}

// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation()
{
	if (_animating) return; // avoid recursive calls
	_animating = true;

	int ind = gs_random(0, rootg()->size() - 1); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0 / 30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t = 0, lt = 0, t0 = gs_time();
	do // run for a while:
	{
		while (t - lt<frdt) { ws_check(); t = gs_time() - t0; } // wait until it is time for next frame
		double yinc = (t - lt)*v;
		if (t>2) yinc = -yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if (m.e24<0) m.e24 = 0; // make sure it does not go below 0
		manip->initial_mat(m);
		render(); // notify it needs redraw
		ws_check(); // redraw now
	} while (m.e24>0);
	_animating = false;
}

void MyViewer::show_normals(bool b)
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for (int k = 0; k<r->size(); k++)
	{
		SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if (!b) { l->visible(false); continue; }
		l->visible(true);
		if (!l->empty()) continue; // build only once
		l->init();
		if (s->instance_name() == SnPrimitive::class_name)
		{
			GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face(fn);
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for (int i = 0; i<m.F.size(); i++)
			{
				const GsVec& a = m.V[m.F[i].a]; l->push(a, a + (*n++)*f);
				const GsVec& b = m.V[m.F[i].b]; l->push(b, b + (*n++)*f);
				const GsVec& c = m.V[m.F[i].c]; l->push(c, c + (*n++)*f);
			}
		}
	}
}

int MyViewer::handle_keyboard(const GsEvent &e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;

	switch (e.key)
	{
	case GsEvent::KeyEsc: gs_exit(); return 1;
	case 'n': { bool b = !_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
	default: gsout << "Key pressed: " << e.key << gsnl;
	}

	return 0;
}

int MyViewer::uievent(int e)
{
	switch (e)
	{
	case EvNormals: show_normals(_nbut->value()); return 1;
	case EvAnimate: run_animation(); return 1;
	case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
