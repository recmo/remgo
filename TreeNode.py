#!/usr/bin/env python
import math
import random
from moe.easy_interface.experiment import Experiment
from moe.easy_interface.simple_endpoint import gp_next_points
from moe.easy_interface.simple_endpoint import gp_hyper_opt
from moe.optimal_learning.python.data_containers import SamplePoint
from moe.views.constant import GP_NEXT_POINTS_KRIGING_ROUTE_NAME
from subprocess import check_output
import re
import datetime
import csv
import numpy
import matplotlib.pyplot as plt

#valmin = 0.0
#valmax = 500.0
valmin = 100.0
valmax = 300.0
#valmin = 180.0
#valmax = 220.0
valrange = valmax - valmin
scale = - 2 / valrange
offset = 1.0 - valmin * scale
variance = 100 / (valrange * valrange) # Empirical

result = re.compile(r'^\s+(\d+)\s+monolith\s*$', re.MULTILINE)


def function_to_minimize(x):
	print x
	
	# Write new config file
	with open("TreeNode.conf.h", mode='w') as f:
		f.write("// One dimensional tweak space, need to optimize with Yelps MOE\n")
		f.write("const float TreeNode::explorationParameter = " + str(x[0]) + ";\n")
	
	# Compile and compete
	res = check_output("make competition 2>&1", shell=True)
	print res
	
	# Extract score
	score = float(result.search(res).group(1))
	print score
	
	# We want to maximize the score
	return score


# Variance estimator arround a point
#values = []
#x = [1.5]
#while True:
#	print values, numpy.mean(values), numpy.var(values)
#	values.append(function_to_minimize(x))

# Experiment tensor product domain
exp = Experiment([[0.000001, 3.0]])

# Bootstrap with some known or already sampled point(s)
samplepoints = []
coords = []
scores = []
with open('TreeNode.csv', 'rb') as csvfile:
	reader = csv.reader(csvfile)
	for row in reader:
		row = [float(x) for x in row]
		x = row[:-1]
		score = row[-1]
		coords.append(x[0])
		scores.append(score)
		print x, score
		samplepoints.append(SamplePoint(x, offset + score * scale, variance))
exp.historical_data.append_sample_points(samplepoints)

with plt.xkcd():
	fig = plt.figure()
	ax = fig.add_axes((0.1, 0.2, 0.8, 0.7))
	ax.spines['right'].set_color('none')
	ax.spines['top'].set_color('none')
	plt.tick_params(color='#ffffff')
	plt.plot(scores, 'k+')
	plt.xticks([])
	plt.yticks([100,150,200])
	plt.title('Superoptimization of the Hyperparameters')
	plt.xlabel('experiment')
	plt.ylabel('score')
plt.show()

with plt.xkcd():
	fig = plt.figure()
	ax = fig.add_axes((0.1, 0.2, 0.8, 0.7))
	ax.spines['right'].set_color('none')
	ax.spines['top'].set_color('none')
	plt.tick_params(color='#ffffff')
	plt.plot(coords, scores, 'k+')
	plt.xticks([0,1,2,3])
	plt.yticks([100,150,200])
	plt.title('Superoptimization of the Hyperparameters')
	plt.xlabel('C')
	plt.ylabel('score')
plt.show()

assert False

def itterate():
	print str(datetime.datetime.now())
	
	# Use MOE to determine what is the point with highest Expected Improvement to use next
	next_point_to_sample = gp_next_points(exp)[0]
	print next_point_to_sample
	# By default we only ask for one point
	
	# Sample the point from our objective function, we can replace this with any function
	value_of_next_point = function_to_minimize(next_point_to_sample)
	
	print next_point_to_sample, value_of_next_point
	
	# Store the sample
	with open('TreeNode.csv', 'a') as csvfile:
		csvfile.write(", ".join([str(k)for k in next_point_to_sample + [value_of_next_point]] ) +"\n")
	
	# Add the information about the point to the experiment historical data to inform the GP
	exp.historical_data.append_sample_points([SamplePoint(next_point_to_sample, offset + value_of_next_point * scale, variance)])
	# We can add some noise


while True:
	itterate()
