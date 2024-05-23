#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens all NeighborHoodAnalysis files, gets the all errors, timings, and check for conscience and unconscience, summarizes min max avg and stddev from all lines and save it on Statistics_Unconscience.txt and Statistics_Conscience.txt
#--------------------------------------------
import csv, os, statistics

folder_path = os.getcwd()

# Get all the csv files in the current folder
csv_files = [file for file in os.listdir(folder_path) if file.endswith('.csv')]
files = os.listdir(folder_path)

for file in csv_files:
    node = file[21:-4]
    print(node)
    with open(file, 'r') as csv_file:
        sent_file = []
        received_file = []
        # Create a csv reader object
        csv_reader = csv.reader(csv_file, delimiter=';')

        # Skip header 
        next(csv_reader)

        time_vector = []
        neighborhood_size = []
        spatial_awareness = False

        time_vector.append(1)
        neighborhood_size.append(0)
        for row in csv_reader :

            if not spatial_awareness:
                if row[5] == '0':
                    time_vector.append(float(row[0]))
                    neighborhood_size.append((row[1]))
                    spatial_awareness = True
            else:
                if row[5] != '0':
                    time_vector.append(float(row[0]))
                    neighborhood_size.append((row[1]))
                    spatial_awareness = False

        time_vector.append(float(row[0]))
        neighborhood_size.append((row[1]))

        for file in files:
            if node in file and "SentMessages" in file:
                sent_file = file
            if node in file and "ReceivedMessages" in file:
                received_file = file
        #print(time_vector)
                
        for i in range(len(time_vector)):
            if i == 0:
                with open(f"Conscience_{node}.txt", "a") as file:
                    file.write("Begin\tEnd\tDuration\tNeighborhoodSize\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages\n")
                with open(f"Unconscience_{node}.txt", "a") as file:
                    file.write("Begin\tEnd\tDuration\tNeighborhoodSize\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages\n")
                continue
            
            sent_vector = [0,0,0,0,0,0,0,0]
            received_vector = [0,0,0,0,0,0,0,0]

            with open(sent_file, 'r') as f:
                lines = f.readlines()
                lines = lines[1:]

                for line in lines:
                    time = float(line.split()[0])
                    begin = time_vector[i - 1]
                    end = time_vector[i]

                    if(line.split()[3] == '0' and time >= begin and time < end):
                        sent_vector[0] += 1
                    elif(line.split()[3] == '1' and time >= begin and time < end):
                        sent_vector[1] += 1
                    elif(line.split()[3] == '2' and time >= begin and time < end):
                        sent_vector[2] += 1
                    elif(line.split()[3] == '3' and time >= begin and time < end):
                        sent_vector[3] += 1
                    elif(line.split()[3] == '4' and time >= begin and time < end):
                        sent_vector[4] += 1
                    elif(line.split()[3] == '5' and time >= begin and time < end):
                        sent_vector[5] += 1
                    elif(line.split()[3] == '6' and time >= begin and time < end):
                        sent_vector[6] += 1
                    
                    if(time >= begin and time < end):
                        sent_vector[7] += 1
                
            with open(received_file, 'r') as f:
                lines = f.readlines()

                for line in lines:
                    
                    if(line.split()[0][0] == 'T'):
                        continue

                    time = float(line.split()[0])
                    begin = time_vector[i - 1]
                    end = time_vector[i]

                    if(line.split()[4] == '0' and time >= begin and time < end):
                        received_vector[0] += 1
                    elif(line.split()[4] == '1' and time >= begin and time < end):
                        received_vector[1] += 1
                    elif(line.split()[4] == '2' and time >= begin and time < end):
                        received_vector[2] += 1
                    elif(line.split()[4] == '3' and time >= begin and time < end):
                        received_vector[3] += 1
                    elif(line.split()[4] == '4' and time >= begin and time < end):
                        received_vector[4] += 1
                    elif(line.split()[4] == '5' and time >= begin and time < end):
                        received_vector[5] += 1
                    elif(line.split()[4] == '6' and time >= begin and time < end):
                        received_vector[6] += 1
                    
                    if(time >= begin and time < end):
                        received_vector[7] += 1

            duration = "{:.6f}".format(end-begin)
            if i % 2 == 0:
                with open(f"Conscience_{node}.txt", "a") as file:
                    file.write(f"{begin}\t{end}\t{duration}\t{neighborhood_size[i-1]}\t{sent_vector[0]}\t{sent_vector[1]}\t{sent_vector[2]}\t{sent_vector[3]}\t{sent_vector[4]}\t{sent_vector[5]}\t{sent_vector[6]}\t{sent_vector[7]}\t{received_vector[0]}\t{received_vector[1]}\t{received_vector[2]}\t{received_vector[3]}\t{received_vector[4]}\t{received_vector[5]}\t{received_vector[6]}\t{received_vector[7]}\t{sent_vector[7] + received_vector[7]}\n")
            else:
                with open(f"Unconscience_{node}.txt", "a") as file:
                    file.write(f"{begin}\t{end}\t{duration}\t{neighborhood_size[i-1]}\t{sent_vector[0]}\t{sent_vector[1]}\t{sent_vector[2]}\t{sent_vector[3]}\t{sent_vector[4]}\t{sent_vector[5]}\t{sent_vector[6]}\t{sent_vector[7]}\t{received_vector[0]}\t{received_vector[1]}\t{received_vector[2]}\t{received_vector[3]}\t{received_vector[4]}\t{received_vector[5]}\t{received_vector[6]}\t{received_vector[7]}\t{sent_vector[7] + received_vector[7]}\n")

nodes = []
values = []
nodes2 = []
values2 = []
files = os.listdir(folder_path)

for file in files:
    if 'Conscience_' in file :
        nodes.append(file)
        info = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        with open(file,'r') as arquivo:
            lines = arquivo.readlines()
            lines = lines[1:]
            for line in lines:
                info[0] += float(line.split()[2])
                info[1] += int(line.split()[4])
                info[2] += int(line.split()[5])
                info[3] += int(line.split()[6])
                info[4] += int(line.split()[7])
                info[5] += int(line.split()[8])
                info[6] += int(line.split()[9])
                info[7] += int(line.split()[10])
                info[8] += int(line.split()[11])
                info[9] += int(line.split()[12])
                info[10] += int(line.split()[13])
                info[11] += int(line.split()[14])
                info[12] += int(line.split()[15])
                info[13] += int(line.split()[16])
                info[14] += int(line.split()[17])
                info[15] += int(line.split()[18])
                info[16] += int(line.split()[19])
                info[17] += int(line.split()[20])

        for index,i in enumerate(info):
            values.append(info[index])

    if 'Unconscience_' in file:
        nodes2.append(file)
        info = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
        with open(file,'r') as arquivo:
            lines = arquivo.readlines()
            lines = lines[1:]
            for line in lines:
                info[0] += float(line.split()[2])
                info[1] += int(line.split()[4])
                info[2] += int(line.split()[5])
                info[3] += int(line.split()[6])
                info[4] += int(line.split()[7])
                info[5] += int(line.split()[8])
                info[6] += int(line.split()[9])
                info[7] += int(line.split()[10])
                info[8] += int(line.split()[11])
                info[9] += int(line.split()[12])
                info[10] += int(line.split()[13])
                info[11] += int(line.split()[14])
                info[12] += int(line.split()[15])
                info[13] += int(line.split()[16])
                info[14] += int(line.split()[17])
                info[15] += int(line.split()[18])
                info[16] += int(line.split()[19])
                info[17] += int(line.split()[20])

        for index,i in enumerate(info):
            values2.append(info[index])

with open('Statistics_Conscience.txt','w') as output:
    output.write("Node\tDuration\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages")
    for i,index in enumerate(values):
        if i % 18 == 0:
            output.write(f"\n{nodes[int (i / 18)][17:-4]}\t")
            output.write(f"{round(values[i],6)}\t")
            i = i+1
        else:
            output.write(f"{values[i]}\t")
    output.write("\n")
  
with open('Statistics_Unconscience.txt','w') as output:
    output.write("Node\tDuration\tSent_0\tSent_1\tSent_2\tSent_3\tSent_4\tSent_5\tSent_6\tTotalSent\tReceived_0\tReceived_1\tReceived_2\tReceived_3\tReceived_4\tReceived_5\tReceived_6\tTotalReceived\tTotalMessages")
    for i,index in enumerate(values2):
        if i % 18 == 0:
            output.write(f"\n{nodes2[int (i / 18)][19:-4]}\t")
            output.write(f"{round(values2[i],6)}\t")
            i = i+1
        else:
            output.write(f"{values2[i]}\t")
    output.write("\n")

with open('Statistics_Conscience.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:-1])
    final_line = file.readline()
    other_lines.sort(key= lambda line: int (line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)
    file.write(final_line)
    file.truncate()

with open('Statistics_Unconscience.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:-1])
    final_line = file.readline()
    other_lines.sort(key= lambda line: int (line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)
    file.write(final_line)
    file.truncate()


with open('Statistics_Conscience.txt', 'r') as file:
    lines = file.readlines()
    data = lines[1:]
    column_sums = [0] * 19
    duration_column = []

    for line in data:
        values = line.split('\t')

        # Iterate over each value in the line, excluding the first one
        for i in range(1, len(values)):
            column_sums[i-1] += float(values[i-1])
            if i == 2:
                duration_column.append(float(values[i-1]))

    column_averages = [round(sum / len(data),6) for sum in column_sums]
    
    duration_min = min(duration_column)
    duration_max = max(duration_column)
    duration_avg = round(statistics.mean(duration_column),6)
    duration_stddev = round(statistics.stdev(duration_column),6)

    with open('Statistics_Conscience.txt', 'a') as file:
        file.write(f"ALL\t{duration_min}\t{duration_max}\t{duration_avg}\t{duration_stddev}\t")
        for i in range(2, len(column_averages)):
            file.write(f"{column_averages[i]}\t")

with open('Statistics_Unconscience.txt', 'r') as file:
    lines = file.readlines()
    data = lines[1:]
    column_sums = [0] * 19
    duration_column = []

    for line in data:
        values = line.split('\t')

        # Iterate over each value in the line, excluding the first one
        for i in range(1, len(values)):
            column_sums[i-1] += float(values[i-1])
            if i == 2:
                duration_column.append(float(values[i-1]))

    column_averages = [round(sum / len(data),6) for sum in column_sums]
    
    duration_min = min(duration_column)
    duration_max = max(duration_column)
    duration_avg = round(statistics.mean(duration_column),6)
    duration_stddev = round(statistics.stdev(duration_column),6)

    with open('Statistics_Unconscience.txt', 'a') as file:
        file.write(f"ALL\t{duration_min}\t{duration_max}\t{duration_avg}\t{duration_stddev}\t")
        for i in range(2, len(column_averages)):
            file.write(f"{column_averages[i]}\t")