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
using namespace std;
constexpr float minimum_p = numeric_limits<float>::epsilon();

// Constructor assumes fully connected sysmetric graph give by a list of coordinates that distances can be calulated from
AntColony::AntColony(const std::vector<std::pair<float,float>> &symmetricCoordinates) : numberOfNodes(symmetricCoordinates.size())
{
    generator.seed(chrono::high_resolution_clock::now().time_since_epoch().count() / getpid());
    numberOfAnts = 10;
    bestTour.reserve(numberOfNodes + 1);
    distances = new_2D_array<int>(numberOfNodes, numberOfNodes);
    pheromones = new_2D_array<float>(numberOfNodes, numberOfNodes);
    graph = new_2D_array<bool>(numberOfNodes, numberOfNodes);
    tours = new_2D_array<int>(numberOfAnts, numberOfNodes + 1);

    nodeCoordinates = new Coordinate[numberOfNodes];
    for(size_t i = 0; i < symmetricCoordinates.size(); i++){
        nodeCoordinates[i].x = symmetricCoordinates[i].first;
        nodeCoordinates[i].y = symmetricCoordinates[i].second;
    }
    initDistanceMatrix();
    for(int i = 0; i < numberOfNodes; i++){
        for(int j = 0; j < numberOfNodes; j++){
            graph[i][j] = 1;
        }
    }
}

AntColony::~AntColony(){
    delete_2D_array<int>(numberOfAnts, numberOfNodes + 1, tours);
    delete_2D_array<int>(numberOfNodes, numberOfNodes, distances);
    delete_2D_array<float>(numberOfNodes, numberOfNodes, pheromones);
    delete_2D_array<bool>(numberOfNodes, numberOfNodes, graph);
    delete[] nodeCoordinates;
}



AntColony& AntColony::optimize(int iterations , int numberOfAnts, float rho , float beta , float decay , float qExplorationFactor)
{
    setParameters(iterations, numberOfAnts, rho, beta, decay, qExplorationFactor);
    reset();
    
    for (int i = 0; i < iterations; i++)
    {

        // Send out ants
        for (int a = 0; a < numberOfAnts; a++)
        {
            while (!findAntTour(a));
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
        this->antsChanged = true;
        }
        this->rho = rho;
        this->beta = beta;
        this->decay = decay;
        this->q0 = qExplorationFactor;
        return *this;
}

void AntColony::reset(){
    if(antsChanged)
    {
        tours = new_2D_array<int>(numberOfAnts, numberOfNodes + 1);
        antsChanged = false;
    }
    
    resetTour(0);
    
    uniform_int_distribution<int> random_value(0, numberOfNodes - 1);

    nearistNeighbor(random_value(generator));

    bestTour_index = 0;
    bestTour_length = length(0);
    for(int i = 0; i < numberOfNodes + 1; i++){
        bestTour.push_back(tours[bestTour_index][i]);
    }
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
    int * tour = tours[0];
    tour[0] = node;
    for(int i = 1; i < numberOfNodes; i++){
        tour[i] = shortestDistance(tour[i-1]);
    }
    tour[numberOfNodes] = tour[0];
}

int AntColony::shortestDistance(int node)
{   int min = numeric_limits<int>::max();
    int min_index = -1;
    for(int n = 0; n < numberOfNodes; n++){
        if(n == node) continue;
        
        
        if(!visited(0,n) && min > distances[node][n]){
            min = distances[node][n];
            min_index = n;
        }
    }
    return min_index;
}

long AntColony::getShortestTourLength()
{
    return bestTour_length;
}

vector<int> AntColony::getShortestTour()
{
    return bestTour;
}

bool AntColony::findAntTour(int ant)
{
    resetTour(ant);
    uniform_int_distribution<int> random_value(0, numberOfNodes - 1);
    int * tour = tours[ant];
    tour[0] = random_value(generator);
    for (int i = 1; i < numberOfNodes; i++)
    {
        int node = tour[i - 1];
        int choice = chooseNextNode(ant, node);
        
        if (choice == -1)
            return false;
        tour[i] = choice;
        updateLocalPheromone(node,choice); // Will need to be atomic
    }
    tour[numberOfNodes] = tour[0];
    updateLocalPheromone(tour[numberOfNodes],tour[0]);
    return true;
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
    float max = numeric_limits<float>::min();
    int bestNode = -1;
    float current;
    for (int n = 0; n < numberOfNodes; n++)
    {
        if (n == currentNode || visited(ant,n) || !exists(currentNode,n)) continue;
        
        current = pheromones[currentNode][n] * distanceHeuristic(currentNode, n);
        if (current > max)
        {
            bestNode = n;
            max = current;
        }
    }
    
    return bestNode;
}


float AntColony::distanceHeuristic(int nodeA, int nodeB)
{
    return pow((1 / ((float)distances[nodeA][nodeB])), beta);
}

int AntColony::biasedExplorationChoice(int ant, int currentNode)
{
    float sum = 0;
    uniform_real_distribution<float> random_value(0.0, 1.0);
    float threshold = random_value(generator);
    for (int n = 0; n < numberOfNodes; n++)
    {
        if (n == currentNode || visited(ant, n) || !exists(currentNode, n) )
            continue;

        sum += probabilityCalculation(currentNode, n, ant);

        if (sum > threshold)
            return n;
    }
    return -1;
}

float AntColony::probabilityCalculation(int nodeA, int nodeB, int ant)
{
    float sum =  0;
    for (int n = 0; n < numberOfNodes; n++)
    {
        if (exists(nodeA, n) && !visited(ant, n))
        {
            sum += pheromones[nodeA][n] * distanceHeuristic(nodeA, nodeB);
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
    for (int i = 0; i < numberOfNodes; i++)
    {
        for (int j = 0; j < numberOfNodes; j++)
        {
            if(i == j) continue;
            pheromones[i][j] = (1 - decay) * pheromones[i][j] + decay * deltaPheromones(i, j);
        }
    }
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
    bestTour.clear();
    for(int i = 0; i < numberOfNodes + 1; i++){
        bestTour.push_back(tours[bestTour_index][i]);
    }
}

float AntColony::deltaPheromones(int nodeA, int nodeB)
{
    if (inGlobalBest(nodeA, nodeB))
    {
        return 1 / ((float)bestTour_length);
    }
    return 0;
}

bool AntColony::inGlobalBest(int nodeA, int nodeB)
{
    for (int i = 0; i < numberOfNodes - 1; i++)
    {
        if (bestTour[i] == nodeA && bestTour[(i + 1)] == nodeB)
            return true;
    }
    return false;
}


void AntColony::resetTour(int ant)
{
    for (int i = 0; i < numberOfNodes + 1; i++)
    {
        tours[ant][i] = -1;
    }
}

// Use Euclidian 2D to start
int AntColony::distance(int nodeA, int nodeB)
{
    Coordinate A = nodeCoordinates[nodeA];
    Coordinate B = nodeCoordinates[nodeB];
    return floor(sqrt(pow((A.x - B.x), 2) + pow(A.y - B.y, 2)));
}

long AntColony::length(int ant)
{
    int * tour = tours[ant];
    long sum = 0;
    for (int i = 0; i < numberOfNodes - 1; i++)
    {
        sum += distances[tour[i]][tour[i + 1]];
    }
    return sum;
}

bool AntColony::exists(int nodeA, int nodeB)
{   
    // Currently assuming symetrical complete graphs
    return true;
    // return graph[nodeA][nodeB];
}

bool AntColony::visited(int ant, int node)
{
    int * tour = tours[ant];
    for (int i = 0; i < numberOfNodes; i++)
    {
        if (tour[i] == -1)
            return false;
        if (tour[i] == node)
            return true;
    }
    return false;
}

// Currently assumes symmetrical distances
void AntColony::initDistanceMatrix(){
    for(int i = 0; i < numberOfNodes; i++){
        for(int j = i; j < numberOfNodes; j++){
            distances[i][j] = distance(i,j);
            distances[j][i] = distances[i][j];
        }
    }
}


void AntColony::printCurrentState(){
    cout << "rho: " << rho << ", decay: " << decay << ", numberOfAnts: " << numberOfAnts << ", Number of Nodes: " << numberOfNodes << ", iterations: "
        << iterations << ", beta: " << beta << ", q0: " << q0  << " , initialPheromones: " << initialPheromones << "\n";
    
    cout << "Best Tour length: " << bestTour_length << "\n";
    cout << "Tour: ";
    for(int i = 0; i < numberOfNodes; i++){
        cout << tours[bestTour_index][i] << " -> ";
    }
     cout << tours[bestTour_index][numberOfNodes];
    cout << endl;
}

void AntColony::printDistanceMatrix(){
    cout << "Distance Matrix" << "\n";
    for(int j = 0; j < numberOfNodes; j++){
            cout << j << " ";
    }   
    cout << "\n";
    for(int i = 0; i < numberOfNodes; i++){
        cout << i << ": ";
        for(int j = 0; j < numberOfNodes; j++){
            cout << distances[i][j] << " ";
    }   
        cout << "\n";
    }
    cout << endl;
}

void AntColony::printPheromones(){
    cout << setprecision(2);
    for(int i = 0; i < numberOfNodes; i++){
        cout << i << ": ";
        for(int j = 0; j < numberOfNodes; j++){
            cout << pheromones[i][j] << " ";
    }   
        cout << "\n";
    }
    cout << endl;
}


