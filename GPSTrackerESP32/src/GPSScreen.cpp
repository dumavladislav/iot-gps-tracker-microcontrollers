#include <GPSScreen.h>

GPSScreen::GPSScreen() {}

GPSScreen::GPSScreen(uint8_t defaultFontSize) {
    this->defaultFontSize = defaultFontSize;
}

void GPSScreen::internalSetFont() {
    switch (this->defaultFontSize)
    {
    case 25:
        u8g2.setFont(u8g2_font_crox5tb_tf);
        break;
    
    default:
        u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
        break;
    }
}

void GPSScreen::init() {
    u8g2.begin();    
    u8g2.clearBuffer();					// clear the internal memory
    internalSetFont();
}

void GPSScreen::clear() {
    u8g2.clearBuffer();					// clear the internal memory
    internalSetFont();
    currYPosition = 0;
}

void GPSScreen::addString(String str) {
    addString(str, defaultFontSize);
}

void GPSScreen::addString(String str, uint8_t fontSize) {
    currYPosition = currYPosition + fontSize;
/*    if (currString > 3) {
        clear();
        currString = 0;
    }*/ 
    u8g2.drawStr(0, currYPosition, str.c_str()); 
}

void GPSScreen::render() {
    u8g2.sendBuffer();
}