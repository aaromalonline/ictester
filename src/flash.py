import serial
import time
import sys

PORT = "/dev/ttyACM0"
BAUD = 1200

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} firmware.hex")
    sys.exit(1)

hexfile = sys.argv[1]

ser = serial.Serial(
    PORT,
    BAUD,
    timeout=2
)

# Opening the Arduino serial port can reset the Uno.
time.sleep(2)

# Clear anything produced during startup/reset.
ser.reset_input_buffer()

with open(hexfile, "r") as f:
    for line in f:
        line = line.strip()

        if not line:
            continue

        # Send one complete Intel HEX record.
        ser.write((line + "\n").encode())
        ser.flush()

        # Wait for Arduino response.
        while True:
            response = ser.readline().decode(errors="replace").strip()

            if response:
                print(response)

            if response.endswith("ok"):
                break

            if "ERROR" in response:
                print("Programming failed.")
                ser.close()
                sys.exit(1)

        # Continue with next HEX record.

ser.close()
