# M16C {#cpu} #
The M16C processors are a 16-bit microcomputer family commonly used in embedded systems.  This page describes the necessary [connections](#connections) between the Pico and the M16C, an [overview of the processor family](#family) and a description of the [memory layout](#memory) of the M16C processors, useful in using the `pico-puller` tool.

## Connections {#connections} ##
Here are the connections from the Pico to the M16C system.  M16C pin numbers refer to square package/rectangular package.

| Pico name | Pico pin | Header pin | M16C pin | M16C name |
|-----------|----------|------------|----------|-----------|
| 3V3(OUT)  |  36      |   1        |  14/16   |  Vcc      |
|  GP21     |  27      |   2        |  32/34   |  BUSY     |
|  GP18     |  24      |   3        |  31/33   |  SCLK     |
|  GP19     |  25      |   4        |  30/32   |  RXD      |
| 3V3(OUT)  |  36      |   5        |  44/46   |  \#CE      |
|  GND      |  28      |   6        |  39/41   |  \#EPM     |
|  GND      |  28      |   7        |  63/64   |  Vss      |
|  GP22     |  29      |   8        |  10/12   |  \#RESET   |
| 3V3(OUT)  |  36      |   9        |   7/9    |  CNVss    |
|  GP20     |  26      |  10        |  29/31   |  TXD      |

## Processor family {#family} ##

The M30624FGAFP/GP and M30624FGMFP/GP versions both have 256K of Flash memory, while the M30620FCAFP/GP and M30620FCMFP/GP have 128K.  The FP suffix indicates a 100P6S-A rectangular package with 30 pins on the long side and 20 on the short side, while a GP suffix indicates a square 100-pin QFP package (designated 100P6Q-A) with 25 pins one each side.

There are three standard ways to program or read the M16C; parallel, synchronous serial, asynchronous serial.  This project uses the synchronous serial mode, which Renesas calls Standard Serial Mode 1, by connecting some of the pins on the Pico board to the corresponding ones on the M16C processor or board.

The synchronous serial protocol uses RXD1, TXD1, CLK1 to transfer data and RTS1 as a BUSY signal. Details of the protocol are in the Users Manual for the M16C/62A but a summary of the connections is here, but all of the documented commands are implemented and described [here](@ref commands).

## Memory layout {#memory} ##
The peripherals, RAM and Flash ROM are all mapped to memory space in the M16C which has an address space of 1M (0x00000 to 0xfffff).  The details of memory mapping and registers are described in the manual for the M16C.  Some of the essential details required for using the `pico-puller` software are documented here.

The overall memory map is shown in the figure below.  This is for the 256K flash part with 20K of RAM.  The first area, starting a memory address 0x00000 contains Special Function Registers (SFRs).  After that is RAM, and then Flash ROM.

| Function | Size | Start |  End  |
|----------|------|-------|-------|
|  SFRs    |   1K | 00000 | 003ff |
|  RAM     |  20K | 00400 | 053ff |
|  unused  | 747K | 05400 | bffff |
|  Flash   | 256K | c0000 | fffff |

The last part of the Flash memory, starting at address 0xFFFDC is a fixed vector table as shown below.

| Interrupt   | Start |  End  |
|-------------|-------|-------|
| UND instr   | fffdc | fffdf |
| Overflow    | fffe0 | fffe3 |
| BRK instr   | fffe4 | fffe7 |
| addr match  | fffe8 | fffeb |
| single step | fffec | fffef |
| watchdog    | ffff0 | ffff3 |
| \#DBC        | ffff4 | ffff7 |
| \#NMI        | ffff8 | ffffb |
| Reset       | ffffc | fffff |


### Flash memory {#blocks} #
The flash memory of the M16C processor is divided into *blocks*.  The blocks are of different sizes and have different locations.  The summary for the 256K flash part is below:

| Block | Size | Start |  End  |
|-------|------|-------|-------|
|  0    |  16K | fc000 | fffff |
|  1    |   8K | fa000 | fbfff |
|  2    |   8K | f8000 | f9fff |
|  3    |  32K | f0000 | f7fff |
|  4    |  64K | e0000 | effff |
|  5    |  64K | d0000 | dffff |
|  6    |  64K | c0000 | cffff |


For 128K parts, everything is the same except the first populated memory is at 0xE0000, so the first two 64K blocks (blocks 5 and 6) do not exist.  It's helpful to remember that each *block* of memory is composed of multiple *pages* and that each page is 256 bytes long.

In this processor, as in many, one can program an individual *page*, but only erase an entire *block*.  Individual pages can be programmed, but only whole blocks can be erased.  When a block is erased, it contains all 0xff bytes.

## Other reading

- [Introduction](@ref mainpage)
- [Commands](@ref commands)
