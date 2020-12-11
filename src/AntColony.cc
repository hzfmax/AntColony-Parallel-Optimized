#include "AntColony.h"
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
#include "ThreeOpt.h"
#include <utility>
using namespace std;
constexpr float minimum_p = numeric_limits<float>::epsilon();

// Constructor assumes fully connected symmetric graph give by a list of coordinates that distances can be calculated from
AntColony::AntColony(const std::vector<std::pair<float,float>> &symmetricCoordinates, unsigned int candidateListSize) : numberOfNodes(symmetricCoordinates.size())
{

    generator.seed(chrono::high_resolution_clock::now().time_since_epoch().count() / getpid());
    numberOfAnts = 10;
    pheromones = new_2D_array<float>(numberOfNodes, numberOfNodes);
    candidateLists = new CandidateLists(numberOfNodes, candidateListSize, symmetricCoordinates);
}

AntColony::~AntColony(){
    delete_2D_array<float>(numberOfNodes, numberOfNodes, pheromones);
    delete candidateLists;
    for(Tour * t : tours){
        delete t;
    }
}



AntColony& AntColony::optimize(int iterations , int numberOfAnts, float rho , float beta , float decay , float qExplorationFactor,  bool biasedExploration)
{
    setParameters(iterations, numberOfAnts, rho, beta, decay, qExplorationFactor, biasedExploration);
    reset();
    ThreeOpt opt(tours[0], candidateLists);
    
    for (int i = 0; i < iterations; i++)
    {
        // Send out ants
        
        #pragma omp parallel for
        for (int a = 0; a < numberOfAnts; a++)
        {
            findAntTour(a);
            ThreeOpt opt(tours[a], candidateLists);
            opt.optimize();
        }
    
        findBestTour();
        updateGlobalPheromones();
    }
    return *this;
}
AntColony& AntColony::setParameters(int iterations, int numberOfAnts, float rho, float beta, float decay, float qExplorationFactor, bool biasedExploration)
{
        this->iterations = iterations;
        if(this->numberOfAnts != numberOfAnts){
        this->numberOfAnts = numberOfAnts;
        }
        this->rho = rho;
        this->beta = beta;
        this->decay = decay;
        this->q0 = qExplorationFactor;
        this->biasedExploration = biasedExploration;
        return *this;
}

void AntColony::reset(){
    for(std::size_t i = 0; i < tours.size(); i++){
        tours[i]->reset();
    }
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
    
    for(unsigned int i = 0; i < numberOfNodes; i++){
        for(unsigned int j = i; j < numberOfNodes; j++){
                    pheromones[i][j] = initialPheromones;
                    pheromones[j][i] = initialPheromones;
        }
    }
}

void AntColony::nearistNeighbor(int node){
    Tour* t = tours[0];
    t->add(node);
    for(unsigned int i = 1; i < numberOfNodes; i++){
        t->add(t->nextUnvisitedNode());
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
    
    tour->add(random_value(generator));

    for (unsigned int i = 1; i < numberOfNodes; i++)
    {
        unsigned int node = tour->getNode(i-1);
        tour->add(chooseNextNode(tour, node));

        #pragma omp critical
        {
            updateLocalPheromone(node,tour->getNode(i));
        }
    }
    #pragma omp critical
        {
            updateLocalPheromone(tour->getNode(numberOfNodes-1),tour->getNode(0));
        }

    return true;
}

// Takes in the current city returns the choice for where to move to next // State Transition Rule
unsigned int AntColony::chooseNextNode(Tour * tour, unsigned int currentNode)
{
    std::uniform_real_distribution<float> random_value(0.0, 1.0);
    float q = random_value(generator);
    
    if (q < q0)
    {
        return maxPheromoneChoice(tour, currentNode);
    }
    if(biasedExploration){
    return biasedExplorationChoice(tour, currentNode); 
    }
    return tour->nextUnvisitedNode();
}

unsigned int AntColony::maxPheromoneChoice(Tour * tour, unsigned int node)
{
    float max = numeric_limits<float>::min();
    float current;
    unsigned int neighbor = tour->nextUnvisitedNode();
    unsigned int best = neighbor;
    for(unsigned int i = 1; i < candidateLists->size(); i++)
    {
        current = pheromones[node][neighbor] * distanceHeuristic(node, neighbor);
        if (current > max)
        {
            best = neighbor;
            max = current;
        }
        neighbor = tour->nextUnvisitedNode();
    }
    return best;
}

inline float AntColony::distanceHeuristic(unsigned int nodeA, unsigned int nodeB)
{
    return pow(1 / ((float) candidateLists->distance(nodeA,nodeB)), beta);
}


unsigned int AntColony::biasedExplorationChoice(Tour * tour, int currentNode)
{
    float sum = 0;
    uniform_real_distribution<float> random_value(0.0, 1.0);
    float threshold = random_value(generator);
    unsigned int n = tour->nextUnvisitedNode();
    for(unsigned int i = 1; i < candidateLists->size(); i++)
    {
        sum += probabilityCalculation(tour, currentNode, n);
        if (sum > threshold)
            break;
        n = tour->nextUnvisitedNode();
    }
    return n;
}

float AntColony::probabilityCalculation(Tour * tour, int nodeA, int nodeB)
{
    float sum =  0;
    for (unsigned int n = 0; n < numberOfNodes; n++)
    {
        if (!tour->visited(n))
        {
            sum += pheromones[nodeA][n] * distanceHeuristic(nodeA,n);
        }
    }
    return (pheromones[nodeA][nodeB] * distanceHeuristic(nodeA, nodeB)) / sum;
}

// We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheromoneLevel
void AntColony::updateLocalPheromone(int nodeA, int nodeB)
{
    pheromones[nodeA][nodeB] = max(minimum_p, (1 - rho) * pheromones[nodeA][nodeB] + rho * initialPheromones);
    pheromones[nodeB][nodeA] = pheromones[nodeA][nodeB]; // Change this if not symetrical problems
}

void AntColony::updateGlobalPheromones()
{
    #pragma omp parallel for
    for (unsigned int i = 0; i < numberOfNodes; i++)
    {
        for (unsigned int j = 0; j < numberOfNodes; j++)
        {
            if(i == j) continue;
            pheromones[i][j] = (1 - decay) * pheromones[i][j] + decay * deltaPheromones(i, j);
             // pheromones[j][i] = pheromones[i][j];
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




