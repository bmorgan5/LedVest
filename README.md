LED Vest
===========================

Purpose
-------

Because LEDs are awesome!


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

