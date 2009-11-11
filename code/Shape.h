
#pragma once

#include "Model.h"
#include "XForm.h"

namespace ncrobot {

	class Shape {	
	public:
		virtual ~Shape() {}
		virtual void draw(XForm xf);

		virtual Mass calcInertia(float mass) =0;

		static Shape* parse(CfgList* cfg);

		// draw in shape space
		virtual void drawLocal () = 0;
	};

	class LineShape : public Shape {
	public:
		LineShape (CfgList *cfg);
		void drawLocal();
		Mass calcInertia(float mass);

		Vector2 start, end;
	};

	class CircleShape : public Shape {
	public:
		CircleShape (CfgList* cfg);
		void drawLocal();
		Mass calcInertia(float mass);

		Vector2 center;
		float radius;
	};

	class BoxShape : public Shape {
	public:
		BoxShape (CfgList *cfg);
		void drawLocal();
		Mass calcInertia(float mass);

		Vector2 center;
		Vector2 size;
	};

}
