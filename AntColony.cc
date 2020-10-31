

#include "AntColony.h"
#include <cmath>
#include <random>
// Use Euclidian 2D to start
int AntColony::distance(int nodeA, int nodeB)
{
    Coordinate A = nodeCoordinates[nodeA];
    Coordinate B = nodeCoordinates[nodeB];
    return sqrt(pow((A.x - B.x), 2) + pow(A.y - B.y, 2));
}

long AntColony::length(int ant)
{
    int *tour = tours[ant];
    long sum = 0;
    for (int i = 0; i < numberOfNodes - 1; i++)
    {
        sum += distances[tour[i]][tour[i + 1]];
    }
    return sum;
}

bool AntColony::exists(int nodeA, int nodeB)
{
    return graph[nodeA][nodeB];
}

bool AntColony::visited(int ant, int node)
{
    int *tour = tours[ant];
    for (int i = 0; i < numberOfNodes; i++)
    {
        if (tour[i] == -1)
            break;
        if (tour[i] == node)
            return true;
    }
    return false;
}

// We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheremoneLevelOfedge
void AntColony::updateLocalPheromone(int nodeA, int nodeB)
{
    pheromones[nodeA][nodeB] = (1 - rho) * pheromones[nodeA][nodeB] + rho * initialPheromones;
}
// Takes in the current city returns the choice for where to move to next // State Transition Rule
int AntColony::chooseNextNode(int ant, int currentNode)
{
    std::uniform_real_distribution<float> random_value(0.0, 1.0);
    float q = random_value(generator);
    if (q < q0)
    {
        return maxPheromoneChoice(ant, currentNode);
    }
    return biasedExplorationChoice(ant, currentNode);
}

int AntColony::maxPheromoneChoice(int ant, int currentNode)
{
    float max = 0;
    int bestNode = -1;
    for (int n = 0; n < numberOfNodes; n++)
    {
        float current = pheromones[currentNode][n] * distanceHeuristic(currentNode, n);
        if (current > max)
        {
            bestNode = n;
            max = current;
        }
    }
    return bestNode;
}

float AntColony::probabilityCalculation(int nodeA, int nodeB, int ant)
{
    float sum = 0.0;
    for (int n = 0; n < numberOfNodes; n++)
    {
        if (exists(nodeA, n) && !visited(ant, n))
        {
            sum += pheromones[nodeA][n] * distanceHeuristic(nodeA, nodeB);
        }
    }
    return (pheromones[nodeA][nodeB] * distanceHeuristic(nodeA, nodeB)) / sum;
}

float AntColony::distanceHeuristic(int nodeA, int nodeB)
{
    return pow(1 / distances[nodeA][nodeB], beta);
}

int AntColony::biasedExplorationChoice(int ant, int currentNode)
{
    int count = 0;
    float sum = 0;
    std::uniform_real_distribution<float> random_value(0.0, 1.0);
    float threshold = random_value(generator);
    for (int n = 0; n < numberOfNodes; n++)
    {
        if (n == currentNode)
            continue;

        if (exists(currentNode, n) && !visited(ant, n))
            sum += probabilityCalculation(currentNode, n, ant);

        if (sum > threshold)
            return n;
    }
    return -1;
}

void AntColony::updateGlobalPheromones()
{
    findBestTour();
    for (int i = 0; i < numberOfNodes; i++)
    {
        for (int j = 0; i < numberOfNodes; j++)
        {
            pheromones[i][j] = (1 - decay) * pheromones[i][j] + decay * deltaPheramones(i, j);
        }
    }
}

float AntColony::deltaPheramones(int nodeA, int nodeB)
{
    if (inGlobalBest(nodeA, nodeB))
    {
        return 1 / ((float)bestTour_length);
    }
    return 0;
}

bool AntColony::inGlobalBest(int nodeA, int nodeB)
{
    int* bestTour = tours[bestTour_index];
    for (int i = 0; i < numberOfNodes - 1; i++)
    {
        if (bestTour[i] == nodeA && bestTour[(i + 1)] == nodeB)
            return true;
    }
    return false;
}

void AntColony::findBestTour()
{
    long bestlength = length(0);
    int bestAnt = 0;
    for (int ant = 1; ant < numberOfAnts; ant++)
    {
        long l = length(ant);
        if (l < bestlength)
        {
            bestlength = l;
            bestAnt = ant;
        }
    }
    bestTour_length = bestlength;
    bestTour_index = bestAnt;
}

void AntColony::resetTour(int ant)
{
    for (int i = 0; i < numberOfNodes; i++)
    {
        tours[ant][i] = -1;
    }
}

bool AntColony::findAntTour(int ant)
{
    resetTour(ant);
    std::uniform_real_distribution<int> random_value(0, numberOfNodes - 1);
    int *tour = tours[ant];
    tour[0] = random_value(generator);
    for (int i = 1; i < numberOfNodes; i++)
    {
        int node = tour[i - 1];
        int choice = chooseNextNode(ant, node);
        if (choice == -1)
            return false;
        tour[i] = choice;
    }
    return true;
}

void AntColony::optimize()
{
    for (int i = 0; i < iterations; i++)
    {

        //Send out ants
        for (int a = 0; a < numberOfAnts; a++)
        {
            while (!findAntTour(a))
                ;
        }

        updateGlobalPheromones();

        //Reset Routes
        for (int a = 0; a < numberOfAnts; a++)
        {
            resetTour(a);
        }
    }
}

long AntColony::getShortestTourLength()
{
    return bestTour_length;
}
int *AntColony::getShortestTour()
{
}