#ifndef ANT_COLONY_H
#define ANT_COLONY_H
#include <vector>
#include <utility>

// TODO: Write macros to convert 1D into 2D coordinates
// TODO: Hold all arrays as 1D arrays instead of 2D

struct Coordinate {
    float x;
    float y;
}; 

class AntColony 
{
private:
    // Data Storage
    int ** distanceMatrix; // Node x Node  edge matrix that gives edge distance from city1->city2 in [city1][city2] and city2->city1 with [city2][city1]
    float ** pheromoneMatrix; // Node x Node edge matrix that holds the global pheromone values
    bool ** graph; // determines which neighbors are valid
    Coordinate * nodeCoordinates; // Nodes with their coordinates for calculating distances
    int ** tours; // Ant X Node matrix, holding each Ant's current Tour.
    int numberOfNodes;

    // Parameters
    
    float rho; // this is parameter that adjusts the ratio of the update for the local update
    float decay; // This is the decay of the pheromones - sometimes called alpha
    int bestTour_index;
    long bestTour_length;
    int beta; //The power to raise the distance heuristic which impacts it's value.
    float initialPheromones; // N * (1 / nearestNeighborlength)


    void init(); // Initilize all memory and starting values
    void initDistanceMatrix();
    int distance(int nodeA, int nodeB);
    long length(int ant);
    bool exists(int nodeA, int nodeB);
    bool visited(int ant, int node);

    void updateLocalPheromone(int nodeA, int nodeB); // We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheremoneLevelOfedge
    int chooseNextNode(int currentNode); // Takes in the current city returns the choice for where to move to next // State Transition Rule
    int biasedExplorationChoice(int currentNode); // This is the biased Exploration from the original Ant System algorithm, use when random is below a certian threshold
    void updateGlobalPheromones();
    void findTour(int ant);
    
public:
    AntColony() = delete;
    // Initial implementation will have a single constructor that builds a sysmetric TSP fully connected graph given points with distances
    // Using the euclidian 2D distance to calculate distances.
    // TODO: Add more constructors that except different formats, and that support asymetrical graphs
    // TODO: Support passing in a distance/cost function provided by the user and given two coordinates - implemented as abstract class? overload operator?
    // TODO: support fully built distance matrixes for custom graphs.
    AntColony(std::vector<std::pair<float,float>> SymetricCoordinates, int numberOfNodes, float rho, float decay, int beta);
    int optimize(); 
    long getShortestTourLength();
    int* getShortestTour(); // Returns a copy of the shortest tour by node index - Maybe make into better format latter

}


#endif