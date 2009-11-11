
#pragma once

class CfgList;


namespace ncrobot {

	class Shape;

	struct Mass {
		Mass() { weight=J=0.0f; }
		float weight;
		float J; // rotational inertia
		Vector2 center; // center of gravity, used as center in model state (rotation is around this point)

		Mass operator*(float a) const {
			Mass r;
			r.weight=weight*a;
			r.J=J*a;
			r.center=center;
			return r;
		}
	};

	class Part
	{
	public:
		Part() {}
		~Part();

		std::string name;
		Mass mass;
		Vector2 center; // center of gravity, defined in model space
		Vector2 refPos; // reference position for constraint definitions
		Shape* shape;
		float drag;
	};

	class Constraint
	{
	public:
		Constraint() {
			parts[0]=parts[1]=0;
		}
		std::string name;

		Vector2 anchor;
		Part* parts[2];
	};

	// Definition of robot model
	class Model
	{
	public:
		Model(std::string mdlFile);
		~Model();

		typedef std::vector<Part*> PartList;
		typedef std::vector<Constraint*> ConstraintList;

		std::string name, file;
		PartList parts;
		ConstraintList constraints;

		Part* getPart (std::string name);

	private:
		void loadPart(std::string name, CfgList* cfg);
		void loadConstraint(std::string name, CfgList* cfg);
	};

}
