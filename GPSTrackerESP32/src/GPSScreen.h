#include <Arduino.h>
#include <U8g2lib.h>

class GPSScreen {

public:
    GPSScreen();
    GPSScreen(uint8_t defaultFontSize);

    void init();
    void clear();
    void addString(String str);
    void addString(String str, uint8_t fontSize);
    void render();


private:

    void internalSetFont();

    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2{U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE};
    // U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2{U8G2_R0,  /* reset=*/ U8X8_PIN_NONE};
    // U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2{U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA};
    // U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2{U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE};
    // U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2{U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA};
    uint8_t currYPosition = 0;
    uint8_t defaultFontSize = 10;

};