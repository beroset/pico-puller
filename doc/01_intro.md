# Introduction {#mainpage} #

This project is intended to allow a programmer to connect the Raspberry Pi Pico to a circuit containing a Renesas M16C/62A processor and extract the flash memory contents whether the ROM code protect function has locked the firmware or not.

## The M16C Processor

The Renesas M16C/62A processors are a 16-bit microcomputer family with 1M address space and multiple packaging and memory options.  It is often used in embedded systems.

## The Raspberry Pi Pico

The Raspberry Pi Pico is a small, inexpensive single-board microcontroller with dual ARM M0 cores and a number of other interesting features, including USB support.

## Details 

The M30624FGAFP/GP and M30624FGMFP/GP versions both have 256K of Flash memory, while the M30620FCAFP/GP and M30620FCMFP/GP have 128K.  The FP suffix indicates a 100P6S-A rectangular package with 30 pins on the long side and 20 on the short side, while a GP suffix indicates a square 100-pin QFP package (designated 100P6Q-A) with 25 pins one each side.

There are three standard ways to program or read the M16C; parallel, synchronous serial, asynchronous serial.  This project uses the synchronous serial mode, which Renesas calls Standard Serial Mode 1, by connecting some of the pins on the Pico board to the corresponding ones on the M16C processor or board.  

The synchronous serial protocol uses RXD1, TXD1, CLK1 to transfer data and RTS1 as a BUSY signal. Details of the protocol are in the Users Manual for the M16C/62A. 

### Commands

The previously mentioned Standard Serial Mode 1 supports the following commands.  Note that before the ID is verified, only the ID check function and version data output function and status check function are accepted.

#### Page Read

\startuml
pico -> M16C : <FF><addr_mid><addr_hi>
pico <- M16C : [256 data bytes] 
\enduml

#### Program Page

\startuml
pico -> M16C : <41><addr_mid><addr_hi>[256 data bytes] 
\enduml

#### Block Erase

\startuml
pico -> M16C : <20><addr_mid><addr_hi><D0>
\enduml

#### Erase All Unlocked Blocks

\startuml
pico -> M16C : <A7><D0>
\enduml

#### Read Status Register

\startuml
pico -> M16C : <70>
pico <- M16C : [ status reg ][ status reg 1 ]
\enduml

#### Clear Status Register

\startuml
pico -> M16C : <50>
\enduml

#### Read Lock Bit Status

\startuml
pico -> M16C : <71><addr_mid><addr_hi>
pico <- M16C : [ lock bit data ]
\enduml

#### Lock Bit Program

\startuml
pico -> M16C : <77><addr_mid><addr_hi><D0>
\enduml

#### Lock Bit Enable

\startuml
pico -> M16C : <7A>
\enduml

#### Lock Bit Disable

\startuml
pico -> M16C : <75>
\enduml

#### ID Check Function

\startuml
pico -> M16C : <F5><addr_lo><addr_mid><addr_hi><ID size><id1><id2><id3><id4><id5><id6><id7>
\enduml

#### Download Function

\startuml
pico -> M16C : <FA><size_lo><size_hi><checksum>[data input]
\enduml

#### Version Data Output Function

\startuml
pico -> M16C : <FB>
pico <- M16C : [8 ASCII data bytes]
\enduml

#### Boot ROM Area Output Function

\startuml
pico -> M16C : <FC><addr_mid><addr_hi>
pico <- M16C : [256 data bytes] 
\enduml

#### Read Check Data

\startuml
pico -> M16C : <FD>
pico <- M16C : <addr_mid><addr_hi>
\enduml

