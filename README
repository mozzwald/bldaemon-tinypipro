#Backlight daemon for TinyPi Pro

based on [bldaemon-zipit](https://github.com/mozzwald/bldaemon-zipit)

Turns off the backlight after 25 seconds of button inactivity,
turns it back on after pressing any button. The pigpio library
is needed (`sudo apt install pigpio`) from raspbian on the pi
to build this little app. Change `LED_TIMEOUT` in bldaemon.c to 
whatever time you want and recompile with make.

Hardware Modifications Required:
(see tinypipro_backlight_mosfet.jpg](https://github.com/mozzwald/bldaemon-tinypipro/raw/master/tinypipro_backlight_mosfet.jpg))

P-Channel MOSFET DMP2035U (or similar)

Move R1 about 45 degrees so the source pin of the mosfet sits
on the pad and drain pin touches the resistor. A wire connects
the gate pin to the raspberry pi zero (w) GPIO 22 (Pin 15).
