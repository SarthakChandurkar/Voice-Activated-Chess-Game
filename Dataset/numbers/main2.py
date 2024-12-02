import os

def rename_files_to_match_subfolder(base_folder):
    # Loop through folders 0 to 9
    for main_folder in range(10):
        main_folder_path = os.path.join(base_folder, str(main_folder))
        
        # Check if the main folder exists
        if not os.path.exists(main_folder_path):
            continue

        # Loop through each subfolder in the main folder
        for subfolder in os.listdir(main_folder_path):
            subfolder_path = os.path.join(main_folder_path, subfolder)

            # Check if it's a valid subfolder with the pattern "number_number"
            if os.path.isdir(subfolder_path) and "_" in subfolder:
                # Construct the correct filename based on the subfolder name
                correct_filename = f"{subfolder}.txt"
                correct_filepath = os.path.join(subfolder_path, correct_filename)
                
                # Find the .txt file inside the subfolder
                for file in os.listdir(subfolder_path):
                    if file.endswith(".txt") and file.startswith(str(main_folder)):
                        current_filepath = os.path.join(subfolder_path, file)
                        
                        # Rename if the current filename does not match the correct filename
                        if file != correct_filename:
                            os.rename(current_filepath, correct_filepath)
                            print(f"Renamed {current_filepath} to {correct_filepath}")
                        break  # Stop after finding one .txt file (since only one is expected)

    print("File renaming completed.")

# Example usage
base_folder = os.getcwd()  # Uses the current working directory where folders 0-9 are located
rename_files_to_match_subfolder(base_folder)
