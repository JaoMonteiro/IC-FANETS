#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  MUST BE PLACED INSIDE LOGS FOLDER! NOT SIMULATION FOLDER
#  This code gets all python files inside current folder, and executes all of them in all folders inside current folder. After that, it gets and summarize all Statistics files generated to create final Reviews.
#--------------------------------------------
import os, shutil

# Get the current directory
current_dir = os.getcwd()

# Get all the python files in the current directory
python_files = [file for file in os.listdir(current_dir) if file.endswith(".py")]

# Iterate through each python file
for file in python_files:
    # Get the file name without the extension
    file_name = os.path.splitext(file)[0]
    
    # Get all the directories in the current directory
    directories = [dir for dir in os.listdir(current_dir) if os.path.isdir(dir)]
    
    if 'Statistics' not in file:
        # Iterate through each directory
        for directory in directories:
        
            print(file_name)
            print(os.path.join(current_dir, directory))   
            
            # Copy the python file to the new directory
            shutil.copy(file, os.path.join(current_dir, directory))
            
            # Change the current directory to the new directory
            os.chdir(os.path.join(current_dir, directory))
            
            # Execute the python file
            os.system(f"python3 {file}")
            
            # Change the current directory back to the original directory
            os.chdir(current_dir)
import os

# Create an output text file
output_file = open("LatencyReview.txt", "w")
output_file.write("Simulation\tMin\tMax\tAverage\tStandardDeviation\n")

# Get the current folder
current_folder = os.getcwd()
last_line = ""

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        # Search for 'Statistics_Latency' file in the folder
        for root, dirs, files in os.walk(folder):
            if 'Statistics_Latency.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_Latency.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()


output_file = open("ConscienceReview.txt", "w")
output_file.write("Simulation\tMin_Duration\tMax_Duration\tAvg_Duration\tStdDev_Duration\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages\n")
last_line = ""

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        for root, dirs, files in os.walk(folder):
            if 'Statistics_Conscience.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_Conscience.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()


output_file = open("UnconscienceReview.txt", "w")
output_file.write("Simulation\tMin_Duration\tMax_Duration\tAvg_Duration\tStdDev_Duration\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages\n")
last_line = ""

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        for root, dirs, files in os.walk(folder):
            if 'Statistics_Unconscience.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_Unconscience.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()


output_file = open("DistanceErrorsReview.txt", "w")
output_file.write("Simulation\tMin\tMax\tAverage\tStandardDeviation\n")

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        for root, dirs, files in os.walk(folder):
            if 'Statistics_DistanceErrors.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_DistanceErrors.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()


# Create an output text file
output_file = open("NeighborHoodErrorsReview.txt", "w")
output_file.write("Simulation\tMin\tMax\tAverage\tStandardDeviation\n")

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        # Search for 'Statistics_Latency' file in the folder
        for root, dirs, files in os.walk(folder):
            if 'Statistics_NeighborhoodConscienceErrors.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_NeighborhoodConscienceErrors.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()

output_file = open("MaliciousNeighborsReview.txt", "w")
output_file.write("Simulation\tIp\tMsgSent\tMsgReceived\tisBlocked\tTimestamp\tTimeToBlock\tWhoBlocked/Suspected\n")
with open("IndirectMaliciousNeighborsReview.txt", "w") as file:
    file.write("Simulation\tIp\tMsgSent\tMsgReceived\tisBlocked\tTimestamp\tTimeToBlock\tWhoBlocked/Suspected\n")

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        for root, dirs, files in os.walk(folder):
            if 'Statistics_MaliciousNeighborsListSummary.txt' in files:
                file_path = os.path.join(root, 'Statistics_MaliciousNeighborsListSummary.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    lines = lines[1:]
                for line in lines:
                    # Checks Received column to see if there was an indirect interaction 
                    values = line.split()[3]
                    if values == '0':
                        with open("IndirectMaliciousNeighborsReview.txt", "a") as file:
                            file.write(line)
                    else:
                        output_file.write(f"{folder}\t" + line)
                        output_file.write('\n')
output_file.close()