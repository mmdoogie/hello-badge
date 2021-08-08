#include "screens/qrtag.h"

#define LOGTAG "QRTag"
#include "esp_log.h"

#include "fonts/NovaFlat-Regular-32.h"
#include "fonts/Nunito-Regular-13.h"

#include "qrcode.h"

QrTag::QrTag(Paint* paintBlack, Paint* paintRed, const char* title, const char* content, const char* preview1, const char* preview2) {
	_paintBlack = paintBlack;
	_paintRed = paintRed;
	_title = title;
	_content = content;
	_preview1 = preview1;
	_preview2 = preview2;
}

void QrTag::render() {
	ESP_LOGI(LOGTAG, "Rendering");
	QRCode qr;
	uint8_t *qrBuff;
	qrBuff = (uint8_t*)malloc(qrcode_getBufferSize(5));

	qrcode_initText(&qr, qrBuff, 5, ECC_HIGH, _content);
	int x0 = EPD_WIDTH / 2 - qr.size;
	int y0 = EPD_HEIGHT / 2 - qr.size;

	for (int y = 0; y < qr.size; y++) {
		for (int x = 0; x < qr.size; x++) {
			_paintBlack->DrawPixel(x0 + 2 * x, y0 + 2 * y, qrcode_getModule(&qr, x, y));
			_paintBlack->DrawPixel(x0 + 2 * x + 1, y0 + 2 * y, qrcode_getModule(&qr, x, y));
			_paintBlack->DrawPixel(x0 + 2 * x, y0 + 2 * y + 1, qrcode_getModule(&qr, x, y));
			_paintBlack->DrawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1, qrcode_getModule(&qr, x, y));
		}
	}

	_paintBlack->DrawStringCenteredIn(0, 0, EPD_WIDTH, y0, _title, NovaFlatRegular32, 1);
	_paintRed->DrawStringCenteredIn(1, 1, EPD_WIDTH, y0, _title, NovaFlatRegular32, 1);
	_paintRed->DrawStringCenteredIn(0, 0, EPD_WIDTH, y0, _title, NovaFlatRegular32, 0);

	y0 += qr.size * 2;
	if (!_preview2) {
		_paintBlack->DrawStringCenteredIn(0, y0, EPD_WIDTH, EPD_HEIGHT - y0, _preview1, NunitoRegular13, 1);
	} else {
		_paintBlack->DrawStringCenteredIn(0, y0 - 7, EPD_WIDTH, EPD_HEIGHT - y0, _preview1, NunitoRegular13, 1);
		_paintBlack->DrawStringCenteredIn(0, y0 + 7, EPD_WIDTH, EPD_HEIGHT - y0, _preview2, NunitoRegular13, 1);
	}

	free(qrBuff);
}