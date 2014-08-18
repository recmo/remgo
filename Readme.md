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


Todo
------

 * https://github.com/Yelp/MOE
 * https://jaberg.github.io/hyperopt/
