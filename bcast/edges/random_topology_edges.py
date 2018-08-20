import sys, getopt, os, random, xml.dom.minidom

if len(sys.argv) < 3:
    print ('invalid args');
    sys.exit(0);


if int(sys.argv[1]) <= 1:
    print ('please input correct topology:');
    sys.exit(0);

if int(sys.argv[2]) < 1:
    print ('please input correct number');
    sys.exit(0);

node_number = int(sys.argv[1]);
order = int(sys.argv[2]);
current_order = [];
random_range = [];
count = 0;
write_list = [];
print (node_number);
print (order);

for num in range(node_number):
    random_range.append(num);
    current_order.append(0);

for num in range(node_number):
	for i in range(order):
		if (current_order[num] < order):
			adj_node = random.choice(random_range);
			while (adj_node == num):
				adj_node = random.choice(random_range);
			current_order[adj_node] = current_order[adj_node] + 1;
			current_order[num] = current_order[num] + 1;
			print (num, adj_node, count);
			write_list.append(str(num) + " " + str(adj_node) + "\n");
			count += 1;
			if (current_order[adj_node] == order):
				random_range.remove(adj_node);
			if (current_order[num] == order):
				random_range.remove(num);
print (count);
print (current_order);
print (random_range);
print (write_list);

file_name = "random_topology_node" + str(node_number) + "order"+ str(order) + ".edges"
current_dir = os.getcwd();
with open(current_dir + "/" + file_name, "w") as f:
	f.writelines(write_list);

"""
current_dir = os.getcwd();
object_dir = current_dir + '/platform';

if (not os.path.exists(object_dir)):
    os.makedirs(object_dir);
    print ("create " + object_dir + " successfully!");

topology = sys.argv[1];
number_node = int(sys.argv[2]);
file_name = topology + "_" + str(number_node) + ".xml";
print (topology);
print (number_node);
print (file_name);
doc = xml.dom.minidom.Document();

object_file = open(object_dir + "/" + file_name, 'w');

doc.writexml(object_file, indent='\t', addindent='\t', newl='\n', encoding="utf-8");

object_file.close();
"""































