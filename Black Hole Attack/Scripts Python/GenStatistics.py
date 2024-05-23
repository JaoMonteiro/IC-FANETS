#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  MUST BE PLACED INSIDE LOGS FOLDER! NOT SIMULATION FOLDER
#  This code gets all python files inside current folder, and executes all of them in all folders inside current folder. After that, it gets and summarize all Statistics files generated to create final Reviews.
#--------------------------------------------
import os,shutil,statistics

# Get the current directory
current_dir = os.getcwd()

# Get the current folder
current_folder = os.getcwd()
last_line = ""

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
       
# Create an output text file
output_file = open("LatencyReview.txt", "w")
output_file.write("Simulation\tMin\tMax\tAverage\tStandardDeviation\n")


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

with open("LatencyReview.txt", 'r+') as file:
    lines = file.readlines()
    num_lines = len(lines)
    min_vector = []
    max_vector = []
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        # Sum up the values for each column
        for i, value in enumerate(values):
            column_sums[i] += float(value)
        min_vector.append(float(values[0]))
        max_vector.append(float(values[1]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / (num_lines - 1) ,2)for sum_value in column_sums]
    min_value = min(min_vector)
    max_value = max(max_vector)
    
    # Open the file again in append mode to write the average at the end
    file.write(f"\nMinimal: {min_value}\tMaximum: {max_value}\tAverages: ")
    file.write("\t".join(map(str, column_averages)))


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

with open('ConscienceReview.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:])
    other_lines.sort(key= lambda line: line.split()[0])
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)


with open("ConscienceReview.txt", 'r+') as file:
    lines = file.readlines()
    num_lines = len(lines)
    min_vector = []
    max_vector = []
    
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        # Sum up the values for each column
        for i, value in enumerate(values):
            column_sums[i] += float(value)
        min_vector.append(float(values[0]))
        max_vector.append(float(values[1]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / (num_lines - 1) ,2)for sum_value in column_sums]
    min_value = min(min_vector)
    max_value = max(max_vector)
    # Open the file again in append mode to write the average at the end
    file.write(f"\nMinimal: {min_value}\tMaximum: {max_value}\tAverages: ")
    file.write("\t".join(map(str, column_averages)))



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

with open('UnconscienceReview.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:])
    other_lines.sort(key= lambda line: line.split()[0])
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)


with open("UnconscienceReview.txt", 'r+') as file:
    lines = file.readlines()
    num_lines = len(lines)
    min_vector = []
    max_vector = []
    
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        # Sum up the values for each column
        for i, value in enumerate(values):
            column_sums[i] += float(value)
        min_vector.append(float(values[0]))
        max_vector.append(float(values[1]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / (num_lines - 1) ,2)for sum_value in column_sums]
    min_value = min(min_vector)
    max_value = max(max_vector)
    # Open the file again in append mode to write the average at the end
    file.write(f"\nMinimal: {min_value}\tMaximum: {max_value}\tAverages: ")
    file.write("\t".join(map(str, column_averages)))


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


with open('NeighborHoodErrorsReview.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:])
    other_lines.sort(key= lambda line: line.split()[0])
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)


with open("NeighborHoodErrorsReview.txt", 'r') as file:
    lines = file.readlines()
    num_lines = len(lines)
    min_vector = []
    max_vector = []
    
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        # Sum up the values for each column
        for i, value in enumerate(values):
            column_sums[i] += float(value)
        min_vector.append(float(values[0]))
        max_vector.append(float(values[1]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / (num_lines - 1) ,2)for sum_value in column_sums]
    min_value = min(min_vector)
    max_value = max(max_vector)

    # Open the file again in append mode to write the average at the end
    with open("NeighborHoodErrorsReview.txt", 'a') as file:
        file.write(f"\nMinimal: {min_value}\tMaximum: {max_value}\tAverages: ")
        file.write("\t".join(map(str, column_averages)))


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

with open('DistanceErrorsReview.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:])
    other_lines.sort(key= lambda line: line.split()[0])
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)

with open("DistanceErrorsReview.txt", 'r') as file:
    lines = file.readlines()
    num_lines = len(lines)
    min_vector = []
    max_vector = []
    
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        # Sum up the values for each column
        for i, value in enumerate(values):
            column_sums[i] += float(value)
        min_vector.append(float(values[0]))
        max_vector.append(float(values[1]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / (num_lines - 1) ,2)for sum_value in column_sums]
    min_value = min(min_vector)
    max_value = max(max_vector)

    # Open the file again in append mode to write the average at the end
    with open("DistanceErrorsReview.txt", 'a') as file:
        file.write(f"\nMinimal: {min_value}\tMaximum: {max_value}\tAverages: ")
        file.write("\t".join(map(str, column_averages)))

# Create an output text file
output_file = open("MaliciousNeighborsReview.txt", "w")
output_file.write("Simulation\tAvgMsgSent\tAvgMsgRcv\tAvgMsgTotal\tMinDuration\tMaxDuration\tAvgDuration\n")

# Get the current folder
current_folder = os.getcwd()
last_line = ""

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        # Search for 'Statistics_Latency' file in the folder
        for root, dirs, files in os.walk(folder):
            if 'Statistics_MaliciousNeighborsListSummary.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'Statistics_MaliciousNeighborsListSummary.txt')
                with open(file_path, 'r') as file:
                    lines = file.readlines()
                    last_line = lines[-1].strip()
                
                # Save the last line with the folder's name in the output file
                output_file.write(f"{folder}\t{last_line[4:]}\n")
output_file.close()

with open("MaliciousNeighborsReview.txt", 'r+') as file:
    lines = file.readlines()
    num_lines = len(lines)
    messagesExchanged = [0] * 2
    min_durations = []
    max_durations = []
    avg_durations = []
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()
        messagesExchanged[0] += float(values[1])
        messagesExchanged[1] += float(values[2])
        min_durations.append(float(values[4]))
        max_durations.append(float(values[5]))
        avg_durations.append(float(values[6]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / num_lines ,2)for sum_value in messagesExchanged]
    total_messages = round(column_averages[0]+column_averages[1],2)
    if(len(avg_durations) > 1):
        min_value = min(min_durations)
        max_value = max(max_durations)
        avg_value = round(statistics.mean(avg_durations),2)
    else:
        min_value = 0
        max_value = 0
        avg_value = 0
    
    # Open the file again in append mode to write the average at the end
    file.write(f"\nMessages Sent: {column_averages[0]}\tReceived: {column_averages[1]}\tTotal: {total_messages}")
    file.write(f"\tMinDuration: {round(min_value,2)}\tMaxDuration: {(round(max_value,2))}\tAvgDuration: {avg_value}\n")
file.close()


# Create an output text file
output_file = open("SuspiciousActivityReview.txt", "w")
output_file.write("Simulation\tAvgMsgSent\tAvgMsgRcv\tAvgMsgTotal\tMinDuration\tMaxDuration\tAvgDuration\n")

# Iterate through each folder in the current folder
for folder in os.listdir(current_folder):
    if os.path.isdir(folder):
        # Search for 'Statistics_Latency' file in the folder
        for root, dirs, files in os.walk(folder):
            content = []
            if 'SuspiciousActivity.txt' in files:
                # Open the file and get the last line
                file_path = os.path.join(root, 'SuspiciousActivity.txt')
                with open(file_path, 'r+') as file:
                  lines = file.readlines()
                  last_lines = lines[-3:]
                  content = last_lines[0].strip('\n') + '\t' + last_lines[1].strip('\n') + '\t' + last_lines[2]
                
                  # Save the last line with the folder's name in the output file
            output_file.write(f"{folder}\t{content}\n")

output_file.close()


with open("SuspiciousActivityReview.txt", 'r+') as file:
    lines = file.readlines()
    lines = lines[1:]
    instantBlocks_vector = []
    minErrors_vector = []
    maxErrors_vector = []
    avgErrors_vector = []
    stdErrors_vector = []
    minLatency_vector = []
    maxLatency_vector = []
    avgLatency_vector = []
    stdLatency_vector = []
    
    # Initialize variables to hold the sum of values for each column
    column_sums = [0] * (len(lines[1].split()) - 1)
    
    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()[1:]  # Exclude the first column
        if(values):
            instantBlocks_vector.append(float(values[0]))
            minErrors_vector.append(float(values[1]))
            maxErrors_vector.append(float(values[2]))
            avgErrors_vector.append(float(values[3]))
            stdErrors_vector.append(float(values[4]))
            minLatency_vector.append(float(values[5]))
            maxLatency_vector.append(float(values[6]))
            avgLatency_vector.append(float(values[7]))
            stdLatency_vector.append(float(values[8]))
    
    stdValue = 0
    avgValue = 0

    if(len(instantBlocks_vector) < 2):
        stdValue = 0
        avgValue = instantBlocks_vector[0]
    else:
        stdValue = round(statistics.stdev(instantBlocks_vector),6)
        avgValue = round(statistics.mean(instantBlocks_vector),6)


    file.write(f"\nMinimal InstantBlocks: {min(instantBlocks_vector)}\tMaximum InstantBlocks: {max(instantBlocks_vector)}\tAverage: {round(avgValue,2)}\tStdDev: {round(stdValue,2)}")
    file.write(f"\nMinimal Errors: {min(minErrors_vector)}\tMaximum: {max(maxErrors_vector)}\tAverage: {round(statistics.mean(avgErrors_vector),2)}\tStdDev: {round(statistics.mean(stdErrors_vector),2)}")
    file.write(f"\nMinimal Latency: {min(minLatency_vector)}\tMaximum: {max(maxLatency_vector)}\tAverage: {round(statistics.mean(avgLatency_vector),6)}\tStdDev: {round(statistics.mean(stdLatency_vector),6)}")