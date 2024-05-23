#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all MaliciousNeighbors files, summarizes messages malicious activities from all lines and save it on Statistics_MaliciousNeighborsListSummary.txt
#--------------------------------------------
import os

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
    f.write("Ip\tMsgSent\tMsgReceived\tisBlocked\tTimestamp\tTimeToBlock\tWhoBlocked/Suspected\n")
    for line in text_vector:
        if line.strip():
            f.write(line)
