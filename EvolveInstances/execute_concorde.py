import os
import sys


argumentList=sys.argv
file_name=argumentList[1]
concorde_invoer_file="concorde_invoer_"+file_name+".tsp";
os.system("./concorde "+concorde_invoer_file)
