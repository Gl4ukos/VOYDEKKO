import os
import random

filename = "temp_data_scenario.txt"

mean_temp = 30
max_deviation = 2
half_max_deviation = max_deviation / 2
value = 0

f1 = open(filename, "w")

for i in range(300): 
    f1.write(str(mean_temp + round((random.random()*max_deviation),2) - half_max_deviation)) 
    f1.write(",")
f1.write("\n")
	
f1.close() 
print("\nSCENARIO GENERATION COMPLETE\n")
print("Data is written into the file:")
print(str(os.getcwd()) + "/" + filename)
print("\n")
