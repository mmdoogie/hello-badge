#include "MonoBitFont.h"
#include <stdlib.h>
#include <string.h>

uint8_t MonoBitFont::getCharWidth(char c) {
	return widthData[getCharIndex(c)];
}

int MonoBitFont::getStringWidth(const char *c) {
	int l = strlen(c);
	int width = 0;
	for (int i = 0; i < l; i++) {
		width += getCharWidth(c[i]) + getSpacing();
	}
	return width;
}

uint8_t MonoBitFont::getCharHeight() {
	return charHeight;
}

uint8_t MonoBitFont::getCharsetOffset() {
	return charsetOffset;
}

uint8_t MonoBitFont::getCharsetMax() {
	return charsetMax;
}

int MonoBitFont::getCharIndex(char c) {
	if (c < charsetOffset) return -1;
	if (c > charsetMax) return -1;

	return c - charsetOffset;
}

int MonoBitFont::getCharOffset(char c) {
	int idx = getCharIndex(c);
	if (idx == -1) return -1;

	return offsetData[getCharIndex(c)];
}

uint8_t *MonoBitFont::getCharData(char c) {
	int off = getCharOffset(c);
	if (off == -1) return 0;
	
	return fontData + off;
}

int8_t MonoBitFont::getExtraSpacing() {
	return extraSpacing;
}

void MonoBitFont::setExtraSpacing(int8_t spacing) {
	this->extraSpacing = spacing;
}

int8_t MonoBitFont::getSpacing() {
	return baseSpacing + extraSpacing;
}

MonoBitFont::MonoBitFont(uint8_t *fontData, uint8_t charWidth, uint8_t charHeight, uint8_t csOffset, uint8_t csMax) {
	this->fontData = fontData;
	this->charHeight = charHeight;
	this->charsetOffset = csOffset;
	this->charsetMax = csMax;
	this->extraSpacing = 0;

	int charCount = csMax - csOffset + 1;
	this->widthData = (uint8_t *)malloc(sizeof(uint8_t) * charCount);
	this->offsetData = (int *)malloc(sizeof(int) * charCount);
	int currOffset = 0;
	for (int i = 0; i < charCount; i++) {
		this->widthData[i] = charWidth;
		this->offsetData[i] = currOffset;
		currOffset = currOffset + (charWidth / 8 + (charWidth % 8 ? 1 : 0)) * charHeight;
	}
}

MonoBitFont::MonoBitFont(uint8_t *fontData, int *offsetData, uint8_t *widthData, uint8_t charHeight, int8_t spacing, uint8_t csOffset, uint8_t csMax) {
	this->fontData = fontData;
	this->offsetData = offsetData;
	this->widthData = widthData;
	this->charHeight = charHeight;
	this->charsetOffset = csOffset;
	this->charsetMax = csMax;
	this->baseSpacing = spacing;
	this->extraSpacing = 0;
}
