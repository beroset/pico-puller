# M16C {#cpu} #
The M16C processors are a 16-bit microcomputer family commonly used in embedded systems.  This page describes an [overview of the processor family](#family) and 
 a description of the [memory layout](#memory) of the M16C processors, useful in using the `pico-puller` tool.

## Processor family {#family} #

The M30624FGAFP/GP and M30624FGMFP/GP versions both have 256K of Flash memory, while the M30620FCAFP/GP and M30620FCMFP/GP have 128K.  The FP suffix indicates a 100P6S-A rectangular package with 30 pins on the long side and 20 on the short side, while a GP suffix indicates a square 100-pin QFP package (designated 100P6Q-A) with 25 pins one each side.

There are three standard ways to program or read the M16C; parallel, synchronous serial, asynchronous serial.  This project uses the synchronous serial mode, which Renesas calls Standard Serial Mode 1, by connecting some of the pins on the Pico board to the corresponding ones on the M16C processor or board.  

The synchronous serial protocol uses RXD1, TXD1, CLK1 to transfer data and RTS1 as a BUSY signal. Details of the protocol are in the Users Manual for the M16C/62A but a summary of the connections is here, but all of the documented commands are implemented and described [here](@ref commands). 

## Connections
Here are the connections from the Pico to the M16C system.  M16C pin numbers refer to square package/rectangular package.

| Pico name | Pico pin | M16C pin | M16C name |
|-----------|----------|----------|-----------|
| 3V3(OUT)  |  36      |  14/16   |  Vcc      |
|  GP5      |   7      |  32/34   |  BUSY     |
|  GP2      |   4      |  31/33   |  SCLK     |   
|  GP3      |   5      |  30/32   |  RXD      |   
| 3V3(OUT)  |  36      |  44/46   |  #CE      |
|  GND      |   3      |  39/41   |  #EPM     |
|  GND      |   3      |  63/64   |  Vss      |
|  GP6      |   9      |  10/12   |  #RESET   |
| 3V3(OUT)  |  36      |   7/9    |  CNVss    |
|  GP4      |   6      |  29/31   |  TXD      |  

## Memory layout {#memory} ##
The peripherals, RAM and Flash ROM are all mapped to memory space in the M16C.  The details of memory mapping and registers are described in the manual for the M16C.  Some of the essential details required for using the `pico-puller` softwarea are documented here.  

### Flash memory
The flash memory of the M16C processor is divided in to *blocks*.  The block are of different sizes and have different locations.  The summary for the 256K Flash part is below:

| Block | Size | Start |  End  |
|-------|------|-------|-------|
|  0    |  16K | FC000 | FFFFF |
|  1    |   8K | FA000 | FBFFF |
|  2    |   8K | F8000 | F9FFF |
|  3    |  32K | F0000 | F7FFF |
|  4    |  64K | E0000 | EFFFF |
|  5    |  64K | D0000 | DFFFF |
|  6    |  64K | C0000 | CFFFF |


For 128K parts, everything is the same except the first populated memory is at 0xE0000, so the first two 64K blocks (blocks 5 and 6) do not exist.