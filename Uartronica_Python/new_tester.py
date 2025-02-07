import serial
import time

# Configure the serial port (update this with your correct COM port)
SERIAL_PORT = "COM9"  # Change to the correct port, e.g., "/dev/ttyUSB0" for Linux/macOS
BAUD_RATE = 115200

# Initialize serial communication
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

def send_command(command):
    """Send a command to STM32 and print the response."""
    ser.write((command + '\n').encode())  # Send command with newline
    time.sleep(0.1)  # Allow time for processing
    response = ser.read_all().decode().strip()
    print(f"Command: {command} | Response: {response}")

try:
    print("Testing STM32 UART Communication...\n")

    # Reset all pins
    send_command("s1 PC9 PB1")

    #send_command("r *")  

    # Configure two input pins and read their states
    send_command("d PC9 PB1")

    # Configure two output pins and set them HIGH
    send_command("s1 PA2 PB3")
    
    # Set output pins LOW
    send_command("s0 PA2 PB3")

    # Unknown command test
    send_command("invalid_command")

except Exception as e:
    print(f"Error: {e}")

finally:
    ser.close()
    print("Serial connection closed.")