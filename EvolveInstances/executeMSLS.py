import sys
import os

def writeLKHParameterFile(inputFileName):
    parName=inputFileName+"lkh_input.par"
    tspName=inputFileName+"lkh_input.tsp"
    lkhOutputFileName=inputFileName+"LKHOutput"
    myfile = open(parName, 'w')
    myfile.write("PROBLEM_FILE = "+tspName+"\n")
    myfile.write("MOVE_TYPE = 5\n")
    myfile.write("PATCHING_C = 3\n")
    myfile.write("PATCHING_A = 2\n")
    myfile.write("RUNS = 1\n")
    myfile.write("TOUR_FILE = "+lkhOutputFileName+"\n")
    myfile.write("OPTIMUM = 0\n")
    myfile.write("STOP_AT_OPTIMUM = YES\n")
    myfile.write("CANDIDATE_SET_TYPE = POPMUSIC\n")
    myfile.write("MAX_TRIALS = 2\n")
    myfile.write("TRACE_LEVEL = 0\n")
    myfile.close()
    
def executeMSLS(inputFileName,preferredRatio,maxAmountInitialSpanningTrees,maxAmountBadPerturbations):
    command="./multiStartLocalSearch "+inputFileName+" "+preferredRatio+" "+maxAmountInitialSpanningTrees+" "+maxAmountBadPerturbations
    os.system(command)

argumentList=sys.argv
if len(argumentList) != 5:
    print("The number of arguments for the file executeMSLS.py was incorrect!")
inputFileName=argumentList[1]
preferredRatio=argumentList[2]
maxAmountInitialSpanningTrees=argumentList[3]
maxAmountBadPerturbations=argumentList[4]

signature=inputFileName+"_"+preferredRatio+"_"+maxAmountInitialSpanningTrees+"_"+maxAmountBadPerturbations+"_"
writeLKHParameterFile(signature)
executeMSLS(inputFileName,preferredRatio,maxAmountInitialSpanningTrees,maxAmountBadPerturbations)
