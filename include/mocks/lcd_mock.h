#ifndef LCD_MOCK
#include <iostream>

class LiquidCrystal_I2C {
public:
    LiquidCrystal_I2C(uint8_t addr, uint8_t cols, uint8_t rows) {}

    void begin() {}

    void backlight() {}

    void setCursor(int col, int row) {}

    void print(std::string str) {}

    void clear() {}
};

#endif
