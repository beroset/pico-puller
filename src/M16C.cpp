#include "M16C.h"
#include <numeric>

static inline void m16c_program_init(PIO pio, uint sm, uint offset, uint clk) {
    const uint txd{clk + 1};
    const uint rxd{clk + 2};
    const uint busy{clk + 3};
    constexpr float clock_freq{1.0e6f};
    pio_sm_config c = m16c_program_get_default_config(offset);

    // Map the state machine's OUT pin group to one pin, namely the `txd`
    // parameter to this function.
    sm_config_set_out_pins(&c, txd, 1);
    sm_config_set_in_pins(&c, rxd);
    sm_config_set_sideset_pins(&c, clk);
    // Slow things down to desired clock_freq
    float div = (float) clock_get_hz(clk_sys) / clock_freq / 4;
    sm_config_set_clkdiv(&c, div);
    // Set the out shift and in shift
    sm_config_set_out_shift(&c, true, true, 8);
    sm_config_set_in_shift(&c, true, true, 8);
    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, txd);
    pio_gpio_init(pio, rxd);
    pio_gpio_init(pio, busy);
    pio_gpio_init(pio, clk);
    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, clk, 2, true);
    pio_sm_set_consecutive_pindirs(pio, sm, rxd, 2, false);
    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);
    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

namespace beroset {
enum commands{
    block_erase = 0x20,
    page_program = 0x41,
    clear_status = 0x50,
    read_status = 0x70,
    read_lock_status = 0x71,
    lock_bit_program = 0x77,
    lock_bit_disable = 0x75,
    lock_bit_enable = 0x7a,
    erase_all = 0xa7,
    id_check = 0xf5,
    download = 0xfa,
    version = 0xfb,
    boot_read = 0xfc,
    check_read = 0xfd,
    page_read = 0xff
};

M16C::M16C(PIO pio, uint clkpin, uint resetpin)
    : pio{pio}
    , clkpin{clkpin}
    , txd{clkpin + 1}
    , rxd{clkpin + 2}
    , busy{clkpin + 3}
    , resetpin{resetpin}
{
    uint offset{pio_add_program(pio, &m16c_program)};
    sm = pio_claim_unused_sm(pio, true);
    m16c_program_init(pio, sm, offset, clkpin);
    // set up reset pin as output
    gpio_init(resetpin);
    gpio_set_dir(resetpin, GPIO_OUT);
}

void M16C::write_byte(uint8_t byte) {
    pio_sm_put_blocking(pio, sm, byte);
    pio_sm_get_blocking(pio, sm);
}

uint8_t M16C::read_byte() {
    pio_sm_put_blocking(pio, sm, 0);
    return pio_sm_get_blocking(pio, sm) >> 24;
}

void M16C::setClockHz(float clock_freq) {
    float div = (float) clock_get_hz(clk_sys) / clock_freq / 4;
    pio_sm_set_clkdiv(pio, sm, div);
}

void M16C::reset(bool state) {
    gpio_put(resetpin, state);
}

std::string M16C::version() {
    write_byte(commands::version);
    std::string version;
    version.reserve(8);
    for (int i{8}; i; --i) {
        version.push_back(read_byte());
    }
    return version;
}

std::pair<uint8_t, uint8_t> M16C::status() {
    write_byte(commands::read_status);
    uint srd{read_byte()};
    uint srd1{read_byte()};
    return {srd, srd1};
}

std::pair<uint8_t, uint8_t> M16C::checkRead() {
    write_byte(commands::check_read);
    uint crcLo{read_byte()};
    uint crcHi{read_byte()};
    return {crcLo, crcHi};
}

FlashPage M16C::pageRead(uint addr) {
    FlashPage page;
    write_byte(commands::page_read);
    write_byte((addr >> 8) & 0xff);
    write_byte((addr >> 16) & 0xff);
    for (int i{0}; i < 256; ++i) {
        page[i] = read_byte();
    }
    // error = gpio_get(busy);
    return page;
}

FlashPage M16C::bootPageRead(uint addr) {
    FlashPage page;
    write_byte(commands::boot_read);
    write_byte((addr >> 8) & 0xff);
    write_byte((addr >> 16) & 0xff);
    for (int i{0}; i < 256; ++i) {
        page[i] = read_byte();
    }
    //error = gpio_get(busy);
    return page;
}

void M16C::pageProgram(uint addr, FlashPage page) {
    uint8_t size = static_cast<uint8_t>(mycode.size());
    write_byte(commands::page_program);
    write_byte((addr >> 8) & 0xff);
    write_byte((addr >> 16) & 0xff);
    write_byte(size);
    for (int i{0}; i < size; ++i) {
        write_byte(page[i]);
    }
    // error = gpio_get(busy);
}

void M16C::download(uint addr, std::vector<uint8_t> code) {
    write_byte(commands::download);
    write_byte((code.size() ) & 0xff);
    write_byte((code.size() >> 8) & 0xff);
    uint8_t checksum{static_cast<uint8_t>(std::accumulate(code.begin(), code.end(), 0))};
    write_byte(checksum);
    for (auto byte : code) {
        write_byte(byte);
    }
    // error = gpio_get(busy);
}

void M16C::id(std::vector<uint8_t> code, uint addr) {
    mycode =  code;
    codeaddr = addr;
    repeatId();
}

void M16C::repeatId() {
    uint8_t size = static_cast<uint8_t>(mycode.size());
    write_byte(commands::id_check);
    write_byte((codeaddr) & 0xff);
    write_byte((codeaddr >> 8) & 0xff);
    write_byte((codeaddr >> 16) & 0xff);
    write_byte(size);
    for (int i{0}; i < size; ++i) {
        write_byte(mycode[i]);
    }
    // error = gpio_get(busy);
}

void M16C::clearStatus() {
    write_byte(commands::clear_status);
}

uint8_t M16C::readLock(uint addr) {
    write_byte(commands::read_lock_status);
    write_byte((addr >> 8) & 0xff);
    write_byte((addr >> 16) & 0xff);
    return read_byte();
}

void M16C::writeLock(uint addr) {
    write_byte(commands::lock_bit_program);
    write_byte((addr >> 8) & 0xff);
    write_byte((addr >> 16) & 0xff);
    write_byte(0xD0);
}

void M16C::lockBitEnable() {
    write_byte(commands::lock_bit_enable);
}

void M16C::lockBitDisable() {
    write_byte(commands::lock_bit_disable);
}

void M16C::eraseAll() {
    write_byte(commands::erase_all);
    write_byte(0xD0);
}

void M16C::eraseBlock(uint addr) {
    write_byte(commands::block_erase);
    write_byte((codeaddr >> 8) & 0xff);
    write_byte((codeaddr >> 16) & 0xff);
    write_byte(0xD0);
}

void M16C::busyWait() {
    while (gpio_get(busy));
}

bool M16C::wasError() {
    bool past = error;
    error = false;
    return past;
}

} // end of namespace beroset
