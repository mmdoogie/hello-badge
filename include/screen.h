#ifndef __hello_screen__
#define __hello_screen__

#include "epdpaint.h"

class Screen {
	public:
		virtual void render() = 0;
	protected:
		Paint* _paintBlack;
		Paint* _paintRed;
};

#endif