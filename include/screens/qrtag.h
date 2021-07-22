#ifndef __hello_qrscreen__
#define __hello_qrscreen__

#include "screen.h"

#define EPD_WIDTH 264
#define EPD_HEIGHT 176
#define COLOR 1
#define BLANK 0

class QrTag : public Screen {
	public:
		QrTag(Paint* paintBlack, Paint* paintRed, const char* title, const char* content, const char* preview);
		void render();
	private:
		const char* _title;
		const char* _content;
		const char* _preview;
};

#endif