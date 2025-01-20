import serial

# Serial port configuration
SERIAL_PORT = "COM9"  # Replace with your port
BAUD_RATE = 115200    # Match STM32 UART baud rate
TIMEOUT = 0.05           # UART timeout in seconds

def open_serial_connection(port, baudrate):
    """Open and return a serial connection."""
    try:
        ser = serial.Serial(port, baudrate, timeout=TIMEOUT)
        print(f"Connected to {port} at {baudrate} baud.")
        return ser
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return None

def send_command(ser, command):
    """Send a command to the STM32."""
    if ser and ser.is_open:
        command = command.strip()
        if not command:
            print("Error: Empty command. Please enter a valid command.")
            return
        command += "\n"
        ser.write(command.encode('utf-8'))
        print(f"Sent: {command.strip()}")

def read_all_responses(ser):
    """Read all available responses from the STM32."""
    if ser and ser.is_open:
        responses = []
        while True:
            response = ser.readline().decode('utf-8').strip()
            if response:
                responses.append(response)
            else:
                break
        return responses

def validate_command(command):
    """Validate the command format."""
    if command.startswith("d") or command.startswith("r"):
        return True
    elif command.startswith("s") and len(command.split()) >= 2:
        return True
    else:
        print("Invalid command format. Please try again.")
        return False

def main():
    ser = open_serial_connection(SERIAL_PORT, BAUD_RATE)
    if not ser:
        return

    try:
        while True:
            print("\nCommands:")
            print("1. Read Inputs:        d (e.g., d PC0 PC1)")
            print("2. State Outputs:      sXY (e.g., s01 PC8)")
            print("3. Reset Output Pins:  r (e.g., r PC8)")
            print("Type 'exit' to quit.\n")

            command = input("Enter a command: ").strip()
            if command.lower() == "exit":
                print("Exiting program.")
                break

            # Validate command before sending
            if not validate_command(command):
                continue

            # Send the command
            send_command(ser, command)

            # Read and print all responses
            responses = read_all_responses(ser)
            if responses:
                print("Received:")
                for response in responses:
                    print(response)
            else:
                print("No response received.")

    except KeyboardInterrupt:
        print("\nProgram interrupted by user.")
    finally:
        if ser and ser.is_open:
            ser.close()
            print("Serial port closed.")

if __name__ == "__main__":
    main()