import serial
import time

# Serial port configuration
SERIAL_PORT = "COM9"  # Replace with your port
BAUD_RATE = 115200    # Match STM32 UART baud rate

def open_serial_connection(port, baudrate):
    """Open and return a serial connection."""
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Connected to {port} at {baudrate} baud.")
        return ser
    except serial.SerialException as e:
        print(f"Error: {e}")
        return None

def send_command(ser, command):
    """Send a command to the STM32."""
    if ser:
        command = command.strip() + "\n"
        ser.write(command.encode('utf-8'))
        print(f"Sent: {command.strip()}")

def read_response(ser):
    """Read a response from the STM32."""
    if ser:
        response = ser.readline().decode('utf-8').strip()
        if response:
            print(f"Received: {response}")
        return response

def main():
    ser = open_serial_connection(SERIAL_PORT, BAUD_RATE)

    if not ser:
        print("Failed to connect to the serial port. Exiting.")
        return

    try:
        while True:
            print("\nCommands:")
            print("1. Read Inputs: d (PC0 PC1 - e.g)")
            print("2. State Outputs: sXY (s01 PC8  - e.g)")
            print("3. Reset Output Pins State: r (PC8 - e.g)")

            command = input("Enter a command: ").strip()
            send_command(ser, command)
            time.sleep(0.1)
            while True:
                response = read_response(ser)
                if not response:
                    break

    except KeyboardInterrupt:
        print("\nProgram interrupted by user.")

    finally:
        if ser and ser.is_open:
            ser.close()
            print("Serial port closed.")

if __name__ == "__main__":
    main()