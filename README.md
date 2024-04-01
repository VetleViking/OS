# OS
Trying to make an os, because it looks fun, and want to learn more about C and ASM.


## Running the OS
You can download the latest version i think is production ready from the releases, which you can just run in any vm that takes .iso files. 

## Compiling the OS
If you want to try the absolute lastest i am working on or mess around and edit the os yourself, you can compile this yourself. I can give a quick tutorial on how to do that. 

You have to start with downloading the necessary tools, NASM, QEMU, GCC, GRUB and WSL (Windows subsystem for Linux), because all of this is much easier with the linux terminal.
We will start with WSL, so that we can download the other stuff there:
```bash
wsl --install
```
After this you will need to restart, then open the wsl terminal. Now we will download all of the other tools. 
The other tools can be easily downloaded by downloading the linked file from [this github page](https://github.com/mell-o-tron/MellOs/blob/main/A_Setup/setup-gcc-debian.sh), then navigating to the file in WSL and running 
```bash
./setup-gcc-debian.sh
```
This will take a while. After the downloads are done, we will add them GCC folder to path. The folder usually is at 
```bash
/usr/local/i386elfgcc/bin
```
Make sure that i386-elf-gcc and i386-elf-ld are there, and then go into the path-variables file with
```bash
sudo vim ~/.bashrc
```
Go to the bottom, and add 
```bash
export PATH="$PATH:/usr/local/i386elfgcc/bin"
```
Its way too much to explain here how vim works, so eiter search it up or use an editor you already know. 
The only things you need here are insert and quit though, so i can explain that. To enter insert mode, you press i. To exit insert mode you press esc.
to quit, you exit insert mode, and then write :wq to write and quit.


Save the file, close and reopen WSL. Now well download GRUB. Its three commands, and you will need to do them in this order:
```bash
sudo apt install grub2-common
sudo apt install xorriso
sudo apt-get install grub-pc-bin
```
Now, navigate to the folder where you have the project downloaded in WSL, and run the command
```bash
make
```
The makefile in the project will then do all the rest of the building for you, and the OS will open in QEMU, the vm.

## Sources
These are the sources i used the most, arranged from oldest to most recently used.

[Playlist on YT](https://www.youtube.com/playlist?list=PLm3B56ql_akNcvH8vvJRYOc7TbYhRs19M)<br>
13/01/2024 - 16/01/2024<br>
Used for finding out basic stuff and getting started.<br>

[OSDev](https://wiki.osdev.org/Main_Page)<br>
13/01/2024 - infinity and beyond<br>
Generally just nice to have, has info on most things.<br>

[OSDev barebones tutorial](https://wiki.osdev.org/Bare_Bones)<br>
17/01/2024 - 18/01/2024<br>
Concidering rewriting the project, reading up here on how i could do it better.<br>

[OSDev meaty skeleton tutorial](https://wiki.osdev.org/Meaty_Skeleton)<br>
18/01/2024 - 19/01/2024<br>
Finished barebones, looking at this next.<br>

[Osdever GDT](http://www.osdever.net/bkerndev/Docs/gdt.htm)<br>
[Osdever IDT](http://www.osdever.net/bkerndev/Docs/idt.htm)<br>
[Osdever ISRS](http://www.osdever.net/bkerndev/Docs/isrs.htm)<br>
26/01/2024 - 28/01/2024<br>
Trying to understand how the GDT, IDT and ISRS works.<br>

[OSDev BGA](https://wiki.osdev.org/Bochs_VBE_Extensions)<br>
23/03/2024 - 23/03/2024<br>
Implementing some better graphics for VMs like QEMU.<br>
