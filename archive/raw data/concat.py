import os, csv
path = "C:/Users/infin/OneDrive/Documents/GitHub/neopixelpi/"
sub = "belize reef data"
outname = "belize"
concat = []
counter = {}
for file in os.listdir(path + sub):
	if file.endswith(".csv"):
		with open(path + file, 'r', newline='') as csvfile:
			reader = csv.reader(csvfile, delimiter=',')
			next(reader)
			next(reader)
			for row in reader:
				concat.append(row)
				year = row[0][:4]
				if not year in counter:
					counter[year] = 1
				else:
					counter[year] += 1

concat.sort()
print(counter)
keys = list(counter.keys())
keys.sort()
print(keys)
        
with open(path + outname + '.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    writer.writerow(["date","lat","lon","dhw", "sst"])
    for row in concat:
    	writer.writerow(row)