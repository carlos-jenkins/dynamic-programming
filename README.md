About
=====

Dynamic Programming (DP) is a method for solving complex problems by breaking
a large problem down (if possible) into incremental steps so that, at any given
stage, optimal solutions are known to sub-problems.

DP was formalized by Richard Ernest Bellman (August 26, 1920 - March 19, 1984)
in 1953 in his paper *Dynamic Programming*. Nowadays DP is an important topic
in Operation Research college courses.

This package includes an example GUI application and several classic dynamic
programming algorithms:

- *Bounded knapsack*, used to find the optimal arrangement of items in a medium.
- *Probabilities to become champion*, used to calculate just that in sport games.
- *Optimal binary search tree*, used to find the best tree given a set of nodes
  and their probability of being requested.
- *Equipment replacement*, used to find the optimal replacement plan for any
  equipment given it's cost, maintenance cost and sale price per year.
- *Floyd algorithm*, used to find the shortest path between any pair of nodes
  in a weighted graph.


This Software is written is C and uses GTK+ 3 for the GUI. It uses LaTeX to render
reports and Graphviz to draw graphs and trees.

![Dynamic Programming GUI and reports](https://raw.github.com/carlos-jenkins/dynamic-programming/master/media/wall.png "Dynamic Programming GUI and reports")


How to build
============

Install dependencies:

```shell
sudo apt-get install build-essential texlive libgtk-3-dev graphviz
```

Then build and run:

```shell
cd download/dir
./configure
make
./bin/main
```

How to hack
===========

Install development Software:

```shell
sudo apt-get install glade geany git devhelp libgtk-3-doc
```

We use Kernel code standards except that identation is 4 spaces wide:

- http://www.kernel.org/doc/Documentation/CodingStyle

This package is developed with Gtk+ and glib:

- http://developer.gnome.org/glib/stable/
- http://developer.gnome.org/gtk3/stable/


License
=======

Copyright (C) 2012 Carolina Aguilar <caroagse@gmail.com>  
Copyright (C) 2012 Carlos Jenkins <carlos@jenkins.co.cr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

