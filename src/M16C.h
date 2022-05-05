#ifndef M16C_H
#define M16C_H
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "m16c.pio.h"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace beroset {
    using FlashPage = std::array<uint8_t, 256>;
class M16C {
public:
    M16C(PIO pio = pio0, uint clkpin = 2, uint resetpin = 6);
    void write_byte(uint8_t byte);
    uint8_t read_byte();
    void setClockHz(float clock_freq);
    void reset(bool state);
    // higher level functions
    std::string version();
    std::pair<uint8_t, uint8_t> status();
    FlashPage pageRead(uint addr);
    void pageProgram(uint addr, FlashPage page);
    FlashPage bootPageRead(uint addr);
    void id(std::vector<uint8_t> code, uint addr = 0x0fffdf);
    void clearStatus();
    void lockBitEnable();
    void lockBitDisable();
    void eraseAll();
    void eraseBlock(uint addr);
    // helper functions
    void busyWait();
    void repeatId();
    bool wasError();

private:
    PIO pio;
    uint clkpin;
    uint txd;
    uint rxd;
    uint busy;
    uint resetpin;
    int sm;
    bool error = false;
    std::vector<uint8_t> mycode;
    uint codeaddr;
};
}
#endif // M16C_H
