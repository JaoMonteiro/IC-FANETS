#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all DistanceErrors files, gets the all errors, summarizes min max avg and stddev from all lines and save it on Statistics_DistanceErrors.txt
#--------------------------------------------

import os, statistics

files = os.listdir('.')
nameVector = []
maxVector = []
minVector = []
avgVector = []
stdDVector = []

for file in files:
    if 'DistanceErrors' in file:
        with open(file, 'r') as f:
            columnHead = f.readline()
            lines = f.readlines()
            values = []
            maxV = []
            minV = []
            std_value = 0
            avg_value = 0
            for line in lines:
                if line.strip():
                    tokens = line.split()
                    
                    for i, value in enumerate(tokens):
                        
                        if i == 1:
                            maxV.append(round(float(value),6))
                        elif i == 2:
                            minV.append(round(float(value),6))
                        elif i > 4:
                            values.append(round(float(value),6))
        if values:
            max_value = max(maxV)
            min_value = min(minV)
            if len(values) > 1:
                avg_value = round(statistics.mean(values),6)
                std_value = round(statistics.stdev(values),6)
            else:
                avg_value = round(values[0],6)
                std_value = round(values[0],6)
            nameVector.append(file)
            avgVector.append(avg_value)
            stdDVector.append(std_value)
            maxVector.append(max_value)
            minVector.append(min_value)
            
        else:
            nameVector.append(file)
            avgVector.append(0)
            stdDVector.append(0)
            maxVector.append(0)
            minVector.append(0)

with open('Statistics_DistanceErrors.txt', 'w') as output_file:
    output_file.write("Node\tMin\tMax\tAverage\tStandard_Deviation\n")
    for index,name in enumerate(nameVector):
        output_file.write(f"{name[21:-4]}\t{minVector[index]}\t{maxVector[index]}\t{avgVector[index]}\t{stdDVector[index]}\n")
    max_value = max(maxVector)
    min_value = min(minVector)
    avg_value = round(statistics.mean(avgVector),6)
    std_value = round(statistics.mean(stdDVector),6)
    output_file.write("ALL:\t")
    output_file.write(f"{min_value}\t{max_value}\t{avg_value}\t{std_value}\t")

with open('Statistics_DistanceErrors.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:-1])
    final_line = file.readline()
    other_lines.sort(key= lambda line: int (line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)
    file.write(final_line)