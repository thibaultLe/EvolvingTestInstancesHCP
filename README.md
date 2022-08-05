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
      
    @article{LechienEtAl2020,
    doi = {10.48550/ARXIV.2011.02291},
    url = {https://arxiv.org/abs/2011.02291},
    author = {Lechien, Thibault and Jooken, Jorik and De Causmaecker, Patrick},
    keywords = {Artificial Intelligence (cs.AI), Discrete Mathematics (cs.DM), Neural and Evolutionary Computing (cs.NE), FOS: Computer and information sciences, FOS: Computer and information sciences, F.2.2; G.2.2},
    title = {Evolving test instances of the Hamiltonian completion problem},
    publisher = {arXiv},
    year = {2020},
    copyright = {arXiv.org perpetual, non-exclusive license}
  }

