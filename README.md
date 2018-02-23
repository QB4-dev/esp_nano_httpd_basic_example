# esp_nano_httpd_basic_example

Clone this repo with option `--recursive` to download __esp_nano_httpd__ submodule  
`git clone https://github.com/QB4-dev/esp_nano_httpd_basic_example --recursive`

Check SDK and toolchain paths in Makefile:
```Makefile 
# base directory for the compiler
XTENSA_TOOLS_ROOT ?= /opt/esp-open-sdk/xtensa-lx106-elf/bin

# base directory of the ESP8266 SDK package, absolute
SDK_BASE	?= /opt/esp-open-sdk/sdk
```
Compile it using:  
`make`

And flash into ESP8266 device:  
`make flash`

Then use your smartphone or computer and scan WiFi networks. `ESP-LED-XXXX` with no password should appear.

Connect to it, and type `http://192.168.4.1` address into web browser. 
Set your router SSID and password and try to access your device from your router network. 
Also check led demo - it's simple example web interface to configure LED blink frequency
