import serial
import time
import csv
from datetime import datetime

# Open serial connection
s = serial.Serial('COM6')

# Create a timestamped CSV filename
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
csv_filename = f"serial_data_{timestamp}.csv"

# Open CSV file
with open(csv_filename, 'w', newline='') as csvfile:
    # Create CSV writer
    csv_writer = csv.writer(csvfile)
    
    # Write header
    #"pitch,roll,yaw,pressure,temperature,CRC_expected,CRC_calculated"
    csv_writer.writerow(['pitch', 'roll', 'yaw', 'pressure', 'temperature', 'CRC_expected', 'CRC_calculated'])

    
    # Send start command
    s.write(b'start\r\n')
    time.sleep(0.1)
    
    # Clear initial lines
    s.readline()
    s.readline()
    
    # Print header
    print('pitch,roll,yaw,pressure,temperature,CRC_expected,CRC_calculated')
    
    try:
        while True:
            # Read a full line
            line = s.readline().decode('ascii').strip()
            
            # Print to console
            print(line)
            
            # Split the line into values and write to CSV
            values = line.split(',')
            csv_writer.writerow(values)
            
            # Flush CSV to ensure data is written immediately
            csvfile.flush()
    
    except KeyboardInterrupt:
        print("\nLogging stopped. Data saved to", csv_filename)
    
    finally:
        # Close serial connection
        s.close()