
#include "StdIncl.h"
#include "CfgParser.h"
#include "Model.h"

#include "Shape.h"

namespace ncrobot {

	Part::~Part() {
		if (shape)
			delete shape;
	}

	Model::Model(std::string file)
	{
		this->file = file;

		CfgList* cfg = CfgValue::LoadFile(file);

		CfgList* modelCfg = cfg->GetList("model");
		this->name = modelCfg->GetLiteral("name", "");

		CfgList* partsCfg = modelCfg->GetList("parts");
		if (partsCfg) {
			for(CfgList::iterator li=partsCfg->childs.begin();li!=partsCfg->childs.end();++li) {
				CfgValue *v = li->value;

				if (v->getType() == CfgValue::List)
					loadPart(li->name, (CfgList*)v);
				else
					d_trace("%s: Ignoring %s\n", file.c_str(), li->name.c_str());
			}
		}
		CfgList* constraintsCfg = modelCfg->GetList("constraints");
		if (constraintsCfg) {
			for(CfgList::iterator li=constraintsCfg->childs.begin();li!=constraintsCfg->childs.end();++li) {
				CfgValue* v=li->value;
				if (v->getType() == CfgValue::List)
					loadConstraint(li->name, (CfgList*)v);
				else
					d_trace("%s: Ignoring %s\n", file.c_str(), li->name.c_str());
			}
		}

		delete cfg;
	}

	void Model::loadPart(std::string name, CfgList* cfg) {
		Part *part = new Part();
		
		part->name = name;
		part->refPos = cfg->GetVector2("pos");
		part->mass.weight = cfg->GetNumeric("weight");
		part->mass.J = cfg->GetNumeric("J",-1.0f);

		// Parse shape
		CfgList *shapeCfg = cfg->GetList("shape");
		if (shapeCfg) {
			part->shape = Shape::parse(shapeCfg);
			Mass m =part->shape->calcInertia(part->mass.weight);
			if (part->mass.J<=0.0f) part->mass.J=m.J;
			part->mass.center=m.center;
		} 
		else {
			if (part->mass.J <= 0.0f)
				throw ContentException(SPrintf("Part '%s' needs polar moment (J) >= 0", name.c_str()));
		}

		part->center = part->refPos + part->mass.center;

		parts.push_back(part);
	}

	void Model::loadConstraint(std::string name, CfgList* cfg) {
		
		Constraint* c= new Constraint();
		c->anchor = cfg->GetVector2("pos");
		c->name = name;

		CfgList* partsList = cfg->GetList("parts");
		std::vector<Part*> parts;

		for(CfgList::iterator li=partsList->childs.begin(); li!=partsList->childs.end();++li) {
			CfgLiteral *partName =  (li->value->getType() == CfgValue::Literal) ? (CfgLiteral*)li->value : 0;
			if (!partName)
				throw ContentException(SPrintf("Invalid part list in constraint '%s'", name.c_str()));
			Part *p = getPart (partName->value);
			if (p)
				parts.push_back(p);
			else
				throw ContentException(SPrintf("Constraint '%s' has invalid part name: '%s'", name.c_str(), partName->value.c_str()));
		}

		assert (parts.size() == 2);
		c->parts[0] = parts[0];
		c->parts[1] = parts[1];

		constraints.push_back(c);
	}

	Part* Model::getPart(std::string name) {
		for (PartList::iterator pi = parts.begin(); pi != parts.end(); ++pi)
			if ( (*pi)->name == name )
				return *pi;

		return 0;
	}


	Model::~Model () {
		DeleteAll(parts);
		DeleteAll(constraints);
	}

}
