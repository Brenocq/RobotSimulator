//--------------------------------------------------
// GuiB
// box.cpp
// Date: 2020-11-28
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/gui/widgets/box.h>
#include <atta/graphics/gui/guiStructs.h>
#include <atta/graphics/gui/guiState.h>
#include <atta/graphics/gui/guiRender.h>
#include <atta/helpers/log.h>
#include <atta/graphics/vulkan/vulkan.h>
#include <atta/math/math.h>

namespace guib
{
	Box::Box(BoxInfo boxInfo):
		Widget({.offset=boxInfo.offset, .size=boxInfo.size, .child=boxInfo.child}), _color(boxInfo.color), _radius(boxInfo.radius)
	{
		Widget::setType("Box");
	}

	void Box::render()
	{
		guib::Color color = getColor();

		GuiObjectInfo objectInfo;
		objectInfo.position = atta::vec4(_offset.x, _offset.y, (float)state::renderDepth, 1.0f);
		objectInfo.size = atta::vec2(_size.width, _size.height);
		objectInfo.color = atta::vec4(color.r, color.g, color.b, color.a);
		objectInfo.isLetter = 0;
		objectInfo.textureIndex = -1;

		// Calculate radius
		float minSize = std::min(_size.height, _size.width);
		objectInfo.radius = _radius.topLeft*minSize/2.0f;

		//Log::debug("Box", "Render [w]$0[] with [w]$1 []--[w] $2 and radius:$3", _type, _size.toString(), _offset.toString(), objectInfo.radius);

		vkCmdPushConstants(
				state::guiRender->getCommandBuffer(),
				state::guiRender->getPipelineLayout()->handle(),
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(GuiObjectInfo),
				&objectInfo);

		vkCmdDraw(state::guiRender->getCommandBuffer(), 6, 1, 0, 0);

		if(_child)
			_child->render();
	}
}
