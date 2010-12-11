
import sys

if __name__ == '__main__':
	f = sys.stdin
	m = {}
	for line in f:
		line = line.replace("cost=", "")
		line = line.replace("size=", "")
		line = line.replace("time=", "")
		line = line.replace("p=", "")
		fields = line.split(" ")
		key = (int(fields[2]), int(fields[1]))
		if m.has_key(key):
			m[key] += [float(fields[3])]
		else:
			m[key] = [float(fields[3])]

	result = []
	for tupla in m.keys():
		s = 0.0
		for x in m[tupla]:
			s += x
		size = len(m[tupla])
		result += [(tupla[0]-1, tupla[1], s/size)]

	result.sort()
	for r in result:
		s =  str(r)
		s = s[1:-1]
		print s
		
