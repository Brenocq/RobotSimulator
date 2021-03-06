//--------------------------------------------------
// Atta Robot Simulator
// scene.h
// Date: 2021-01-11
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_CORE_SCENE_H
#define ATTA_CORE_SCENE_H

#include <vector>
#include <memory>
#include <atta/core/robot.h>
#include <atta/objects/object.h>
#include <atta/graphics/core/light.h>

namespace atta
{
	class Scene
	{
		public:
			struct CreateInfo
			{
				std::vector<std::shared_ptr<Object>> objects = {};
				std::vector<std::shared_ptr<Robot>> robots = {};
			};

			Scene(CreateInfo info);
			~Scene();

			//---------- Getters ----------//
			std::vector<std::shared_ptr<Object>> getObjects() const { return _objects; }
			std::vector<std::shared_ptr<Object>> getObjectsFlat() const { return _objectsFlat; }
			std::vector<std::shared_ptr<Robot>> getRobots() const { return _robots; }
			std::vector<std::shared_ptr<Object>> getLights() const { return _lights; }

		private:
			std::vector<std::shared_ptr<Object>> _objects;// All objects
			std::vector<std::shared_ptr<Object>> _lights;// Only light objects
			std::vector<std::shared_ptr<Object>> _objectsFlat;
			std::vector<std::shared_ptr<Robot>> _robots;
	};
}

#endif// ATTA_CORE_SCENE_H
