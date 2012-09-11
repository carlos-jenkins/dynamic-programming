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
- *Optimal binary search trees*, used to find the best tree given a set of nodes
  and their probability of being requested.
- *Replacement of equipment*, used to find the optimal replacement plan for any
  equipment.
- *Floyd Algorithm*, used to find the shortest path in a weighted graph.

This Software is written is C and uses GTK+ 3 for the GUI.


How to build
============

Install dependencies:

```shell
sudo apt-get install build-essential texlive libgtk-3-dev
```

Then build and run:

```
cd download/dir
make
./main
```

How to hack
===========

Install development Software:

```shell
sudo apt-get install glade geany git devhelp libgtk-3-doc
```
