## Petya2 - Ransomware Open Source Project
This project is meant for Educational Purposes, CyberSecurity Researches and similar stuff. This repository doesn't include any downloadable Binaries as this Project acts the same as the actual Petya Ransomware Executable.

## What is Petya2 ?
Petya2 is an Open Source (manually coded by me) executable that (has to) acts the same as Petya Ransomware Executable, that has the role of writting the malicious bootloader & micro-kernel, setting up the encryption keys and data for the micro-kernel to work and make the decryption process successful.

#### How did I make this ?
Good question! This took me a good time to recognize and understand how Petya actually works.

Petya2 is made by Reverse Engineering Petya Ransomware, by reading lots of information and by Clean Room Reverse Engineering.

### How did I get Petya's Bootloader and Micro-Kernel ?
Petya Ransomware **overwrites** the ***M***aster ***B***oot ***R***ecord with its own Bootloader which with, when loaded, jumps and executes the **Sector 34** of the Disk.
Petya writes the Micro-Kernel at **Sector 34** and it is **16 Sectors Long** (16*512 Bytes Long). From there, you can simply extract what you want.


## Ransomware Structure
Petya Executable is actually structured like this:

   - Petya.exe - **Dropper**
      - Setup.dll - **Core**
      - ~~Mischa.dll~~ (Not present in this version)
   - Petya's Bootloader - **Real Mode Bootloader**
   - Petya's Micro-Kernel - **Real Mode Kernel**
 
 ## Ransomware Structure based on Sectors
 Petya will configure itself using Harddisk's Sectors as the following:
 
 #### Petya's actions
 1. Petya will make a copy of system's ***M***aster ***B***oot ***R***ecord, encrypt it with xor (hex 0x37, ansi '7') and place it in sector 56 as backup.
 2. The ***M***aster ***B***oot ***R***ecord is going to be overwritten with Petya's Bootloader but before that, a copy of the Partition Table will be created (found at offset 446 - 510 in sector 0) and placed on Petya's Bootloader for $MFT's Encryption Process.
 3. The Micro-Kernel is going to be placed at sector 34 (where it is going to be read, it is 16 sectors long).
 4. Petya will now encrypt every sector from sector 1 to sector 33 with XOR (hex 0x37, ansi '7').
 5. Now, the verification sector, which is going to be encrypted & decrypted by the ring 0 kernel, it is located at sector 55, Petya will XOR (hex 0x37, ansi '7') the whole sector and write the result in it.
 6. The last step, Configuring Sector 54. This sector is the most important one because Petya needs to read it (you'll see more down bellow).

## Sector 54 (Configuration Sector)
I made this small picture scheme of how the sector looks like
