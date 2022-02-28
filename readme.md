## Petya2 - Ransomware Open Source Project
This project is meant for Educational Purposes, CyberSecurity Researches and similar stuff. This repository doesn't include any downloadable Binaries as this Project acts the same as the actual Petya Ransomware Executable.

## What is Petya2 ?
Petya2 is an Open Source (manually coded) executable that acts the same as Petya Ransomware Executable, that has the role of writting the malicious bootloader & micro-kernel, setting up the encryption keys and data for the micro-kernel to work and make the decryption process successful.

#### How did I make this ?
Good question! This took me a good time to recognize and understand how Petya actually works.

Petya2 is made by Reverse Engineering Petya Ransomware, by reading lots of information and by Clean Room Reverse Engineering.

### How did I get Petya's Bootloader and Micro-Kernel ?
Petya Ransomware **overwrites** the ***M***aster ***B***oot ***R***ecord with its own Bootloader which with, when loaded, jumps and executes the **Sector 34** of the Disk.
Petya writes the Micro-Kernel at **Sector 34** and it is **16 Sectors Long** (16*512 Bytes Long). From there, you can simply extract what you want.


