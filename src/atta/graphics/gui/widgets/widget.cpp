//--------------------------------------------------
// GuiB
// widget.cpp
// Date: 2020-11-28
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/gui/widgets/widget.h>
#include <atta/graphics/gui/guiState.h>
#include <atta/helpers/log.h>
#include <math.h>
#include <cmath>

namespace guib
{
	Widget::Widget(WidgetInfo widgetInfo):
		_size(widgetInfo.size), _offset(widgetInfo.offset), _child(widgetInfo.child), _parent(nullptr), _type("Widget"),
		_widgetInfo(widgetInfo)
	{

		// Convert from UNIT_PIXEL to UNIT_SCREEN
		solveDimensionsPixel();
		// Conversion from UNIT_PERCENT to UNIT_SCREEN occurs in preProcess or called by parent
	}

	Widget::~Widget()
	{
		if(_child!=nullptr)
		{
			delete _child;
			_child = nullptr;
		}
	}

	void Widget::render()
	{
		//	Log::debug("Widget", "Render [c]$0 [] Off:$1", _type, _offset.toString());
		if(_child)
			_child->render();
	}


	//--------------------------------//
	//--------- Size/Offset ----------//
	//--------------------------------//
	void Widget::getParentSizeOffset(Size &pSize, Offset &pOffset)
	{
		pSize = {1,1, UNIT_SCREEN, UNIT_SCREEN};
		pOffset = {0,0, UNIT_SCREEN, UNIT_SCREEN};
		if(_parent)
		{
			pSize = _parent->getSize();
			pOffset = _parent->getOffset();
		}
	}

	void Widget::parentAsksSizeOffset(Size &size, Offset &offset)
	{
		// The parent can call this method if it needs to now the child size/offset to define its own
		startPreProcess();
		preProcessSizeOffset();
		size = _size;
		offset = _offset;

		if(std::isnan(size.width) || std::isnan(size.height) || std::isnan(offset.x) || std::isnan(offset.y))
		{
			Log::error("guib::Align", "Child size/offset must be possible to calculate!");
			return;
		}
	}

	//--------------------------------//
	//---------- PreProcess ----------//
	//--------------------------------//
	void Widget::startPreProcess()
	{
		// Convert from UNIT_PERCENT to UNIT_SCREEN
		// It is made in pre preocess because it needs to have a reference to the parent, which occurs after the tree is created
		solveDimensionsPercent();

		if(_child)
			_child->setParent(this);
	}

	void Widget::endPreProcess()
	{
		//Log::debug("Widget", "End PreProcess [w]$0[] with [w]$1 []--[w] $2", _type, _size.toString(), _offset.toString());
		if(_child)
			_child->treePreProcess();
	}


	void Widget::preProcessSizeOffset()
	{
		// Executes when defining tree widgets size and offset
	}

	void Widget::preProcess()
	{
		// Executes one time after the tree is created
	}

	void Widget::treePreProcess()
	{
		startPreProcess();
		{
			preProcessSizeOffset();

			// Local to global offset (local+parent)
			if(_parent) _offset += _parent->getOffset();

			preProcess();
		}
		endPreProcess();
	}

	//--------------------------------//
	//---------- Tree Wrap -----------//
	//--------------------------------//
	void Widget::wrapChild()
	{
		// Sometimes the child needs to now the parent size/offset to calculate your own size/offset
		fillParent();

		// Wrap child if it exists
		if(_child)
		{
			Size parentSize;
			Offset parentOffset;
			getParentSizeOffset(parentSize, parentOffset);

			Size childSize;
			Offset childOffset;
			_child->parentAsksSizeOffset(childSize, childOffset);

			if(std::isnan(childSize.width) || std::isnan(childSize.height) || std::isnan(childOffset.x) || std::isnan(childOffset.y))
			{
				Log::error("guib::Widget", "[w](wrapChild)[] Child size/offset must be possible to calculate!");
				return;
			}

			_size = childSize;
			_offset = _offset+childOffset;
		}
	}

	void Widget::fillParent()
	{
		Size parentSize;
		Offset parentOffset;

		getParentSizeOffset(parentSize, parentOffset);
		// Calculate local coord size and offset
		_size = parentSize;
		_offset = {0,0};
	}

	//--------------------------------//
	//------------ Update ------------//
	//--------------------------------//
	void Widget::update()
	{
		//startPreProcess();
		//preProcessSizeOffset();
		//if(_parent) _offset += _parent->getOffset();

		//Log::debug("Widget", "Update [w]$0[] with [w]$1 []--[w] $2", _type, _size.toString(), _offset.toString());

		if(_child)
			_child->update();
	}

	void Widget::addOffsetTree(Offset offset)
	{
		_offset += offset;
		//Log::debug("Widget", "AddOff [y]$0[] with [w]$1", _type, _offset.toString());
		if(_child)
			_child->addOffsetTree(offset);
	}

	//--------------------------------//
	//----------- SolveDim -----------//
	//--------------------------------//
	void Widget::solveDimensionsPercent()
	{
		// Convert from UNIT_PERCENT to UNIT_SCREEN
		Size parentSize;
		Offset parentOffset;
		getParentSizeOffset(parentSize, parentOffset);

		if(_size.unitW == guib::UNIT_PERCENT)
		{
			_size.width *= parentSize.width;
			_size.unitW = guib::UNIT_SCREEN;
		}
		if(_size.unitH == guib::UNIT_PERCENT)
		{
			_size.height *= parentSize.height;
			_size.unitH = guib::UNIT_SCREEN;
		}

		if(_offset.unitX == guib::UNIT_PERCENT)
		{
			_offset.x = parentSize.width*_offset.x;
			_offset.unitX = guib::UNIT_SCREEN;
		}
		if(_offset.unitY == guib::UNIT_PERCENT)
		{
			_offset.y = parentSize.height*_offset.y;
			_offset.unitY = guib::UNIT_SCREEN;
		}
	}

	void Widget::solveDimensionsPixel()
	{
		// Convert from UNIT_PIXEL to UNIT_SCREEN
		float invWidth = 1.0f/state::screenSize.width;
		float invHeight = 1.0f/state::screenSize.height;

		if(_size.unitW == guib::UNIT_PIXEL)
		{
			_size.width *= invWidth;
			_size.unitW = guib::UNIT_SCREEN;
		}
		if(_size.unitH == guib::UNIT_PIXEL)
		{
			_size.height *= invHeight;
			_size.unitH = guib::UNIT_SCREEN;
		}

		if(_offset.unitX == guib::UNIT_PIXEL)
		{
			_offset.x *= invWidth;
			_offset.unitX = guib::UNIT_SCREEN;
		}
		if(_offset.unitY == guib::UNIT_PIXEL)
		{
			_offset.y *= invHeight;
			_offset.unitY = guib::UNIT_SCREEN;
		}
	}
}
