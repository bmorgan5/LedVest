LED Vest
===========================

About
-------

I built a 25x24 pixel LED Vest to wear at Burningman 2016!

[![LedVest](https://img.youtube.com/vi/wfAjaYMx2MA/0.jpg)](https://www.youtube.com/watch?v=wfAjaYMx2MA)

Parts List
----------

[2x 300 WS2812B LED Strip] (https://www.amazon.com/gp/product/B018X04ES2/ref=oh_aui_detailpage_o08_s00?ie=UTF8&psc=1)

[Teensy 3.2] (https://www.pjrc.com/store/teensy32.html)

[2x RAVPower Battery Pack] (https://www.amazon.com/gp/product/B00HFMUBYG/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1)

[DROK Step Down Voltage Regulator] (https://www.amazon.com/gp/product/B00C4QVTNU/ref=oh_aui_detailpage_o04_s00?ie=UTF8&psc=1)

[25mm Fan] (https://www.amazon.com/gp/product/B00MYNWUP4/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1)

Optional Parts:

For Testing at Home - [DROK Step Down Voltage Regulator with LED Voltage Display] (https://www.amazon.com/gp/product/B015Z02TE4/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1)

For Charging in the Desert - [ALLPOWERS 18W Portable Solar Charger Panel] (https://www.amazon.com/gp/product/B00G6CDTGS/ref=oh_aui_detailpage_o05_s00?ie=UTF8&psc=1)


Setup
-----

Install the Teensy udev rule: `sudo cp tools/49-teensy.rules /etc/udev/rules.d/`

Then unplug your Teensy and plug it back in.


Using
-----

1. Put your code in `src/main.cpp`
2. Put any libraries you need in `libraries`
3. Set the TEENSY variable in `Makefile` according to your teensy version
4. Build your code ```make```
5. Upload your code ```make upload```


Make Targets
------------

- `make` alias for `make hex`
- `make build` compiles everything and produces a .elf
- `make hex` converts the elf to an intel hex file
- `make post_compile` opens the launcher with the correct file
- `make upload` uploads the hex file to a teensy board
- `make reboot` reboots the teensy


Thanks to Austin Morton for the template
----------------------------------------
Teensy 3.X Project Template
github.com/apmorton/teensy-template


Where everything came from
--------------------------

- First download your favorite Arduino src and install [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
- The `teensy3` sub-folder is taken from <path-to-arduino-x.y.z>/hardware/teensy/avr/cores/teensy3
- The `tools` sub-folder is taken from <path-to-arduino-x.y.z>/hardware/tools and/or [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
- The `src/main.cpp` file is moved, unmodified from `teensy3/main.cpp`
- The `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)

Modifications to `Makefile` include
- Add support for arduino libraries
- Change tools directory
- Calculate target name from current directory
- Prettify rule output
- Do not upload by default, only build

