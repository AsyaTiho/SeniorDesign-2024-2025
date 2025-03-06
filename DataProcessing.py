import pandas as pd

# Prompt user for elevation value
elevation = float(input("Enter elevation value: "))

# Load CSV file
df = pd.read_csv("your_file_name.csv")  # Replace with your actual file name

# Add elevation column
df["Elevation"] = elevation  # Assign the same value to all rows

# Valid angles we care about
valid_angles = [0, 30, 45, 90, 180]

def is_valid_tilt(tilt_val):
    """Keep tilt if it's within ±5 degrees of a valid angle."""
    for angle in valid_angles:
        if (angle - 5) <= tilt_val <= (angle + 5):
            return True
    return False

# Filter rows based on tilt column
df = df[df["tilt"].apply(is_valid_tilt)]

# Save cleaned CSV with elevation-based filename
output_filename = f"{elevation}_processed_data.csv"
df.to_csv(output_filename, index=False)

print(f"Filtered data saved as '{output_filename}'")
