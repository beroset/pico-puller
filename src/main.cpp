/**
 * Copyright (c) 2022 Edward J. Beroset
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "menu.h"
#include "M16C.h"
#include <iostream>
#include <iomanip>
#include <functional>
#include <limits>
#include <locale>
#include <string_view>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

struct State {
    bool running{true};
    bool showhelp{true};
    beroset::M16C target{pio0, 18, 22};
};

static State state;


class MenuEntry {
    std::string_view args;
    std::string_view desc;
    void(*function)();
public:
    void operator()() { function(); }
    constexpr MenuEntry(std::string_view args, std::string_view desc, void(*function)());
    friend std::ostream& operator<<(std::ostream& out, const MenuEntry& me);
};

constexpr MenuEntry::MenuEntry(std::string_view args, std::string_view desc, void(*function)())
    : args{args}
    , desc{desc}
    , function{function}
{}

std::ostream& operator<<(std::ostream& out, const MenuEntry& me) {
    return out << me.args << "\n\t" << me.desc;
}

std::ostream& dump(std::ostream& out, beroset::FlashPage page, uint addr) {
    addr &= 0xffffff00;
    auto it{std::begin(page)};
    for (int row{0}; row < 16; ++row) {
        out << '\n' << std::hex << std::setfill('0') 
            << std::setw(8) << addr << ':';
        addr += 16;
        for (int col{0}; col < 16; ++col, ++it) {
            out << ' ' << std::setw(2) << static_cast<unsigned>(*it);
        }
    }
    return out << '\n';
}

int main() {

    stdio_init_all();
    // short delay to allow host to enumerate USB and reconnect
    sleep_ms(2000);
    static constexpr beroset::ConsoleMenu<std::string_view, MenuEntry, 21> menu{
        "pico-puller Main Menu:\n",
        "That is not a valid choice.\n",
        "> ",
        " ",
        std::cin,
        std::cout,
        {{
            { "version", {"", "get 8-char version string", []{
                std::cout << "Version: \"" << state.target.version() << "\"\n";
            }}},
            { "id", {"id1 ... id7 [addr]", "check up to 7-byte ID", []{
                std::vector<uint8_t> id;
                for (int i = 0; i < 7; ++i) {
                    unsigned code;
                    std::cin >> std::hex >> code >> std::dec;
                    id.push_back(code & 0xff);
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Attempting id =";
                for (auto ch : id) {
                    std::cout << ' ' << std::hex << std::setfill('0') 
                        << std::setw(2) 
                        << static_cast<unsigned>(ch) << std::dec;
                }
                std::cout << '\n';
                state.target.id(id);
            }}},
            { "bpage", {"addr", "read 256-byte bootloader page", []{
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Reading from 0x" << std::hex << addr << std::dec << '\n';
                auto page = state.target.bootPageRead(addr);
                if (state.target.wasError()) {
                    std::cout << "Error reading page\n";
                } else {
                    dump(std::cout, page, addr);
                }
            }}},
            { "rpage", {"addr", "read 256-byte page", []{
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Reading from 0x" << std::hex << addr << std::dec << '\n';
                auto page = state.target.pageRead(addr);
                if (state.target.wasError()) {
                    std::cout << "Error reading page\n";
                } else {
                    dump(std::cout, page, addr);
                }
            }}},
            { "bdump", {"", "dump entire boot flash contents", []{
                for (unsigned addr{0xf'e000}; addr < 0x10'0000; addr += 256) {
                    state.target.busyWait();
                    auto page = state.target.bootPageRead(addr);
                    dump(std::cout, page, addr);
                }
            }}},
            { "dump", {"", "dump entire flash contents", []{
                for (unsigned addr{0xC'0000}; addr < 0x10'0000; addr += 256) {
                    state.target.busyWait();
                    auto page = state.target.pageRead(addr);
                    dump(std::cout, page, addr);
                }
            }}},
            { "status", {"", "read status register", []{
                auto [srd, srd1] = state.target.status();
                std::cout << "SRD = 0x" << std::hex
                    << static_cast<uint>(srd)
                    << ", SRD1 = 0x"
                    << static_cast<uint>(srd1)
                    << std::dec
                    << '\n';
            }}},
            { "clear", {"", "clear status register", []{
                state.target.clearStatus();
            }}},
            { "checkread", {"", "read CRC of written data", []{
                auto [crclo, crchi] = state.target.checkRead();
                std::cout << "CRClo = 0x" << std::hex
                    << static_cast<uint>(crclo)
                    << ", CRChi = 0x"
                    << static_cast<uint>(crchi)
                    << std::dec
                    << '\n';
            }}},
            { "lockread", {"addr", "read lock bit", []{
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Lock status for Block at 0x" << std::hex << addr 
                    << " = 0x" << static_cast<unsigned>(state.target.readLock(addr))
                    << std::dec << '\n';
            }}},
            { "lockwrite", {"addr", "write lock bit", []{
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Locking Block at 0x" << std::hex << addr 
                    << std::dec << '\n';
                state.target.writeLock(addr);
            }}},
            { "enable", {"", "enable lock bit", []{
                state.target.lockBitEnable();
            }}},
            { "disable", {"", "disable lock bit", []{
                state.target.lockBitDisable();
            }}},
            { "eblock", {"addr", "erase block", []{
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Erasing block at 0x" << std::hex << addr << std::dec << '\n';
                state.target.eraseBlock(addr);
            }}},
            { "ppage", {"addr byte0 ... byte255", "program the given page at addr", []{
                beroset::FlashPage page;
                unsigned addr;
                std::cin >> std::hex >> addr >> std::dec;
                for (unsigned i = 0; i < page.size(); ++i) {
                    unsigned code;
                    std::cin >> std::hex >> code >> std::dec;
                    page[i] = code & 0xff;
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Programming page at 0x" << std::hex << addr << std::dec;
                dump(std::cout, page, addr);
                std::cout << '\n';
                state.target.pageProgram(addr, page);
            }}},
            { "ramload", {"addr byte0 ... byteXX", "download into RAM at addr and execute", []{
                std::vector<uint8_t> code;
                unsigned addr;
                std::cin >> std::hex >> addr;
                unsigned codebyte;
                while (std::cin >> codebyte) {
                    code.push_back(static_cast<uint8_t>(codebyte));
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Programming RAM at 0x" << std::hex << addr << std::dec;
                std::cout << '\n';
                state.target.download(addr, code);
            }}},
            { "all_erase", {"", "erase all unlocked blocks", []{
                state.target.eraseAll();
            }}},
            { "set_speed", {"freq", "change the clock frequency", []{
                float clock_freq = 1e6;
                std::cin >> clock_freq;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                state.target.setClockHz(clock_freq);
                std::cout << "setting clock to " << clock_freq << "\n";
            }}},
            { "reset", {"0|1", "release or assert CPU's reset", []{
                std::string sense;
                std::cin >> sense;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (sense == "0") {
                    state.target.reset(true);
                    std::cout << "CPU is not in reset\n";
                } else {
                    state.target.reset(false);
                    std::cout << "CPU is in reset\n";
                }
            }}},
            { "help", {"", "display this help", []{
                state.showhelp = true;
            }}},
            { "quit", {"", "reset Pico to program mode", []{
                state.running = false;
            }}},
        }}
    };
    while (state.running) {
        if (state.showhelp) {
            menu.showPrompt();
            state.showhelp = false;
        }
        menu();
    }
    // No LED output, disable PicoBoot
    // reset the USB into MSI mode for reprogramming
    reset_usb_boot(0, 2);
}

