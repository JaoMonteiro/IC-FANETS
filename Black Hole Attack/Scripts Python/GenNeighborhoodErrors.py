#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all NeighborHoodAnalysis files, gets the all errors, summarizes min max avg and stddev from all lines and save it on Statistics_NeighborhoodConscienceErrors.txt
#--------------------------------------------
import csv, os, statistics

# Open the file
with open('Statistics_NeighborhoodConscienceErrors.txt', 'w') as output_file:

    maxValues = []
    minValues = []
    avgValues = []
    stdValues = []

    output_file.write("Node\tMin\tMax\tAverage\tStandard_Deviation\n")

    # Loop through each csv file in the current folder
    for file_name in os.listdir('.'):
        if file_name.endswith('.csv'):
            with open(file_name, 'r') as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=';')
                
                # Ignore header
                next(csv_reader)
                
                # Read column 6 and convert to float
                column_6_values = [round(float(row[5]),6) for row in csv_reader]
                
                average = round(statistics.mean(column_6_values),6)
                maximum = max(column_6_values)
                minimum = min(column_6_values)
                standard_deviation = round(statistics.stdev(column_6_values),6)

                minValues.append(minimum)
                maxValues.append(maximum)
                avgValues.append(average)
                stdValues.append(standard_deviation)
        
                output_file.write(f"{file_name[27:-4]}	{minimum}   {maximum}	{average}	{standard_deviation}\n")
    
    max_value = max(maxValues)
    min_value = min(minValues)
    avg_value = round(statistics.mean(avgValues),6)
    std_value = round(statistics.mean(stdValues),6)
    output_file.write(f"ALL\t{min_value}\t{max_value}\t{avg_value}\t{std_value}\t")

with open('Statistics_NeighborhoodConscienceErrors.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:-1])
    final_line = file.readline()
    other_lines.sort(key= lambda line: int (line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)
    file.write(final_line)