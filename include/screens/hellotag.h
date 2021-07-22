#ifndef __hello_tagscreen__
#define __hello_tagscreen__

#include "screen.h"

#define EPD_WIDTH 264
#define EPD_HEIGHT 176
#define COLOR 1
#define BLANK 0

#define HELLO_RAD 16
#define HELLO_DS 1
#define BORDER_WIDTH 4
#define NAMEAREA_LEFT 16
#define NAMEAREA_WIDTH 232
#define NAMEAREA_REFRESH_LEFT 32
#define NAMEAREA_REFRESH_WIDTH 200
#define NAMEAREA_TOP 88
#define NAMEAREA_HEIGHT 72
#define NAME_DS 1
#define NAMEIS_OFFSET 5

class HelloTag : public Screen {
	public:
		HelloTag(Paint* paintBlack, Paint* paintRed, const char* header, const char* subtitle, const char* name);
		void render();
	private:
		const char* _header;
		const char* _subtitle;
		const char* _name;
};

#endif