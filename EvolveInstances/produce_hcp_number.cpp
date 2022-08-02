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
	ifstream file;
	file.open(file_name);
	int aantal_knopen, aantal_edges;
    file >> aantal_knopen >> aantal_edges;
    vector< vector<int> > cost_matrix(aantal_knopen, vector<int>(aantal_knopen,1));
    vector< vector<int> > graph(aantal_knopen);
    for(int i=0; i<aantal_edges; i++)
    {
        int u,v;
        file >> u >> v;
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
	file.close();
    
    string concorde_invoer_file_solution="concorde_invoer_"+file_name;
    if(concorde_invoer_file_solution.substr(concorde_invoer_file_solution.size()-3,3)==".in")
    {
        concorde_invoer_file_solution=concorde_invoer_file_solution.substr(0,concorde_invoer_file_solution.size()-3);
    }
    concorde_invoer_file_solution+=".sol";
    cerr << concorde_invoer_file_solution << endl;
	file.open(concorde_invoer_file_solution);
	file >> aantal_knopen;
	vector<int> volgorde;
	for(int i=0; i<aantal_knopen; i++)
	{
		int x;
		file >> x;
		volgorde.push_back(x);
	}
	file.close();
    
	int cost=0;
	for(int i=0; i<volgorde.size(); i++)
	{
		cost+=cost_matrix[volgorde[i]][volgorde[(i+1)%volgorde.size()]];
	}
	if(aantal_knopen==2)
	{
		if(graph[0].size() >= 1) cost=0;
		else cost=1;
	}
	ofstream file2;
    string output_file_name="hcp_number_output_"+file_name;
	file2.open(output_file_name);
	file2 << cost;
	file2.close();
	//cerr << "cerr output: cost: " << cost << endl;
	return 0;
}
