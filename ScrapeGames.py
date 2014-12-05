#!/bin/sh
# coding=utf8
''':'
exec ipython --no-banner --no-confirm-exit -i "$0" "$@"
'''
from urllib2 import urlopen
from bs4 import BeautifulSoup

def getSoup(url):
	print url
	html = urlopen(url).read()
	soup = BeautifulSoup(html)
	return soup

def parseTable(table):
	return [[x.text for x in row.find_all('td')] for row in table.find_all('tr')]

def scrapeGame(game):
	url = "http://www.codecup.nl/showgame.php?ga=" + str(game)
	soup = getSoup(url)
	table = parseTable(soup.find_all("table","moves")[0])
	moves = []
	for move in table:
		if len(move) != 3:
			continue
		white = move[1].replace("'","")
		black = move[2].replace("'","")
		if white == "":
			moves.append(black)
		else:
			moves.append(white)
	return ', '.join(moves)

games = [g for r in [range(305, 482+1), range(713,1202+1), range(1365,1946+1),range(2173,2962+1), range(3084,3979+1)] for g in r]
with open("games.txt", mode='w') as f:
	for game in games:
		try:
			f.write(scrapeGame(game) + "\n")
		except:
			pass

