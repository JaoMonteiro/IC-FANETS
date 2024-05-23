#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all ReceivedMessages files, summarizes messages latencies from all lines and save it on Statistics_Latency.txt
#--------------------------------------------
import os, statistics

files = os.listdir('.')
nameVector = []
maxVector = []
minVector = []
avgVector = []
stdDVector = []

for file in files:
    if 'ReceivedMessages' in file:
        with open(file, 'r') as f:
            lines = f.readlines()
            # Skip header
            lines = lines[1:]  
            values = []
            for line in lines:
                if line.strip():  # Check if line is not empty
                    tokens = line.split()
                    # Save every value in second column if the TagType is an Update
                    if int(tokens[4]) == 2:
                        values.append(round(float(tokens[1]),6))
        if values:
            nameVector.append(file)
            maxVector.append(max(values))
            minVector.append(min(values))
            avgVector.append(round(statistics.mean(values),6))
            
            if len(values) > 1:
                stdDVector.append(round(statistics.stdev(values),6))
            else:
                stdDVector.append(round(values[0],6))
        else:
            nameVector.append(file)
            avgVector.append(0)
            stdDVector.append(0)
            maxVector.append(0)
            minVector.append(0)


with open('Statistics_Latency.txt', 'w') as output_file:
    output_file.write("Node\tMin\tMax\tAverage\tStandard_Deviation\n")

    for index,name in enumerate(nameVector):
        output_file.write(f"{name[23:-4]}\t{minVector[index]}\t{maxVector[index]}\t{avgVector[index]}\t{stdDVector[index]}\n")

    max_value = max(maxVector)
    min_value = min(minVector)

    if len(stdDVector) > 1:
        std_value = round(statistics.stdev(stdDVector),6)
        avg_value = round(statistics.mean(avgVector),6)
    else:
        std_value = stdDVector[0]
        avg_value = avgVector[0]

    output_file.write("ALL:\t")
    output_file.write(f"{min_value}\t{max_value}\t{avg_value}\t{std_value}\t")
output_file.close()

with open('Statistics_Latency.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:-1])
    final_line = file.readline()
    other_lines.sort(key= lambda line: int (line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)
    file.write(final_line)
