import random
import os

import itertools 
import numpy
import statistics
import matplotlib.pyplot as plt

from deap import algorithms
from deap import base
from deap import creator
from deap import tools
    

def evalGraph(individual):
    #Check runtime fitness or HCP number difference
    # timeFitness = True
    
    #Write edges to file
    #individual = set of edges
    f = open("dolphins2.in", "w")
    f.write(str(NBR_NODES) + " " + str(len(individual)) + "\n")
    
    unsortedListOfEdges = []
    sortedListFirstEdge = []
    for edge in individual:
        unsortedListOfEdges.append(str(edges[edge][0]) + " " + str(edges[edge][1]) + "\n")
        sortedListFirstEdge.append((edges[edge][0],edges[edge][1]))
        
    # Sort set of edges so it is compatible with concorde and MSLS solvers
    sortedListOfEdges = [x for _,x in sorted(zip(sortedListFirstEdge,unsortedListOfEdges))]
    for elem in sortedListOfEdges:
        f.write(elem)
    f.close()
    
    #Evaluate fitness
    runtime = 0
    # if timeFitness:
    concordeTimes = []
    mslsTimes = []
    file_name = "dolphins2.in"
    
    #Solve with concorde
    os.system("python3 chain_programs.py dolphins2.in > /dev/null 2>&1")    
    
    #Get Concorde runtime file
    fCtime = open("concorde_time_"+file_name,"r")
    #Read runtime
    timeString = fCtime.readline()
    concorderuntime = float(timeString.split()[0])
    fCtime.close()
    concordeTimes.append(concorderuntime)
    
    #solve with MSLS:
    os.system("python3 executeMSLS.py dolphins2.in 25 10 3000> /dev/null 2>&1")    
    
    #Get MSLS runtime file
    fCtime = open(file_name + "_25_10_3000__total_time","r")
    #Read HCP number
    timeString = fCtime.readline()
    mslsruntime = float(timeString.split()[0])
    fCtime.close()
    mslsTimes.append(mslsruntime)
        
    #Positive = MSLS takes longer than Concorde
    runtime = statistics.mean(mslsTimes) - statistics.mean(concordeTimes)
        
        
    #Check if HCP number is the same:
    # fCtime = open("hcp_number_output_"+file_name,"r")
    # #Read HCP number
    # timeString = fCtime.readline()
    # concordeNumber = float(timeString.split()[0])
    # fCtime.close()
        
        
    #To calculate the difference in HCP number:
    # #solve with concorde:
    # os.system("python3 chain_programs.py dolphins2.in > /dev/null 2>&1")    
    # #Get Concorde HCP number file
    # file_name = "dolphins2.in"
    # fCtime = open("hcp_number_output_"+file_name,"r")
    # #Read HCP number
    # timeString = fCtime.readline()
    # concordeNumber = float(timeString.split()[0])
    # fCtime.close()
    
    # #solve with MSLS:
    # os.system("python3 executeMSLS.py dolphins2.in 25 10 3000> /dev/null 2>&1")    
    
    # #Get MSLS HCP number file
    # fCtime = open(file_name + "_25_10_3000__objective_value","r")
    # #Read HCP number
    # timeString = fCtime.readline()
    # MSLSNumber = float(timeString.split()[0])
    # fCtime.close()
    
    # #Positief getal = MSLS geeft hoger getal dan Concorde 
    # #Concorde geeft altijd het minimum
    # runtime = MSLSNumber - concordeNumber
    
    return runtime,


#Uniform crossover (doesn't work well here, loses too much of the structure)
def cxGraphUniform(ind1, ind2, indpb):
    ## Crossover: take a part of one graph, add it to the other one
    ## indpb : probabability that an edge is exchanged between ind1 and ind2
    # exchange (nbr_edges * indpb) number of edges
    origLength1 = len(ind1) 
    origLength2 = len(ind2) 
    
    for i in range(round(NBR_EDGES * indpb)):
        #Try 100 times to add a non-duplicate edge
        remFrom1 = random.choice(tuple(ind1))
        counter = 0
        while remFrom1 in ind2 and counter < 100:
            remFrom1 = random.choice(tuple(ind1))
            counter += 1
            
        remFrom2 = random.choice(tuple(ind2))
        counter = 0
        while remFrom2 in ind1 and counter < 100:
            remFrom2 = random.choice(tuple(ind2))
            counter += 1
        
        #Remove from 1, add to 1
        ind1.remove(remFrom1)
        ind1.add(remFrom2)
        #Remove from 2, add to 2
        ind2.remove(remFrom2)
        ind2.add(remFrom1)
    
    #Making sure it keeps same amount of edges
    while len(ind1) < MIN_NBR_EDGES or len(ind1) < origLength1:
        ind1.add(random.randrange(NBR_EDGES))
    while len(ind2) < MIN_NBR_EDGES or len(ind2) < origLength2:
        ind2.add(random.randrange(NBR_EDGES))
        
    return ind1, ind2
    
#One-point crossover
def cxGraph(ind1, ind2):
    ## Crossover: take a part of one graph, add it to the other one
    #One point crossover:
    #Randomly choose pivot from ind1
    #Remove all edges below pivot from ind1
    #Remove all edges below pivot from ind2
    #Add all removed edges from ind1 to ind2
    #Add all removed edges from ind2 to ind1
    removedFrom1 = []
    removedFrom2 = []
    
    #Pivot = crossover point
    pivot = random.choice(tuple(ind1))
    #Remove all edges below pivot
    for edge in range(pivot+1):
        if edge in tuple(ind1):
            ind1.remove(edge)
            removedFrom1.append(edge)
        if edge in tuple(ind2):
            ind2.remove(edge)
            removedFrom2.append(edge)
            
    #Add all removed edges to the other ind
    for edge in removedFrom1:
        ind2.add(edge)
    for edge in removedFrom2:
        ind1.add(edge)
            
    
    while len(ind1) < MIN_NBR_EDGES:
        ind1.add(random.randrange(NBR_EDGES))
    while len(ind2) < MIN_NBR_EDGES:
        ind2.add(random.randrange(NBR_EDGES))
        
    return ind1, ind2

#Two-point crossover
def cxGraphTwoPoint(ind1, ind2):
    ## Crossover: take a part of one graph, add it to the other one
    #Two point crossover:
    #Randomly choose pivot from ind1
    #Remove all edges below pivot1 from ind1 and ind2
    #Remove all edges above pivot2 from ind1 and ind2
    #Add all removed edges from ind1 to ind2
    #Add all removed edges from ind2 to ind1
    removedFrom1 = []
    removedFrom2 = []
    
    #Pivot = crossover point
    pivot1 = random.choice(tuple(ind1))
    pivot2 = random.choice(tuple(ind1))
    
    #Make sure pivot1 is the smallest
    if pivot2 < pivot1:
        pivot1, pivot2 = pivot2, pivot1
        
    #Remove all edges between pivot1 and pivot2
    for edge in range(pivot1,pivot2+1):
        if edge in tuple(ind1):
            ind1.remove(edge)
            removedFrom1.append(edge)
        if edge in tuple(ind2):
            ind2.remove(edge)
            removedFrom2.append(edge)
            
    #Add all removed edges to the other ind
    for edge in removedFrom1:
        ind2.add(edge)
    for edge in removedFrom2:
        ind1.add(edge)
            
    
    while len(ind1) < MIN_NBR_EDGES:
        ind1.add(random.randrange(NBR_EDGES))
    while len(ind2) < MIN_NBR_EDGES:
        ind2.add(random.randrange(NBR_EDGES))
        
    return ind1, ind2

#Mutate a graph: either replace a few edges with random edges
#                either add edges
#                either remove edges
def mutGraph(individual):
    randomnbr = random.random()
    pm = 0.03
    
    if randomnbr < 0.33:
        ## Each edge has a probability of pm (3%) to be replaced by a random edge
        origLength = len(individual)
        for i in range(len(individual)):
            if random.random() < pm:
                individual.remove(random.choice(tuple(individual)))
                individual.add(random.randrange(NBR_EDGES))
        
        while len(individual) < MIN_NBR_EDGES or len(individual) < origLength:
            individual.add(random.randrange(NBR_EDGES))
    else:
        #randomly add/remove 0-3% of edges
        if random.random() < 0.5 and len(individual) < NBR_EDGES:
            #Randomly add between 0 and 3% of edges
            nbrAdded = round(random.random() * pm * NBR_EDGES)
            while len(individual) < origLength + nbrAdded and len(individual) < NBR_EDGES -3:
                individual.add(random.randrange(NBR_EDGES))
        else:
            nbrRemoved = round(random.random() * pm * NBR_EDGES)
            if len(individual) > nbrRemoved:
                for i in range(nbrRemoved):
                    individual.remove(random.choice(tuple(individual)))
            
                
    while len(individual) < MIN_NBR_EDGES:
        individual.add(random.randrange(NBR_EDGES))
        
    ## Comma makes individual into a tuple
    return individual,


## Individuals are graphs = set of edges
## Edge name is an integer, value is its nodes (node1,node2) 2-uple
## Nodes are numbered from 0 to NBR_NODES-1
NBR_NODES = 50
s = set(range(NBR_NODES)) 
listOfAllEdges = list(itertools.combinations(s, 2)) 
NBR_EDGES = len(listOfAllEdges)
#50 nodes-> 1225 = number of possible edges

edges = {}
for i in range(NBR_EDGES):
    edges[i] = listOfAllEdges[i]

#Number of edges that the generator randomly adds
#Note: duplicate edges are possible, so might result in less
EDG_INIT_SIZE = 300
MIN_NBR_EDGES = 1


# Maximize or minimise fitness:
MAXIMISE = True
#1.0 == where concorde is better
#-1.0 == where MSLS is better
if MAXIMISE:
    creator.create("Fitness", base.Fitness, weights=(1.0,))
else:
    creator.create("Fitness", base.Fitness, weights=(-1.0,))
    
    
creator.create("Individual", set, fitness=creator.Fitness)
toolbox = base.Toolbox()
# Attribute generator
toolbox.register("attr_item", random.randrange, NBR_EDGES)
# Structure initializers
toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_item, EDG_INIT_SIZE)

##Function to load starting population instead of generating a random one
def load_individuals(creator,n):
    individuals = []
    for i in range(n):
        f = open("./startPop/ind{}.in".format(i), "r")
        individual = set()
        infoline = f.readline()
        infoList = infoline.split()
        NbrEdges = int(infoList[1])
        
        for j in range(NbrEdges):
            edgelist = f.readline().split()
            # print(edgelist)
            edge1 = int(edgelist[0])
            edge2 = int(edgelist[1])
            edge = (edge1,edge2)
            edgeNbr = list(edges.keys())[list(edges.values()).index(edge)]
            individual.add(edgeNbr)
        
        #Transform set to individual
        individual = creator(individual)
        individuals.append(individual)
    return individuals
        
#Population
CUSTOM_START_POP = False
if CUSTOM_START_POP:
    # population: customize initial population
    toolbox.register("population",load_individuals, creator.Individual)
else:
    # population: random population
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)

toolbox.register("evaluate", evalGraph)
toolbox.register("mate", cxGraphTwoPoint)
toolbox.register("mutate", mutGraph)
toolbox.register("select", tools.selTournament, tournsize = 2)

def main():
    #Number of generations
    NGEN = 100
    #Number of individuals to select for the next generation = Population size
    MU = 20
    #Number of children to produce at each generation.
    LAMBDA = 30
    #Probability that an offspring is produced by crossover
    CXPB = 0.33
    #Probability that an offspring is produced by mutation
    MUTPB = 0.33
    
    #Starting population
    pop = toolbox.population(n=MU)
    
    #HallOfFame = best individuals ever
    hofSize = NGEN*MU
    hof = tools.HallOfFame(hofSize)
    
    #Stats
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats_size = tools.Statistics(key=len)
    mstats = tools.MultiStatistics(fitness=stats, size=stats_size)
    mstats.register("avg", numpy.mean)
    mstats.register("std", numpy.std)
    mstats.register("min", numpy.min)
    mstats.register("max", numpy.max)
    
    
    #Logbook to record stats
    logbook = tools.Logbook()
    logbook.header = "gen", "evals", "fitness", "size"
    logbook.chapters["fitness"].header = "min", "avg", "max"
    logbook.chapters["size"].header = "min", "avg", "max"
    
    #in algorithms.eaMuPlusLambda: varOr veranderd door varAnd
    # pop, logbook = algorithms.eaMuPlusLambda(pop, toolbox, MU, LAMBDA, CXPB, MUTPB, NGEN, mstats,
    #                           halloffame=hof)
    pop, logbook = algorithms.eaMuCommaLambda(pop, toolbox, MU, LAMBDA, CXPB, MUTPB, NGEN, mstats,
                              halloffame=hof)
    
    
    
    #Write halloffame to files
    #fitness values for color map in PCA
    fitnessValues = open("./hof/fitness/fitnessValues.in", "w")
    
    for i in range(len(hof)):
        f = open("./hof/ind{}.in".format(i), "w")
        f.write(str(NBR_NODES) + " " + str(len(hof[i])) + "\n")
        
        # Sort the list of edges so it is a valid '.in' file
        unsortedListOfEdges = []
        sortedListFirstEdge = []
        for edge in hof[i]:
            unsortedListOfEdges.append(str(edges[edge][0]) + " " + str(edges[edge][1]) + "\n")
            sortedListFirstEdge.append((edges[edge][0],edges[edge][1]))
        # Sort on the first node
        sortedListOfEdges = [x for _,x in sorted(zip(sortedListFirstEdge,unsortedListOfEdges))]
        for elem in sortedListOfEdges:
            f.write(elem)
        f.close()
        
        #write fitness values to file
        fitnessValues.write(str(hof[i].fitness.values[0])+"\n")
        
        
    #Write final population to files
    for i in range(len(pop)):
        f = open("./pop/ind{}.in".format(i), "w")
        f.write(str(NBR_NODES) + " " + str(len(pop[i])) + "\n")
        
        unsortedListOfEdges = []
        sortedListFirstEdge = []
        for edge in pop[i]:
            unsortedListOfEdges.append(str(edges[edge][0]) + " " + str(edges[edge][1]) + "\n")
            sortedListFirstEdge.append((edges[edge][0],edges[edge][1]))
            
        # Sort on the first node
        sortedListOfEdges = [x for _,x in sorted(zip(sortedListFirstEdge,unsortedListOfEdges))]
        for elem in sortedListOfEdges:
            f.write(elem)
        f.close()
    
    gen = logbook.select("gen")
    fit_avgs = logbook.chapters["fitness"].select("avg")
    fit_mins = logbook.chapters["fitness"].select("min")
    fit_maxs = logbook.chapters["fitness"].select("max")
    size_avgs = logbook.chapters["size"].select("avg")
    size_mins = logbook.chapters["size"].select("min")
    size_maxs = logbook.chapters["size"].select("max")

    
    fig, ax1 = plt.subplots()
    line1 = ax1.plot(gen, fit_avgs, "b-", label="Average Fitness")
    ax1.set_xlabel("Generation")
    ax1.set_ylabel("Fitness (seconds)", color="b")
    for tl in ax1.get_yticklabels():
        tl.set_color("b")
        
    line3 = ax1.plot(gen, fit_mins, color="navy", linestyle="--", label="Minimum Fitness",alpha = 0.4)
        
    line4 = ax1.plot(gen, fit_maxs, color="navy", linestyle="--",  label="Maximum Fitness",alpha = 0.4)
    
    #lns = line1 + line2
    lns =line4 + line1 + line3 
    labs = [l.get_label() for l in lns]
    ax1.legend(lns,labs,loc='best')
    
    
    if MAXIMISE:
        plt.title("Maximising [runtime(MSLS) - runtime(Concorde)]")
    else:
        plt.title("Minimising [runtime(MSLS) - runtime(Concorde)]")
    
    plt.show()
    
    #Plot edge counts:
    fig, ax3 = plt.subplots()
    ax3.set_xlabel("Generation")
        
    line3 = ax3.plot(gen, size_mins,  color="green", linestyle="--",label="Minimum Edge Count",alpha = 0.4)
        
    line4 = ax3.plot(gen, size_maxs, color="green", linestyle="--", label="Maximum Edge Count",alpha = 0.4)
    
    line2 = ax3.plot(gen, size_avgs, "g-", label="Average Edge Count",alpha = 0.3)
    ax3.set_ylabel("Edge count", color="g")
    for tl in ax3.get_yticklabels():
        tl.set_color("g")
    
    #lns = line1 + line2
    lns =  line4 + line2 +line3
    labs = [l.get_label() for l in lns]
    ax3.legend(lns,labs,loc='best')
    
    if MAXIMISE:
        plt.title("Maximising [runtime(MSLS) - runtime(Concorde)]")
    else:
        plt.title("Minimising [runtime(MSLS) - runtime(Concorde)]")
    
    plt.show()
    
    
    
    return pop, stats, hof
                 

main()               