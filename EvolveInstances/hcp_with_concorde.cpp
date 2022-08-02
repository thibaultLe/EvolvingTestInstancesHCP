#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("The number of arguments for the program hcp_with_concorde was incorrect!");
        return 0;
    }
    string file_name=argv[1];
    // de verwachte vorm is:
    // eerste lijn met 2 getallen: aantal_knopen en aantal_edges
    // de volgende aantal_edges lijnen bevatten 2 getallen: uiteinde1 en uiteinde2 (ongerichte, ongewogen grafe).
    // Er geldt 0 <= uiteinde < aantal_knopen
    ifstream graphFile;
    graphFile.open(file_name.c_str());
    int aantal_knopen, aantal_edges;
    graphFile >> aantal_knopen >> aantal_edges;
    vector< vector<int> > cost_matrix(aantal_knopen, vector<int>(aantal_knopen,1));
    vector< vector<int> > graph(aantal_knopen);
    for(int i=0; i<aantal_edges; i++)
    {
        int u,v;
        graphFile >> u >> v;
        if(!(0 <= u && u < aantal_knopen && 0 <= v && v < aantal_knopen))
        {
            printf("De id's van de knopen moeten liggen tussen 0 en %d (inclusief)\n", aantal_knopen-1);
            return 0;
        }
        cost_matrix[u][v]=cost_matrix[v][u]=0;
        if(aantal_knopen==2 && u!=v)
        {
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
    }

    ofstream concordeFile;
    string concorde_invoer_file="concorde_invoer_"+file_name+".tsp";
    concordeFile.open(concorde_invoer_file);
    concordeFile << "NAME: concorde_invoer_" << file_name << "\n";
    concordeFile << "TYPE: TSP\n";
    concordeFile << "COMMENT: No comment\n";
    concordeFile << "DIMENSION: " << aantal_knopen << "\n";
    concordeFile << "EDGE_WEIGHT_TYPE: EXPLICIT\n";
    concordeFile << "EDGE_WEIGHT_FORMAT: LOWER_DIAG_ROW\n";
    concordeFile << "EDGE_WEIGHT_SECTION\n";
    for(int i=0; i<aantal_knopen; i++)
    {
        for(int j=0; j<=i; j++)
        {
            concordeFile << " " << cost_matrix[i][j];
        }
        concordeFile << "\n";
    }
    concordeFile << "EOF\n";
    return 0;
}
