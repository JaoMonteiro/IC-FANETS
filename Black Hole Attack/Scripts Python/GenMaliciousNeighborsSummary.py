#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all MaliciousNeighbors files, summarizes messages malicious activities from all lines and save it on Statistics_MaliciousNeighborsListSummary.txt
#--------------------------------------------
import os, statistics

text_vector = []

# Get all text files that have "MaliciousNeighbors" in the name
files = [file for file in os.listdir('.') if file.endswith('.txt') and 'MaliciousNeighbors_' in file]

for file in files:
    with open(file, 'r') as f:
        lines = f.readlines()
        lines = lines[1:]
        name_part = file[19:-4]
        
        # Save the lines in the vector created with the part of the name
        text_vector.extend([(line.strip() + "\t" + name_part + "\n" ) for line in lines if line.strip()])
text_vector.sort()

with open('Statistics_MaliciousNeighborsListSummary.txt', 'w') as f:
    f.write("Ip\tMsgSent\tMsgReceived\tisBlocked\tTimestamp\tTimeToBlock\tWhoSuspected\n")
    for line in text_vector:
        if line.strip():
            f.write(line)
f.close()

with open("Statistics_MaliciousNeighborsListSummary.txt", 'r+') as file:
    lines = file.readlines()
    lines = lines[1:]
    durations = []
    # Initialize variables to hold the sum of values for each column
    messagesExchanged = [0] * 2
    blockedNeighbors = []

    # Iterate over each line except the first
    for line in lines[1:]:
        values = line.split()
        if(line.split()[3] == '1'):
            blockedNeighbors.append(line)

    num_lines = len(blockedNeighbors)
        
    for line in blockedNeighbors:
        values = line.split()
        messagesExchanged[0] += float(values[1])
        messagesExchanged[1] += float(values[2])
        durations.append(float(values[5]))
    
    # Calculate the average for each column
    column_averages = [round(sum_value / num_lines ,2)for sum_value in messagesExchanged]
    if(len(durations) > 1):
        min_value = min(durations)
        max_value = max(durations)
        avg_value = round(statistics.mean(durations),2)
    else:
        min_value = 0
        max_value = 0
        avg_value = 0
    
    # Open the file again in append mode to write the average at the end
    file.write(f"Messages Sent: {column_averages[0]}\tReceived: {column_averages[1]}\tTotal: {column_averages[0]+column_averages[1]}")
    file.write(f"\tMinDuration: {round(min_value,2)}\tMaxDuration: {(round(max_value,2))}\tAvgDuration: {round(avg_value,2)}\n")

    file.write(f"ALL:\t{column_averages[0]}\t{column_averages[1]}\t{round(column_averages[0]+column_averages[1],2)}\t{min_value}\t{max_value}\t{avg_value}")
file.close()

