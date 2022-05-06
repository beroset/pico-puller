# Commands {#commands} #

### Commands

The previously mentioned Standard Serial Mode 1 supports the following commands.  Note that before the ID is verified, only the ID check function and version data output function and status check function are accepted.

#### Page Read
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

