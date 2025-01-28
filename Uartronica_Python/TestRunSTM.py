import serial
import time
import pandas as pd
import random
import os
from datetime import datetime
import itertools
import re

# Ensure openpyxl is installed
try:
    import openpyxl
except ImportError:
    print("Error: 'openpyxl' module is not installed. Install it with 'pip install openpyxl'")
    exit()

# Serial port configuration
SERIAL_PORT = "COM9"  # Replace with your port
BAUD_RATE = 115200    # Match STM32 UART baud rate
TIMEOUT = 0.1         # Lower timeout for ultra-fast execution
TIME_STM = 1

# Log data storage
log_data = []

# Set the log file path in the same directory as the Python script
script_dir = os.path.dirname(os.path.abspath(__file__))  # Get the script's directory
excel_filename = os.path.join(script_dir, "log_stm.xlsx")


def sanitize_response(response):
    """ Remove illegal characters from a response string. """
    return re.sub(r"[^\x20-\x7E]", "", response)  # Remove non-printable ASCII characters


def open_serial_connection(port, baudrate, time_stm):
    """ Open serial connection to STM32 """
    try: 
        ser = serial.Serial(port, baudrate, timeout=TIMEOUT)
        time.sleep(0.5)  # Shorter wait time for STM32 initialization
        print(f"Connected to {port} at {baudrate} baudrate!")
        return ser
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return None


def generate_command_list():
    """ Generate a list of 30 random commands with balanced `s` and `d` commands. """
    pins = ["PA6", "PA7", "PB14", "PB15", "PC0", "PC1", "PC2", "PC3"]

    # Generate all possible commands
    d_combinations = [f"d {p1} {p2}" for p1, p2 in itertools.combinations(pins, 2)]  # Read 2 pins
    s1_combinations = [f"s1 {p1} {p2}" for p1, p2 in itertools.combinations(pins, 2)]  # Set 2 pins high
    s0_combinations = [f"s0 {p}" for p in pins]  # Set 1 pin low

    # Select more `s` and `d` commands to balance inputs
    selected_d = random.sample(d_combinations, 15)  # 15 read commands
    selected_s1 = random.sample(s1_combinations, 10)  # 10 set-high commands
    selected_s0 = random.sample(s0_combinations, 5)  # 5 set-low commands

    # Combine and shuffle the commands
    all_commands = selected_d + selected_s1 + selected_s0
    random.shuffle(all_commands)

    # Ensure `r *` is the final command of the random commands
    all_commands.append("r *")

    # Add explicit `d` commands for all pin combinations after `r *`
    all_pins_d = [f"d {p1} {p2}" for p1, p2 in itertools.combinations(pins, 2)]
    all_commands.extend(all_pins_d)

    return all_commands


def execute_commands(ser, command_list):
    """ Execute commands on STM32 and log responses """
    global log_data  # Store logs globally
    
    for cmd in command_list:
        if ser and ser.is_open:
            # Reset buffer to avoid old responses
            ser.reset_input_buffer()

            # Send command (single write, no separate flush)
            ser.write(f"{cmd}\r\n".encode('utf-8'))

            # Log command with timestamp
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            log_entry = {"Timestamp": timestamp, "Command": cmd, "Response": ""}

            # Read response within a 0.2s window (faster than looping indefinitely)
            responses = []
            start_time = time.time()
            while time.time() - start_time < 0.2:
                line = ser.readline()
                if line:
                    responses.append(line.decode('utf-8').strip())

            # Sanitize responses
            sanitized_responses = [sanitize_response(r) for r in responses]

            # Store response in log
            log_entry["Response"] = "; ".join(sanitized_responses) if sanitized_responses else "No response"
            log_data.append(log_entry)

            # Terminal output: Show command + OK/NOK
            status = "OK" if responses else "NOK"
            print(f"{cmd}: {status}")


def save_log_to_excel():
    """ Save log data to an Excel file in the script's directory. """
    df = pd.DataFrame(log_data)
    df.to_excel(excel_filename, index=False, engine='openpyxl')
    print(f"Log saved to {excel_filename}")


def main():
    ser = open_serial_connection(SERIAL_PORT, BAUD_RATE, TIME_STM)
    if not ser:
        return

    # Generate a list of 30 random commands
    commands = generate_command_list()
    execute_commands(ser, commands)

    # Save log to Excel
    save_log_to_excel()


# Start Program - Entry Point
if __name__ == "__main__":
    main()
