# OS
Trying to make an os, because i am so good in Assembly (and forcing Kasper to join me)


## Compiling the OS
After some time, there will probably be some real releases here, which you can just run in any vm that takes .bin files. 
But for now, you have to compile this yourself if you want to run it, so i can give a quick tutorial. 

You have to start with downloading the necessary tools, NASM, QEMU, GCC and WSL (Windows subsystem for Linux), because all of this is much easier with the linux terminal.
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
Save the file, close and reopen WSL then navigate to the folder where you have the project downloaded, and run the command
```bash
make
```
The makefile in the project will then do all of the building for you, and the OS will open in QEMU, the vm.
