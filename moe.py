#!/usr/bin/env python
import math
import random
from moe.easy_interface.experiment import Experiment
from moe.easy_interface.simple_endpoint import gp_next_points
from moe.optimal_learning.python.data_containers import SamplePoint
from subprocess import check_output
import re
import datetime
import csv
import numpy


# scale = 1
# variance = 30.0

scale = 500.0
variance = 0.00012


result = re.compile(r'^\s+(\d+)\s+heuristic\s*$', re.MULTILINE)

def function_to_minimize(x):
	print x
	
	# Write new config file
	with open("MoveHeuristic.conf.h", mode='w') as f:
		f.write("// Six dimensional tweak space, need to optimize with Yelps MOE\n")
		f.write("const float MoveHeuristic::improveMSTScore = " + str(x[0]) + ";\n")
		f.write("const float MoveHeuristic::nonLeafPieceScore = " + str(x[1]) + ";\n")
		f.write("const float MoveHeuristic::opponentHinderScore = " + str(x[2]) + ";\n")
		f.write("const float MoveHeuristic::opponentBlockScore = " + str(x[3]) + ";\n")
		f.write("const float MoveHeuristic::freeNeighborScore = " + str(x[4]) + ";\n")
		f.write("const float MoveHeuristic::opponentNeighborScore = " + str(x[5]) + ";\n")
	
	# Compile and compete
	res = check_output("make competition 2>&1", shell=True)
	
	# Extract score
	score = float(result.search(res).group(1))
	print score
	
	# We want to maximize the score
	return -score


# Variance estimator arround a point
#values = []
#while True:
#	print values, numpy.mean(values), numpy.var(values)
#	values.append(function_to_minimize([-0.5, 0.3, 0.7, -0.4, 0.6]))

# 6D experiment, we build a tensor product domain
exp = Experiment([[-1, 1]] * 6)

# Bootstrap with some known or already sampled point(s)
samplepoints = []
with open('moveheuristic.csv', 'rb') as csvfile:
	reader = csv.reader(csvfile)
	for row in reader:
		row = [float(x) for x in row]
		x = row[:-1]
		y = row[-1]
		samplepoints.append(SamplePoint(x, y / scale, variance))
exp.historical_data.append_sample_points(samplepoints)

def itterate():
	print str(datetime.datetime.now())
	
	# Use MOE to determine what is the point with highest Expected Improvement to use next
	next_point_to_sample = gp_next_points(exp)[0]
	# By default we only ask for one point
	
	# Sample the point from our objective function, we can replace this with any function
	value_of_next_point = function_to_minimize(next_point_to_sample)
	
	print next_point_to_sample, value_of_next_point
	
	# Store the sample
	with open('moveheuristic.csv', 'a') as csvfile:
		csvfile.write(", ".join([str(k)for k in next_point_to_sample + [value_of_next_point]] ) +"\n")
	
	# Add the information about the point to the experiment historical data to inform the GP
	exp.historical_data.append_sample_points([SamplePoint(next_point_to_sample, value_of_next_point  / scale, variance)])
	# We can add some noise


while True:
	itterate()
