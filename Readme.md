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

improveMST is fixed to 1.0 to set a scale. The rest is searched in the tensor product domain [-5,5]^5.

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

point: [-4.36972170015, -3.85753717399, 3.77157840772, -4.12768346872, 2.35091234855]
results: [-206.0, -210.0, -206.0, -206.0, -210.0, -206.0, -208.0, -210.0, -210.0, -212.0]
mean: -208.4
variance: 4.64

point: [0, 0, 0, 0, 0]
results: [-196.0, -202.0, -206.0, -190.0, -208.0, -196.0, -194.0, -206.0, -198.0, -196.0, -196.0, -202.0]
mean: -199.166666667
variance: 28.3055555556

point: [-0.5, 0.3, 0.7, -0.4, 0.6]
results: [-188.0, -204.0, -196.0, -188.0, -196.0, -198.0, -196.0, -190.0, -192.0, -198.0, -202.0, -190.0]
mean: -194.833333333
variance: 25.6388888889

Let's fix variance to 30 in moe.py

Superoptimization
~~~~~~~~~~~~~~~~~

Todo
------

 * https://jaberg.github.io/hyperopt/

