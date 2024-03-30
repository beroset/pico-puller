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

## This project

This project is intended to allow a programmer to connect the Raspberry Pi Pico to a circuit containing a Renesas M16C/62A processor and extract the flash memory contents whether the ROM code protect function has locked the firmware or not.

## How to use it

### Step 1: Physical connections

First, make the following ten connections between the Pico and the circuit with the M16C processor.  M16C pin numbers refer to square package/rectangular package.

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

@image html pico_adapter.png "Adapter board"
@image latex pico_adapter.png "Adapter board" width=\textwidth

Typically, the board incorporating the M16C does not present a large current load, so in most cases both the circuit under test and the Raspberry Pi Pico can be powered by the USB connection.  In other words, no other power is usually needed. 

If the device is not incorporated into a circuit board, a minimal interface circuit (not including oscillator cicuit) is shown below.

@image html flash_circuit.png "Minimal flash circuit"
@image latex flash_circuit.png "Minimal flash circuit" width=\textwidth

### Step 2: Configure CMake for build
There are only required configuration item to define.  The first is to define a `PICO_MOUNT_LOCATION`.  This is the directory in which your Raspberry Pi Pico appears when first plugged in as an unprogrammed part.  On a Fedora Linux distribution this is `/run/media/username` where `username` is whatever name you use to log in.

The second is `PICO_SDK_PATH` which should point to wherever you've installed and built the [`pico-sdk`](https://github.com/raspberrypi/pico-sdk.git).

Optional but recommended are to build the documentation for this program (`WITH_DOC`) and whether to build a Release or Debug version.  A sample command line for this, if you are in `pico-puller` subdirectory is this:

```
cmake -DPICO_SDK_PATH=~/tools/pico/pico-sdk -DPICO_MOUNT_LOCATION=/run/media/beroset -DWITH_DOC=ON -DCMAKE_BUILD_TYPE=Release -S . -B build
``` 

This will create and configure the `build` subdirectory.

### Step 3: Build the software for the Pico
If the configuration described above has been done, building the software is very simple:

```
cmake --build build
```

### Step 4: Install the software
Once the software is built, copy the `pico-puller.uf2` program to the Pico.  This can be done automatically using `sudo cmake --build build -t install`.  That will also automatically install the `pico-puller` script on your machine.

### Step 5: Use the software
If everything was installed, using the software is very simple.  If you want to get a dump of the contents of the user flash, use this:

```
pico-puller dump
```

You can redirect this to a file for further analysis:

```
pico-puller dump > foo.hex
```

If you wish to covert that to a binary file, one simple way to do it is to use a combination of `awk` to remove all lines that don't start with a hex address and then feed the result to `xxd`.

```
awk '/^[0-9a-f]{8}:/' foo.hex |xxd -r -g -1 > foo.bin
```

Or if you only want the binary file, that can be done all in a single line:

```
pico-puller dump | awk '/^[0-9a-f]{8}:/'  |xxd -r -g -1 > foo.bin
```

Note that the script assumes that the ID for the M16C is `00 00 00 00 00 00 00`.  If it's anything other than this, you will need to supply the correct value before doing much else.

## Going further
That is a very quick introduction to generally how the `pico-puller` software works.  Here are links for more details:

- [M16C](@ref cpu)
- [Commands](@ref commands)
