//--------------------------------------------------
// GuiB
// topBar.cpp
// Date: 2021-01-24
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "topBar.h"
#include "box.h"
#include "row.h"

namespace guib
{
	TopBar::TopBar(TopBarInfo info):
		Widget({
				.offset = {0,0}, 
				.size={1,20, guib::UNIT_PERCENT, guib::UNIT_PIXEL}}),
		_color(info.color)
	{
		Widget::setType("TopBar");

		Widget::setChild(
			new guib::Box(
			{
				.color = info.color,
				.size  = {1, 1},
				.child = new Row(
				{
					.children =	info.buttons
				})

			})
		);
	}
}

