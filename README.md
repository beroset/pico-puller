# pico-puller

A Raspberry Pi Pico-based project for reading and programming the flash memory of Renesas M16C/62A processors.

## The M16C Processor

The Renesas M16C/62A processors are a 16-bit microcomputer family with 1M address space and multiple packaging and memory options.  It is often used in embedded systems.

## The Raspberry Pi Pico

The Raspberry Pi Pico is a small, inexpensive single-board microcontroller with dual ARM M0 cores and a number of other interesting features, including USB support.

## This project

This project is intended to allow a programmer to connect the Raspberry Pi Pico to a circuit containing a Renesas M16C/62A processor and extract the flash memory contents whether the ROM code protect function has locked the firmware or not.

## How to use it

Full details on how to build the project and use it are in the [manual](doc/pico-puller.pdf).

## Details 

The M30624FGAFP/GP and M30624FGMFP/GP versions both have 256K of Flash memory, while the M30620FCAFP/GP and M30620FCMFP/GP have 128K.  The FP suffix indicates a 100P6S-A rectangular package with 30 pins on the long side and 20 on the short side, while a GP suffix indicates a square 100-pin QFP package (designated 100P6Q-A) with 25 pins one each side.

There are three standard ways to program or read the M16C; parallel, synchronous serial, asynchronous serial.  This project uses the synchronous serial mode, which Renesas calls Standard Serial Mode 1, by connecting some of the pins on the Pico board to the corresponding ones on the M16C processor or board.  

The synchronous serial protocol uses RXD1, TXD1, CLK1 to transfer data and RTS1 as a BUSY signal. Details of the protocol are in the Users Manual for the M16C/62A. 
