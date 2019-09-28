island-lab
==========

``island-lab`` is a some kind of laboratory work. a resolver of *some* game's
puzzle.

*TODO: what's the game's (puzzle's) rules that solve here?* See `The Original
Paper <original-task.jpg>`_

How To Compile And Run?
-----------------------

Compiling
~~~~~~~~~

You need The ``Meson Build System``, The ``GNU Compiler Collection``
(or The ``Clang``) and surely a good mood ;-).

Step-by-step::

        meson setup ../builddir --warnlevel=2 \
                        --buildtype=release -Db_lto=true \
                        --prefix=`pwd`/../island-lab.BUILD

        cd ../builddir

        ninja install

        cd ../island-lab.BUILD

First Run, Getting Options\' Help
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Running visual utility (getting help)::

       bin/island-lab-visual -?

The output::

        Usage: island-lab-visual [OPTION...]
                    PERIMETER-SEQUENCE [BUILDINGS-SEQUENCE [BUILDINGS-SEQUENCE [...]]]

          -i, --indent=INDENT-SIZE   Indentation size
          -r, --print-raw            Print input sequence strings in raw format besides
                                     visualisation
          -?, --help                 Give this help list
              --usage                Give a short usage message
          -V, --version              Print program version

        Mandatory or optional arguments to long options are also mandatory or optional
        for any corresponding short options.

Running resolver utility (getting help)::

       bin/island-lab-resolver -?

The output::

        Usage: island-lab-resolver [OPTION...]

          -1, --single               return no more than single solution, randomly
                                     selected from all resolved
          -c, --continue             continue resolving using explicit passed
                                     buildings-sequence
          -?, --help                 Give this help list
              --usage                Give a short usage message
          -V, --version              Print program version

Usage Example: Resolving Puzzle's Task
--------------------------------------

Assume the task is::

        bin/island-lab-visual 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0

The output::

                  2 2   
          +-------------+ 
          |             |   
          |             |   
        3 |             |   
          |             | 6 
        4 |             | 3 
        4 |             |   
          +-------------+ 
                    4   


Running resolver with it::

        time echo 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0 |
                        ISLAND_LAB_RESOLVER_SHOW_ITERATIONS=1 \
                        ISLAND_LAB_RESOLVER_MAX_BRANCHES=1000000 \
                        bin/island-lab-resolver

The output::

        iteration 1: 6 branches
        iteration 2: 30 branches
        iteration 3: 75 branches
        iteration 4: 315 branches
        iteration 5: 1260 branches
        iteration 6: 3168 branches
        iteration 7: 12672 branches
        iteration 8: 42876 branches
        iteration 9: 52110 branches
        iteration 10: 81342 branches
        iteration 11: 169578 branches
        iteration 12: 126082 branches
        iteration 13: 252164 branches
        iteration 14: 207856 branches
        iteration 15: 213546 branches
        iteration 16: 365456 branches
        iteration 17: 99692 branches
        iteration 18: 17150 branches
        iteration 19: 14057 branches
        iteration 20: 11812 branches
        iteration 21: 31821 branches
        iteration 22: 63760 branches
        iteration 23: 63328 branches
        iteration 24: 12995 branches
        iteration 25: 19790 branches
        iteration 26: 9044 branches
        iteration 27: 9740 branches
        iteration 28: 6699 branches
        iteration 29: 3756 branches
        iteration 30: 185 branches
        iteration 31: 54 branches
        iteration 32: 51 branches
        iteration 33: 24 branches
        iteration 34: 3 branches
        iteration 35: 1 branches
        iteration 36: 1 branches
        5,6,1,4,3,2,4,1,3,2,6,5,2,3,6,1,5,4,6,5,4,3,2,1,1,2,5,6,4,3,3,4,2,5,1,6

        real	0m1.963s
        user	0m1.825s
        sys	0m0.135s

The answer graphically is::

        bin/island-lab-visual 0,0,0,2,2,0,0,0,0,6,3,0,0,4,0,0,0,0,4,4,0,3,0,0 \
                        5,6,1,4,3,2,4,1,3,2,6,5,2,3,6,1,5,4,6,5,4,3,2,1,1,2,5,6,4,3,3,4,2,5,1,6

The output::

                  2 2   
          +-------------+ 
          | 5 6 1 4 3 2 |   
          | 4 1 3 2 6 5 |   
        3 | 2 3 6 1 5 4 |   
          | 6 5 4 3 2 1 | 6 
        4 | 1 2 5 6 4 3 | 3 
        4 | 3 4 2 5 1 6 |   
          +-------------+ 
                    4   

Advanced Compiling
------------------

Before executing ``ninja`` command you can fine tune a compile configuration.
Being in ``builddir`` to see all options::

        meson configure

The output fragment::

        Project options:
          Option               Current Value Possible Values Description                                                                         
          ------               ------------- --------------- -----------                                                                         
          custom-reallocarray  false         [true, false]   Custom (foolish) implementation of reallocarray()                                   
          default-max-branches 16777216      >=1             Default limit value how many branches can exist at the same time while resolving    
          general-size         6             >=1             Puzzle's general size value                                                         
          init-buildings-heap  16384         >=1             How big memory block should be allocated first time each resolving iteration started
          link-argp            false         [true, false]   Link argp library                                                                   
          no-random            false         [true, false]   Don't use getrandom()                                                               

Example of setting::

        meson setup ../builddir --warnlevel=2 \
                        --buildtype=release -Db_lto=true \
                        --prefix=`pwd`/../island-lab.BUILD

        cd ../builddir

        meson configure \
                -Dinit-buildings-heap=524288 \
                -Ddefault-max-branches=1048576

        ninja install

        cd ../island-lab.BUILD

Compiling With Elbrus or With Another Stupid Compiler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Stupid compilers like ``Elbrus`` (``lcc``) will success compile the project
with the next options::

        meson configure \
                -Dcustom-reallocarray=true \
                -Dno-random=true

Compiling With Clang
~~~~~~~~~~~~~~~~~~~~

It's enough to set ``CC`` environment to ``clang`` value on ``meson setup``
stage, like in this example::

        CC=clang meson setup ../buildclang --warnlevel=2 \
                        --buildtype=release -Db_lto=true \
                        --prefix=`pwd`/../island-lab.BUILDCLANG

        cd ../buildclang

        ninja install

        cd ../island-lab.BUILDCLANG
