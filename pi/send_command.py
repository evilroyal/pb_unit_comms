import smbus
import sys

I2C_BUS = 1
ARDUINO_I2C_ADDR = 0x20

class Broue:
    def __init__(self, bus_num = I2C_BUS) -> None:
        self.bus = smbus.SMBus(bus_num)

    def send_command(self, reg: int, val: int) -> None:
        self.bus.write_byte(ARDUINO_I2C_ADDR, reg & 0xff)
        self.bus.write_byte(ARDUINO_I2C_ADDR, val & 0xff)

    def __del__(self):
        self.bus.close()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage : {sys.argv[0]} <register> <value>")
        sys.exit(0)

    register = int(sys.argv[1])
    value = int(sys.argv[2])

    broue = Broue()
    broue.send_command(register, value)
