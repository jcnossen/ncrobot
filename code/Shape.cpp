
#include "StdIncl.h"
#include "CfgParser.h"
#include "Shape.h"

#include "OpenGL.h"

namespace ncrobot {


Shape* Shape::parse (CfgList *cfg) {
	std::string type = cfg->GetLiteral("type", "");

	if (type == "line")
		return new LineShape(cfg);
	if (type == "circle")
		return new CircleShape(cfg);
	if (type == "box")
		return new BoxShape(cfg);

	throw ContentException("Invalid shape type: " + type);
}

void Shape::draw(XForm xf) {
	glPushMatrix();
		float d[16];
		xf.toMatrix().transposed44Matrix(d);
		glMultMatrixf(d);

		drawLocal();

	glPopMatrix();
}

// ---------------------------- Line shape

LineShape::LineShape(CfgList *cfg) {
	start = cfg->GetVector2("start");
	end = cfg->GetVector2("end");
}

void LineShape::drawLocal() {
	glBegin(GL_LINES);
	glColor(Color::White);
	glVertex(start);
	glVertex(end);
	glEnd();
}

Mass LineShape::calcInertia(float mass)
{
	Mass m;
	//m.center = (start + end) * .5f;
	m.weight = mass;
	float len = (start-end).length();
	m.J = 1/12.0f * mass * len * len; // J = mL^2 / 12
	return m;
}

// ---------------------------- Circle shape

CircleShape::CircleShape(CfgList* cfg) {
	radius = cfg->GetNumeric("radius", 1);
}

void CircleShape::drawLocal() {

	int n = 10;
	glBegin(GL_TRIANGLE_FAN);
	glColor(Color::Grey);
	glVertex(center);
	glColor(Color::White);
	for(int i=0;i<=n;i++) {
		glVertex(Vector2((2*PI/n*i)));
	}
	glEnd();
}

Mass CircleShape::calcInertia(float mass)
{
	Mass m;
	m.weight=mass;
	m.J = mass*radius*radius*0.5f;
	return m;
}

// ---------------------------- Box shape


BoxShape::BoxShape(CfgList *cfg) {
	size = cfg->GetVector2("size", Vector2(1,1));
}

void BoxShape::drawLocal() {
	Vector2 hs = size*0.5f;
	glColor(Color::White);
	glRectf(center.x-hs.x,center.y+hs.y,center.x+hs.x,center.y-hs.y);
}

Mass BoxShape::calcInertia(float mass)
{
	Mass m;
	float area = size.x*size.y; // surface area
	float dens = mass / area;
	m.J = dens * size.x * size.y / 3.0f * (size.x*size.x + size.y*size.y); // bh/3 (h^2 + b^2)
	return m;
}

}