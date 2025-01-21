import serial

# Serial port configuration
SERIAL_PORT = "COM9"  # Replace with your port
BAUD_RATE = 115200    # Match STM32 UART baud rate
TIMEOUT = 0.05        # UART timeout in seconds

def open_serial_connection(port, baudrate):
    """
    Open and return a serial connection.
    
    Parameters:
    - port: The COM port to connect to.
    - baudrate: The baud rate for the UART connection.

    Returns:
    - Serial object if successful, None otherwise.
    """
    try:
        # Attempt to open the serial port
        ser = serial.Serial(port, baudrate, timeout=TIMEOUT)
        print(f"Connected to {port} at {baudrate} baud.")
        return ser
    except serial.SerialException as e:
        # Handle errors if the port cannot be opened
        print(f"Error opening serial port: {e}")
        return None

def send_command(ser, command):
    """
    Send a command to the STM32 via UART.
    
    Parameters:
    - ser: The serial connection object.
    - command: The command string to send.
    """
    if ser and ser.is_open:  # Ensure the serial connection is open
        command = command.strip()  # Remove extra whitespace
        if not command:  # Check for empty command
            print("Error: Empty command. Please enter a valid command.")
            return
        command += "\n"  # Append a newline to signal end of command
        ser.write(command.encode('utf-8'))  # Send the command as UTF-8 encoded bytes
        print(f"Sent: {command.strip()}")  # Log the sent command

def read_all_responses(ser):
    """
    Read all available responses from the STM32.
    
    Parameters:
    - ser: The serial connection object.

    Returns:
    - List of response strings received from the STM32.
    """
    if ser and ser.is_open:  # Ensure the serial connection is open
        responses = []  # Initialize an empty list for responses
        while True:
            # Read a single line from the serial buffer
            response = ser.readline().decode('utf-8').strip()  # Decode bytes to string and strip whitespace
            if response:  # If a response was received, add it to the list
                responses.append(response)
            else:
                break  # Exit the loop if no more responses are available
        return responses

def validate_command(command):
    """
    Validate the command format to ensure it's acceptable.

    Parameters:
    - command: The command string to validate.

    Returns:
    - True if the command is valid, False otherwise.
    """
    if command.startswith("d") or command.startswith("r"):  # Commands starting with 'd' or 'r' are valid
        return True
    elif command.startswith("s") and len(command.split()) >= 2:  # 's' commands must have at least one argument
        return True
    else:
        print("Invalid command format. Please try again.")
        return False

def main():
    """
    Main program function to handle user interaction and communication with STM32.
    """
    ser = open_serial_connection(SERIAL_PORT, BAUD_RATE)  # Open the serial connection
    if not ser:  # Exit if the connection fails
        return

    try:
        while True:  # Main loop for user interaction
            # Display command options to the user
            print("\nCommands:")
            print("1. Read Inputs:        d (e.g., d PC0 PC1 - MAXIMUM 2 PINS)")
            print("2. State Outputs:      sX (e.g., s1 PC8 - ANY Nº OF PINS)")
            print("3. Reset Output Pins:  r *")
            print("Type 'exit' to quit.\n")

            # Get user input
            command = input("Enter a command: ").strip()
            if command.lower() == "exit":  # Exit the program if the user types 'exit'
                print("Exiting program.")
                break

            # Validate the command format
            if not validate_command(command):
                continue

            # Send the validated command to the STM32
            send_command(ser, command)

            # Read and print all responses from the STM32
            responses = read_all_responses(ser)
            if responses:
                print("Received:")
                for response in responses:  # Display each response line
                    print(response)
            else:
                print("No response received.")  # If no response, notify the user

    except KeyboardInterrupt:  # Handle Ctrl+C interruption
        print("\nProgram interrupted by user.")
    finally:
        # Ensure the serial port is closed on exit
        if ser and ser.is_open:
            ser.close()
            print("Serial port closed.")

# Entry point of the program
if __name__ == "__main__":
    main()