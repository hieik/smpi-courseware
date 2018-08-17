import sys, getopt, os, re




file_name = "random.edges";
current_dir = os.getcwd();
contents = "test";
print(file_name);
print(current_dir);
with open(current_dir + "/" + file_name, "w") as f:
	f.write(contents);

































