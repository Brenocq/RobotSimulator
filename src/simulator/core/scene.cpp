//--------------------------------------------------
// Atta Robot Simulator
// scene.cpp
// Date: 2021-01-11
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "scene.h"
#include "simulator/objects/lights/lights.h"

namespace atta
{
	Scene::Scene(CreateInfo info):
		_objects(info.objects), _robots(info.robots)
	{
		for(auto object : _objects)
		{
			if(object->isLight())
			{
				std::string type = object->getType();
				if(type == "PointLight")
				{
					std::shared_ptr<PointLight> l = std::static_pointer_cast<PointLight>(object);
					_lights.push_back(Light::point(l->getPosition(), l->getIntensity()));
				}
				else if(type == "SpotLight")
				{
					std::shared_ptr<SpotLight> l = std::static_pointer_cast<SpotLight>(object);
					_lights.push_back(Light::spot(l->getPosition(), l->getDirection(), l->getIntensity(), l->getFalloffStart(), l->getTotalWidth()));
				}
				if(type == "DistantLight")
				{
					std::shared_ptr<DistantLight> l = std::static_pointer_cast<DistantLight>(object);
					_lights.push_back(Light::distant(l->getRadiance(), l->getDirection()));
				}
			}
		}
	}

	Scene::~Scene()
	{

	}
}
