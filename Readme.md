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

Laptop sse: 10101.4 (Componentized) 10293 (Monolith)
10764.4
11408.8

Todo
------

 * https://github.com/Yelp/MOE
 * https://jaberg.github.io/hyperopt/

