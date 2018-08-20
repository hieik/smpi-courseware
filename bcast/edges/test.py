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

count = 0;
for num in range(0, 64):
	for element in result:
		if (num == element):
			count += 1;
	print(num, count);
	count = 0;
print(len(result));
































