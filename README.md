# 8051 IC Tester

AT89S52-based 74-series logic IC tester.

## Requirements

* SDCC
* `packihx`
* Python 3
* Python `pyserial` - start a vev, ```pip install pyserial```
* Arduino Uno used as ISP programmer: upload /src/at89s-isp.ino sketch to arduino uno

```bash
sudo apt install python3-serial
```

## Build 8051 firmware hex 

From `src/`:

```bash
mkdir -p build
sdcc --model-small -o ./build/ ./firmware_allics.c
packihx ./build/firmware_allics.ihx > ./build/firmware_allics.hex
```

The compiled firmware is: (pre-built at /src/build)

```text
build/firmware_allics.hex
```

## Flash 8051 Firmware hex

Connect the Arduino Uno ISP to the AT89S52, then run:

```bash
python3 flash.py ./build/firmware_allics.hex
```
