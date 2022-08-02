#include <bits/stdc++.h>
#include <chrono>
#include <random>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define debug_mode_on false

string inputFileName;
string signature;

struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return (v.first<<5)+v.second;
    }
};
unordered_set< pair<int, int>, pair_hash> all_edges_in_component;

//set< pair<int, int> > all_edges_in_component;
vector< pair<int, int> > all_edges_in_component_vec;
vector<int> leader;
//map< pair<int, int>, int> index_lookup;
unordered_map< pair<int, int>, int, pair_hash> index_lookup;
vector<int> whole_permutation;
vector<int> part_of_permutation;
const int amount_of_random_numbers=10000;
int idx_random_numbers=1;
vector<int> random_numbers;

int maxAmountBadPerturbations;
double preferredRatio;
int maxAmountInitialSpanningTrees;
int maxAllowedSeconds;

struct edge
{
    int buur;
    int index_bij_buur;
    bool actief;
    bool deel_van_cykel;
	bool geselecteerd_in_spanning_tree;
    edge()
    {
        buur=-1;
        index_bij_buur=-1;
        actief=true;
        deel_van_cykel=false;
		geselecteerd_in_spanning_tree=false;
    }
    edge(int _buur, int _index_bij_buur, bool _actief, bool _deel_van_cykel)
    {
        buur=_buur;
        index_bij_buur=_index_bij_buur;
        actief=_actief;
        deel_van_cykel=_deel_van_cykel;
		geselecteerd_in_spanning_tree=false;
    }
	edge(const edge& other)
	{
		buur=other.buur;
		index_bij_buur=other.index_bij_buur;
		actief=other.actief;
		deel_van_cykel=other.deel_van_cykel;
		geselecteerd_in_spanning_tree=other.geselecteerd_in_spanning_tree;
	}
	edge& operator= (const edge& other)
	{
		if(this != &other)
		{
			buur=other.buur;
			index_bij_buur=other.index_bij_buur;
			actief=other.actief;
			deel_van_cykel=other.deel_van_cykel;
			geselecteerd_in_spanning_tree=other.geselecteerd_in_spanning_tree;
		}
		return *this;
	}
};

mt19937 rng;

//double rand_between_time=0.0;
// random integer between a and b (inclusive)
inline int rand_between(int a, int b)
{
    //auto start = std::chrono::steady_clock::now( );
	if(idx_random_numbers==0)
	{
		uniform_int_distribution<int> dis(0,INT_MAX);
		int num=dis(rng);
		for(int i=0; i<amount_of_random_numbers; i++)
		{
			random_numbers[i]=random_numbers[i]^num;
		}
	}
	int ret=(random_numbers[idx_random_numbers]%(b-a+1))+a;
	idx_random_numbers++;
	if(idx_random_numbers==amount_of_random_numbers) idx_random_numbers=0;
    /*auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
	rand_between_time += elapsed.count();*/
	return ret;
}

// random permutation of the number 0, 1, ..., n-1
vector<int> random_permutation(int n)
{
	vector<int> ret(n);
	for(int i=0; i<n; i++) ret[i]=i;
	int idx1, idx2;
	for(int i=0; i<3*n; i++)
	{
		idx1=rand_between(0,n-1);
		idx2=rand_between(0,n-1);
		swap(ret[idx1],ret[idx2]);
	}
	return ret;
}

void init_random_numbers()
{
	uniform_int_distribution<int> dis(0,INT_MAX);
	for(int i=0; i<amount_of_random_numbers; i++)
	{
		random_numbers.push_back(dis(rng));	
	}
}

/*
void read_parameters()
{
    ifstream file;
    string name=inputFileName;
    name+="MSLSParameters";
	file.open(name.c_str());
    file >> maxAmountBadPerturbations;
    file >> preferredRatio;
    file >> maxAmountInitialSpanningTrees;
    file.close();
}
*/

void read_max_allowed_time()
{
    ifstream file;
    file.open("maxAllowedSeconds");
    file >> maxAllowedSeconds;
    file.close();
}

//double f_time=0.0;
// zoek minimal path cover van de boom
//long long f_cost=0;
void f(int wortel,vector< vector<edge> > &grafe, vector<int> &graad,int aantal_knopen)
{
    //f_cost += aantal_knopen;
    //auto start = std::chrono::steady_clock::now( );
    //cerr << "wortel: " << wortel << endl;
    if(wortel<0) return;
    vector<bool> bfs_bezocht(aantal_knopen,false);
    vector<int> bfs_ouder(aantal_knopen,-1);
    vector<int> bfs_volgorde;
    queue<int> q;
    bfs_bezocht[wortel]=true;
    bfs_volgorde.push_back(wortel);
    q.push(wortel);
    while(!q.empty())
    {
        int huidig=q.front();
        q.pop();
        //cerr << "huidig: " << huidig << endl;
        for(edge e : grafe[huidig])
        {
            if(!e.actief) continue;
            if(bfs_bezocht[e.buur]) continue;
            bfs_bezocht[e.buur]=true;
            bfs_volgorde.push_back(e.buur);
            q.push(e.buur);
            bfs_ouder[e.buur]=huidig;
        }
    }
    //cerr << "graad[3]: " << graad[3] << endl;
    for(int i=bfs_volgorde.size()-1; i>=0; i--)
    {
        int huidig=bfs_volgorde[i];
        int kind_teller=0;
        if(graad[huidig]>=3)
        {
            //cerr << "hier: " << huidig << endl;
            for(int j=0; j<grafe[huidig].size(); j++)
            {
                edge &e=grafe[huidig][j];
                if(!e.actief) continue;
                //cerr << "buur: " << e.buur << endl;
                if(e.buur==bfs_ouder[huidig])
                {
                    //cerr << "ok" << endl;
                    e.actief=false;
                    grafe[e.buur][e.index_bij_buur].actief=false;
                    graad[e.buur]--;
                }
                else
                {
                    kind_teller++;
                    if(kind_teller>2)
                    {
                        e.actief=false;
                        grafe[e.buur][e.index_bij_buur].actief=false;
                        graad[e.buur]--;
                    }
                }
            }
            graad[huidig]=2;
        }
    }
    /*auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	f_time += elapsed.count();*/
}
 
// bereken het aantal paden in de minimum path cover (of 0 indien we een cycle kunnen maken)
// grafe moet een boom voorstellen (a.d.h.v. edge.actief)
//double get_hcn_time=0.0;
int get_hcn_voor_boom(vector< vector<edge> > &grafe, vector<int> &graad, int aantal_knopen)
{
    //auto start = std::chrono::steady_clock::now( );
    int aantal_componenten=0;
    vector<bool> bezocht(aantal_knopen,false);
	int uiteinde1=-1;
	int uiteinde2=-1;
    for(int i=0; i<aantal_knopen; i++)
    {
        if(bezocht[i]) continue;
		if(graad[i]>=2) continue;
        aantal_componenten++;
        bezocht[i]=true;
        queue<int> q;
        q.push(i);
		uiteinde1=uiteinde2=i;
        while(!q.empty())
        {
            int huidig=q.front();
			uiteinde2=huidig;
            q.pop();
            for(edge e : grafe[huidig])
            {
                if(!e.actief) continue;
                if(bezocht[e.buur]) continue;
                bezocht[e.buur]=true;
                q.push(e.buur);
            }
        }
    }
    /*auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	get_hcn_time += elapsed.count();*/
	if(aantal_componenten==1 && all_edges_in_component.count(make_pair(uiteinde1,uiteinde2))>0) return 0;	
	return aantal_componenten;
}

// geselecteerd_in_spanning_tree en graad worden aangepast
void willekeurige_opspannende_boom_met_root(int r, vector< vector<edge> > &originele_grafe, vector< vector<edge> > &grafe, vector<int> &graad, int aantal_knopen)
{
	vector<bool> in_tree(aantal_knopen,false);
	vector<int> nxt(aantal_knopen,-1);
	in_tree[r]=true;
	for(int i=0; i<aantal_knopen; i++)
	{
		int u=i;
		while(!in_tree[u])
		{
			int rand_idx=rand_between(0,(int)originele_grafe[u].size()-1);
			nxt[u]=originele_grafe[u][rand_idx].buur;
			originele_grafe[u][rand_idx].geselecteerd_in_spanning_tree=true;
			originele_grafe[nxt[u]][originele_grafe[u][rand_idx].index_bij_buur].geselecteerd_in_spanning_tree=true;
			u=nxt[u];
		}
		u=i;
		while(!in_tree[u])
		{
			in_tree[u]=true;
			grafe[u].push_back(edge(nxt[u],grafe[nxt[u]].size(),true,false));
			grafe[nxt[u]].push_back(edge(u,grafe[u].size()-1,true,false));
			graad[u]++;
			graad[nxt[u]]++;
			u=nxt[u];
		}
	}
}


// vul grafe en graad zo dat het een boom voorstelt
void genereer_willekeurige_opspannende_boom(vector< vector<edge> > &originele_grafe, int aantal_knopen, vector< vector<edge> > &grafe, vector<int> &graad)
{
	int willekeurige_root=rand_between(0,aantal_knopen-1);
	willekeurige_opspannende_boom_met_root(willekeurige_root,originele_grafe,grafe,graad,aantal_knopen);
}

bool debug_graad_check(vector< vector<edge> > &grafe, vector<int> &graad)
{
	for(int i=0; i<grafe.size(); i++)
	{
		int verwacht_aantal=0;
		for(edge e : grafe[i])
		{
			if(e.actief) verwacht_aantal++;
		}
		if(graad[i] != verwacht_aantal) return false;
	}
	return true;
}

bool debug_is_verbonden(vector< vector<edge> > &grafe)
{
	vector<bool> vis(grafe.size(),false);
	queue<int> q;
	q.push(0);
	vis[0]=true;
	int aantal=1;
	while(!q.empty())
	{
		int now=q.front();
		q.pop();
		for(edge e : grafe[now])
		{
			if(!e.actief) continue;
			if(vis[e.buur]) continue;
			vis[e.buur]=true;
			q.push(e.buur);
			aantal++;
		}
	}
	return aantal==(int)grafe.size();
}

int debug_aantal_edges(vector< vector<edge> > &grafe)
{
	int ret=0;
	for(int i=0; i<grafe.size(); i++)
	{
		for(edge e : grafe[i])
		{
			if(e.actief) ret++;
		}
	}
	return ret;
}

bool debug_is_boom(vector< vector<edge> > &grafe)
{
	return debug_is_verbonden(grafe) && debug_aantal_edges(grafe)==2*((int)grafe.size()-1);
}

bool debug_clean_input(vector< vector<edge> > &originele_grafe, int verwacht_aantal_knopen, int verwacht_aantal_edges)
{
	int werkelijk_aantal_edges=0;
	for(int i=0; i<originele_grafe.size(); i++)
	{
		for(edge e : originele_grafe[i])
		{
			if(!e.actief)
			{
				printf("e was not active!\n");
				return false;
			}
			if(e.deel_van_cykel)
			{
				printf("e was part of cycle!\n");
				return false;
			}
			if(e.geselecteerd_in_spanning_tree)
			{
				printf("e was selected in spanning tree!\n");
				return false;
			}
			if(!(0<=e.buur && e.buur<originele_grafe.size()))
			{
				printf("e.buur was not in valid range!\n");
				return false;
			}
			if(!(0 <= e.index_bij_buur && e.index_bij_buur < originele_grafe[e.buur].size()))
			{
				printf("index bij buur was not in valid range!\n");
				return false;
			}
			if(i!=originele_grafe[e.buur][e.index_bij_buur].buur) 
			{
				printf("index bij buur doesn't refer to this node!\n");
				return false;
			}
			werkelijk_aantal_edges++;
		}
	}
	if(!((int)originele_grafe.size()==verwacht_aantal_knopen))
	{
		printf("verwacht aantal_knopen klopt niet!\n");
		return false;
	}
	if(!(verwacht_aantal_edges==werkelijk_aantal_edges))
	{
		printf("verwacht aantal_edges klopt niet!\n");
		return false;
	}
	return (int)originele_grafe.size()==verwacht_aantal_knopen && verwacht_aantal_edges==werkelijk_aantal_edges;
}

//double greedy_join_time=0.0;
// join de uiteinden van de paden op een greedy manier
//int join_calls=0;
void join_edges_greedy(vector< vector<edge> > &tree, vector<int> &graad, vector< vector<edge> > &component, int aantal_knopen)
{
    //join_calls++;
	//auto start = std::chrono::steady_clock::now( );
	//printf("DDDDDDDDDDDDDDDDDDDDDDD\n");
	vector<int> ander_uiteinde(aantal_knopen,-1);
	vector<bool> bezocht(aantal_knopen,false);
	vector<int> vorig(aantal_knopen,-1);
	vector<int> cykel_id(aantal_knopen,-1);
	/*printf("EEEEEEEEEEEEEEEEEEEEEEE\n");*/
	if(debug_mode_on)
    {
        bool b=debug_graad_check(tree,graad);
	    if(!b) cout << "b: " << b << endl;
    }
	int id_ctr=0;
	// make cycles when possible
	for(int i=0; i<aantal_knopen; i++)
	{
		if(bezocht[i]) continue;
		if(graad[i] >= 2) continue;
		queue<int> q;
		q.push(i);
		bezocht[i]=true;
		int end1=i;
		int end2=i;
		while(!q.empty())
		{
			int now=q.front();
			q.pop();
			for(edge e : tree[now])
			{
				if(!e.actief) continue;
				if(bezocht[e.buur]) continue;
				end2=e.buur;
				bezocht[end2]=true;
				q.push(end2);
				vorig[end2]=now;
			}
		}
		ander_uiteinde[end1]=end2;
		ander_uiteinde[end2]=end1;
		if(vorig[end2] != end1 && all_edges_in_component.count(make_pair(end1,end2))>0)
		{
			if(index_lookup.count(make_pair(end1,end2))==0)
			{
				tree[end1].push_back(edge(end2,tree[end2].size(),true,false));
				tree[end2].push_back(edge(end1,tree[end1].size()-1,true,false));
				index_lookup[make_pair(end1,end2)]=tree[end1].size()-1;
				index_lookup[make_pair(end2,end1)]=tree[end2].size()-1;
			}
			tree[end1][index_lookup[make_pair(end1,end2)]].actief=true;
			tree[end2][index_lookup[make_pair(end2,end1)]].actief=true;
			graad[end1]++;
			graad[end2]++;
			ander_uiteinde[end1]=ander_uiteinde[end2]=-1;
			int now=end2;
			while(now != -1)
			{
				cykel_id[now]=id_ctr;
				now=vorig[now];
			}
			id_ctr++;
		}
	}
	/*printf("FFFFFFFFFFFFFFFFFFFFF\n");*/
	if(debug_mode_on)
    {
        bool b7=debug_graad_check(tree,graad);
	    if(!b7) cout << "b7: " << b7 << endl;
    }
	/*for(int i=0; i<aantal_knopen; i++)
	{
		if(graad[i]==1 && ander_uiteinde[i]==-1)
		{
			printf("Fout bij knoop: %d\n", i);
		}
	}*/
	// join cycles+paths
	for(int i=0; i<aantal_knopen; i++)
	{
		if(!(graad[i]<=1 || cykel_id[i]>=0)) continue;
		for(edge e : component[i])
		{
			if(graad[i]<=1)
			{
				if(!((graad[e.buur]<=1 && ander_uiteinde[i]!=e.buur) || (cykel_id[e.buur]>=0))) continue;
			}
			if(cykel_id[i]>=0)
			{
				if(!(graad[e.buur]<=1 || (cykel_id[e.buur]>=0 && cykel_id[e.buur] != cykel_id[i]))) continue;
			}
			if(cykel_id[i]>=0) // unmark cycle + break it
			{
				int now=i;
				while(true)
				{
					bool found=false;
					for(edge e2 : tree[now])
					{
						if(!e2.actief) continue;
						if(cykel_id[e2.buur]==cykel_id[now])
						{
							cykel_id[now]=-1;
							now=e2.buur;
							found=true;
							break;
						}
					}
					if(!found) break;
				}
				cykel_id[now]=-1;
				bool entered=false;
				for(edge &e2 : tree[i])
				{
					if(e2.actief)
					{
						ander_uiteinde[i]=e2.buur;
						ander_uiteinde[e2.buur]=i;
						graad[i]--;
						graad[e2.buur]--;
						e2.actief=false;
						tree[e2.buur][e2.index_bij_buur].actief=false;
						entered=true;
						break;
					}
				}
				if(!entered)
				{
					printf("Moet sowieso geenterd worden1!\n");
				}
			}
			if(cykel_id[e.buur]>=0) // unmark cycle+break it
			{
				//printf("Enet\n");
				int now=e.buur;
				while(true)
				{
					bool found=false;
					for(edge e2 : tree[now])
					{
						if(!e2.actief) continue;
						if(cykel_id[e2.buur]==cykel_id[now])
						{
							cykel_id[now]=-1;
							now=e2.buur;
							found=true;
							break;
						}
					}
					if(!found) break;
				}
				cykel_id[now]=-1;
				bool entered=false;
				for(edge &e2 : tree[e.buur])
				{
					if(e2.actief)
					{
						ander_uiteinde[e.buur]=e2.buur;
						//printf("ander_uiteinde[%d]=%d;",e.buur,e2.buur);
						ander_uiteinde[e2.buur]=e.buur;
						graad[e.buur]--;
						graad[e2.buur]--;
						e2.actief=false;
						tree[e2.buur][e2.index_bij_buur].actief=false;
						entered=true;
						break;
					}
				}
				if(!entered)
				{
					printf("Moet sowieso geenterd worden2!\n");
				}
			}
			if(index_lookup.count(make_pair(i,e.buur))==0)
			{
				tree[i].push_back(edge(e.buur,tree[e.buur].size(),true,false));
				tree[e.buur].push_back(edge(i,tree[i].size()-1,true,false));
				index_lookup[make_pair(i,e.buur)]=tree[i].size()-1;
				index_lookup[make_pair(e.buur,i)]=tree[e.buur].size()-1;
			}
			tree[i][index_lookup[make_pair(i,e.buur)]].actief=true;
			tree[e.buur][index_lookup[make_pair(e.buur,i)]].actief=true;
			graad[i]++;
			graad[e.buur]++;
			int nieuw_uiteinde1=ander_uiteinde[i];
			int nieuw_uiteinde2=ander_uiteinde[e.buur];
			//printf("i: %d, e.buur: %d, nieuw_uiteinde1: %d, nieuw_uiteinde2: %d\n",i,e.buur,nieuw_uiteinde1,nieuw_uiteinde2);
			//printf("Graad i: %d, graad e.buur: %d, cykel_id i: %d, cykel_id e.buur: %d\n", graad[i], graad[e.buur], cykel_id[i], cykel_id[e.buur]);
			ander_uiteinde[i]=ander_uiteinde[e.buur]=-1;
			ander_uiteinde[nieuw_uiteinde1]=nieuw_uiteinde2;
			ander_uiteinde[nieuw_uiteinde2]=nieuw_uiteinde1;
			if(!(graad[i]<=1 || cykel_id[i]>=0)) break;
		}
	}
	//printf("GGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
	if(debug_mode_on)
    {
        bool b8=debug_graad_check(tree,graad);
	    if(!b8) cout << "b8: " << b8 << endl;
    }
	// cycles die nog overblijven verbreken
	for(int i=0; i<aantal_knopen; i++)
	{
		if(cykel_id[i]>=0) // unmark cycle + break it
		{
			int now=i;
			while(true)
			{
				bool found=false;
				for(edge e2 : tree[now])
				{
					if(!e2.actief) continue;
					if(cykel_id[e2.buur]==cykel_id[now])
					{
						cykel_id[now]=-1;
						now=e2.buur;
						found=true;
						break;
					}
				}
				if(!found) break;
			}
			cykel_id[now]=-1;
			for(edge &e2 : tree[i])
			{
				if(e2.actief)
				{
					ander_uiteinde[i]=e2.buur;
					ander_uiteinde[e2.buur]=i;
					graad[i]--;
					graad[e2.buur]--;
					e2.actief=false;
					tree[e2.buur][e2.index_bij_buur].actief=false;
					break;
				}
			}
		}
	}
	//printf("HHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
	for(int i=0; i<aantal_knopen; i++)
	{
		for(edge &e : component[i]) e.geselecteerd_in_spanning_tree=false;
	}
	//printf("IIIIIIIIIIIIIIIIIIIIIIIIII\n");
	/*auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	greedy_join_time += elapsed.count();*/
}

int find_leader(int u)
{
	if(u==leader[u]) return u;
	return leader[u]=find_leader(leader[u]);
}

void voeg_samen(int u, int v)
{
	leader[find_leader(u)]=find_leader(v);
}

bool in_same_component(int u, int v)
{
	return find_leader(u)==find_leader(v);
}

//double voeg_volgorde_toe_time=0.0;
void voeg_volgorde_toe(vector< vector<edge> > &grafe, vector<int> &graad, int aantal_knopen)
{
    //auto start = std::chrono::steady_clock::now( );
	vector<bool> vis(aantal_knopen,false);
	for(int i=0; i<aantal_knopen; i++)
	{
		if(vis[i]) continue;
		if(graad[i]>=2) continue;
		queue<int> q;
		q.push(i);
		vis[i]=true;
		part_of_permutation.push_back(i);
		while(!q.empty())
		{
			int now=q.front();
			q.pop();
			for(edge e : grafe[now])
			{
				if(!e.actief) continue;
				if(vis[e.buur]) continue;
				vis[e.buur]=true;
				q.push(e.buur);
				part_of_permutation.push_back(e.buur);
			}
		}
	}
    /*auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
	voeg_volgorde_toe_time += elapsed.count();*/
}

//double make_tree_time=0.0;

//double component_time=0.0;
//double rotation_time=0.0;
//double initial_components_time=0.0;
//double subset_adding_time=0.0;
//double reset_time=0.0;

//double new_part_time=0.0;
//int make_tree_calls=0;
//long long make_tree_cost=0;
vector<int> random_perm;

void random_permutation2(int amount)
{
    random_perm.assign(amount,-1);
    for(int i=0; i<amount; i++) random_perm[i]=i;
    random_shuffle(random_perm.begin(),random_perm.end());
}

void make_tree_again(vector< vector<edge> > &originele_grafe, int aantal_knopen, int aantal_edges, vector< vector<edge> > &grafe, vector<int> &graad)
{
    /*make_tree_cost += aantal_edges;
    make_tree_calls++;
	auto start = std::chrono::steady_clock::now( );

    auto start2 = std::chrono::steady_clock::now( );*/
	//random_perm=random_permutation(aantal_edges);
    random_permutation2(aantal_edges);
    /*auto elapsed2 = ( std::chrono::steady_clock::now( ) - start2 );
	new_part_time += elapsed2.count();
    
    auto start3 = std::chrono::steady_clock::now( );*/
	vector<int> component_number(aantal_knopen,-1);
	vector< vector<int> > nodes_in_component;
	vector< pair<int, int> > sides;
	vector<int> node_on_side1(aantal_knopen);
	int comp_ctr=0;
	for(int i=0; i<aantal_knopen; i++)
	{
		if(graad[i]>1) continue;
		if(component_number[i] != -1) continue;
		component_number[i]=comp_ctr;
		vector<int> nodes;
		nodes.push_back(i);
		int curr=i;
		while(true)
		{
			bool found=false;
			for(edge e : grafe[curr])
			{
				if(!e.actief) continue;
				int neigh=e.buur;
				if(component_number[neigh]!=-1) continue;
				component_number[neigh]=comp_ctr;
				nodes.push_back(neigh);
				node_on_side1[neigh]=curr;
				curr=neigh;
				found=true;
				break;
			}
			if(!found) break;
		}
		nodes_in_component.push_back(nodes);
		sides.push_back(make_pair(i,curr));
		comp_ctr++;
	}
    /*auto elapsed3 = ( std::chrono::steady_clock::now( ) - start3 );
	component_time += elapsed3.count();
    
    auto start4 = std::chrono::steady_clock::now( );*/
	vector<bool> rotated(aantal_knopen,false);
	// do rotation moves on paths
	for(int i=0; i<aantal_edges; i++)
	{
		int u1=all_edges_in_component_vec[random_perm[i]].first;
		int u2=all_edges_in_component_vec[random_perm[i]].second;
		if(!(!rotated[u1] && !rotated[u2] && component_number[u1]==component_number[u2] && (graad[u1]==1 || graad[u2]==1) && node_on_side1[u1]!=u2 && node_on_side1[u2]!=u1)) continue;
		for(int nd : nodes_in_component[component_number[u1]]) rotated[nd]=true;
		if(graad[u1] != 1) swap(u1,u2);
		bool u1_is_first_side=true;
		if(u1 != sides[component_number[u1]].first) u1_is_first_side=false;
		for(edge &e : grafe[u2])
		{
			if(!e.actief) continue;
			int neigh=e.buur;
			if(!((u1_is_first_side && neigh==node_on_side1[u2]) || (!u1_is_first_side && neigh!=node_on_side1[u2]))) continue;
			graad[u2]--;
			graad[neigh]--;
			e.actief=false;
			grafe[neigh][e.index_bij_buur].actief=false;
			break;
		}
		if(index_lookup.count(make_pair(u1,u2))==0)
		{
			grafe[u1].push_back(edge(u2,grafe[u2].size(),true,false));
			grafe[u2].push_back(edge(u1,grafe[u1].size()-1,true,false));
			index_lookup[make_pair(u1,u2)]=grafe[u1].size()-1;
			index_lookup[make_pair(u2,u1)]=grafe[u2].size()-1;
		}
		grafe[u1][index_lookup[make_pair(u1,u2)]].actief=true;
		grafe[u2][index_lookup[make_pair(u2,u1)]].actief=true;
		graad[u1]++;
		graad[u2]++;
	}
    /*auto elapsed4 = ( std::chrono::steady_clock::now( ) - start4 );
	rotation_time += elapsed4.count();
    
    auto start5 = std::chrono::steady_clock::now( );*/
	for(int i=0; i<aantal_knopen; i++) leader[i]=i;
	//set< pair<int, int> >  deel_van_opspannende_boom;
	unordered_set< pair<int, int>, pair_hash> deel_van_opspannende_boom;
	for(int i=0; i<aantal_knopen; i++)
	{
		for(edge e : grafe[i])
		{
			if(!e.actief) continue;
			deel_van_opspannende_boom.insert(make_pair(i,e.buur));
			deel_van_opspannende_boom.insert(make_pair(e.buur,i));
			voeg_samen(i,e.buur);
		}
	}

	unordered_set<int> component_numbers_set;
	for(int i=0; i<aantal_knopen; i++) component_numbers_set.insert(find_leader(i));
    /*auto elapsed5 = ( std::chrono::steady_clock::now( ) - start5 );
	initial_components_time += elapsed5.count();

    auto start6 = std::chrono::steady_clock::now( );*/
	//printf("REACHED. deb_ctr: %d. deb_ctr2: %d\n",deb_ctr,deb_ctr2);
	int aantal_componenten=component_numbers_set.size();
	// add a subset of edges. Prefer edges that have at least one node which is the end of a path.
	if(aantal_componenten>1)
	{
		vector<int> preferred_indices;
		vector<int> normal_indices;
		for(int i=0; i<aantal_edges; i++)
		{
			int u1=all_edges_in_component_vec[random_perm[i]].first;
			int u2=all_edges_in_component_vec[random_perm[i]].second;
			if(in_same_component(u1,u2)) continue;
			if(graad[u1]<=1 || graad[u2]<=1) preferred_indices.push_back(i);
			else normal_indices.push_back(i);
		}
		int ptr_preferred=0;
		int ptr_normal=0;
		while(aantal_componenten>1)
		{
			int i=-1;
			if(ptr_preferred==preferred_indices.size()) i=normal_indices[ptr_normal++];
			if(ptr_normal==normal_indices.size()) i=preferred_indices[ptr_preferred++];
			if(i==-1)
			{
				int r=rand_between(0,INT_MAX);
				if(r<=preferredRatio*INT_MAX/(preferredRatio+1)) i=preferred_indices[ptr_preferred++];
				else i=normal_indices[ptr_normal++];
			}
			int u1=all_edges_in_component_vec[random_perm[i]].first;
			int u2=all_edges_in_component_vec[random_perm[i]].second;
			if(!in_same_component(u1, u2))
			{
				voeg_samen(u1,u2);
				aantal_componenten--;
				if(index_lookup.count(make_pair(u1,u2))==0)
				{
					grafe[u1].push_back(edge(u2,grafe[u2].size(),true,false));
					grafe[u2].push_back(edge(u1,grafe[u1].size()-1,true,false));
					index_lookup[make_pair(u1,u2)]=grafe[u1].size()-1;
					index_lookup[make_pair(u2,u1)]=grafe[u2].size()-1;
				}
				grafe[u1][index_lookup[make_pair(u1,u2)]].actief=true;
				grafe[u2][index_lookup[make_pair(u2,u1)]].actief=true;
				graad[u1]++;
				graad[u2]++;
				deel_van_opspannende_boom.insert(make_pair(u1,u2));
				deel_van_opspannende_boom.insert(make_pair(u2,u1));
			}
		}
	}
    /*auto elapsed6 = ( std::chrono::steady_clock::now( ) - start6 );
	subset_adding_time += elapsed6.count();

    auto start7 = std::chrono::steady_clock::now( );*/
	for(int i=0; i<aantal_knopen; i++)
	{
		for(edge &e : originele_grafe[i])
		{
			e.actief=true;
			e.deel_van_cykel=false;
			e.geselecteerd_in_spanning_tree=false;
			if(deel_van_opspannende_boom.count(make_pair(i,e.buur))>0) e.geselecteerd_in_spanning_tree=true;
		}
	}
    /*auto elapsed7 = ( std::chrono::steady_clock::now( ) - start7 );
	reset_time += elapsed7.count();*/

    if(debug_mode_on)
    {
        if(!debug_is_boom(grafe))
	    {
		    printf("Tree expected, but was not a tree2!\n");
	    }
	    bool b4=debug_graad_check(grafe,graad);
	    if(!b4) cout << "b4: " << b4 << endl;
    }
	/*auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	make_tree_time += elapsed.count();*/
}

void schrijf_invoer_voor_lkh(vector< vector<edge> > &originele_grafe, int aantal_knopen)
{
	ofstream file;
    string name=signature;
    name+="lkh_input.tsp";
	file.open(name.c_str());
	file << "NAME : lkh_invoer\n";
	file << "TYPE : TSP\n";
	file << "COMMENT : No comment\n";
	file << "DIMENSION : ";
	file << aantal_knopen << "\n";
	file << "EDGE_WEIGHT_TYPE : EXPLICIT\n";
	file << "EDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW\n";
	file << "EDGE_DATA_FORMAT : EDGE_LIST\n";
	file << "EDGE_DATA_SECTION :\n";
	for(int i=0; i<aantal_knopen; i++)
	{
		for(edge e : originele_grafe[i])
		{
			file << " " << i+1 << " " << e.buur+1 << "\n"; 
		}
	}
	file << "EOF\n";
	file.close();
}

void voer_lkh_uit()
{
    string name=signature;
    name+="lkh_input.par";
    string command="./LKH ";
    command+=name;
	system(command.c_str());
}

void writeLKHTime(double t)
{
    ofstream file;
    string name=signature;
    name+="lkhPreprocessingTime";
	file.open(name.c_str());
    file << "Amount of nanoseconds spent on LKH preprocessing: " << t;
    file.close();
}

void writeLKHResult(int r)
{
    ofstream file;
    string name=signature;
    name+="lkhPreprocessingResult";
	file.open(name.c_str());
    file << r;
    file.close();
}

int LKHMinimaalAantal;
// opmerking: laatste en eerste pad worden niet samengevoegd
// paden die uit 1 knoop bestaan worden overgeslagen
vector< vector<int> > get_paden_van_lkh(int aantal_knopen)
{
	ifstream file;
    string name=signature;
    name+="LKHOutput";
	file.open(name.c_str());
	string s;
	while(true)
	{
		file >> s;
		if(s=="TOUR_SECTION") break;
	}
	vector<int> lkh_tour(aantal_knopen);
	for(int i=0; i<aantal_knopen; i++)
	{
		file >> lkh_tour[i];
		//cerr << "testen: " << lkh_tour[i] << endl;
		lkh_tour[i]--;		
	}
	vector< vector<int> > ret;
	vector<int> huidig_pad;
	bool cycle=true;
    int HCN=aantal_knopen;
	for(int i=0; i<aantal_knopen; i++)
	{
		int nd1=lkh_tour[i];
		int nd2=lkh_tour[(i+1)%aantal_knopen];
		if(all_edges_in_component.count(make_pair(nd1,nd2))>0)
		{
            HCN--;
			if(huidig_pad.size()==0)
			{
				huidig_pad.push_back(nd1);
			}
			huidig_pad.push_back(nd2);
		}
		else
		{
			if(huidig_pad.size()>0) ret.push_back(huidig_pad);
			huidig_pad.clear();
			cycle=false;
		}
	}
	if(cycle) huidig_pad.pop_back();
	if(huidig_pad.size()>0) ret.push_back(huidig_pad);
	file.close();
    LKHMinimaalAantal=min(LKHMinimaalAantal,HCN);
	return ret;
}

double LKHTime = 0.0;
void genereer_boom_via_lkh(vector< vector<edge> > &originele_grafe, int aantal_knopen, int aantal_edges, vector< vector<edge> > &boom, vector<int> &graad)
{
	auto start = std::chrono::steady_clock::now( );
	if(aantal_knopen<=2)
	{
		genereer_willekeurige_opspannende_boom(originele_grafe,aantal_knopen,boom,graad);
        LKHMinimaalAantal=0;
		auto elapsed = ( std::chrono::steady_clock::now( ) - start );
		LKHTime += elapsed.count();
		return;
	}
	schrijf_invoer_voor_lkh(originele_grafe,aantal_knopen);
	voer_lkh_uit();
	vector< vector<int> > paden=get_paden_van_lkh(aantal_knopen);
	// cerr << "ok1" << endl;
	for(vector<int> pad : paden)
	{
		for(int i=0; i+1<pad.size(); i++)
		{
			int nd1=pad[i];
			int nd2=pad[i+1];
			graad[nd1]++;
			graad[nd2]++;
			boom[nd1].push_back(edge(nd2,boom[nd2].size(),true,false));
			boom[nd2].push_back(edge(nd1,boom[nd1].size()-1,true,false));
			for(edge &e : originele_grafe[nd1])
			{
				if(e.buur==nd2)
				{
					e.geselecteerd_in_spanning_tree=true;
					originele_grafe[nd2][e.index_bij_buur].geselecteerd_in_spanning_tree=true;
					break;
				}
			}
		}
	}
    auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	LKHTime += elapsed.count();
	// cerr << "ok2" << endl;
	index_lookup.clear();
	for(int i=0; i<aantal_knopen; i++)
	{
		for(int j=0; j<boom[i].size(); j++)
		{
			index_lookup[make_pair(i,boom[i][j].buur)]=j;
		}
	}
	make_tree_again(originele_grafe,aantal_knopen,aantal_edges,boom,graad);
	//cerr << "ok3" << endl;	
}


// genereer een willekeurige opspannende boom en bereken het antwoord
// originele_grafe moet uit exact 1 component bestaan

//edge.geselecteerd_in_spanning_tree om spanning tree zelf aan te geven
//edge.actief om paths in die spanning tree aan te geven
//int deb_ctr=0;
int minimaal_aantal;
//double willekeurige_opspannende_boom_time=0.0;
//double while_true_time=0.0;
//double double_for_time=0.0;
int antwoord_willekeurige_opspannende_boom(vector< vector<edge> > &originele_grafe, int aantal_knopen, int aantal_edges)
{
	//auto start2 = std::chrono::steady_clock::now( );
    if(debug_mode_on)
    {
        if(!debug_clean_input(originele_grafe,aantal_knopen,aantal_edges))
	    {
		    printf("originele_grafe was not clean!\n");
	    }
    }
	//deb_ctr++;
	all_edges_in_component.clear();
	all_edges_in_component_vec.clear();
	for(int i=0; i<aantal_knopen; i++)
	{
		for(int j=0; j<originele_grafe[i].size(); j++)
		{
			edge e = originele_grafe[i][j];
			all_edges_in_component.insert(make_pair(i,e.buur));
			all_edges_in_component_vec.push_back(make_pair(i,e.buur));
		}
	}
	vector<edge> lege_vector;
	vector< vector<edge> > grafe;
	grafe.assign(aantal_knopen,lege_vector);
	vector<int> graad(aantal_knopen,0);
	//genereer_willekeurige_opspannende_boom(originele_grafe,aantal_knopen,grafe,graad);
	genereer_boom_via_lkh(originele_grafe,aantal_knopen,aantal_edges,grafe,graad);
    if(debug_mode_on)
    {
        if(!debug_is_boom(grafe))
	    {
		    printf("Tree expected, but was not a tree1!\n");
	    }
    }	
	int antwoord=aantal_knopen+5;
	//int deb_ctr2=0;
	int nb_equal=0;
    int wortel;
    int nieuw_antwoord;
    //auto start3 = std::chrono::steady_clock::now( );
	while(true)
	{
        //auto start = std::chrono::steady_clock::now( );
		//deb_ctr2++;
		//printf("AAAAAAAAAAAAA. deb_ctr: %d. deb_ctr2: %d\n",deb_ctr,deb_ctr2);
		wortel=rand_between(0,aantal_knopen-1);
		f(wortel,grafe,graad,aantal_knopen);
		//printf("BBBBBBBBBBBBB. deb_ctr: %d. deb_ctr2: %d\n",deb_ctr,deb_ctr2);
		index_lookup.clear();
		for(int i=0; i<aantal_knopen; i++)
		{
			for(int j=0; j<grafe[i].size(); j++)
			{
				index_lookup[make_pair(i,grafe[i][j].buur)]=j;
			}
		}
        if(debug_mode_on)
        {
            bool b2=debug_graad_check(grafe,graad);
		    if(!b2) cout << "b2: " << b2 << endl;
        }
		join_edges_greedy(grafe,graad,originele_grafe,aantal_knopen);
        if(debug_mode_on)
        {
            bool b3=debug_graad_check(grafe,graad);
		    if(!b3) cout << "b3: " << b3 << endl;
        }
		/*printf("CCCCCCCCCCCCC. deb_ctr: %d. deb_ctr2: %d\n",deb_ctr,deb_ctr2);*/
		nieuw_antwoord=get_hcn_voor_boom(grafe,graad,aantal_knopen);
		if(nieuw_antwoord>antwoord)
		{
			printf("Nieuw antwoord is groter dan antwoord. Dit kan niet!\n");
		}
		if(nieuw_antwoord>=antwoord && nb_equal>maxAmountBadPerturbations) break;
		if(nieuw_antwoord==antwoord) nb_equal++;
		antwoord=nieuw_antwoord;
		if(antwoord<minimaal_aantal)
		{
			part_of_permutation.clear();
			voeg_volgorde_toe(grafe,graad,aantal_knopen);			
		}
        //auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start );
	    //double_for_time += elapsed.count();
		if(antwoord==0) break;
		make_tree_again(originele_grafe,aantal_knopen,aantal_edges,grafe,graad);
	}
    /*auto elapsed3 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start3 );
	while_true_time += elapsed3.count();*/
	for(int i=0; i<originele_grafe.size(); i++)
	{
		for(edge &e : originele_grafe[i])
		{
			e.geselecteerd_in_spanning_tree=false;
		}
	}
	/*auto elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now( ) - start2 );
	willekeurige_opspannende_boom_time += elapsed2.count();*/
	return antwoord;
}

void write_heuristic_tour(int aantal_knopen, int totaal)
{
    ofstream file;
    string name=signature+"_heuristic_tour";
    file.open(name.c_str());
    //freopen("heuristic_tour.out", "w", stdout);
    file << aantal_knopen << " " << totaal << endl;
	for(int x : whole_permutation)
	{
        file << x << endl;
	}
    file.close();
}

void write_objective_value(int totaal)
{
    ofstream file;
    string name=signature+"_objective_value";
    file.open(name.c_str());
    //freopen("heuristic_tour.out", "w", stdout);
    file << totaal << endl;
    file << "The used parameters are: " << endl;
    file << "preferredRatio: " << preferredRatio << endl;
    file << "maxAmountInitialSpanningTrees: " << maxAmountInitialSpanningTrees << endl;
    file << "maxAmountBadPerturbations: " << maxAmountBadPerturbations << endl;
    file.close();
}

void writeTotalTime(double total_time)
{
    ofstream file;
    string name=signature+"_total_time";
    file.open(name.c_str());
    double in_seconds=total_time/1e9;
    file << in_seconds << " seconds used for computation" << endl;
    file.close();
}

int main(int argc, char** argv)
{
    if(argc!=5)
    {
        cout << "Wrong number of arguments!" << endl;
        cout << "An example of a correct command is: ./multiStartLocalSearch inputFile.in" << endl;
        return 0;
    }
    inputFileName=argv[1];
    string par1=argv[2];
    string par2=argv[3];
    string par3=argv[4];
    signature=inputFileName+"_"+par1+"_"+par2+"_"+par3+"_";
    preferredRatio=atof(par1.c_str());
    maxAmountInitialSpanningTrees=atoi(par2.c_str());
    maxAmountBadPerturbations=atoi(par3.c_str());
    ios::sync_with_stdio(false);
	double total_time=0.0;
	auto start = std::chrono::steady_clock::now( );
	init_random_numbers();
    //read_parameters();
    read_max_allowed_time();
    rng=mt19937(chrono::steady_clock::now().time_since_epoch().count());
	ifstream file;
	file.open(inputFileName.c_str());
	int aantal_knopen, aantal_edges;
	file >> aantal_knopen >> aantal_edges;
	leader.assign(aantal_knopen,-1);
    vector<edge> lege_vector;
	vector< vector<edge> > originele_grafe;
    originele_grafe.assign(aantal_knopen, lege_vector);
	// lees de grafe in
    for(int i=0; i<aantal_edges; i++)
    {
        int u, v;
		file >> u >> v;
        //cerr << u << " " << v << endl;
        if(!(0<=u && u <= aantal_knopen-1 && 0 <= v && v <= aantal_knopen-1))
        {
            printf("De knopen moeten een nummer hebben tussen 0 en %d (inclusief).\n", aantal_knopen-1);
            return 0;
        }
        originele_grafe[u].push_back(edge(v,originele_grafe[v].size(),true,false));
        //cerr << "u: " << u << endl;
        //cerr << grafe[u].size() << endl;
        if(u != v)
        {
            originele_grafe[v].push_back(edge(u,originele_grafe[u].size()-1,true,false));
        }
    }
	file.close();

    if(aantal_knopen==2)
    {
        if(originele_grafe[0].size()>=1) printf("0\n");
		else printf("1\n");
        printf("Attention: the input file only contained 2 nodes. Not all output files were generated for this trivial case.");
        return 0;
    }
    
	int totaal=0;
    int LKHTotal=0;
	vector<bool> bezocht(aantal_knopen,false);
	vector<int> naar_component_index(aantal_knopen,-1);
	queue<int> q;
	// splits op in componenten
	for(int i=0; i<aantal_knopen; i++)
	{
		if(bezocht[i]) continue;
		vector<int> indices;
		indices.push_back(i);
		q.push(i);
		bezocht[i]=true;
		naar_component_index[i]=0;
		while(!q.empty())
		{
			int now=q.front();
			q.pop();
			for(edge e : originele_grafe[now])
			{
				int neigh=e.buur;
				if(bezocht[neigh]) continue;
				naar_component_index[neigh]=indices.size();
				indices.push_back(neigh);
				bezocht[neigh]=true;
				q.push(neigh);
			}
		}
		vector<edge> lege_vector;
		int aantal_in_component=indices.size();
		vector< vector<edge> > component(aantal_in_component,lege_vector);
		int aantal_edges_in_component=0;
		for(int original_idx : indices)
		{
			for(edge e : originele_grafe[original_idx])
			{
				int neigh=e.buur;
				int i1=naar_component_index[original_idx];
				int i2=naar_component_index[neigh];
				if(i2>=i1) continue;
				aantal_edges_in_component+=2;
				component[i1].push_back(edge(i2,component[i2].size(),true,false));
				component[i2].push_back(edge(i1,component[i1].size()-1,true,false));
			}
		}
		minimaal_aantal=aantal_knopen+5;
        LKHMinimaalAantal=aantal_knopen+5;
		//deb_ctr=0;
		part_of_permutation.clear();
		// zoek antwoord voor huidige component
		for(int poging=1; poging<=maxAmountInitialSpanningTrees && minimaal_aantal != 0; poging++)
		{
			int antwoord=antwoord_willekeurige_opspannende_boom(component,aantal_in_component,aantal_edges_in_component);
			minimaal_aantal=min(minimaal_aantal,antwoord);
            auto elapsed = ( std::chrono::steady_clock::now( ) - start );
            if(elapsed.count() >= 1e9*maxAllowedSeconds) break;
		}
        auto elapsed = ( std::chrono::steady_clock::now( ) - start );
        if(elapsed.count() >= 1e9*maxAllowedSeconds) break;
		for(int x : part_of_permutation) whole_permutation.push_back(indices[x]);
		// speciaal geval waarbij het antwoord mogelijk gelijk kan zijn aan 0
		if(aantal_in_component==aantal_knopen)
		{
			totaal=minimaal_aantal;
            LKHTotal=LKHMinimaalAantal;
			break;
		}
		if(minimaal_aantal==0) minimaal_aantal=1;
        if(LKHMinimaalAantal==0) LKHMinimaalAantal=1;
		totaal += minimaal_aantal;	
        LKHTotal += LKHMinimaalAantal;
	}
	printf("%d\n",totaal);
	auto elapsed = ( std::chrono::steady_clock::now( ) - start );
	total_time += elapsed.count();
    writeTotalTime(total_time);
    if(total_time >= 1e9*maxAllowedSeconds) return 0;
    writeLKHTime(LKHTime);
    writeLKHResult(LKHTotal);
	//cerr << "Amount of nanoseconds spent on LKH in step 1: " << LKHTime << endl;
	/*cerr << "Amount of time spent on make_tree: " << make_tree_time << endl;
	cerr << "Amount of time spent on greedy join: " << greedy_join_time << endl;
	cerr << "Amount of time spent on willekeurige opspannende boom: " << willekeurige_opspannende_boom_time << endl;
    cerr << "Amount of time spent of f: " << f_time << endl;
    cerr << "Amount of time spent on get_hcn: " << get_hcn_time << endl;
    cerr << "Amount of time spent of voeg_volgorde_toe: " << voeg_volgorde_toe_time << endl;
    cerr << "Amount of time spent on rand_between: " << rand_between_time << endl;
    cerr << "Amount of time spent on part: " << double_for_time << endl;
    cerr << "Amount of time spent on while(true): " << while_true_time << endl;
    cerr << "Amount of time spent on new part: " << new_part_time << endl;
    
    cerr << "Amount of time spent on finding components: " << component_time << endl;
    cerr << "Amount of time spent on doing rotations: " << rotation_time << endl;
    cerr << "Amount of time spent on initial component merging: " << initial_components_time << endl;
    cerr << "Amount of time spent on subset adding: " << subset_adding_time << endl;
    cerr << "Amount of time spent on resetting: " << reset_time << endl;*/
    
	//cerr << "Amount of total time in nanoseconds: " << total_time << endl;
    /*cerr << "Join calls: " << join_calls << endl;
    cerr << "Make tree calls: " << make_tree_calls << endl;
    cerr << "f cost: " << f_cost << endl;
    cerr << "make tree cost: " << make_tree_cost << endl;*/
    write_heuristic_tour(aantal_knopen, totaal);
    write_objective_value(totaal);
    return 0;
}
