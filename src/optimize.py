#!/usr/bin/env python

import sys
import re

class ThreeAC:
	# Initializer
	def __init__(self, cmd, arg1, arg2, arg3):
		self.cmd = cmd
		self.arg1 = arg1
		self.arg2 = arg2
		self.arg3 = arg3
	
	# String Representation
	def __str__(self):
		rep = self.cmd + ": " + str(self.arg1) + " " + str(self.arg2) + " " + str(self.arg3)
		return rep

class Tiny:
	# Initializer
	def __init__(self, cmd, arg1, arg2, p1=None, p2=None):
		self.cmd = cmd
		self.arg1 = arg1
		self.arg2 = arg2
	
	# String Representation
	def __str__(self):
		if self.arg1 == None and self.arg2 == None:
			rep = self.cmd
		elif self.arg1 != None and self.arg2 == None:
			rep = self.cmd + " " + str(self.arg1)
		else:
			rep = self.cmd + " " + str(self.arg1) + " " + str(self.arg2)
		return rep

if __name__ == '__main__':
	# 3AC Data Structure
	ThreeAC_lines = []
	ThreeAC_DS = []
	start = False
	# Tiny Data Structure
	Tiny_lines = []
	Tiny_DS = []
	# Symbol Table Data Structure
	SymbolTable_DS = {}
	SymbolTable = []
	st_flag = False
	# 4-Register Status Hash Table
	reg4 = {}
	reg4['r0'] = [None, 'ND']
	reg4['r1'] = [None, 'ND']
	reg4['r2'] = [None, 'ND']
	reg4['r3'] = [None, 'ND']
	reg200 = {}
	prev_reg = None
	prev_pop = None

	with open(sys.argv[1], 'r') as myFile:
		all_lines = myFile.readlines()
		for line in all_lines:
			if line != "\n":
				line = line.rstrip("\n")
				if line[0] == ';':
					ThreeAC_lines.append(line)
				else:
					Tiny_lines.append(line)
	
	for ThreeAC_line in ThreeAC_lines:
		# Begin collecting data for Symbol Tables
		if ThreeAC_line.split()[0] == ";Symbol":
			if st_flag == True:
				SymbolTable_DS[st_key] = SymbolTable
			st_key = ThreeAC_line.split()[2]
			st_flag = True
			SymbolTable = []
		elif st_flag == True:
			SymbolTable.append(ThreeAC_line)
		
		# Begin collecting data for IR nodes
		if ThreeAC_line == ";IR code":
			st_flag = False
			start = True
		elif start == True:
			split = ThreeAC_line.split()
			if len(split) == 1:
				ThreeAC_DS.append(ThreeAC(split[0], None, None, None))
			elif len(split) == 2:
				ThreeAC_DS.append(ThreeAC(split[0], split[1], None, None))
			elif len(split) == 3:
				ThreeAC_DS.append(ThreeAC(split[0], split[1], split[2], None))
			elif len(split) == 4:
				ThreeAC_DS.append(ThreeAC(split[0], split[1], split[2], split[3]))
	
	# Print SymbolTable (debug)
	#for thing in SymbolTable_DS:
	#	print(thing)
	#	print(SymbolTable_DS[thing])
	# Print ThreeAC (debug)
	#for thing in ThreeAC_DS:
	#	print(thing)
	
	for Tiny_line in Tiny_lines:
		split = Tiny_line.split()
		if len(split) == 1:
			Tiny_DS.append(Tiny(split[0], None, None))
		elif len(split) == 2:
			Tiny_DS.append(Tiny(split[0], split[1], None))
		elif len(split) == 3:
			Tiny_DS.append(Tiny(split[0], split[1], split[2]))
		elif len(split) > 3:
			Tiny_DS.append(Tiny(split[0], split[1], ' '.join(split[2:])))
	
	# Print Tiny (debug)
	#for thing in Tiny_DS:
	#	print(thing)

	# Open Output File
	myFile = open(sys.argv[1], 'w')
	
	# Push Flag
	pushf = 0

	# Write out to the file in 4 registers
	for node in Tiny_DS:
		if node.cmd == "str":
			myFile.write(str(node) + "\n")
		elif node.cmd == "var":
			myFile.write(str(node) + "\n")
		elif node.cmd == "push":
			if node.arg1 == None:
				myFile.write(str(node) + "\n")
			else:
				if re.match(r"r[\d]+", node.arg1):
					reg4['r0'] = [None, 'ND']
					reg4['r1'] = [None, 'ND']
					reg4['r2'] = [None, 'ND']
					reg4['r3'] = [None, 'ND']
					if node.arg1 == "r0":
						buf = str(node) + "\n"
						pushf += 1
					elif node.arg1 == "r1" and pushf == 1:
						buf = buf + str(node) + "\n"
						pushf += 1
					elif node.arg1 == "r2" and pushf == 2:
						buf = buf + str(node) + "\n"
						pushf += 1
					elif node.arg1 == "r3" and pushf == 3:
						buf = buf + str(node) + "\n"
						pushf = 0
						myFile.write(buf)
					else:
						myFile.write(node.cmd + " " + reg200[node.arg1] + "\n")
				else:
					myFile.write(str(node) + "\n")
		elif node.cmd == "pop":
			if node.arg1 == None:
				myFile.write(str(node) + "\n")
			else:
				if node.arg1 in reg200:
					myFile.write(str(node) + "\n")
				#	reg4[node.arg1][0] = "something"
				#	reg200[node.arg1] = node.arg1
				#	prev_pop = node.arg1
				else:
					# SPILL REGISTER
					for r in ['r0', 'r1', 'r2', 'r3']:
						if reg4[r][1] == 'ND' and prev_pop != r:
							reg4[r][0] = "something"
							reg200[node.arg1] = r
							prev_pop = r
							break
					myFile.write(node.cmd + " " + reg200[node.arg1] + "\n")
		elif node.cmd[:3] == "cmp":
			reg4[reg200[node.arg1]] = [None, 'ND']
			reg4[reg200[node.arg2]] = [None, 'ND']
			myFile.write(node.cmd + " " + reg200[node.arg1] + " " + reg200[node.arg2] + "\n")
		elif node.cmd[:3] in ["add", "sub", "mul", "div"]:
			if re.match(r"r[\d]+", node.arg1):
				reg4[reg200[node.arg1]][1] = 'ND'
			reg4[reg200[node.arg2]][1] = 'D'
			if node.arg1[0] == "r":				myFile.write(node.cmd + " " + reg200[node.arg1] + " " + reg200[node.arg2] + "\n")
			else:
				myFile.write(node.cmd + " " + node.arg1 + " " + reg200[node.arg2] + "\n")
		elif node.cmd == "sys":
			myFile.write(str(node) + "\n")
		elif node.cmd == "label":
			myFile.write(str(node) + "\n")
		elif node.cmd == "link":
			myFile.write(str(node) + "\n")
		elif node.cmd == "unlnk":
			myFile.write(str(node) + "\n")
		elif node.cmd == "ret":
			reg4['r0'] = [None, 'ND']
			reg4['r1'] = [None, 'ND']
			reg4['r2'] = [None, 'ND']
			reg4['r3'] = [None, 'ND']
			myFile.write(str(node) + "\n")
		elif node.cmd[0] == "j":
			myFile.write(str(node) + "\n")
		elif node.cmd == "move":
			if re.match(r"r[\d]+", node.arg2):
				if reg4['r0'][0] == None and prev_reg != 'r0':
					prev_reg = 'r0'
					reg200[node.arg2] = 'r0'
					node.p2 = 'r0'
					reg4['r0'][0] = node.arg1
					reg4['r0'][1] = 'ND'
				elif reg4['r1'][0] == None and prev_reg != 'r1':
					prev_reg = 'r1'
					reg200[node.arg2] = 'r1'
					node.p2 = 'r1'
					reg4['r1'][0] = node.arg1
					reg4['r1'][1] = 'ND'
				elif reg4['r2'][0] == None and prev_reg != 'r2':
					prev_reg = 'r2'
					reg200[node.arg2] = 'r2'
					node.p2 = 'r2'
					reg4['r2'][0] = node.arg1
					reg4['r2'][1] = 'ND'
				elif reg4['r3'][0] == None and prev_reg != 'r3':
					prev_reg = 'r3'
					reg200[node.arg2] = 'r3'
					node.p2 = 'r3'
					reg4['r3'][0] = node.arg1
					reg4['r3'][1] = 'ND'
				else:
					# SPILL REGISTER
					for r in ['r0', 'r1', 'r2', 'r3']:
						if reg4[r][1] == 'ND' and prev_reg != r:
							prev_reg = r
							reg200[node.arg2] = r
							node.p2 = r
							reg4[r][0] = node.arg1
							reg4[r][1] = 'ND'
							break
			else:
				node.p2 = node.arg2 
			# arg1
			if re.match(r"r[\d]+", node.arg1):
				node.p1 = reg200[node.arg1]
				reg4[node.p1] = [None, 'ND']
			else:
				node.p1 = node.arg1
			
			myFile.write(node.cmd + " " + node.p1 + " " + node.p2 + "\n")

	myFile.close()
