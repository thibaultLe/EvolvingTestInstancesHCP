import os
import sys
import time

argumentList=sys.argv
file_name=argumentList[1]

#tijd moet nog opgemeten worden+stilleggen na 100 seconden

# startTime=time.time()
# startTime = time.process_time()
os.system("./hcp_with_concorde_executable"+" "+file_name)

startTime = time.perf_counter()
os.system("timeout 1000 python3 execute_concorde.py"+" "+file_name)

endTime = time.perf_counter()
# time.perf_counter()
os.system("./produce_hcp_number_executable"+" "+file_name)
tspFileName="concorde_invoer_"+file_name+".tsp"
os.system("rm "+tspFileName)

# endTime=time.time()
# endTime = time.process_time()
secondsUsed=endTime-startTime
f=open("concorde_time_"+file_name,"w")
f.write(str(secondsUsed)+" secondsUsed\n")
f.close()

