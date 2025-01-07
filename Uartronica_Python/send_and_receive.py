import serial
import time

# Configure the serial port
serial_port = "COM9"  # Replace with your COM port
baud_rate = 115200      # Match the STM32 UART baud rate

try:
    # Open the serial port
    ser = serial.Serial(port=serial_port, baudrate=baud_rate, timeout=1)
    print(f"Connected to {serial_port} at {baud_rate} baud.\n")

    while True:

        # Get user input
        user_input = input("Enter a command: ").strip()
        #print("\n" + "-" * 50)
        print(f"You entered: {user_input}")

        # Send the message with newline
        message = f"{user_input}\n"
        ser.write(message.encode('utf-8'))
        print(f"Sent: {message.strip()} (Length: {len(message.strip())} bytes)")

        # Read until newline (command response)
        response = ser.readline().decode('utf-8').strip()
        print(f"Received: {response}\n")

        # Read any additional data in the buffer for debugging
        #buffer_content = ""
        #while ser.in_waiting:
        #    buffer_content += ser.read(1).decode('utf-8', errors='ignore')
        #if buffer_content:  # If there's anything in the buffer
        #    print(f"Additional Buffer Content: {buffer_content}")

        #print("-" * 50 + "\n")

except serial.SerialException as e:
    print(f"Error: {e}")
except KeyboardInterrupt:
    print("Program interrupted by user.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port closed.")
