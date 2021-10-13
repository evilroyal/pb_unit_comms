import smbus
import sys

I2C_BUS = 1
ARDUINO_I2C_ADDR = 0x20

if __name__ == "__main__":
    bus = smbus.SMBus(I2C_BUS)

    if len(sys.argv) != 3:
        print(f"Usage : {sys.argv[0]} <register> <value>")
        sys.exit(0)

    register = int(sys.argv[1])
    value = int(sys.argv[2])

    bus.write_byte(ARDUINO_I2C_ADDR, register & 0xff)
    bus.write_byte(ARDUINO_I2C_ADDR, value & 0xff)
