#include "AntColony.h"
#include <cmath>
#include <random>
#include <vector>
#include <limits>
#include "utility.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include "Tour.h"
#include "CandidateLists.h"
#include <omp.h>
using namespace std;
constexpr float minimum_p = numeric_limits<float>::epsilon();

// Constructor assumes fully connected sysmetric graph give by a list of coordinates that distances can be calulated from
AntColony::AntColony(const std::vector<std::pair<float,float>> &symmetricCoordinates) : numberOfNodes(symmetricCoordinates.size())
{
    generator.seed(chrono::high_resolution_clock::now().time_since_epoch().count() / getpid());
    numberOfAnts = 10;
    unsigned int **distances = new_2D_array<unsigned int>(numberOfNodes, numberOfNodes);
    pheromones = new_2D_array<float>(numberOfNodes, numberOfNodes);


    nodeCoordinates = new Coordinate[numberOfNodes];
    for(size_t i = 0; i < symmetricCoordinates.size(); i++){
        nodeCoordinates[i].x = symmetricCoordinates[i].first;
        nodeCoordinates[i].y = symmetricCoordinates[i].second;
    }
    initDistanceMatrix(distances);
    candidateLists = new CandidateLists(numberOfNodes, distances);
    delete_2D_array<unsigned int>(numberOfNodes, numberOfNodes, distances);
}

AntColony::~AntColony(){
    delete_2D_array<float>(numberOfNodes, numberOfNodes, pheromones);
    delete[] nodeCoordinates;
}



AntColony& AntColony::optimize(int iterations , int numberOfAnts, float rho , float beta , float decay , float qExplorationFactor)
{
    setParameters(iterations, numberOfAnts, rho, beta, decay, qExplorationFactor);
    reset();
    
    for (int i = 0; i < iterations; i++)
    {
        // Send out ants
        
        #pragma omp parallel for
        for (int a = 0; a < numberOfAnts; a++)
        {
            // while (!findAntTour(a));
            findAntTour(a);
        }
    
        findBestTour();
        updateGlobalPheromones();
    }
    return *this;
}
AntColony& AntColony::setParameters(int iterations, int numberOfAnts, float rho, float beta, float decay, float qExplorationFactor)
{
        this->iterations = iterations;
        if(this->numberOfAnts != numberOfAnts){
        //delete_2D_array(this->numberOfAnts, numberOfNodes + 1, tours);
        this->numberOfAnts = numberOfAnts;
        }
        this->rho = rho;
        this->beta = beta;
        this->decay = decay;
        this->q0 = qExplorationFactor;
        return *this;
}

void AntColony::reset(){
    for(int i = tours.size(); i < numberOfAnts; i++) {
        tours.push_back(new Tour(numberOfNodes, candidateLists));
    }
    
    uniform_int_distribution<int> random_value(0, numberOfNodes - 1);

    nearistNeighbor(random_value(generator));

    bestTour_index = 0;
    bestTour_length = tours[0]->getTotalTourDistance();
    tours[0]->reset();
    
    cout << "First best tour: " << bestTour_length << endl;
    initialPheromones = 1 / ((float)bestTour_length * numberOfNodes);
    
    for(int i = 0; i < numberOfNodes; i++){
        for(int j = i; j < numberOfNodes; j++){
                    pheromones[i][j] = initialPheromones;
                    pheromones[j][i] = initialPheromones;
        }
    }
}

void AntColony::nearistNeighbor(int node){
    Tour* t = tours[0];
    t->add(node, 0);
    for(int i = 1; i < numberOfNodes; i++){
        Node n;
        t->nextUnvisitedNode(n); // When we no longer have fully connected symetrical graphs, this will need to be an if
        t->add(n);
    }
}

long AntColony::getShortestTourLength()
{
    return bestTour_length;
}

Tour* AntColony::getShortestTour()
{
    return bestTour;
}

bool AntColony::findAntTour(int ant)
{
    Tour* tour = tours[ant];
    tour->reset();
    
    uniform_int_distribution<int> random_value(0, numberOfNodes - 1);
    
    tour->add(random_value(generator), 0);
    for (int i = 1; i < numberOfNodes; i++)
    {
        int node = tour->getNode(i-1);
        Node choice;
        chooseNextNode(tour, node, choice);
        tour->add(choice);
        #pragma omp critical
        {
            updateLocalPheromone(node,choice.nodeId);
        }
    }
    updateLocalPheromone(tour->getNode(numberOfNodes-1),tour->getNode(numberOfNodes));
    return true;
}

// Takes in the current city returns the choice for where to move to next // State Transition Rule
void AntColony::chooseNextNode(Tour * tour, int currentNode, Node & n)
{
    std::uniform_real_distribution<float> random_value(0.0, 1.0);
    float q = random_value(generator);
    
    if (q < q0)
    {
        maxPheromoneChoice(tour, currentNode, n);
    }
    tour->nextUnvisitedNode(n); // When we no longer have fully connected symetrical graphs, this will need to be an if
}

void AntColony::maxPheromoneChoice(Tour * tour, int currentNode, Node & best)
{
    float max = numeric_limits<float>::min();
    float current;
    Node n;
    tour->nextUnvisitedNode(n);
    unsigned int elementsLeft = candidateLists->getNumberLeftInTier(tour->getCurrentSearchTier(), currentNode);
    for(unsigned int i = 1; i < elementsLeft; i++)
    {
        current = pheromones[currentNode][n.nodeId] * distanceHeuristic(n.distance);
        if (current > max)
        {
            best = n;
            max = current;
        }
        tour->nextUnvisitedNode(n);
    }
}

inline float AntColony::distanceHeuristic(unsigned int d)
{
    return pow(1 / ((float) d), beta);
}

//** The Ant Colony System removed the biased exploration after implementing 3opt.
//** It instead chooses the closest neighbor when biased Exploration was the choice
//** 
// Node AntColony::biasedExplorationChoice(Tour * tour, int currentNode)
// {
//     float sum = 0;
//     uniform_real_distribution<float> random_value(0.0, 1.0);
//     float threshold = random_value(generator);
//     Node n;
//     while(tour->nextUnvisitedNode(n))
//     {
//         sum += probabilityCalculation(currentNode, n.nodeId, n.distance);

//         if (sum > threshold)
//             return n;
//     }
//     return n;
// }

// float AntColony::probabilityCalculation(int nodeA, int nodeB, unsigned int d)
// {
//     float sum =  0;
//     for (int n = 0; n < numberOfNodes; n++)
//     {
//         if (exists(nodeA, n) && !visited(ant, n))
//         {
//             sum += pheromones[nodeA][n] * distanceHeuristic(d);
//         }
//     }
//     return (pheromones[nodeA][nodeB] * distanceHeuristic(d)) / sum;
// }

// We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheromoneLevel
void AntColony::updateLocalPheromone(int nodeA, int nodeB)
{
    pheromones[nodeA][nodeB] = max(minimum_p, (1 - rho) * pheromones[nodeA][nodeB] + rho * initialPheromones);
    pheromones[nodeB][nodeA] = pheromones[nodeA][nodeB]; // Change this if not symetrical problems
}

void AntColony::updateGlobalPheromones()
{
    #pragma omp parallel for
    for (int i = 0; i < numberOfNodes; i++)
    {
        for (int j = i; j < numberOfNodes; j++)
        {
            if(i == j) continue;
            pheromones[i][j] = (1 - decay) * pheromones[i][j] + decay * deltaPheromones(i, j);
            pheromones[j][i] = pheromones[i][j];
        }
    }
}

void AntColony::findBestTour()
{
    long bestlength = tours[0]->getTotalTourDistance();
    int bestAnt = 0;
    for (int ant = 1; ant < numberOfAnts; ant++)
    {
        long l = tours[ant]->getTotalTourDistance();
        if (l < bestlength)
        {
            bestlength = l;
            bestAnt = ant;
        }
    }
    bestTour_length = bestlength;
    bestTour_index = bestAnt;
    bestTour = tours[bestAnt];
}

float AntColony::deltaPheromones(int nodeA, int nodeB)
{
    if (bestTour->edgeInTour(nodeA,nodeB))
    {
        return 1 / ((float)bestTour_length);
    }
    return 0;
}

// Use Euclidian 2D to start
int AntColony::distance(int nodeA, int nodeB)
{
    Coordinate A = nodeCoordinates[nodeA];
    Coordinate B = nodeCoordinates[nodeB];
    return floor(sqrt(pow((A.x - B.x), 2) + pow(A.y - B.y, 2)));
}

// Currently assumes symmetrical distances
void AntColony::initDistanceMatrix(unsigned int ** distances){
    #pragma omp parallel for
    for(int i = 0; i < numberOfNodes; i++){
        for(int j = i; j < numberOfNodes; j++){
            distances[i][j] = distance(i,j);
            distances[j][i] = distances[i][j];
        }
    }
}


// void AntColony::printCurrentState(){
//     cout << "rho: " << rho << ", decay: " << decay << ", numberOfAnts: " << numberOfAnts << ", Number of Nodes: " << numberOfNodes << ", iterations: "
//         << iterations << ", beta: " << beta << ", q0: " << q0  << " , initialPheromones: " << initialPheromones << "\n";
    
//     cout << "Best Tour length: " << bestTour_length << "\n";
//     cout << "Tour: ";
//     for(int i = 0; i < numberOfNodes; i++){
//         cout << tours[bestTour_index][i] << " -> ";
//     }
//      cout << tours[bestTour_index][numberOfNodes];
//     cout << endl;
// }

// void AntColony::printDistanceMatrix(){
//     cout << "Distance Matrix" << "\n";
//     for(int j = 0; j < numberOfNodes; j++){
//             cout << j << " ";
//     }   
//     cout << "\n";
//     for(int i = 0; i < numberOfNodes; i++){
//         cout << i << ": ";
//         for(int j = 0; j < numberOfNodes; j++){
//             cout << distances[i][j] << " ";
//     }   
//         cout << "\n";
//     }
//     cout << endl;
// }

// void AntColony::printPheromones(){
//     cout << setprecision(2);
//     for(int i = 0; i < numberOfNodes; i++){
//         cout << i << ": ";
//         for(int j = 0; j < numberOfNodes; j++){
//             cout << pheromones[i][j] << " ";
//     }   
//         cout << "\n";
//     }
//     cout << endl;
// }


