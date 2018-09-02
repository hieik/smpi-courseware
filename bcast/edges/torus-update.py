import sys, getopt, os, re




file_name = sys.argv[1];
current_dir = os.getcwd();
result = [];
print(file_name);
print(current_dir);
with open(current_dir + "/" + file_name, "r") as f:
	contents = f.read();
	result = re.split(" |\n", contents);
result.remove("");
for num in range(0, len(result)):
	result[num] = int(result[num]);
obj_list = [];
for num1 in range(0, len(result)):
	if ((num1+1) % 3 != 0):
		obj_list.append(result[num1]);	
		
print (obj_list);
write_list = [];
for num in range(0,len(obj_list)):
	if (num % 2 == 0):
		write_list.append(str(obj_list[num]) + " " + str(obj_list[num+1]) + "\n");
print (write_list);
file_name = "update_" + file_name; 
current_dir = os.getcwd();
with open(current_dir + "/" + file_name, "w") as f:
	f.writelines(write_list);


"""
remove_index = [];
for num1 in range(0, len(result)):
	for num2 in range(num1, len(result)):
		if (result[num1] == result[num2] and num1 != num2):
			if (num1 % 2 == 0):
				if (num2 % 2 == 0):
					if (result[num1+1] == result[num2+1]):
						print (num1, num2);
						print (result[num1], result[num1+1], result[num2], result[num2+1]);
						remove_index.append(num2);
				else:
					if (result[num1+1] == result[num2-1]):
						remove_index.append(num2);
			else:
				if (num2 % 2 == 0):
					if (result[num1-1] == result[num2+1]):
						remove_index.append(num2);
				else:
					if (result[num1-1] == result[num2-1]):
						remove_index.append(num2);

remove_index = list(set(remove_index));
remove_index.sort();
for num in range(0,len(remove_index)):
	result.pop(remove_index[num] - num);

print ("\n");
print (result);
"""

