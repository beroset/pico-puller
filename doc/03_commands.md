# Commands {#commands} #

The `pico-puller` program supports the following commands.

## Command summary ##
When the program starts, it shows the menu of choices which are also summarized here

|  Command  | argument(s)                 | description                      |
|-----------|-----------------------------|----------------------------------|
| reset     | 0 or 1                      | release or assert CPU's reset
| set\_speed | freq                        | change the clock frequency
| version   |                             | get 8-char version string
| id        | id1 ... id7 [addr]          | check up to 7-byte ID
| dump      |                             | dump entire flash contents
| bdump     |                             | dump entire boot flash contents
| status    |                             | read status register
| rpage     | addr                        | read 256-byte page
| bpage     | addr                        | read 256-byte bootloader page
| clear     |                             | clear status register
| checkread |                             | read CRC of written data
| lockread  | addr                        | read lock bit
| lockwrite | addr                        | write lock bit
| enable    |                             | enable lock bit
| disable   |                             | disable lock bit
| eblock    | addr                        | erase block
| ppage     | addr byte0 ... byte255      | program the given page at addr
| ramload   | addr byte0 ... byteXX       | download into RAM at addr and execute
| all\_erase |                             | erase all unlocked blocks
| help      |                             | display this help
| quit      |                             | reset Pico to program mode

The typical sequence of commands would be first to release the CPU from reset using the `reset 0` command.  Then one could issue the `version` command to verify that the device is correctly connected and operating in the Serial Mode 1 as described in the Renesas documentation.  You should see an 8-byte string that looks something like "VER.4.04".  If instead you see all 0xff characters, or `<break x 008>`, something is wrong: either the wiring is not correct or your device doesn't support Serial Mode 1 or is still in reset.

One can then set a higher clock speed if the device supports it.  The default is 1MHz, so the clock transitions every 500 nanoseconds, but the the Pico is capable of going much faster.  Note that the argument is a floating point number in hertz, not megahertz, so `set_speed 2.5e6` sets the clock to 2.5MHz.  Higher speeds might not be supported by the M16C, depending on the frequency of its clock.  The `version` command is a handy way to tell if the faster speed is working or not.

The next command is the `id` command.  When the M16C Flash memory is written, by default, the 7-byte ID code is `00 00 00 00 00 00 00`.  If this has been changed, or the verification address has been changed, the `id` command will fail to unlock the device and it will not be possible to read, program or erase it.  To determine whether the command has successfully unlocked the device, one can use the `status` command.  As described below, it returns two status bytes.  If the second one has bits 2 and 3 set (a bit mask of 0x0c) then the verification code is correct and SRD1 will have a value of something like 0x6c.  If not, it will have bit 2 set, but bit 3 clear and might return a value of 0x64.

Many of the reading and writing commands require that the ID is verified first. Before the ID is verified, only the following commands are accepted:

 - `id`
 - `status`
 - `version`
 - `set_speed`
 - `reset`
 - `help`
 - `quit`

If the ID has been verified, then one can dump the contents of the user Flash memory using the `dump` command.  This assumes that the part is the 256K Flash part, and so it starts at address 0xc0000, but it will also work for the 128K version, but the first 128K will consist solely of 0xff bytes.

One can also dump the boot flash using the `bdump` command.

## Command details
This describes each command in detail and also shows an sequence diagram for each of the commands that involve communication of bytes between the Pico and the M16C.

### Reset
Reset sets the level of the line according to the supplied argument.  The `reset 1` command holds the CPU in reset (that is, it drives the \#RESET line low) while `reset 0` releases the CPU from the reset condtion (driving the \#RESET line high).

Example:
```
> reset 0
CPU is not in reset
> reset 1
CPU is in reset
```

### Page Read
Read a 256-byte page of flash memory and dump it to the screen in hex.

Example:
```
> rpage cf000
000cf000: 00 32 dd 63 8d 79 b3 71 d3 ed 73 14 23 35 72 44
000cf010: f3 38 33 21 98 30 33 2e 39 38 20 00 00 00 70 22
 etc.
```

\startuml
pico -> M16C : <FF><addr_mid><addr_hi>
pico <- M16C : [256 data bytes]
\enduml

### Program Page
Write a 256-byte page of memory to flash memory.

Example:
```
> ppage cf000 00 32 dd 63 8d 79 b3 71 d3 ed 73 14 23 35 72 44 ... etc.
```

\startuml
pico -> M16C : <41><addr_mid><addr_hi>[256 data bytes]
\enduml

### Block Erase
Erase the block of memory containing the address.  Note that Renasas documents say that the address should be the highest address (end address) of the block.  See the [M16C](@ref cpu) page for block addresses.

Example:
```
> eblock f7fff
```
This example erases the 32K block spanning 0xf0000 to 0xf7fff.  Verify any erase command by examing the `status`.

\startuml
pico -> M16C : <20><addr_mid><addr_hi><D0>
\enduml

### Erase All Unlocked Blocks ###
Erase all unlocked blocks.  All blocks not protected with the lock bit will be erased by this command.

Example:
```
> all_erase
```

\startuml
pico -> M16C : <A7><D0>
\enduml

### Read Status Register
Read the status registers SRD and SRD1 as defined below:

#### SRD {#srd} #
| bit | meaning                        |
|-----|--------------------------------|
|  7  | write state machine ready      |
|  6  | reserved                       |
|  5  | erase operation error          |
|  4  | program operation error        |
|  3  | program data corrupted         |
|  2  | reserved                       |
|  1  | reserved                       |
|  0  | reserved                       |

#### SRD1 {#srd1} #
| bit | meaning                        |
|-----|--------------------------------|
|  7  | boot update complete           |
|  6  | reserved                       |
|  5  | reserved                       |
|  4  | download checksum OK           |
|  3  | ID check verified OK           |
|  2  | ID check attempted             |
|  1  | data receive timeout           |
|  0  | reserved                       |

Example:
```
> status
> SRD = 0x80, SRD1 = 0x60
```

\startuml
pico -> M16C : <70>
pico <- M16C : [ status reg ][ status reg 1 ]
\enduml

### Clear Status Register
The `clear` command issues a request to clear the error bits in the [SRD](@ref srd), specifically, bit 5, "erase operation error", bit 4, "program operation error", and bit 3, "program data corrupted".

\startuml
pico -> M16C : <50>
\enduml

### Read Lock Bit Status
Each of the seven flash memory [blocks](@ref blocks) has an associated non-volatile lock bit.  After the `disable` command, all blocks are unlocked regardless of lock bit status.  After the `enable` command, the bits can be set (unlocked) or cleared (locked) using the `lockwrite` command.

The `lockread` command reads these lock bits.  Note that the address should be the highest address in the block, and that the data is in bit 6, so an unlocked block will show a value of 0x40.

Example:
```
> lockread cffff
Lock status for Block at 0xcffff = 0x40
```

\startuml
pico -> M16C : <71><addr_mid><addr_hi>
pico <- M16C : [ lock bit data ]
\enduml

### Lock Bit Program
After the `enable` command, each block's lock bits cleared (locked) using the `lockwrite` command.

Example:
```
> lockread cffff
Lock status for Block at 0xcffff = 0x40
> lockwrite cffff
Locking Block at 0xcffff
> lockread cffff
Lock status for Block at 0xcffff = 0x0
```

\startuml
pico -> M16C : <77><addr_mid><addr_hi><D0>
\enduml

### Lock Bit Enable
The `enable` command enables the block locking mechanism, but does not alter any lock bits.  See the "Lock Bit Program" command listed above.  This is the default condition on leaving RESET.

\startuml
pico -> M16C : <7A>
\enduml

### Lock Bit Disable
The `disable` command disables the block locking mechanism but does not alter any lock bits.

\startuml
pico -> M16C : <75>
\enduml

### ID Check Function
The ID check function enables the other read, write and erase functions.  After a successful invocation, bits 2 and 3 of the [SRD1](ref srd1) are both set.  If the invocation is unsuccessful (that is, if the id bytes do not match those programmed into the Flash), bit 2 is set but bit 3 is cleared.

Example:
```
> id 00 00 00 00 00 00 01
Attempting id = 00 00 00 00 00 00 01
> status
SRD = 0x80, SRD1 = 0x64
> id 00 00 00 00 00 00 00
Attempting id = 00 00 00 00 00 00 00
> status
SRD = 0x80, SRD1 = 0x6c
```
In this example, the correct ID is all zero bytes, but we try an incorrect code first.  The [SRD1](@ref srd1) bit 2 is set and bit 3 is clear (the '4' part of 0x64) indicating a failed attempt.  Next we try the correct ID and verify that afterwards, bits 2 and 3 are both set (the 'c' part of 0x6c) indicating that the ID has been validated.

\startuml
pico -> M16C : <F5><addr_lo><addr_mid><addr_hi><ID size><id1><id2><id3><id4><id5><id6><id7>
\enduml

### Download Function
The `ramload` function allows one to download a program into RAM and execute it.  The program sent is downloaded to the address 0x600 and begins executing as soon as the data is transferred and the checkum verified.

\startuml
pico -> M16C : <FA><size_lo><size_hi><checksum>[data input]
\enduml

### Version Data Output Function
The `version` command get the version of the bootloader that is currently running.

Example:
```
> version
Version: "VER.4.04"
```

\startuml
pico -> M16C : <FB>
pico <- M16C : [8 ASCII data bytes]
\enduml

### Boot ROM Area Output Function
The `bpage` function works just like the `rpage` function but extracts bootloader pages rather than user Flash pages.  The bootloader is an addition 8K of ROM area that is mapped to 0xfe000 through 0xfffff.  The bootloader is activated when the CNVss pin and \#CE pin are both high when \#RESET goes high; otherwise it is not mapped into the M16C's address space.  This section may only be programmed in parallel mode (that is, in a special Flash programmer tool).

\startuml
pico -> M16C : <FC><addr_mid><addr_hi>
pico <- M16C : [256 data bytes]
\enduml

### Read Check Data
The `checkread` function reads back the CRC of the data that was sent with a page program command.

\startuml
pico -> M16C : <FD>
pico <- M16C : <addr_mid><addr_hi>
\enduml

### Help
The `help` command simply reprints the main menu and enumerates all of these commands.

### Quit
The `quit` command stops the Pico's program and puts into mass storage mode where it appears to the computer to be a USB memory stick.  This allows the Pico to be reprogrammed simply by copying a file.

## Other reading

- [Introduction](@ref mainpage)
- [M16C](@ref cpu)
