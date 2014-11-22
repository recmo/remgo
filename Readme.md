Codecup 2015: AYU
=================

Links:
------

 * http://www.codecup.nl/rules_ayu.php
 * http://www.codecup.nl/sample_game_ayu.php
 * http://www.codecup.nl/download_caia.php
 * http://www.mindsports.nl/index.php/arena/ayu


Setting up the source code
------

    git clone git@github.com:Recmo/Codecup-2015-Ayu.git
    cd Codecup-2015-Ayu

Setting up caia
------

    wget http://www.codecup.nl/download/caia_ayu/linux64/caia_ayu_linux64.tar.gz
    tar xvf caia_ayu_linux64.tar.gz
    caia_install_ayu/install_game.sh
    mv ~/caia .
    rm -r caia_ayu_linux64.tar.gz caia_install_ayu
    sed -i 's|$HOME/caia/ayu|$(dirname $0)/..|' caia/ayu/bin/competition.sh
    sed -i 's/| sort | uniq |//' caia/ayu/bin/competition.sh

Compiling
------

    make

Testing
------

    make competition


Uint128 benchmarking
---------------------

The competition server has no __int128 intrinsics, so they need to be replaced by either a pair of uint64s or 128 bit sse register (which can be enable trough some trickery, despite the compilation flags).

A benchmark shows the following:

| Tech      | Componentized | Monolith | Competition
| intrinsic |      18367.8  | 19104.7  | unavailable
| uint64    |      12852    | 12884.9  |     4351.73
| sse       |       4279.59 |  4279.59 |     1648.84

But the SSE one still allows optimizations:

http://mischasan.wordpress.com/2011/04/04/what-is-sse-good-for-2-bit-vector-operations/
http://mischasan.wordpress.com/2013/04/07/the-c-preprocessor-not-as-cryptic-as-youd-think/

After some optimizations:

| Tech      | Componentized | Monolith | Competition
| intrinsic |      18098.8  | 18948.2  | unavailable
| uint64    |      14596.1  | 14664.5  |      4283.9
| sse       |      15853.9  | 16202.8  |      5866.11

-> Go for SSE

Laptop sse: 10101.4 (Componentized)
10293.0 (Monolith)
10764.4
11408.8
14650.4
18751.3

Competition server non-sse random point: 10724.9
Competition server sse random point:      7741.27

-> non-sse random

Heuristic
----------

Dijsktra minimal spanning forest.

| player - opponent | 146
| player            | 142
| - opponent        | 120
| opponent          |  74
| - player          |  76
| opponent - player |  64


MoveHeuristic superoptimization
-------------------------------

Using https://github.com/Yelp/MOE

Coordinates improveMST, nonLeafPiece, opponentHinder, freeNeighbor, opponentNeighbor

Parameters are searched in the tensor product domain [-1,1]^6.

Against: random player1 player2 d930825 d17a66e

Iteration time: a little over 2 minutes.

Example ranking:

   292  d930825
   244  d17a66e
   231  player2
   188  heuristic
   126  player1
   116  random


Variance estimation:
~~~~~~~~~~~~~~~~~~~~

point: [1.0, -4.36972170015, -3.85753717399, 3.77157840772, -4.12768346872, 2.35091234855]
results: [-206.0, -210.0, -206.0, -206.0, -210.0, -206.0, -208.0, -210.0, -210.0, -212.0]
mean: -208.4
variance: 4.64

point: [1.0, 0, 0, 0, 0, 0]
results: [-196.0, -202.0, -206.0, -190.0, -208.0, -196.0, -194.0, -206.0, -198.0, -196.0, -196.0, -202.0]
mean: -199.166666667
variance: 28.3055555556

point: [1.0, -0.5, 0.3, 0.7, -0.4, 0.6]
results: [-188.0, -204.0, -196.0, -188.0, -196.0, -198.0, -196.0, -190.0, -192.0, -198.0, -202.0, -190.0]
mean: -194.833333333
variance: 25.6388888889

Let's fix variance to 30 in moe.py

Scale by 500 so target is in [-1, 1] range. Variance will then be 0.00012

[0.930474106789, -0.918639224958, 0.571296722195, 0.233767389304, -0.908095495768, -0.116614567415] 234.0 -3.26666666667
[0.0413357599157, -0.995151974535, 0.645893726483, 0.368686978302, -0.323848742195, 0.0300609091988] 234.0 -1.06153846154
[0.790257842448, -0.827154010516, 0.755149924007, 0.620733077547, -0.615833535838, 0.308632890251] 232.0


Superoptimization
~~~~~~~~~~~~~~~~~

Top five:

A [0.4757581709, -0.9427917926, 0.9835744161, 0.8082476345, -0.6968700888, 0.1697805126] 242
B [0.2585045229, -0.8168569930, 0.9809096927, 0.9944286241, -0.6304206655, 0.0525435460] 240
C [0.8882947183, -0.8947302137, 0.7937447684, 0.9759108708, -0.5054821013, 0.1531433269] 236
D [0.9904633522, -0.9713187387, 0.7988958854, 0.5083063491, -0.4856704579, 0.2060350792] 236
E [0.9966120224, -0.8513541553, 0.5827791035, 0.5507991994, -0.4879240932, 0.0900042965] 236

Average of top ten:

F [0.5650600414, -0.9113862364, 0.755007548, 0.7574727316, -0.488547432, 0.1166214699]

Mix of B and C

G [0.5733996206, -0.8557936033, 0.8873272305, 0.9851697474, -0.5679513834, 0.1028434364]

Accurate repeated measurements:

A [204.0, 218.0, 224.0, 220.0, 224.0, 220.0, 220.0, 226.0, 236.0, 234.0]                      222.6 71.24
B [242.0, 220.0, 232.0, 228.0, 228.0, 232.0, 220.0, 224.0, 226.0, 216.0, 222.0, 238.0, 226.0] 227.230769231 50.1775147929
C [236.0, 216.0, 232.0, 214.0, 214.0, 218.0, 242.0, 226.0, 240.0, 234.0]                      227.2 108.96
D [216.0, 232.0, 228.0, 216.0, 220.0, 218.0, 224.0, 218.0, 224.0, 218.0, 210.0]               220.363636364 34.7768595041
E [226.0, 218.0, 230.0, 226.0, 224.0, 240.0, 230.0, 214.0, 218.0, 220.0]                      224.6 52.04
F [232.0, 224.0, 224.0, 222.0, 208.0, 220.0, 230.0, 226.0, 226.0, 232.0, 224.0, 218.0]        223.833333333 40.3055555556
G 

Winner:

B [0.2585045229, -0.8168569930, 0.9809096927, 0.9944286241, -0.6304206655, 0.0525435460]


Superoptimization of MCTS
~~~~~~~~~~~~~~~~~~~~~~~~~

TreeNode::explorationParameter = 0.35;


Fat playouts
~~~~~~~~~~~~~~~~~~~~~~~~~

Using randomMove() in rollOut(): 291

Using heuristicMove() in rollOut(): 179




New heuristic optimizer
~~~~~~~~~~~~~~~~~~~~~~~

1) Construct a large set of board positions
2) Use many MCTS iterations to get accurate game theoretic values of the moves
3) Compare the MCTS sort with the heuristic sort




HashGo
-------

Starting position, nine-fold split visiting

Un-oriented: 3736 nodes, 47 unique

Oriented: 3736 nodes, 36 unique

| Random moves | Un oriented regions | Oriented regions |
|  0 |  47     | 36      |
| 10 | 103.331 | 75.0983 |
| 20 | 119.368 | 87.3449 |
| 30 | 127.238 | 92.9821 |
| 40 | 130.784 | 96.4891 |
| 50 | 130.168 | 98.0147 |
| 60 | 126.836 | 97.6558 |

All games: 1309097 vs 967344
Symmeterized: 4445041  vs 3480153 

Symmetrization gives about a 25% reduction.


5465435

10000 games simulated for 50 random moves, then recursively scanned on all sub pieces: 255638

With colour-symmetry: 249341


Test symmetry finding:
~~~~~~~~~~~~~~~~~~~~~~

Dataset: games.csv, 2031 games

9084e6a: 1279838 fragments

without unique optimization: 1190328 fragments
with colour symmetry detection: 1116401 fragments
with full symmetry reduction: 762933 fragments


Todo
------

 * https://jaberg.github.io/hyperopt/

