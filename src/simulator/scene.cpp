//--------------------------------------------------
// Robot Simulator
// scene.cpp
// Date: 16/07/2020
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "scene.h"
#include "vulkan/vertex.h"

Scene::Scene(CommandPool* commandPool, std::vector<Model*> models, std::vector<Texture*> textures, bool enableRayTracing):
	_models(std::move(models)), _textures(std::move(textures))
{
	_commandPool = commandPool;

	// Concatenate all the models
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::uvec2> offsets;

	for(const auto model : _models)
	{
		// Remember the index, vertex offsets.
		const uint32_t indexOffset = static_cast<uint32_t>(indices.size());
		const uint32_t vertexOffset = static_cast<uint32_t>(vertices.size());

		offsets.emplace_back(indexOffset, vertexOffset);

		// Copy model data one after the other.
		vertices.insert(vertices.end(), model->getVertices().begin(), model->getVertices().end());
		indices.insert(indices.end(), model->getIndices().begin(), model->getIndices().end());
	}

	const auto flag = enableRayTracing ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0;

	createSceneBuffer(_vertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|flag, vertices);
	createSceneBuffer(_indexBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT|flag, indices);
	//createSceneBuffer(_materialBuffer;
	createSceneBuffer(_offsetBuffer, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, offsets);
}

Scene::~Scene()
{
	if(_vertexBuffer != nullptr)
	{
		delete _vertexBuffer;
		_vertexBuffer = nullptr;
	}

	if(_indexBuffer != nullptr)
	{
		delete _indexBuffer;
		_indexBuffer = nullptr;
	}

	if(_offsetBuffer != nullptr)
	{
		delete _offsetBuffer;
		_offsetBuffer = nullptr;
	}

	for(auto model : _models)
	{
		delete model;
		model = nullptr;
	}

	// TODO delete when texture isnt deleted by the model
	//for(auto texture : _textures)
	//{
	//	delete texture;
	//	texture = nullptr;
	//}
}

void Scene::addModel(Model* model)
{
	_models.push_back(model);
}

template <class T>
void Scene::createSceneBuffer(Buffer*& buffer,
		const VkBufferUsageFlags usage, 
		const std::vector<T>& content)
{
	const int size = sizeof(content[0]) * content.size();

	buffer = new Buffer(_commandPool->getDevice(), 
			size, 
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
