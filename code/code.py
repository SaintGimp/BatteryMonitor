import time
import board
import displayio
import terminalio
from digitalio import DigitalInOut, Direction
from adafruit_display_text import label
import adafruit_displayio_ssd1306
import adafruit_ina260

first_line_y = 3
line_height = 11
line_glyphs = 63

# These voltage ranges assume that we have minimal voltage
# drop between the battery terminals and the monitor
battery_voltage_green = 12.2
battery_voltage_yellow = 11.9
battery_voltage_red = 11.7
battery_voltage_cutoff = 11.5

i2c = board.I2C()

# With these settings, the readings will update every 1055 ms
# As of 10/29/2020, the INA260 library has a bug where the averaging count constant is off by one,
# so COUNT_16 is really 64 samples
ina260 = adafruit_ina260.INA260(i2c)
ina260.mode = adafruit_ina260.Mode.CONTINUOUS
ina260.averaging_count = adafruit_ina260.AveragingCount.COUNT_16
ina260.voltage_conversion_time = adafruit_ina260.ConversionTime.TIME_8_244_ms
ina260.current_conversion_time = adafruit_ina260.ConversionTime.TIME_8_244_ms

displayio.release_displays()
display_bus = displayio.I2CDisplay(i2c, device_address=0x3C)
display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=128, height=32)

display_group = displayio.Group(max_size=3)
display.show(display_group)

line1 = label.Label(terminalio.FONT, color=0xFFFF00, max_glyphs=line_glyphs, line_spacing=0.85, x=0, y=first_line_y)
display_group.append(line1)

# For Adafruit M0 Express:
# power_enable = DigitalInOut(board.D5)
# green_led = DigitalInOut(board.D6)
# yellow_led = DigitalInOut(board.D9)
# red_led = DigitalInOut(board.D10)

# Pinout for Particle Xenon
power_enable = DigitalInOut(board.D2)
power_enable.direction = Direction.OUTPUT
power_enable.value = False
green_led = DigitalInOut(board.D3)
green_led.direction = Direction.OUTPUT
green_led.value = False
yellow_led = DigitalInOut(board.D4)
yellow_led.direction = Direction.OUTPUT
yellow_led.value = False
red_led = DigitalInOut(board.D5)
red_led.direction = Direction.OUTPUT
red_led.value = False

# We'd like to turn off the RGB led on the board, which is currently
# defaulting to white, but that's not possible yet (https://github.com/adafruit/circuitpython/issues/2707)

#line1.text = "Saint Gimp\nBattery monitor"
#time.sleep(1.055)

voltage_low_point = 1000
amp_hours = 0.0
energy = 0.0
then = time.monotonic()

while True:
    time.sleep(1.055)

    now = time.monotonic()
    elapsed_time = now - then
    elapsed_hours = (elapsed_time / 60 / 60)
    then = now

    voltage = ina260.voltage
    current = ina260.current / 1000
    power = ina260.power / 1000
    amp_hours = amp_hours + (current * elapsed_hours)
    energy = energy + (power * elapsed_hours)
    
    line1.text = "{:.2f}".format(voltage) + " V  " + "{:.3f}".format(current) + " A\n" + "{:.2f}".format(power) + " W\n" + "{:.3f}".format(amp_hours) + " Ah  " + "{:.2f}".format(energy) + " Wh"

    if voltage < voltage_low_point:
        voltage_low_point = voltage

    if voltage_low_point > battery_voltage_green:
        green_led.value = True
        yellow_led.value = False
        red_led.value = False
    elif voltage_low_point > battery_voltage_yellow:
        green_led.value = False
        yellow_led.value = True
        red_led.value = False
    elif voltage_low_point > battery_voltage_red:
        green_led.value = False
        yellow_led.value = False
        red_led.value = True
    else:
        green_led.value = False
        yellow_led.value = False
        red_led.value = False

    if voltage_low_point > battery_voltage_cutoff:
        power_enable.value = True
    else:
        power_enable.value = False

