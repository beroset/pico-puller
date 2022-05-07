The `scripts` directory contains some `expect` scripts that make it easy to work with `pico-puller`.  The basic one is called `pico-puller` and is intended to be used with `expect` on a Linux machine.  It opens the `/dev/ttyACM0` device (which we assume is where the Pico is attached) and allows you to interact with the program via its internal menu.  Try `help` to display the menu.

Here is a list of all of the scripts:

| Name            | Description                                     |
|-----------------|-------------------------------------------------|
| `pico-puller`   | basic interactive mode to use the menu system
| `log2prog`      | transforms the log created by `dump` into programming script
| `dump.cmd`      | a script to dump the user flash 
| `readlocks.cmd` | a script to read all of the block lock bits


## Examples
Here are some examples of how these scripts can be used.  

### `pico-puller`
The first one shows how one might interact with the menu using `pico-puller`:

```
Connecting to pico, exit with ~.
> help
pico-puller Main Menu:
version 
	get 8-char version string
id id1 ... id7 [addr]
	check up to 7-byte ID
bpage addr
	read 256-byte bootloader page
rpage addr
	read 256-byte page
bdump 
	dump entire boot flash contents
dump 
	dump entire flash contents
status 
	read status register
clear 
	clear status register
checkread 
	read CRC of written data
lockread addr
	read lock bit
lockwrite addr
	write lock bit
enable 
	enable lock bit
disable 
	disable lock bit
eblock addr
	erase block
ppage addr byte0 ... byte255
	program the given page at addr
ramload addr byte0 ... byteXX
	download into RAM at addr and execute
all_erase 
	erase all unlocked blocks
set_speed freq
	change the clock frequency
reset 0|1
	release or assert CPU's reset
help 
	display this help
quit 
	reset Pico to program mode
> reset 0
> CPU is not in reset
> status
SRD = 0x80, SRD1 = 0x6c
> bpage fffff
Reading from 0xfffff

000fff00: 14 72 2f 40 05 e8 82 72 2f 41 05 e8 82 72 2f 42
000fff10: 05 0b 40 05 b3 7c e0 06 03 43 05 72 fa 40 05 14
000fff20: c4 b0 f5 39 00 f4 4e ff 5d 01 68 0f 7e 9f bd 1d
000fff30: 7e 8f b9 1d 7e 9f b9 1d fe 0d 7e 9f bd 0d 7e 8f
000fff40: b9 0d 7e 9f b9 0d 75 80 fc ff 0f 75 81 fe ff 0f
000fff50: 91 10 d1 f0 6e 07 4a 01 4b 01 49 0c 72 af 14 a9
000fff60: 03 b7 ad 03 b7 a8 03 c7 05 a8 03 c7 04 ac 03 b7
000fff70: b0 03 b7 ad 03 f3 04 04 ff ff ff ff ff ff ff ff
000fff80: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
000fff90: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
000fffa0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
000fffb0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
000fffc0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
000fffd0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 4d
000fffe0: ff ff ff 53 ff ff ff ff ff ff ff 43 ff ff ff 38
000ffff0: 20 f0 0f 34 ff ff ff 33 20 f0 0f 35 20 f0 0f ff
> status
SRD = 0x80, SRD1 = 0x6c
> ~. 
```

Note that to exit the program, you should type `~.` and then Enter (or end-of-file, typically ^D on Linux).

## `pico-puller` known commands
The `pico-puller` program has two known commands that are built in.  If invoked with `pico-puller dump` it will set the speed to 2MHz, release reset, check the status, issue an `id` command with all zeroes, and then checks the status again and, if all is well, dumps the user flash.

## `pico-puller` script mode
The other way to use `pico-puller` is in script mode.  In this mode, it will read commands from a file and execute them one by one as though you had typed them from the command line.  

Example:
```
pico-puller readlocks.cmd
CPU is not in reset
> SRD = 0x80, SRD1 = 0x6c
> Lock status for Block at 0xcffff = 0x0
> Lock status for Block at 0xdffff = 0x0
> Lock status for Block at 0xeffff = 0x0
> Lock status for Block at 0xf7fff = 0x40
> Lock status for Block at 0xf9fff = 0x0
> Lock status for Block at 0xfbfff = 0x0
> Lock status for Block at 0xfffff = 0x0
> 
```

This script clears the reset line, reads the current status and then reads the lock bits on each of the seven blocks.  Note, however, that it does no error checking, so that if, for instance, the SRD value returned by `status` had been 0x0, indicating that 

## `dump.cmd` command file
The `dump.cmd` file attempts to do the same thing as the `dump` command, but has no error checking.  It's included here to show how such a script might be written, but the better solution is to use the built-in function.
