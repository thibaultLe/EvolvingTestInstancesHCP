# EvolvingTestInstancesHCP

These are all the test instances and code used in the paper 'Evolving test instances of the Hamiltonian completion problem'.

In order to run the evolutionary algorithm, run EvolveInstances.py in the folder /EvolveInstances. It is compatible with Linux environments.

In order to run the PCA, run PCAmain.py.

In order to access the test instances, consult the folder /Test instances.

The subfolders are named after how the instances were added, namely:

'Evo' means we evolved them by maximizing the difference in runtime of the 2 considered solvers. 

'NT' means we evolved away from other instances, thereby maximizing the distance to the nearest other instance.

'T' means we evolved to specific target points where there were gaps in the instance space.


If you use these instances or code, please cite 
      
    @article{LechienEtAl2022,
	title = {Evolving test instances of the Hamiltonian completion problem},
	journal = {Computers & Operations Research},
	pages = {106019},
	year = {2022},
	issn = {0305-0548},
	doi = {https://doi.org/10.1016/j.cor.2022.106019},
	url = {https://www.sciencedirect.com/science/article/pii/S0305054822002490},
	author = {Thibault Lechien and Jorik Jooken and Patrick {De Causmaecker}},
	keywords = {Evolving instances, Instance space, Hamiltonian completion problem},
	abstract = {Predicting and comparing algorithm performance on graph instances is challenging for multiple reasons. First, there is not always a standard set of instances to benchmark performance. Second, using existing graph generators results in a restricted spectrum of difficulty and the resulting graphs are not always diverse enough to draw sound conclusions. That is why recent work proposes a new methodology to generate a diverse set of instances by using evolutionary algorithms. We can then analyze the resulting graphs and get key insights into which attributes are most related to algorithm performance. We can also fill observed gaps in the instance space in order to generate graphs with previously unseen combinations of features. We apply this methodology to the instance space of the Hamiltonian completion problem using two different solvers, namely the Concorde TSP Solver and a multi-start local search algorithm.}
	}
      
      
    @article{JookenEtAl2020,
	author = {Jooken, Jorik and Leyman, Pieter and De Causmaecker, Patrick},
	year = {2020},
	month = {07},
	pages = {},
	title = {A multi-start local search algorithm for the {Hamiltonian} completion problem on undirected graphs},
	journal = {Journal of Heuristics},
	doi = {10.1007/s10732-020-09447-9}
      }
      
    @misc{Concorde,
	author = {David L. Applegate},
	title = {Concorde {TSP} {Solver}},
	year = {2003},
	howpublished = {\url{https://www.math.uwaterloo.ca/tsp/concorde/index.html}},
	note = "[Online; accessed 7 August 2021]"
      }

