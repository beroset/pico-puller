# Introduction {#mainpage} #

This project is intended to allow a programmer to connect a computer with a USB port to a Raspberry Pi Pico to a circuit containing a Renesas M16C/62A processor and extract the flash memory contents whether the ROM code protect function has locked the firmware or not.

\startuml
:human: -> (computer): commands
(computer) -> (pico): relay commands via USB
(pico) -> (M16C): execute commands
(M16C) -> (pico) : response
(pico) -> (computer) : response
(computer) -> :human: : response
\enduml

## The M16C Processor

The Renesas M16C/62A processors are a 16-bit microcomputer family with 1M address space and multiple packaging and memory options.  It is often used in embedded systems.

## The Raspberry Pi Pico

The Raspberry Pi Pico is a small, inexpensive single-board microcontroller with dual ARM M0 cores and a number of other interesting features, including USB support.

## `pico-puller`
This software runs on a Raspberry Pi Pico.  Once it is physically connected to a target M16C processor, that M16C target can be manipulated via the software running on the Pico.  The `pico-puller` software presents a serial interface to the host computer's USB port, so a terminal emulator program such as [Cutecom](https://sourceforge.net/projects/cutecom/) is required to interact directly with the software.  Both the Pico and the connected M16C can potentially run from the power supplied via the USB port.

## Going further
That is a very quick introduction to generally how the `pico-puller` software works.  Here are links for more details:

- [M16C](@ref cpu)
- [Commands](@ref commands)
