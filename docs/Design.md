# Algorithm
Ant colony Optimization with 3 opt added based on this [paper](http://people.idsia.ch/~luca/acs-ec97.pdf). 


## ACO Algirthm Basic
    Initilize Distance Matrix
    Initilize Nearest Neighbor based pheramone
        For number of Iterations:
            Each ant randomly positions at a starting Node
            For each ant:
                Each at takes a step based on state transisiton rule and does a local update to the pheromones
                - State Tranision rule: max(pheramone[node1][node2] * (1/distance[node1][node2])^b) or old AS biased exploration. Depends on random roll
                - Update pheromones: 


### ACO Algorithm with 3-opt
    Initilization    
    For number of iterations:
        Each ant randomly positioned at a starting node 
        For each ant:  O(N*Branching Factor(often just N))
            Each ant builds a solution
            Applies local update at each node
            Chooses next node based on heuristic / random choice. Time complexity, given node N, iterates over the 15 closest first, if they are all visited then looks outside this set of 15. If random move is selected use original ACS biased exploration method.
        Wait for ant's to finish building solutions
        For each ant's tour:
            Apply 3 opt optimization O(nodes^3) time complexity
        Choose best Ant's tour, update the global pheromones. O(Edges(~Node^2)) time

### 3 Opt


    use best so we can parallelize in CUDA http://tsp-basics.blogspot.com/2017/03/3-opt-iterative-basic-algorithm.html
    Use restrictive three-opt for async support and reduce computations
    
       
## Initialization
    Build Distance Matrix: O(N^2) (N * N possible combination of edges)
    Build Candidate Lists: O(N^2) (N sorts over N nodes)
    Apply Nearest Neighbor for pheremone update O(N)

## Important Notes
- Keep a candidate list for each node that represents the closest neighbors. Store as integer indexes for the given node, with index 0 of the list representing the best node. Size of candidate list could depend on graph size or is fixed
- Initilization of pheromones are based on a nearest neighbor tour
- Keep a Distance matrix for distance calculations between any two nodes, this can be achieved with a NxN matrix
- Define negative max(Int) value as invalid edge instead of using a different matrix - this will reduce our memory usage for representing the graph for large problems. Distances will be stored as integers as these alogrithms typically require it.
- Paralizing Ant's will need to guard agianst race conditions when ants happen to be at the same node at the same time. Care will have to be take to only require this when it is in issue to prevent over-syncing. Collisions between ants on a graph should be fairly rare if it is large enough and the number of ant's is small enough. Use a critical section will solve this, the key will be to find a way to limit the need for critical when it is unnessicary. 
- By applying a candidate list of the shortest nearest nieghbors - Only keep the distances of the nearist ~20 neighbors, reduces memory foot print significantly. N x 20, where it's [node][ordered_list_of_structs] and at each node is a list of structs with a distance and the Node's id. This means we use N x 40 Memory.
-Compute the distance on the fly for values outside those 20, as the algorithm will in general, not select them - or in the ase of 3-opt not use them at all. 
-Initial implementation for ease: NxN matrix of chars represening the graph: 1 for connected, 0 for not
- (Fututre improvements)
- For values outside these 15, we store the valid neighbors as 1D array, where the second dimnesion is along the bit axis. We store a bit pattern that has 1 at every index that is a valid neighbor. When searching for next neighbor to visit, if force outside of top 15, we use a bit mask to determine if the bit is turned on, if it is we compute the relative index, and check that index against the current ant's tour. If it is a valid neighbor and 

- Tour's have both the node's and the distances of associated steps stored and the current length stored at the very back. Orginized into a Class.

- **Make Class called Graph that has optimized performance and exposes methods to AntColony, so I can change the graph implementation without changing the AntColony Implementation too much**
### Memory Usage Anaylsis:
Each Ant needs a unique Tour. N x Ants 
Pheromone Matrix: N x N
Candidate List: C_ListSize x N
Total: 2 * N x N + N x C_listSize + N x Ants
Roughly 2*NxN:  
At 100,000 nodes it will take about ~40 GB of memory
At 50,000 nodes it will take about 10-11 GB of memory, small enough to fit into most mondern systems main memory, but hard to fit into GPUs 

### Time Complexity
    O(N^2) + O(N^2) + O(N) + O(N^2) + O(Ants * N^3)


## Paralization Opportunities
-Low Hanging Fruit: Each ant being assigned a Thread - both in the main run(ant updates will need a atomic guard) and in the 3opt optimization. All of the init calculations, 
-Harder: CUDA calculations of the Pheremone Global Update, CUDA speed up of the 3-opt local search solutions.