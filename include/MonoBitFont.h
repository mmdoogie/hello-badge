#ifndef __mono_bit_font_h
#define __mono_bit_font_h

#include <stdint.h>

class MonoBitFont {
	public:
		MonoBitFont(uint8_t *fontData, uint8_t charWidth, uint8_t charHeight, uint8_t csOffset = 32, uint8_t csMax = 126);
		MonoBitFont(uint8_t *fontData, int *offsetData, uint8_t *widthData, uint8_t charHeight, int8_t spacing = 0, uint8_t csOffset = 32, uint8_t csMax = 127);
		~MonoBitFont();
		
		uint8_t getCharWidth(char c);
		int getStringWidth(const char *c);
		uint8_t getCharHeight();
		uint8_t getCharsetOffset();
		uint8_t getCharsetMax();
		uint8_t *getCharData(char c);
		int8_t getExtraSpacing();
		void setExtraSpacing(int8_t spacing);
		int8_t getSpacing();

	protected:
		uint8_t *fontData;
		int *offsetData;
		uint8_t *widthData;
		uint8_t charHeight;
		uint8_t charsetOffset;
		uint8_t charsetMax;
		int8_t baseSpacing;
		int8_t extraSpacing;
		int getCharOffset(char c);
		int getCharIndex(char c);

		MonoBitFont();
};

#endif
