//--------------------------------------------------
// GuiB
// text.h
// Date: 2020-12-08
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef GUIB_TEXT_H
#define GUIB_TEXT_H

#include <atta/graphics/gui/widgets/widget.h>
#include <atta/graphics/gui/widgets/widgetStructs.h>

namespace guib {
	struct TextInfo {
		Color color = {1,1,1,1};
		std::string text = "text";
		int textSize = 13;
		Offset offset = {0,0};
	};

	class Text : public Widget
	{
		public:
			Text(TextInfo info);

			//void preProcessSizeOffset() override;
			void render() override;

			//---------- Getters and Setters ----------//
			void setColor(Color color) { _color=color; }
			Color getColor() const { return _color; }
			std::string getText() const { return _text; }
			int getTextSize() const { return _textSize; }

		private:
			Size calculateTextSize(std::string text, unsigned textSize);

			Color _color;
			std::string _text;
			int _textSize;
	};
}

#endif// GUIB_TEXT_H
