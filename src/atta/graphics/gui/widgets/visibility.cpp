//--------------------------------------------------
// GuiB
// visibility.cpp
// Date: 2020-12-03
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/gui/widgets/visibility.h>
#include <atta/helpers/log.h>

namespace guib
{
	Visibility::Visibility(VisibilityInfo info):
		Widget({.child=info.child}), _visible(info.visible)
	{
		Widget::setType("Visibility");
	}

	void Visibility::preProcessSizeOffset()
	{
		Widget::wrapChild();
	}

	void Visibility::render()
	{
		if(_visible && _child)
			_child->render();
	}
}

