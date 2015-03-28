#!/usr/bin/env python
import matplotlib.pyplot as plt
import csv

scores = []
with open('moveheuristic.csv', 'rb') as csvfile:
	reader = csv.reader(csvfile)
	for row in reader:
		row = [float(x) for x in row]
		scores.append(row[-1])

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
