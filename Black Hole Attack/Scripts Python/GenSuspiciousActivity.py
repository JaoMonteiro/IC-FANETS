#--------------------------------------------
#  Code developed by João Monteiro Delveaux Silva - UFMG <joaomont@ufmg.br>
#  Must be placed inside simulation folder
#  This code opens  SuspiciousActivity file and summarizes min max avg and stddev from all lines
#--------------------------------------------
import statistics

with open('SuspiciousActivity.txt', 'r+') as file:
    lines = file.readlines()
    first_line = lines[0]
    other_lines = (lines[1:])
    other_lines.sort(key= lambda line: float(line.split()[0]))
    file.seek(0)
    file.write(first_line)
    file.writelines(other_lines)

division_results = []
latencies = []
fourth_column_numbers = []
instant_blocks = 0


with open("SuspiciousActivity.txt", "r") as file:
    lines = file.readlines()



for i in range(len(lines)):
    if i == 0:  
        continue

    
    
    parts = lines[i].split("\t")

    if parts[4] == "-1":
        
        parts[5] = "1"
        instant_blocks+=1

    fourth_column_number = int(parts[4])
    fourth_column_numbers.append(fourth_column_number)
    

    if float(parts[8]) > 1:
        
        division_result = float(parts[9]) / (float(parts[8]))
        division_results.append(division_result)
    else:
        division_results.append(0)

    
min_value = round(min(division_results),2)
max_value = round(max(division_results),2)
average_value = round(statistics.mean(division_results),2)
std_deviation = round(statistics.stdev(division_results),2)

for u in range(1, len(fourth_column_numbers)):
    if fourth_column_numbers[u-1] == 0 and fourth_column_numbers[u] == -1:
        parts1 = lines[u+1].split("\t")
        parts2 = lines[u].split("\t")
        if(parts1[3] == parts2[3]):
            if(float(parts1[0]) - float (parts2[0]) < 1):
                latencies.append(0)
    if fourth_column_numbers[u] == 6:
      parts = lines[u+1].split("\t")
      latencies.append(float(parts[1]))


print(latencies)
lines.append(f"\nInstant Blocks: {instant_blocks}")

lines.append(f"\n{instant_blocks}")
lines.append(f"\n{min_value}\t{max_value}\t{average_value}\t{std_deviation}")
lines.append(f"\n{min_value}\t{max_value}\t{average_value}\t{std_deviation}")


    
min_value = round(min(latencies),6)
max_value = round(max(latencies),6)
average_value = round(statistics.mean(latencies),6)
std_deviation = round(statistics.stdev(latencies),6)




with open("SuspiciousActivity.txt", "w") as file:
    file.writelines(lines)