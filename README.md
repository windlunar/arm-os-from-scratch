# BeagleboneLabOS

Hi ,<br><br>
## I'm trying to make a toy operating system on Beaglebone black in order to learn some OS concept and pratice my C languege skill.<br>
Currently ,this tiny kernal can switch between user tasks and kernel mode.


Enviroment Setup:
=

<br>
Need to prepare:
1. A PC with Ubuntu 18.04
2. Beaglebone black
3. TTL to USB converter
4. SD card with U-boot
<br>

<br><br>
2.Install "gcc-arm-none-eabi-6-2017-q2-update" to compile source code
<br>
3.Install "minicom"

        sudo apt-get install minicom

<br>
4. Move to your working space or any directory you like.<br>
5. clone this project:

        git clone https://github.com/windlunar/BeagleboneLabOS

<br>
6. Move to the BeagleboneLabOS folder , and comile all.

        make
<br>
7.Connect Beaglebone's UART0 to TTL-USB converter ,and plug TTL-USB converter to PC's usb port.


<br>
8. Open another terminal ,execute minicom:

        sudo minicom
<br>
Then you should see:

<br>        
9.Hold the button "S2" ,and power on beaglebone(through usb or 5V).
Then the board should boot up by U-boot which is in SD card.

<br>
10.Press space while you see "Press SPACE to abort autoboot in 2 seconds".
Then you will see the command line provided by U-boot

<br>
11.Type the following command at U-boot's commmand line to transfer binary file form pc to beaglebone through UART0.
And the elf will be uploaded to beaglebone's memory ,and place at 0x81000000
        loadx 0x81000000

<br>
12.And type "CTRL+A" and then "S" ,you should see:

<br>
13.Choose "xmodem"

<br>
13.Choose "[Goto]"

<br>
14.Go to : "</your workspace/>/BeagleboneLabOS/bin"

<br>
15.Choose the elf binary and wait for upload :beaglebonLabOS.elf

<br>
16.After upload successfully, type the command to execute the kernal:
        bootelf 0x81000000
You can see task1 started and and back to kernal mode by software interrupt.
And then run task2 ---SWI---->kernal
-------->run task3 ---SWI---->kernal
-------->run task1 ......
