#ifndef ANT_COLONY_H
#define ANT_COLONY_H
#include <vector>
#include <utility>
#include <random>
#include "time.h"
#include "Tour.h"
#include "CandidateLists.h"
// TODO: Write macros to convert 1D into 2D coordinates
// TODO: Hold all arrays as 1D arrays instead of 2D

struct Coordinate
{
    Coordinate() = default;
    Coordinate(float X, float Y) : x(X), y(Y){};
    float x;
    float y;
};

class AntColony
{
public:
    AntColony() = delete;
    // Initial implementation will have a single constructor that builds a sysmetric TSP fully connected graph given points with distances
    // Using the euclidian 2D distance to calculate distances.
    // TODO: Add more constructors that except different formats, and that support asymmetrical graphs
    // TODO: Support passing in a distance/cost function provided by the user and given two coordinates - implemented as abstract class? overload operator?
    // TODO: support fully built distance matrixes for custom graphs.
    AntColony(const std::vector<std::pair<float, float>> &symmetricCoordinates);
    ~AntColony();
    AntColony &optimize(int iterations = 100, int numberOfAnts = 10, float rho = 0.1, float beta = 2, float decay = 0.1, float qExplorationFactor = 0.9);
    AntColony &setParameters(int iterations, int numberOfAnts, float rho, float beta, float decay, float qExplorationFactor);
    long getShortestTourLength();
    Tour* getShortestTour(); 
    // void printCurrentState();
    // void printDistanceMatrix();
    // void printPheromones();

private:
    // Data Storage
    float **pheromones;          // Node x Node edge matrix that holds the global pheromone value
    std::vector<Tour*> tours;
    CandidateLists* candidateLists; 
    Coordinate * nodeCoordinates;                // 
    int numberOfNodes;
    Tour* bestTour;
    int numberOfAnts;
    int iterations;
    // Parameters

    float rho;   // this is parameter that adjusts the ratio of the update for the local update
    float decay; // This is the decay of the pheromones - sometimes called alpha
    int bestTour_index;
    long bestTour_length;
    int beta;                // The power to raise the distance heuristic which impacts it's value.
    float initialPheromones; // N * (1 / nearestNeighborlength)
    float q0;                // Exploration factor when choosing which nodes to explore
    std::mt19937 generator;

    void reset();
    void initDistanceMatrix(unsigned int ** distances);
    void nearistNeighbor(int node);
    int shortestDistance(int node);

    bool findAntTour(int ant);

    void updateLocalPheromone(int nodeA, int nodeB); // We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheremoneLevelOfedge

    void chooseNextNode(Tour * tour, int currentNode, Node & n); // Takes in the current city returns the choice for where to move to next // State Transition Rule
    void maxPheromoneChoice(Tour * tour, int currentNode, Node & best);
    // Node biasedExplorationChoice(int ant, int currentNode); // This is the biased Exploration from the original Ant System algorithm, use when random is below a certian threshold
    // float probabilityCalculation(int nodeA, int nodeB, unsigned int d);

    void updateGlobalPheromones();

    void findBestTour();
    float deltaPheromones(int nodeA, int nodeB);

    int distance(int nodeA, int nodeB);
    float distanceHeuristic(unsigned int d);
    // bool exists(int nodeA, int nodeB); // Depracted until I add more support for different types of graphs
    // void resetTour(int ant);
};

#endif