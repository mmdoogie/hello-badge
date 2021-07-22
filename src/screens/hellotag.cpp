#include "screens/hellotag.h"

#define LOGTAG "HelloTag"
#include "esp_log.h"

#include "fonts/DancingScript-Regular-72.h"
#include "fonts/NovaFlat-Regular-32.h"
#include "fonts/Nunito-Regular-13.h"

HelloTag::HelloTag(Paint* paintBlack, Paint* paintRed, const char* header, const char* subtitle, const char* name) {
	_paintBlack = paintBlack;
	_paintRed = paintRed;
	_header = header;
	_subtitle = subtitle;
	_name = name;
}

void HelloTag::render() {
	ESP_LOGI(LOGTAG, "Rendering");
	_paintRed->DrawFilledRoundedRectangle(BORDER_WIDTH, BORDER_WIDTH, EPD_WIDTH - BORDER_WIDTH, EPD_HEIGHT - BORDER_WIDTH, HELLO_RAD, COLOR);
	_paintRed->DrawFilledRoundedRectangle(NAMEAREA_LEFT, NAMEAREA_TOP, NAMEAREA_LEFT + NAMEAREA_WIDTH, NAMEAREA_TOP + NAMEAREA_HEIGHT, HELLO_RAD, BLANK);

	_paintRed->DrawStringCenteredIn(HELLO_DS, HELLO_DS, EPD_WIDTH, NAMEAREA_TOP, _header, DancingScriptRegular72, BLANK);
	_paintBlack->DrawStringCenteredIn(HELLO_DS, HELLO_DS, EPD_WIDTH, NAMEAREA_TOP, _header, DancingScriptRegular72, COLOR);
	_paintBlack->DrawStringCenteredIn(0, 0, EPD_WIDTH, NAMEAREA_TOP, _header, DancingScriptRegular72, BLANK);
	_paintRed->DrawStringCenteredIn(0, 0, EPD_WIDTH, NAMEAREA_TOP, _header, DancingScriptRegular72, BLANK);

	MonoBitFont* font = NunitoRegular13;
	int l = font->getStringWidth(_subtitle);
	int x = (EPD_WIDTH - l) / 2;
	int y = NAMEAREA_TOP - NAMEIS_OFFSET - font->getCharHeight();
	_paintRed->DrawStringAt(x+1, y+1, _subtitle, font, BLANK);
	_paintBlack->DrawStringAt(x+1, y+1, _subtitle, font, COLOR);
	_paintBlack->DrawStringAt(x, y, _subtitle, font, BLANK);
	_paintRed->DrawStringAt(x, y, _subtitle, font, BLANK);

	_paintRed->DrawStringCenteredIn(NAMEAREA_LEFT+NAME_DS, NAMEAREA_TOP+NAME_DS, NAMEAREA_WIDTH, NAMEAREA_HEIGHT, _name, NovaFlatRegular32, COLOR);
	_paintRed->DrawStringCenteredIn(NAMEAREA_LEFT, NAMEAREA_TOP, NAMEAREA_WIDTH, NAMEAREA_HEIGHT, _name, NovaFlatRegular32, BLANK);
	_paintBlack->DrawStringCenteredIn(NAMEAREA_LEFT, NAMEAREA_TOP, NAMEAREA_WIDTH, NAMEAREA_HEIGHT, _name, NovaFlatRegular32, COLOR);
}