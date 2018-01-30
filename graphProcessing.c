/* The following structure is used:
*   There is a Graph struct which consists of a List of Node Pointers and an int Adjacency Matrix
*   Nodes are structs that contain one opinion (at the moment)
*   The adjacency matrix contains all information about connectivity
*   So far, it is always symmetrized. Later one could limit the calculations to one half to save memory/processing power
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NNodes 6400
#define NEdges 3200
#define NOpinions 320
#define phi100 44
#define MAX_ITER 370000

typedef struct Graph{
    struct Node* NList[NNodes];
    int Adj_Matrix[NNodes][NNodes];
} Graph;


typedef struct Node{
    int opinion;
} Node;

typedef struct int_array{
    int len;
    int * data;
} int_array;



Graph * create_graph()
{
    Graph *G = (Graph *) malloc(sizeof(Graph));
    if (G == NULL)
    {   
        printf("malloc of Graph failed \n");
        return NULL;
    }
    // G->NList = malloc(NNodes * sizeof(Node*));
    // if (G->NList == NULL)
    // {
    //     printf("malloc of NList of Graph failen \n");
    //     free(G->NList);
    //     free(G);
    //     return NULL;
    // }
   /* For creating Random edges we use the following procedure: 
    * We have in general max_Edges = 1/2 * NNodes * (NNodes - 1) possible edges while only NEdges allowed
    * We iterize over all node pairs and connect them with probability prob_Edge
    * prob_Edge = (NEdges - Curr_NEdges)/(max_Edges - Curr_NEdges)
    */
    int Curr_NEdges = 0;
    int max_Edges = (NNodes * (NNodes -1))/2;
    int k=0;
    for(int i=0; i<NNodes; i++)
    {
        for(int j=0; j<i; j++)
        {
            //prob is Number of remaining edges to initialize / still possible edges
            k+=1;
            int remaining_Edges = max_Edges - k;
            float prob_Edge = (float) (NEdges - Curr_NEdges) / (float) (remaining_Edges);
            int rnd = rand() %(100);
            //printf("rnd: %d , prob: %.2f \n", rnd, prob_Edge);
            if(rnd < (prob_Edge*100))
            {
               G->Adj_Matrix[i][j] = 1; 
               Curr_NEdges +=1 ;
            }
            else
            {
                G->Adj_Matrix[i][j] = 0;
            }
            G->Adj_Matrix[j][i] = G->Adj_Matrix[i][j];
            
        }
        G->Adj_Matrix[i][i] = 0;
    }

    //printf("Number of Edges %d\n", Curr_NEdges);
    //printf("Number of MAXEDGE %d\n", max_Edges);

    for(int i=0; i<NNodes; i++)
    {  
        Node *new_node = (Node *)malloc(sizeof(Node));
        if (new_node ==NULL)
        {
            free(new_node);
            printf("Allocation for new_node failed\n");
            return NULL;
        }
        new_node -> opinion = rand() %NOpinions;
        G->NList[i] = new_node;
    }
    return G;
}

void check_graph(Graph * G)
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
}

/*needs Graph and Index of a node
 * ouput: List of Indices that correspond to the neighbor nodes of the input node
 */

int_array * getNeighbors(Graph * G, int NodeIndex)
{
    int_array * Neighborlist = (int_array *) malloc(sizeof(int_array));

    if (Neighborlist == NULL)
    {
        printf("Allocation of Nehborlist failed \n");
        free(Neighborlist);
        return NULL;
    }

    Neighborlist->data = (int *) malloc(NNodes * sizeof(int));
    if (Neighborlist->data == NULL)
    {
        printf("Allocation of Neighborlist failed \n");
        free(Neighborlist);
        return NULL;
    }

    Neighborlist->len = 0;

    for (int i=0; i<NNodes; i++)
    {
        if (G->Adj_Matrix[NodeIndex][i] == 1)
        {
            Neighborlist->data[Neighborlist->len]=i;
            Neighborlist->len +=1; 
        }
    }

    Neighborlist->data = realloc(Neighborlist->data, Neighborlist->len * sizeof(int));
    if (Neighborlist == NULL) {
        printf("Reallocation of Neighborlist failed \n");
        free(Neighborlist);
        return NULL;
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
    free(MyFriends);

    //return NULL pointer if no stranger shares my opinion 
    if (bubblestranger->len == 0)
    {
        free(bubblestranger);
        return NULL;
    }

    //Reallocate memory for bubblestranger
    bubblestranger->data = realloc(bubblestranger->data, bubblestranger->len * sizeof(int));
    if (bubblestranger->data == NULL)
    {
        printf("Reallocation of bubblestranger data failed\n");
        printf("No space for %d integers \n", bubblestranger->len );
        return NULL;
    }
    return bubblestranger;
}


void process1(Graph * G)
{

    //Pick random Node from Nodelist
    
    int NodeIndex = rand() %NNodes ;
    //Node * curr_Node = &(G->NList[NodeIndex]);

    //Get its opinion
    //int curr_opinion = curr_Node->opinion;
    
    //Get Neighbors
    //If no neighbors return
    int_array * Neighbors = getNeighbors(G, NodeIndex);
    if (Neighbors->len == 0){
        free(Neighbors->data);
        free(Neighbors);
        return;
    }
    
    //PickRandom Neighbor
    int NeighborIndex = rand() %Neighbors->len;
    int old_neighbor = Neighbors->data[NeighborIndex];
    
    int_array * bubblepeople = getSameOpinion(G, NodeIndex);

    //Exclude lonely nodes:
    if (bubblepeople == NULL || bubblepeople->len == 0 ||bubblepeople->data == NULL)
    {
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
    if (new_friend_index>NNodes || new_friend_index < 0)
    {
        printf("ALLAAAAARM \n");
    }
    G->Adj_Matrix[NodeIndex][new_friend_index] = 1;
    G->Adj_Matrix[new_friend_index][NodeIndex] = 1;

    free(bubblepeople->data);
    free(bubblepeople);
    free(Neighbors->data);
    free(Neighbors);
    //Is whole struct now freed??

    return;
}

void process2(Graph * G)
{
    //Pick random Node from Nodelist
    
    int NodeIndex = rand() %NNodes ;
    Node * curr_Node = G->NList[NodeIndex];

    //Get its opinion
    int curr_opinion = curr_Node->opinion;

    int_array * Neighbors = getNeighbors(G, NodeIndex);
    if (Neighbors->len == 0){
        free(Neighbors->data);
        free(Neighbors);
        return;
    }
    int ith_Neighbor = rand ()%Neighbors->len;
    int old_friend_index = Neighbors->data[ith_Neighbor];
    (G->NList[old_friend_index])->opinion = curr_opinion;

    free(Neighbors->data);
    free(Neighbors);
    return;
}


/*Think of a more efficient algorithm!!! */
int check_consensus(Graph * G)
{
    for (int i = 0; i < NNodes; i++)
    {
        int_array * curr_friends = getNeighbors(G, i);

        
        int curr_opinion = (G->NList[i])->opinion;

        for (int j = 0; j < curr_friends->len; j++)
        {
            if ((G->NList[(curr_friends->data[j])])->opinion != curr_opinion)
                {
                    free(curr_friends->data);
                    free(curr_friends);
                    return 0;
                }    
        }

        free(curr_friends->data);
        free(curr_friends);
    }

    return 1;
}


/* Check Consensus state using adjacency matrix
* does not really need additional memory
*/
int check_consensus2(Graph * G)
{
    //for each row of adjacency matrix
    for (int i = 0; i < NNodes; ++i)
    {
        //set opinion of this line to -1 
        int lineopinion = G->NList[i]->opinion;

        //for every column (only half diagonal since symmetric matrix)
        for (int j = 0; j < i; ++j)
        {
            //if node i and j are connected
            if (G->Adj_Matrix[i][j] == 1)
            {
                
                //check if all connected nodes have same opinion
                if (G->NList[j]->opinion != lineopinion)
                {
                    //return as soon as possible
                    return 0;
                }
            }
        }
    }
    return 1;
}


int comp(const void * elem1, const void * elem2)
{
    int f =*((int *)elem1);
    int s = *((int *)elem2);
    if(f > s) return 1;
    if(f < s) return -1;
    return 0;
}

void connected_components(Graph * G)
{
    int first = 0;
    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (G->Adj_Matrix[i][j]==1)
            {
                int comp_opinion = G->NList[i]->opinion;
                first = 1;
                int NIndex = i;
                break;
            }
        }
        if (first == 1)
        {
            break;
        }
    }

    int_array * component = (int_array *) malloc(sizeof(int_array));
    if (component == NULL)
    {
        printf("Allocation of component failed\n");
        return NULL;
    }
    component->data = malloc(NNodes * sizeof(int))
    if (component->data == NULL)
    {
        free(component);
        return NULL;
    }
    component->len = 0;


    int_array * new_part = getNeighbors(G, NIndex);
    for (int i = 0; i < new_part->len; ++i)
    {
        component->data[component->len + i] = new_part->data[i];
    }
    component->len += new_part->len;
    

}

void add_comp(Graph * G)
{
    int_array * component = (int_array *) malloc(sizeof(int_array));
    if (component == NULL)
    {
        printf("Allocation of component failed\n");
        return NULL;
    }
    component->data = malloc(NNodes * sizeof(int))
    if (component->data == NULL)
    {
        free(component);
        return NULL;
    }
}

// void connected_components(Graph * G)
// {
//     int_array * component = malloc(sizeof(int_array));
//     if (component == NULL)
//     {
//         printf("Allocation of component failed\n");
//         return NULL;
//     }

//     component->data = malloc(NNodes*sizeof(int));
//     if (component->data == NULL)
//     {
//         printf("Allocation of component data failed \n");
//         free(component);
//         return NULL;
//     }
// }

// void add_comp(Graph * G, int_array * component, int_array * noncomp)
// {
//     for (int i = 0; i < noncomp->len; ++i)
//     {
//         add_comp(G, component, getNeighbors(G, i))
//     }
// }





// Gets two int_arrays of node indices and returns one int_array of nodes that appear in at least one of them

int_array * mergesort_int_array(int_array * list1, int_array * list2)
{
    int_array * merged_list = malloc(sizeof(int_array));
    if (merged_list == NULL)
    {
        printf("Allocation of merged_list failed\n");
        return NULL;
    }

    merged_list->data = malloc((list1->len + list2->len) *sizeof(int));
    if (merged_list->data == NULL)
    {
        printf("Allocation of merged_list data failed \n");
        free(merged_list);
        return NULL;
    }

    int i = 0;
    int j = 0;

    while(i < list1->len || j < list2->len)
    {
        if (list1->data[i] < list2->data[j])
        {
            merged_list->data[len] = list1->data[i];
            i++;
        }

        else if (list1->data[i] < list2->data[j])
        {
            merged_list->data[len] = list2->data[j];
            j++;
        }
        else
        {
            merged_list->data[len] = list1->data[i];
            i++;
            j++;
        }
        merged_list->len += 1;
        if (i == list1->len)
        {
            /* code */
        }
    }
    free(list1);
    free(list2);
    return merged_list;
}

int main()
{
    clock_t begin = clock();
    //For Random Numbers
    srand(time(NULL));

    Graph * G= create_graph();
    check_graph(G);


    // for (int i=0; i<NNodes; i++)
    // {
    //     for (int j=0; j<NNodes; j++)
    //     {
    //     printf("%d", G->Adj_Matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    int iter;
    for (iter = 0; iter < MAX_ITER; iter++)
    {
        // int check = check_consensus2(G);
        // if (check == 1)
        // {
        //     printf("CONSENSUS STATE\n");
        //     
        //     int check2 = check_consensus(G);
        //     if (check2 != 1)
        //     {
        //         printf("CONSENSUS ERROR!!!!!!\n");
        //     }
        //     break;
        // }
        if ( rand() %100 < phi100 )
        {
            process1(G);
        }
        else
        {
            process2(G);
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
    }

    //free(G->NList);
    free(G);
    
    return 0;
}
