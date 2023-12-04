# hidftp

## Overview

In the corporate IT environment it is a common security practice to disable users' ability to connect removable media such as USB thumbdrives. It's motivated by the desire to prevent the execution of malicious software, and in some cases it is also intended to prevent leakage of sensitive data, whether intentional or accidental.

hidftp is a thought experiment on the topic of data exfiltration - intentional copying of data from a machine which does not allow removable media. It is implemented as a USB-connected SoC that presents itself as a Human Interface Device, and uses a Python ftp-like interface to list, download and upload files to an SD card.

**Disclaimer: this is intended for educational purposes only. Any attempts to circumvent corporate security policies may result in employment and legal penalties.**

## Setup

This project was created using VSCode/PlatformIO. Python client may have to be run with root privileges to access the USB input device. Tested on Ubuntu 20.04.

# Hardware

Target is a ESP32 S3 based development board with two USB connectors, such as ESP32-S3-DevKitC-1

SD card adapter wiring :

GPIO 36 -> SCK  
GPIO 35 -> MOSI  
GPIO 37 -> MISO  
3.3V -> 3.3 VDD  

![alt text](https://github.com/xba1k/hidftp/blob/main/hidftp.jpg?raw=true)

# Todo

Add better error handling. Make things faster. Test on OSX.

# Demo

```
[44347.329624] usb 1-1.3: new full-speed USB device number 23 using ehci-pci
[44347.452176] usb 1-1.3: New USB device found, idVendor=feed, idProduct=c0de, bcdDevice= 1.00
[44347.452180] usb 1-1.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[44347.452182] usb 1-1.3: Product: Data Exfil PoC
[44347.452184] usb 1-1.3: Manufacturer: xba1k
[44347.452185] usb 1-1.3: SerialNumber: 0000-0001
[44347.453491] hid-generic 0003:FEED:C0DE.0011: hiddev1,hidraw4: USB HID v1.11 Device [xba1k Data Exfil PoC] on usb-0000:00:1a.0-1.3/input0
```

```
# ./hidftp.py 
opened Data Exfil PoC
hidftp> lls
limon_receiver.py hidftp.py 
hidftp> ls  
fsck0000.000    FSCK0000.REC    ._.Trashes      .Trashes        .fseventsd      .Spotlight-V100 switch_mount.gcode      switch_mount_p2.gcode
hidftp> put hidftp.py
2193 bytes sent
hidftp> ls
fsck0000.000    FSCK0000.REC    ._.Trashes      hidftp.py       .Trashes        .fseventsd      .Spotlight-V100 switch_mount.gcode      switch_mount_p2.gcode
hidftp> get switch_mount_p2.gcode
629441 bytes received
hidftp> lls
switch_mount_p2.gcode limon_receiver.py hidftp.py 
hidftp> 
```