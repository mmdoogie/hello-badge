/**
 *  @filename   :   epd2in7b.cpp
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd2in7b.h"

Epd::~Epd() {
};

Epd::Epd() {
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

int Epd::SpiInit(void) {
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return -1;
    } else {
        return 0;
    }
}

void Epd::DispInit(void) {
    Reset();

    SendCommand(POWER_ON);
    WaitUntilIdle();

    SendCommand(PANEL_SETTING);
    SendData(0xAF);

    SendCommand(PLL_CONTROL);
    SendData(0x3A);

    SendCommand(POWER_SETTING);
    SendData(0x03);
    SendData(0x00);
    SendData(0x2b);
    SendData(0x2b);
    SendData(0x09);

    SendCommand(BOOSTER_SOFT_START);
    SendData(0x07);
    SendData(0x07);
    SendData(0x17);

    SendCommand(POWER_OPTIMIZATION);
    SendData(0x60);
    SendData(0xA5);
    SendCommand(POWER_OPTIMIZATION);
    SendData(0x89);
    SendData(0xA5);
    SendCommand(POWER_OPTIMIZATION);
    SendData(0x90);
    SendData(0x00);
    SendCommand(POWER_OPTIMIZATION);
    SendData(0x93);
    SendData(0x2A);
    SendCommand(POWER_OPTIMIZATION);
    SendData(0x73);
    SendData(0x41);

    SendCommand(VCM_DC_SETTING_REGISTER);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0xC7);

    SetLut();

    SendCommand(TCON_RESOLUTION);
    SendData(0x00);
    SendData(0xB0); // 0x00B0 = 176
    SendData(0x01);
    SendData(0x08); // 0x0108 = 264
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, 0);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) {
    DigitalWrite(dc_pin, 1);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void) {
    while(DigitalRead(busy_pin) == 0) {      //0: busy, 1: idle
        DelayMs(100);
    }      
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see EPD::Sleep();
 */
void Epd::Reset(void) {
    DigitalWrite(reset_pin, 0);                //module reset    
    DelayMs(200);
    DigitalWrite(reset_pin, 1);
    DelayMs(200);    
}

/**
 *  @brief: set the look-up tables
 */
void Epd::SetLut(void) {
    unsigned int count;     
    SendCommand(LUT_FOR_VCOM);
    for(count = 0; count < 44; count++) {
        SendData(lut_vcom[count]);
    }
    SendCommand(LUT_FOR_WHITE_TO_WHITE);
    for(count = 0; count < 42; count++) {
        SendData(lut_ww[count]);
    }
    SendCommand(LUT_FOR_BLACK_TO_WHITE);
    for(count = 0; count < 42; count++) {
        SendData(lut_bw[count]);
    }
    SendCommand(LUT_FOR_WHITE_TO_BLACK);
    for(count = 0; count < 42; count++) {
        SendData(lut_wb[count]);
    }
    SendCommand(LUT_FOR_BLACK_TO_BLACK);
    for(count = 0; count < 42; count++) {
        SendData(lut_bb[count]);
    }
}

void Epd::DisplayFrame(const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red) {
    if (frame_buffer_black != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (int i = 0; i < this->width * this->height / 8; i++) {
            SendData(frame_buffer_black[i]);
        }
        DelayMs(2);
        SendCommand(DATA_STOP);
    }
    if (frame_buffer_red != NULL) {
        SendCommand(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (int i = 0; i < this->width * this->height / 8; i++) {
            SendData(frame_buffer_red[i]);
        }
        DelayMs(2);
        SendCommand(DATA_STOP);
    }
    SendCommand(DISPLAY_REFRESH);
    WaitUntilIdle();
}

void Epd::DisplayArea(const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red, int x0, int y0, int width, int length) {
    if (frame_buffer_black != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (int i = 0; i < this->width * this->height / 8; i++) {
            SendData(frame_buffer_black[i]);
        }
        DelayMs(2);
        SendCommand(DATA_STOP);
    }

    if (frame_buffer_red != NULL) {
        SendCommand(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (int i = 0; i < this->width * this->height / 8; i++) {
            SendData(frame_buffer_red[i]);
        }
        DelayMs(2);
        SendCommand(DATA_STOP);
    }

    if (x0 % 8) x0 -= (x0 % 8);
    if (width % 8) width += 8 - (width % 8);
    
    SendCommand(DISPLAY_PARTIAL_REFRESH);
    SendData((x0 & 0x100) >> 8);
    SendData(x0 & 0xF8);
    SendData((y0 & 0x100) >> 8);
    SendData(y0 & 0xFF);
    SendData((width & 0x100) >> 8);
    SendData(width & 0xF8);
    SendData((length & 0x100) >> 8);
    SendData(length & 0xFF);
    WaitUntilIdle();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5. 
 *          You can use Epd::Init() to awaken
 */
void Epd::Sleep() {
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0xF7);
    SendCommand(POWER_OFF);
    SendCommand(DEEP_SLEEP);
    SendData(0xA5);
}

const unsigned char lut_vcom[] =
{
    0x00, 0x00,
    0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x00, 0x0E, 0x01, 0x0E, 0x01, 0x10,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

const unsigned char lut_ww[] =
{
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

const unsigned char lut_bw[] = 
{
    0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
    0x90, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0xB0, 0x04, 0x10, 0x00, 0x00, 0x05,
    0xB0, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0xC0, 0x23, 0x00, 0x00, 0x00, 0x01
};

const unsigned char lut_bb[] = 
{
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

const unsigned char lut_wb[] = 
{
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x20, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
    0x10, 0x0A, 0x0A, 0x00, 0x00, 0x08,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

/* END OF FILE */
