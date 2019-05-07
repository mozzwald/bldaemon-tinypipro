# Backlight daemon for TinyPi Pro

based on [bldaemon-zipit](https://github.com/mozzwald/bldaemon-zipit)

Turns off the backlight after 25 seconds of button inactivity,
turns it back on after pressing any button. The bcm2835 gpio library
is required for this to build. Build it on the raspberry pi:

```
cd ~
mkdir build
cd build
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.58.tar.gz
tar -xvf bcm2835-1.58.tar.gz
cd bcm2835-1.58
./configure --prefix=/usr
make
sudo make install
```

Get the bldaemon source code and build/install it:

```
cd ~/build
git clone https://github.com/mozzwald/bldaemon-tinypipro
cd bldaemon-tinypipro
# <optional: edit bldaemon.c and change LED_TIMEOUT>
make
sudo make install
```

Change `LED_TIMEOUT` in bldaemon.c to whatever time you want and
compile with `make`.

## Hardware Modifications Required:
(see [tinypipro_backlight_mosfet.jpg](https://github.com/mozzwald/bldaemon-tinypipro/raw/master/tinypipro_backlight_mosfet.jpg))

P-Channel MOSFET DMP2035U (or similar)

Move R1 about 45 degrees so the source pin of the mosfet sits
on the pad and drain pin touches the resistor. A wire connects
the gate pin to the raspberry pi zero (w) GPIO 22 (Pin 15).
