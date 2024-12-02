import os
import shutil
import re

def organize_files(base_folder):
    # Get all files in the base folder
    for filename in os.listdir(base_folder):
        file_path = os.path.join(base_folder, filename)

        # Check if it is a file and matches the pattern "Number_Number.extension"
        if os.path.isfile(file_path) and re.match(r"^\d+_\d+\..+", filename):
            # Extract the main folder name (first number) and subfolder name (e.g., "0_1" from "0_1.txt")
            main_folder_name = filename.split("_")[0]  # First number in filename
            sub_folder_name = os.path.splitext(filename)[0]  # Filename without extension
            
            # Define paths for main and subfolders
            main_folder_path = os.path.join(base_folder, main_folder_name)
            sub_folder_path = os.path.join(main_folder_path, sub_folder_name)

            # Create main and subfolders only if they don't exist
            os.makedirs(sub_folder_path, exist_ok=True)

            # Move the file to the appropriate subfolder (overwriting if it exists)
            shutil.move(file_path, os.path.join(sub_folder_path, filename))

    print("Files organized into folders successfully.")

# Example usage
base_folder = os.getcwd()  # Uses the current working directory where the script is running
organize_files(base_folder)
