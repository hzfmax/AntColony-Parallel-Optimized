

#include "AntColony.h"
#include <cmath>

// Use Euclidian 2D to start
int AntColony::distance(int nodeA, int nodeB){
    Coordinate A = nodeCoordinates[nodeA];
    Coordinate B = nodeCoordinates[nodeB];
    return sqrt(pow((A.x - B.x), 2) + pow(A.y-B.y,2));
}

long AntColony::length(int ant){
    int * tour = tours[ant];
    long sum = 0;
    for(int i = 0; i < numberOfNodes - 1; i++){
        sum += distanceMatrix[tour[i]][tour[i+1]];
    }
    return sum;
}

bool AntColony::exists(int nodeA, int nodeB){
    return graph[nodeA][nodeB];
}

bool AntColony::visited(int ant, int node){
    int * tour = tours[ant];
    for(int i = 0; i < numberOfNodes; i++){
        if(tour[i] == -1)
            break;
        if(tour[i] == node)
            return true;
    }
    return false;
}

// We will use the pheromoneMatrix[nodeA][nodeB] = (1 - Rho)*pheromoneMatrix[nodeA][nodeB] + (Rho)*initialPheremoneLevelOfedge
void AntColony::updateLocalPheromone(int nodeA, int nodeB){
    pheromoneMatrix[nodeA][nodeB] = (1 - rho) * pheromoneMatrix[nodeA][nodeB] + rho * initialPheromones;
} 
// Takes in the current city returns the choice for where to move to next // State Transition Rule
int AntColony::chooseNextNode(int currentNode){
    
} 

void AntColony::updateGlobalPheromones(){

}
void AntColony::findTour(int ant){

}