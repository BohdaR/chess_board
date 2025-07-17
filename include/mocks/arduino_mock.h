#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

using byte = unsigned char;
using boolean = bool;

inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }
inline void delay(unsigned long) {}
inline int analogRead(int) { return 0; }
inline unsigned long millis() {
    static unsigned long fakeTime = 0;
    fakeTime += 100;
    return fakeTime;
}

#endif // ARDUINO_MOCK_H
