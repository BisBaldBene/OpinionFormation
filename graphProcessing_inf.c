/* The following structure is used:
*   There is a Graph struct which consists of a List of Node Pointers and an int Adjacency Matrix
*   Nodes are structs that contain one opinion
*   The adjacency matrix contains all information about connectivity
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


//Input parameter
/*#define NNodes 100
#define NEdges 30
#define NOpinions 25
#define phi100 12
#define MAX_ITER 100000000000*/
int NNodes;
int NEdges;
int NOpinions;
float phi;
int MAX_ITER;

typedef struct Graph{
    struct Node** NList;
    int ** Adj_Matrix;
} Graph;


typedef struct Node{
    int opinion;
    int isInfluencer;
    int degree;
} Node;


typedef struct int_array{
    int len;
    int * data;
} int_array;


void iter_neighbors(Graph * G, int_array * Checklist, int NIndex, int * consensus, int op, int * preventSO, int * comp);

//function that creates a random graph and returns a pointer to it
Graph * create_graph()
{
    Graph *G = (Graph *) malloc(sizeof(Graph));
    if (G == NULL)
    {   
        printf("malloc of Graph failed \n");
        return NULL;
    }
    G->NList = malloc(NNodes * sizeof(Node *));
    if (G->NList == NULL)
    {
        printf("malloc failed\n");
        free(G);
        exit(0);
    }
    G->Adj_Matrix = (int **) malloc(NNodes * sizeof(int *));
    if (G->Adj_Matrix == NULL)
    {
        printf("malloc failed\n");
        exit(0);
    }
    for (int i = 0; i < NNodes; ++i)
    {
        G->Adj_Matrix[i] = (int *)calloc(NNodes, sizeof(int));
        if (G->Adj_Matrix[i] == NULL)
        {
            printf("malloc failed\n");
            exit(0);
        }
    }    


    //gesetzte Kanten
    int Curr_NEdges = 0;

    //Bis alle Kanten gesetzt
    while(Curr_NEdges < NEdges)
    {
        //Beschränkung auf unteren Diagnoalmatrix
        //Random Zahl für Zeilenindex
        int rnd_i = rand() %NNodes;
        
        //0 Ausschließen, da die 0.te Zeile nur den Diagonaleintrag in der unteren Diagonalmatrix hat
        if (rnd_i == 0)
        {
            continue;
        }
        //Random Zahl für Spaltenindex, wobei Spaltenindex < Zeilenindex für Zelle in unterer Diagonalmatrix
        int rnd_j = rand() %rnd_i;

        //Falls die zufällige Zelle noch auf 0 (keine Kante)
        if (G->Adj_Matrix[rnd_i][rnd_j] == 0)
        {
            //setze Kante
            G->Adj_Matrix[rnd_i][rnd_j] = 1;
            G->Adj_Matrix[rnd_j][rnd_i] = 1;
            Curr_NEdges += 1;
        }
    }


/*    printf("Check Graph\n");
    int count = 0;
    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            if (G->Adj_Matrix[i][j] != G->Adj_Matrix[i][j])
            {
                printf("GRAPH ERROR\n");
                return NULL;
            }
            if (G->Adj_Matrix[i][j] == 1)
            {
                count+=1;
            }
            if (G->Adj_Matrix[i][i] == 1)
            {
                printf("Diagonal Error\n");
                return NULL;
            }
        }
        printf("count: %d\n", count);
    }*/

    for(int i=0; i<NNodes; i++)
    {  
        Node *new_node = (Node *)malloc(sizeof(Node));

        //if not successfull free all nodes and the graph:
        if (new_node ==NULL)
        {
            //free(new_node);
            for (int g = 0; g < i; ++g)
            {
                free(G->NList[g]);
                G->NList[g] = NULL;
            }
            free(G);
            G = NULL;
            printf("Allocation for new_node failed\n");
            return NULL;
        }

        //if success: get random opinion and append to nodelist
        new_node -> opinion = rand() %NOpinions;
        new_node -> isInfluencer = 0;
        new_node -> degree = -1;
        G->NList[i] = new_node;
    }
    return G;
}

//Checks whether initialization failed

/*void check_graph(Graph * G)
{
    int ones = 0;
    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            if (G->Adj_Matrix[i][j] ==1)
            {
                ones +=1;
            }
            else if (G->Adj_Matrix[i][j]!=0)            {
                printf("initialization error in adj matrix\n");
            }
            {

            }
        }
    }
    printf("Number of 1 entries in Adj_Matrix: %d \n", ones);
    printf("Should be %d \n", 2*NEdges);
}*/

/*Iterates throught graph and updates the degree attribute of each Node
*/
void update_degrees(Graph * G)
{
    for (int i = 0; i < NNodes; ++i)
    {
        G->NList[i]->degree = 0;
        for (int j = 0; j < NNodes; ++j)
        {
            if (G->Adj_Matrix[i][j] == 1)
            {
                G->NList[i]->degree +=1;
            }
        }
    }
}

//May and should retunr int_array of length 0
int_array * NodesOfDegreeK(Graph * G, int K)
{
    int * connectivity = (int *)calloc(NNodes, sizeof(int));
    if (connectivity == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            connectivity[i] += G->Adj_Matrix[i][j];
        }
    }

    int_array * NodesOfDegreeK = (int_array *) malloc(sizeof(int_array));

    if (NodesOfDegreeK == NULL)
    {
        printf("Allocation of NodesOfDegreeK failed \n");
        free(connectivity);
        return NULL;
    }

    NodesOfDegreeK->data = (int *) malloc(NNodes* sizeof(int));
    if (NodesOfDegreeK->data == NULL)
    {
        printf("Allocation of NodesOfDegreeK failed \n");
        free(NodesOfDegreeK);
        free(connectivity);
        NodesOfDegreeK = NULL;
        return NULL;
    }
    NodesOfDegreeK->len = 0;

    for (int i = 0; i < NNodes; ++i)
    {
        if (connectivity[i] == K)
        {
            NodesOfDegreeK->data[NodesOfDegreeK->len] = i;
            NodesOfDegreeK->len +=1;
        }
    }

    int * temp = (int *) realloc(NodesOfDegreeK->data, NodesOfDegreeK->len * sizeof(int));
    if (temp == NULL) {
        printf("length %d\n", NodesOfDegreeK->len );
        printf("Reallocation of NodesOfDegreeK failed \n");
        free(NodesOfDegreeK->data);
        NodesOfDegreeK->data = NULL;
        free(NodesOfDegreeK);
        NodesOfDegreeK = NULL;
        free(connectivity);
        return NULL;
    }
    else
    {
        NodesOfDegreeK->data = temp;
    }
    free(connectivity);
    return NodesOfDegreeK;
}


/*Function that manipulates the opinion of "NInfluencer" Nodes of Degree "InfDegree" to the opinion "Opinion"
* InfDegree = -2 -> Take Random Nodes
* InfDegree = -1 -> Take Nodes with max degree
* Returns int_array of length NInfluencer that contains the indices of the Influencer Nodes
*/
int_array * Op_Manipulate(Graph * G, int NInfluencer, int Opinion, int InfDegree)
{
    //Create int_array
    int_array * Influencer = (int_array *) malloc(sizeof(int_array));

    if (Influencer == NULL)
    {
        printf("Allocation of Influencer failed \n");
        return NULL;
    }

    Influencer->data = (int *) malloc(NNodes* sizeof(int));
    if (Influencer->data == NULL)
    {
        printf("Allocation of Influencer failed \n");
        free(Influencer);
        Influencer = NULL;
        return NULL;
    }
    Influencer->len = 0;
    
    //InfDegree = -2 means, get random influencer
    if (InfDegree == -2)
    {
        //As long as not enough Influencers
        while(Influencer->len != NInfluencer)
        {
            int choice = rand()%NNodes;
            //Does this work??ß
            for (int i = 0; i < Influencer->len; ++i)
            {
                if (Influencer->data[i] == choice)
                {
                    continue;
                }
                Influencer->data[Influencer->len] = choice;
                G->NList[choice]->isInfluencer = 1;
                G->NList[choice]-> opinion = Opinion;
                Influencer->len +=1;
            }
        }
        return Influencer;
    }


    //Create Array with all degrees 
    int * connectivity = (int *)calloc(NNodes, sizeof(int));
    if (connectivity == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }

    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            connectivity[i] += G->Adj_Matrix[i][j];
        }
    }
 
    //InfDegre = -1 means get the NInfluencer Nodes with highest connectivity
    if (InfDegree == -1)
    {
        for (int j = 0; j < NInfluencer; ++j)
        {
            int max = 0;
            int curr_index = -1;
            for (int i = 0; i < NNodes; ++i)
            {
                if (max < connectivity[i])
                {
                    max = connectivity[i];
                    curr_index = i;
                }
            }
            if (max != 0)
            {
                Influencer->data[j] = curr_index;
                G->NList[curr_index]->isInfluencer = 1;
                G->NList[curr_index]-> opinion = Opinion;
                curr_index = -1;
                //Delete max_value from connectivity list to get the 2nd (3rd...) max value the next tim
                connectivity[curr_index] = 0;
            }
        }
    }

    else if (InfDegree > 0)
    {
        int_array * DegreeK = NodesOfDegreeK(G, InfDegree);

        if (DegreeK->len == NInfluencer)
        {
            free(Influencer->data);
            free(Influencer);
            free(connectivity);
            return DegreeK;
        }
        if(DegreeK-> len > NInfluencer)
        {
            while(Influencer->len < NInfluencer)
            {
                int choice = rand() %DegreeK->len;
                for (int i = 0; i < Influencer->len; ++i)
                {
                    if (Influencer->data[i] == choice)
                    {
                        continue;
                    }
                }
                Influencer->data[Influencer->len] = choice;
                G->NList[choice]->opinion = Opinion;
                G->NList[choice]-> isInfluencer = 1;
                Influencer->len += 1;
            }
            free(DegreeK->data);
            free(DegreeK);
            free(connectivity);
            return Influencer;
        }

        if(DegreeK->len < NInfluencer)
        {
            for (int i = 0; i < DegreeK->len; ++i)
            {
                Influencer->data[i] = DegreeK->data[i];
                Influencer->len +=1;
            }
            free(DegreeK->data);
            free(DegreeK);
            int x = 0;
            while(Influencer->len + x < NInfluencer)
            {
                int_array * DegreeKp = NodesOfDegreeK(G, InfDegree +1);
                int_array * DegreeKm = NodesOfDegreeK(G, InfDegree -1);
                x += DegreeKm->len + DegreeKp->len;
                if (Influencer-> len + x > NInfluencer)
                {
                    while(Influencer-> len < NInfluencer)
                    {
                        int choice = rand()%(DegreeKm->len + DegreeKp->len);
                        if (choice >= DegreeKm->len)
                        {
                            for (int i = 0; i < Influencer->len; ++i)
                            {
                                if (Influencer->data[i] == DegreeKm->data[choice])
                                {
                                    continue;
                                }

                            }
                            Influencer->data[Influencer->len] = choice;
                            G->NList[choice]->isInfluencer = 1;
                            G->NList[choice]->opinion = Opinion;
                            Influencer->len += 1;
                        }
                        else
                        {
                            for (int i = 0; i < Influencer->len; ++i)
                            {
                                if (Influencer->data[i] == DegreeKp->data[choice-DegreeKm->len])
                                {
                                    continue;
                                }

                            }
                            Influencer->data[Influencer->len] = choice;
                            G->NList[choice]->isInfluencer = 1;
                            Influencer->len += 1;

                        }
                    }
                }
            free(DegreeKp->data);
            free(DegreeKm->data);
            free(DegreeKp);
            free(DegreeKm);
            free(connectivity);
            }
            
            
            
        }
        
    }

    else
    {
        exit(1);
    }
    
    return Influencer;
}

/*needs Graph and Index of a node
 * ouput: List of Indices that correspond to the neighbor nodes of the input node
 * NULL pointer if any kind of memory problem OR No Neighbors!
 */
int_array * getNeighbors(Graph * G, int NodeIndex)
{
    int_array * Neighborlist = (int_array *) malloc(sizeof(int_array));

    if (Neighborlist == NULL)
    {
        printf("Allocation of Nehborlist failed \n");
        return NULL;
    }

    Neighborlist->data = (int *) malloc(NNodes * sizeof(int));
    if (Neighborlist->data == NULL)
    {
        printf("Allocation of Neighborlist failed \n");
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }

    Neighborlist->len = 0;

    //Add Neighbors to Neighborlist
    for (int i=0; i<NNodes; i++)
    {
        if (G->Adj_Matrix[NodeIndex][i] == 1)
        {
            Neighborlist->data[Neighborlist->len]=i;
            Neighborlist->len +=1; 
        }
    }

    //If no neighbors, free space and return NULL
    if (Neighborlist->len == 0)
    {
        free(Neighborlist->data);
        Neighborlist->data = NULL;
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }

    //If some neighbors, reallocate for less than NNodes neighbors
    int * temp = (int *) realloc(Neighborlist->data, Neighborlist->len * sizeof(int));
    if (temp == NULL) {
        printf("length %d\n", Neighborlist->len );
        printf("Reallocation of Neighborlist failed \n");
        free(Neighborlist->data);
        Neighborlist->data = NULL;
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }
    else
    {
        Neighborlist->data = temp;
    }
    return Neighborlist;
}

//Returns int_array with all Nodes that are not yet connected but share the opinion
int_array * getSameOpinion(Graph * G, int NIndex)
{

    /*Create int_array (Node - Index - List) that contains all the other Nodes that
    have my opinion but are not yet connected to me */
    int_array * bubblestranger = malloc(sizeof(int_array));
    if (bubblestranger == NULL)
    {
        printf("Allocation of bubblestranger failed\n");
        return NULL;
    }

    bubblestranger->data = malloc(NNodes*sizeof(int));
    if (bubblestranger->data == NULL)
    {
        printf("Allocation of bubblestranger data failed \n");
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }
    bubblestranger->len = 0;
    
    //Get My opinion
    int curr_opinion = (G->NList[NIndex])->opinion;

    //Get My neighbors
    int_array * MyFriends = getNeighbors(G, NIndex);


    //Iterate all Nodes
    
    for (int i=0; i<NNodes; i++)
    {
        //If Opinion of ith Node is my opinion and its not me
        if( (G->NList[i])->opinion == curr_opinion && i != NIndex)
        {

            //Iterate through my friends and check whether i is already my friend
            
            int IsAlreadyMyFriend = 0;

            for (int j=0; j<MyFriends->len; j++)
            {
                if(MyFriends->data[j] == i)
                {
                    IsAlreadyMyFriend = 1;
                    break;
                }
            
            }
            //when not yet myfriend
            if (IsAlreadyMyFriend == 0)
            {
                //i is appended to bubblestranger list
                bubblestranger->data[bubblestranger->len] = i;
                //and list is now 1 element longer
                bubblestranger->len+=1;
            }
        }
    }

    //We no longer need my friends
    free(MyFriends->data);
    MyFriends->data = NULL;
    free(MyFriends);
    MyFriends = NULL;

    //return NULL pointer if no stranger shares my opinion 
    if (bubblestranger->len == 0)
    {
        free(bubblestranger->data);
        bubblestranger->data = NULL;
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }

    //Reallocate memory for bubblestranger
    bubblestranger->data = realloc(bubblestranger->data, bubblestranger->len * sizeof(int));
    if (bubblestranger->data == NULL)
    {
        printf("Reallocation of bubblestranger data failed\n");
        printf("No space for %d integers \n", bubblestranger->len );
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }
    return bubblestranger;
}

//Function gets Node and Graph and returns all Neighbors that share not its opinion

int_array * GetFalseFriends(Graph *G, int NodeIndex)
{
    int curr_opinion = G->NList[NodeIndex]->opinion;
    int_array * Neighbors = getNeighbors(G, NodeIndex);

    //return if no neighbors
    if (Neighbors == NULL){
        return NULL;
    }
    
    int_array * FalseFriends = (int_array *)malloc(sizeof(int_array));
    if (FalseFriends == NULL)
    {
        printf("Malloc problem\n");
        exit(1);
    }

    FalseFriends->data = (int *)malloc(Neighbors->len * sizeof(int));
    if (FalseFriends->data == NULL)
    {
        printf("Malloc problem\n");
        exit(1);
    }
    FalseFriends->len = 0;
    for (int i = 0; i < Neighbors->len; ++i)
    {
        int curr_neighbor_index = Neighbors->data[i];
        if (G->NList[curr_neighbor_index]->opinion != curr_opinion)
        {
            FalseFriends->data[FalseFriends->len] = curr_neighbor_index;
            FalseFriends->len += 1;
        }
    }

    free(Neighbors->data);
    Neighbors->data = NULL;
    free(Neighbors);
    Neighbors = NULL;

    if (FalseFriends->len == 0)
    {
        free(FalseFriends->data);
        free(FalseFriends);
        return NULL;
    }

    return FalseFriends;
}

void process1(Graph * G)
{

    //Pick random Node from Nodelist
    int NodeIndex = rand() %NNodes ;

    //Get False Friends
    int_array * FalseFriends = GetFalseFriends(G, NodeIndex);
    if (FalseFriends == NULL)
    {
        return;
    }

    int NeighborIndex = rand() %FalseFriends->len;
    int old_neighbor = FalseFriends->data[NeighborIndex];

    free(FalseFriends->data);
    FalseFriends = NULL;
    free(FalseFriends);
    FalseFriends = NULL;

    
    int_array * bubblepeople = getSameOpinion(G, NodeIndex);

    //Exclude lonely nodes:
    if (bubblepeople == NULL)
    {
        return;
    }
    if (bubblepeople->data == NULL)
    {
        free(bubblepeople);
        bubblepeople = NULL;
        return;
    }

    if (bubblepeople->len == 0)
    {
        free(bubblepeople->data);
        bubblepeople->data = NULL;
        free(bubblepeople);
        bubblepeople = NULL;
        return;
    }
    
    //Take ith person that is not yet connected to you but has same opinion
    int new_friend_Bubbleindex = rand () %bubblepeople->len;

    int new_friend_index = -1;
    if (bubblepeople->data[new_friend_Bubbleindex] > -1)
        {
            new_friend_index = bubblepeople->data[new_friend_Bubbleindex];
        }
    //Get index in NList of this person
    
    //Delete old neighbor
    G->Adj_Matrix[NodeIndex][old_neighbor] = 0;
    G->Adj_Matrix[old_neighbor][NodeIndex] = 0;

    //Create new neighbor
    G->Adj_Matrix[NodeIndex][new_friend_index] = 1;
    G->Adj_Matrix[new_friend_index][NodeIndex] = 1;

    
    free(bubblepeople->data);
    bubblepeople->data = NULL;
    free(bubblepeople);
    bubblepeople = NULL;


    return;
}

void process2(Graph * G)
{
    //Pick random Node from Nodelist
    int NodeIndex = rand() %NNodes ;
    Node * curr_Node = G->NList[NodeIndex];

    //Get its opinion
    int curr_opinion = curr_Node->opinion;

    //Get False Friends
    int_array * FalseFriends = GetFalseFriends(G, NodeIndex);
    if (FalseFriends == NULL)
    {
        return;
    }

    //Take random false friend who is not influencer
    int Neighbor = -1;
    while (Neighbor == -1)
    {
        //Get random false friend
        int ith_Neighbor = rand () %FalseFriends->len;
        Neighbor = FalseFriends->data[ith_Neighbor];

        //Check if it is an influencer
        if (G->NList[Neighbor]->isInfluencer == 1)
        {   
            //Set Neighbor in FalseFriendlist to -1
            printf("Influencer friend\n");
            FalseFriends->data[ith_Neighbor] = -1;
            //Iterate through false friends
            for (int i = 0; i < FalseFriends->len; ++i)
            {
                if (FalseFriends->data[i] != -1)
                {
                    continue;
                }
            }
        printf("All friends are influencers :O \n");
        return;
        }
        
    }
    

    //Change its opinion to my opinion
    (G->NList[Neighbor])->opinion = curr_opinion;

    free(FalseFriends->data);
    FalseFriends->data = NULL;
    free(FalseFriends);
    FalseFriends = NULL;
    return;
}


/*Think of a more efficient algorithm!!! */
int check_consensus(Graph * G)
{
    for (int i = 0; i < NNodes; i++)
    {
        int_array * curr_friends = getNeighbors(G, i);
        if (curr_friends == NULL)
        {
            continue;
        }

        
        int curr_opinion = (G->NList[i])->opinion;

        for (int j = 0; j < curr_friends->len; j++)
        {
            if ((G->NList[(curr_friends->data[j])])->opinion != curr_opinion)
                {
                    free(curr_friends->data);
                    curr_friends->data = NULL;
                    free(curr_friends);
                    curr_friends = NULL;
                    return 0;
                }    
        }

        free(curr_friends->data);
        curr_friends->data = NULL;
        free(curr_friends);
        curr_friends = NULL;
    }

    return 1;
}


int_array *check_consensus_fast(Graph * G)
{
    //printf("NEW CONSENSUS TEST\n");
    //Create array [0,0,..,0] Indicates for each node whether it is already in "its" consensus state
    int_array * Checklist = (int_array *) malloc(sizeof(int_array));
    if (Checklist==NULL)
    {
        printf("Malloc failed\n");
        exit(0);
        return NULL;
    }
    Checklist->data = calloc(NNodes, sizeof(int));
    if (Checklist->data == NULL)
    {
        printf("Malloc failed\n");
        free(Checklist);
        Checklist = NULL;
        exit(0);
        return NULL;
    }
    Checklist->len = NNodes;


    //Create "global variable" consensus which will be -1 during the process, 1 when finished in consensus state and 0 for abortion
     int * consensus = (int *) malloc(sizeof(int));
     if (consensus == NULL)
     {
         printf("Malloc failed\n");
         free(Checklist->data);
         Checklist->data = NULL;
         free(Checklist);
         Checklist = NULL;
         exit(0);
         return NULL;
     }
     * consensus = -1;
    
    //Prevent Stack Overflow, just a debug variable
     int * preventSO = (int *) malloc(sizeof(int));
     if (preventSO == NULL)
     {
         exit(0);
     }
     *preventSO = 0;

     //Prevent Stack Overflow, just a debug variable
     int * comp = (int *) malloc(sizeof(int));
     if (comp == NULL)
     {
         exit(0);
     }
     *comp = 0;

    //Create int that corresponds to the number of ordered nodes that are already checked
    int checked = 0;
    //iterate over components
    while(*consensus == -1)
        {
            *comp +=1;
            int j = 0;
            for (int i = 0; i < NNodes ; ++i)
            {
                if(Checklist->data[i] == 0)
                {
                    break;
                }
                j++;
            }
            checked = j;
            //printf("checked: %d \n", checked);

            // if array = [1,1,1..1] we are in consensus state !!
            if (checked == NNodes)
            {
                printf("CONSENSUS\n");
                *consensus = 1;
                break;
            }

            //get opinion of first unchecked node
            int op = G->NList[checked] -> opinion;


            //start recursion process to check connected component
            iter_neighbors(G, Checklist, checked, consensus, op, preventSO, comp);
        }

        if (*consensus != 1)
        {
            free(Checklist->data);
            Checklist->data = NULL;
            free(Checklist);
            Checklist = NULL; 
            free(consensus);
            consensus = NULL;
            return NULL;
        }

        else
        {
            free(consensus);
            consensus = NULL;
            return Checklist;
        }
    }




void iter_neighbors(Graph * G, int_array * Checklist, int NIndex, int * consensus, int op, int * preventSO, int * comp)
{
    //exit if process already finished 
    if (*consensus != -1)
    {
        return;
    }

    //Get Neighbors
    int_array * neighbors = getNeighbors(G, NIndex);

    if (neighbors == NULL)
    {
        Checklist->data[NIndex] = *comp;
        return;
    }

    //Check if NIndex - Node is in local consensus state
    for (int i = 0; i < neighbors->len; ++i)
    {
        int curr_neighbor = neighbors->data[i];
        //If a neighbor has a different opinion -> No consensus state!
        if (G->NList[curr_neighbor]->opinion != op) 
        {
            *consensus = 0;
            free(neighbors->data);
            neighbors->data = NULL;
            free(neighbors);
            neighbors = NULL;
            return;
        }
    }
    Checklist->data[NIndex] = *comp;
    //printf("Node on CList set to %d \n", Checklist->data[NIndex]);


    //Setze alle aktiven Nachbarn auf -1
    for (int i = 0; i < neighbors->len; ++i)
    {
        int curr_neighbor = neighbors->data[i];
        if(Checklist->data[curr_neighbor] != *comp)
        {
            Checklist->data[curr_neighbor] = -1;
        }
    }
    //free heap memory for neighbors
    free(neighbors->data);
    neighbors->data = NULL;
    free(neighbors);
    neighbors = NULL;

    

    for (int i = 0; i < NNodes; ++i)
    {
        if (Checklist->data[i] == -1 )
        {
            iter_neighbors(G, Checklist, i, consensus, op, preventSO, comp);
        }
        
    }

    
}


void export_data(Graph * G, int_array * Checklist, int iterations, char* fname, int_array * Influencer)
{
    char str[25];
    strcpy(str, "C:\Miniconda\envs\opinion\OpinionFormationCode\data");
    strcat(str, fname);
    strcat(str, ".txt");
    printf("Data written to %s\n", str);
    FILE *fp = fopen(str,"a");
/*Vier Zeilen:
1. Zeile Input Parameter
2. Zeile Adjazenzmatrix
3. Meinungsvektor
4. Checklist
*/
    //Print parameters
    fprintf(fp, "Nodes: %d, Edges: %d, phi: %.3f, Iterations: %d \n", NNodes, NEdges, phi, iterations);
    //print adj Matrix
    /*for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            fprintf(fp, "%d ", G->Adj_Matrix[i][j]);
        }
        fprintf(fp, ";");
    }
    fprintf(fp, "\n");*/

    //print Opinions

    for (int i = 0; i < NNodes; ++i)
    {
        fprintf(fp, "%d ", G->NList[i]->opinion);
    }
    fprintf(fp, "\n");

    //print checklist

    for (int i = 0; i < Checklist->len; ++i)
    {
        fprintf(fp, "%d ", Checklist->data[i]);
    }
    fprintf(fp, "\n");

    //print connectivity list of influencer
    update_degrees(G);
    if (Influencer != NULL)
    {
        fprintf(fp, "inf data:\n");
        for (int i = 0; i < Influencer->len; ++i)
            {
                int curr_deg = G->NList[(Influencer->data[i])]->degree;
                int curr_op = G->NList[Influencer->data[i]]->opinion;
                fprintf(fp, "%d : degree %d, opinion %d ", Influencer->data[i], curr_deg, curr_op);
            }    
        fprintf(fp, "\n");
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc != 11 || strlen(argv[7]) > 20)
    {
        printf("Call function with arguments: NNodes, NEdges, NOpinions, phi, MAX_ITER, ITER_Step, file name shorter than 20char, Manip Boolean, Number Influencer, Influencer Degree\n");
        exit(0);
    }
    NNodes = atoi(argv[1]);
    NEdges = atoi(argv[2]);
    NOpinions = atoi(argv[3]);
    phi = atof(argv[4]);
    MAX_ITER = atoi(argv[5]);
    int ITER_Step = atoi(argv[6]);
    char* fname = argv[7];
    int manip = atoi(argv[8]);
    int NInf = atoi(argv[9]);
    int InfD = atoi(argv[10]);
    int InfOp = 0;
    int max_Edges = (NNodes * (NNodes -1))/2;

    if (manip != 0 && manip != 1)
    {
        printf("Manip Boolean has to be 1 or 0\n");
        return 1;
    }
    if ( InfD >= NNodes)
    {
        printf("Too high Influencer Degree, choose -1 for max instead\n");
        return 1;

    }
    if (NOpinions>NNodes || NEdges > max_Edges || phi*100 > 100)
    {
        if (NOpinions>NNodes)
        {
            printf("NO > NN\n");
        }
        if (NEdges > 1/2*(NNodes - 1)*NNodes)
        {
            printf("NNodes: %d \n", NNodes);
            printf("NEdges = %d, but max_Edges = %d \n", NEdges, max_Edges);
            printf("Too many edges\n");
        }
        printf("Check input parameters!\n");
        return 1;
    }
    if (ITER_Step > MAX_ITER)
    {
        printf("Iteration steps larger than total number of iterations\n");
        printf("MAX_Iter: %d, Iter_step: %d \n", MAX_ITER, ITER_Step);
        return 1;
    }

    clock_t begin = clock();
    //For Random Numbers
    srand(time(NULL));

    Graph * G= create_graph();
    //check_graph(G);
    printf("Graph created \n");

    int_array * Influencer = NULL;
    if (manip == 1)
    {
        Influencer = Op_Manipulate(G, NInf, InfOp, InfD);
        printf("Influencer set\n");
    }

    if (Influencer != NULL)
    {
        for (int i = 0; i < Influencer->len; ++i)
        {
            Node * curr_infl = G->NList[Influencer->data[i]];
            printf("Inf: %d, deg %d op: %d \n", Influencer->data[i], curr_infl->degree, curr_infl->opinion );
        }
    }
    printf("Iteration starts\n");
    int iter;
    for (iter = 0; iter < MAX_ITER; iter++)
    {
        //printf("Consensus Check No %d\n", iter);
        int check = 0;
        if (iter%ITER_Step == 0)
        {
            check = check_consensus(G);
        }
        if (check == 1)
        {
            //printf("CONSENSUS STATE\n");
            
            int_array * Checklist = check_consensus_fast(G);
            if (Checklist != NULL)
            {
                printf("confirmed \n");
                export_data(G, Checklist, iter, fname, Influencer);
                free(Checklist->data);
                free(Checklist);
            }
            else
            {
                printf("CONSENSUS FATAL ERROR\n");
                exit(0);
            }
            break;
        }
        if ( rand() %101 < phi*100 )
        {
            process1(G);
            //printf("1 \n");
        }
        else
        {
            //printf("process2!!!\n");
            process2(G);
            //printf("2 \n");
        }
        

    }
    printf("Finished after %d iterations \n", iter);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("computation time: %e \n", time_spent);

    for (int i = 0; i < NNodes; ++i)
    {
        Node * curr_node = G->NList[i];
        free(curr_node);
        curr_node = NULL;
    }

    //free(G->NList);
    if (Influencer != NULL)
    {
        free(Influencer->data);
        free(Influencer);
    }
    free(G);
    G = NULL;
    
    return 0;
}
