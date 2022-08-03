import os
import snap
import numpy as np
import itertools 
from scipy.stats import skew, kurtosis
import math
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA

#Read instances
rootdir = './Test instances/'
nbrOfFiles = 0
y = []
fitnessValues = []
HCPNumbers = []
UsingAllInstancesForPCA = False

#iterate over ind files
for subdir, dirs, files in os.walk(rootdir):
    for file in files:
        stringList = subdir.split("/")
        #Ignore extra individuals or files in subdirectories
        if len(stringList) < 4:
            
            #Label each instance
            #Evolved
            if stringList[2][0] == "E":
                y.append(0)
            #Generators
            elif stringList[2][0] == "G":
                y.append(2)
            #With target, evolved towards a point
            elif stringList[2][0] == "T" and UsingAllInstancesForPCA:
                y.append(3)
            #No target, evolved away from others
            elif stringList[2][0] == "N" and UsingAllInstancesForPCA:
                y.append(4)
            else:
                break
            
            nbrOfFiles += 1
    
            #Read the graph:
            #Read fitness value
            fitnessfile_name = os.path.join(subdir, "fitness/fitnessValues.in")
            
            filestring = file.split(".")
            fitnessIndex = int(filestring[0][3:])
            fitnessFile = open(fitnessfile_name,"r")
            lines = fitnessFile.readlines()
            fitnessValues.append(float(lines[fitnessIndex][:-2]))
            
            #Read HCP number:
            HCPfile_name = os.path.join(subdir, "fitness/HCPNumber.in")
            HCPfile = open(HCPfile_name,"r")
            lines = HCPfile.readlines()
            HCPNumbers.append(float(lines[fitnessIndex]))
            
            
            file_name = os.path.join(subdir, file)
            indFile = open(file_name,"r")
            info = indFile.readline()
            infoList = info.split()
            NbrNodes = int(infoList[0])
            NbrEdges = int(infoList[1])
            
            Graph = snap.TUNGraph.New()
            for i in range(NbrNodes):
                Graph.AddNode(i)
                 
            for i in range(NbrEdges):
                edgeStr = indFile.readline()
                edgeIds = edgeStr.split()
                SrcId = int(edgeIds[0])
                DesId = int(edgeIds[1])
                Graph.AddEdge(SrcId,DesId)
                
            #Max number of edges to calculate p (for clustering coefficient)
            s = set(range(NbrNodes)) 
            listOfAllEdges = list(itertools.combinations(s, 2)) 
            maxNbrEdges = len(listOfAllEdges)
            p = NbrEdges/maxNbrEdges
            
            
            #Degree distribution
            degreeX = []
            degreeY = []
            DegToCntV= snap.TIntPrV()
            snap.GetDegCnt(Graph, DegToCntV)
            for item in DegToCntV:
                # print("%d nodes with degree %d" % (item.GetVal2(), item.GetVal1()))
                degreeX.append(item.GetVal1())
                degreeY.append(item.GetVal2())
            #1D array of the degrees for distribution analysis
            degree1DArray = []
            for i in degreeX:
                for j in degreeY:
                    degree1DArray.append(i)
                    
            #Dangling node = a node with degree 1
            PrcntDanglingNodes = 0.0
            if len(degreeY) > 1:
                PrcntDanglingNodes = degreeY[1]/NbrNodes
                
            # #Bridge node (own term) = a node with degree 2
            PrcntBridgeNodes = 0.0
            if len(degreeY) > 2:
                PrcntBridgeNodes = degreeY[2]/NbrNodes
                
            sumOfEignV = 0
            EigVals = 10
            PEigV = snap.TFltV()
            snap.GetEigVals(Graph, EigVals, PEigV)
            for item in PEigV:
                sumOfEignV += abs(item)
                
            attrList = np.zeros(10)
            #Attribute 0: Amount of edges (Graph Density)
            attrList[0] = math.log(p)
            #Attribute 1: Cluster coefficient
            attrList[1] = math.log(snap.GetClustCf(Graph)+1)
            #Attribute 2: Energy of the graph
            attrList[2] = math.log(sumOfEignV+1)
            #Attribute 3: Maximum degree of a node
            attrList[3] = math.log(max(degreeX)+1)
            #Attribute 4: Standard deviation of degree distribution
            attrList[4] = math.log(np.std(degree1DArray)+1)
            #Attribute 5: Skewness of degree distribution
            attrList[5] = math.log(skew(degree1DArray)+2)
            #Attribute 6: Kurtosis of degree distribution
            attrList[6] = math.log(kurtosis(degree1DArray)+5)
            # #Attribute 7: Full diameter
            attrList[7] = math.log(snap.GetBfsEffDiam(Graph,NbrNodes)+1)
            #Attribute 8: Percentage of nodes with a degree of 1 ('dangling nodes')
            attrList[8] = math.log(PrcntDanglingNodes+1)
            #Attribute 9: Percentage of nodes with a degree of 2 ('bridge nodes')
            attrList[9] = math.log(PrcntBridgeNodes+1)
            
            
            #append instance to dataset x
            if nbrOfFiles == 1:
                x = [attrList]
            else:
                x = np.concatenate((x,[attrList]),axis=0)
                
            
            
#PCA
atrNames = ["Density","ClustCf","Energy", \
            "MaxDeg","StdDevDeg","SkewDeg","KurtDeg","Diameter","%DegreeOf1","%DegreeOf2"]



#Normalise the attributes
scaler=StandardScaler()#instantiate
scaler.fit(x) # compute the mean and standard which will be used in the next command
X_scaled=scaler.transform(x)
print("Means:")
compList = []
for axis in scaler.mean_:
    compList.append(axis)
print(compList)
# print(scaler.mean_)
print("Variances:")
compList = []
for axis in scaler.var_:
    compList.append(axis)
print(compList)
# print(scaler.var_)

pca=PCA(n_components=2) 

X_scaled[np.isnan(X_scaled)] = 0
pca.fit(X_scaled) 
X_pca=pca.transform(X_scaled) 


print("\nNumber of individual graphs:",nbrOfFiles)
print("Explained variance of the first 2 principal components", \
      str(format(sum(pca.explained_variance_ratio_),".3f")))
print(pca.explained_variance_ratio_)
# print(pca.singular_values_)
print("Coefficients of each principal component")
# print(pca.components_)
compList = []
for axis in pca.components_:
    axisList =[]
    for elem in axis:
        axisList.append(elem)
    compList.append(axisList)
print(compList)

import matplotlib.pyplot as plt
Xax=X_pca[:,0]
Yax=X_pca[:,1]
        
# with open('ExtraInfo.txt', 'w') as f:
#     for i in range(len(fitnessValues)):
#         #Runtime fitness, Origin, HCP number
#         f.write("{},{},{}\n".format(fitnessValues[i],y[i],HCPNumbers[i]))
        
# with open('x.txt','w') as f:
#     for row in x:
#         #Write attributes to 1 file
#         for elem in row:
#             f.write("%s," % elem)
#         f.write("\n")

# print("Min and max of fitness values:")
# print(min(fitnessValues))
# print(max(fitnessValues))

# plt.hist(fitnessValues,bins="auto")
# plt.xlabel("Difference in runtime (seconds)",fontsize=12)
# plt.ylabel("Amount of instances",fontsize=12)
# plt.title("Differences in runtime of the evolved instances",fontsize=13)
# plt.show

    
    
#Names = attribute to show on hover
HOVER = False
if HOVER:
    names1 = np.array(x[:,0])
    namesreverselogged = math.e**names1
    names = namesreverselogged.astype(str)
    c = fitnessValues
    cmap = plt.cm.rainbow
    fig,ax = plt.subplots()
    sc = plt.scatter(Xax,Yax, c=fitnessValues, cmap="rainbow",s=15)
    
    annot = ax.annotate("", xy=(0,0), xytext=(20,20),textcoords="offset points",
                        bbox=dict(boxstyle="round", fc="w"),
                        arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    
    def update_annot(ind):
        pos = sc.get_offsets()[ind["ind"][0]]
        annot.xy = pos
        text = "{}, {}".format(" ".join(list(map(str,ind["ind"]))), 
                                " ".join([names[n] for n in ind["ind"]]))
        annot.set_text(text)
        annot.get_bbox_patch().set_facecolor(cmap(c[ind["ind"][0]]))
        annot.get_bbox_patch().set_alpha(0.4)
    
    
    def hover(event):
        vis = annot.get_visible()
        if event.inaxes == ax:
            cont, ind = sc.contains(event)
            if cont:
                update_annot(ind)
                annot.set_visible(True)
                fig.canvas.draw_idle()
            else:
                if vis:
                    annot.set_visible(False)
                    fig.canvas.draw_idle()
    
    fig.canvas.mpl_connect("motion_notify_event", hover)
else:
    names1 = np.array(x[:,0])
    namesreverselogged = math.e**names1
    plt.figure()
    plt.scatter(Xax,Yax, c=fitnessValues, cmap="rainbow",s=12,alpha=0.5)


# # Green dot if Concorde is faster:
# # for i in range(len(Xax)):
# #     if fitnessValues[i] > 0:
# #         plt.plot(Xax[i],Yax[i],marker='o',markersize=1.5,c="springgreen")

# #Green dot if HCPN == 1
# # for i in range(len(Xax)):
# #     if HCPNumbers[i] == 0:
# #         plt.plot(Xax[i],Yax[i],marker='o',markersize=2.5,c="red")
#     # if HCPNumbers[i] == 1 and fitnessValues[i] > 10:
#     #     plt.plot(Xax[i],Yax[i],marker='o',markersize=1.5,c="springgreen",alpha=0.5)

# # Optionally show a Red dot if tie (timediff between -0.5 and 0.5)
SHOW_TIES = False
if SHOW_TIES:
    for i in range(len(Xax)):
        TIE_VALUE = 0.2
        if fitnessValues[i] > -TIE_VALUE and fitnessValues[i] < TIE_VALUE:
            plt.plot(Xax[i],Yax[i],marker='o',markersize=1.5,c="r")

# #Mark generated instances by generators:
# # for i in range(len(Xax)):
# #     if y[i] == 2:
# #         plt.plot(Xax[i],Yax[i],marker='o',markersize=2.5,c="r")
     

cbar = plt.colorbar()
cbar.set_label("Difference in runtime (seconds)",fontsize=12)
plt.clim(-2,3)
plt.xlabel("First Principal Component",fontsize=12)
plt.ylabel("Second Principal Component",fontsize=12)
import matplotlib.lines as mlines
green_leg = mlines.Line2D([], [], color='darkorange', marker='o', linestyle='None',
                          markersize=10, label='Concorde faster')
blue_leg = mlines.Line2D([], [], color='deepskyblue', marker='o', linestyle='None',
                          markersize=10, label='MSLS faster')
if SHOW_TIES:
    red_leg = mlines.Line2D([], [], color='r', marker='o', linestyle='None',
                          markersize=10, label='Tie (between -{}s and {}s)'.format(TIE_VALUE,TIE_VALUE))
    plt.legend(handles=[green_leg, blue_leg, red_leg])
else:
    plt.legend(handles=[green_leg, blue_leg])
plt.show()

# #Coefficients of principal components
SHOW_CFTS = True
if SHOW_CFTS:
    plt.matshow(pca.components_,cmap='RdYlGn')
    plt.yticks([0,1],['1st Comp','2nd Comp'],fontsize=10)
    plt.colorbar()
    plt.clim(-1,1)
    plt.xticks(range(len(attrList)),atrNames,rotation=65,ha='left')
    for (i, j), z in np.ndenumerate(pca.components_):
        plt.text(j, i, '{:0.2f}'.format(z), ha='center', va='center')
    # plt.title("Coefficients of the first 2 principal components",y=-0.4,fontsize=14)
    plt.show()