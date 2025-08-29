import machine
from machine import Pin
import utime
import neopixel

NUM_LEDS = 10
ledpin = machine.Pin(10, Pin.OUT)
SCAN_PINS = (0, 1, 2)
READ_PINS = (3, 4, 5)

leds = neopixel.NeoPixel(ledpin, NUM_LEDS)

slider = machine.ADC(26)

n = 0

while True:
    n = n + 1
    reading = slider.read_u16()
    print(((reading / 65535) * 3.3))
    utime.sleep(1)
    for i in range(NUM_LEDS):
        leds[i] = (i * 8, 0, 0)
    # Update the strip.
    leds.write()
    if n > 1000:
        break
