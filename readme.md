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
![sector54](https://user-images.githubusercontent.com/68382500/156068841-1dca1305-babf-4a60-880b-02eb50f8c7c3.png)

## How does all of that crap work?
#### First byte from the sector
First byte of the sector will tell the Micro-Kernel, the stage of your computer.
If that field is set to **0x00**, Petya's Micro-Kernel will know that the system hasn't been encrypted yet, will generate the Fake Chkdsk screen that will encrypt the $MFT and after that, the field is going to be set to **0x01**.

If the field is set to **0x01**, Petya's Micro-Kernel will know that the system is at the moment encrypted and will display the flashing skull followed by the payment screen.

Once you enter a valid decryption key in the payment screen, the field is going to be set to **0x02** (which means that the system has been dercypted).

#### Decryption key - Next 32 Bytes from the sector
The next 32 Bytes is actually the decryption key (that you'll need to write in Petya's payment screen) that is encoded. The process is not really that complex, it is clearly explained in the picture placed upper.

The decryption key is generated random, it is a 16 bytes long key generated from this charset "123456789abcdefghijkmnopqrstuvwxABCDEFGHJKLMNPQRSTUVWX". If the key doesn't follow this charset, it is claimed as being invalid.

After generating the 16 bytes long key, Petya will encode the key using a really strange algorithm:
```
bool encode(char* key, BYTE *encoded)
{
    if (!key || !encoded) {
        printf("Invalid buffer\n");
        return false;
    }
    size_t len = strlen(key);
    if (len < 16) {
        printf("Invalid key\n");
        return false;
    }
    if (len > 16) len = 16;

    int i, j;
    i = j = 0;
    for (i = 0, j = 0; i < len; i++, j += 2) {
        char k = key[i];

        encoded[j] = k + 'z';
        encoded[j+1] = k * 2;
    }
    encoded[j] = 0;
    encoded[j+1] = 0;
    return true;
}
```
(Thanks Malwarebytes - Hasherezade)

This algorithm will convert the 16 bytes long key into a 32 bytes long key. This 32 bytes long key will be placed in the Configuration Sector for future Salsa20 Encryption usage.

Petya's Micro-Kernel is going to Encrypt the Sector 55 (Verification Sector) with 256-bit Salsa20 with this 32 bytes long key and with the Initialization Vector (down bellow).


### Initialization Vector - Next 8 bytes
This key is a randomized 8 bytes long key, which is going to be used for encryption.

### Sector 55 ... What is that ?
The Sector 55 is a verification sector for the Ransomware. Everytime you enter a key in Petya's payment screen, Petya will do a check for that key. The key will pass **ONLY IF** the key is 16 bytes long and **IF** decrypting Sector 55 with this key encoded using the algorithm function shown upper (the one that makes the key 32 bytes long) and using the Initialization Vector key will result in a bunch of 0x37 hex values or ansi '7', the key will pass and the $MFT Decryption Process will begin.


