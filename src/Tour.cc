#include "Tour.h"
#include <iostream>

using namespace std;

void swap(unsigned int * arr, unsigned int i, unsigned int j);

Tour::Tour(unsigned int nodes, CandidateLists * cl): numberOfNodes(nodes), candidateLists(cl){
    tour = new unsigned int [numberOfNodes];
    v = new bool[numberOfNodes];
    position = new unsigned int[numberOfNodes];
}

Tour::~Tour(){
    delete[] tour;
    delete[] v;
    delete[] position;
}

unsigned int Tour::nextUnvisitedNode() {
    unsigned int nodeId = tour[size-1];
    if(!searching){
        searching = true;
        currentSearchingTier = 0;
        lastSearchedIndex = 0;
        currentTierList = candidateLists->getTierList(currentSearchingTier, nodeId);
    }

    while(currentSearchingTier < candidateLists->getNumberOfTiers() ){    
        for(unsigned int i = lastSearchedIndex; i < candidateLists->getTierSize(currentSearchingTier); i++ ){
            if(!visited(currentTierList[i])){
                lastSearchedIndex = i;
                return currentTierList[lastSearchedIndex];
            }
        }
        currentSearchingTier++;
        currentTierList = candidateLists->getTierList(currentSearchingTier,nodeId);
    }
    return currentTierList[lastSearchedIndex];
}

// TODO: throw an error if trying to add to something when no room - default constructor will do this
void Tour::add(unsigned int node){
    if(isTourComplete())
        return; // TO DO: Make this throw an exception instead;
    position[node] = size;
    tour[size] = node;
    size++; 
    v[node] = true;
    changed = true;
    searching = false;
}

//To Do: implement reversal from index i to k
void Tour::reverse(unsigned int start, unsigned int end){
    changed = true;
    unsigned int size = ((end - start + 1 + numberOfNodes) % numberOfNodes) / 2;
    unsigned int left = start;
    unsigned int right = end;
    for(unsigned int i = 0; i < size; i++){
        swap(tour, left, right);
        position[tour[left]] = left;
        position[tour[right]] = right;
        left = (left + 1) % numberOfNodes;
        right = (numberOfNodes + right - 1) % numberOfNodes;
    }
}

unsigned int Tour::getNode(unsigned int index) {
    return tour[index];
}

unsigned long Tour::getTotalTourDistance(){
    totalTourDistance = 0;
    if(changed){
    for(unsigned int i = 1; i < size; i++){
        totalTourDistance += candidateLists->nodeDistance(tour[i-1], tour[i]);
    }
    }
    return totalTourDistance;
}
bool Tour::isTourComplete(){
    return size == numberOfNodes;
}
void Tour::reset(){
    #pragma omp parallel for
    for(unsigned int i = 0; i < numberOfNodes; i++){
        v[i] = false;
        position[i] = 0;
        changed = false;
    }
    totalTourDistance = 0;
    size = 0;
}
unsigned int Tour::getSize(){
    return size;
}

bool Tour::edgeInTour(unsigned int nodeA, unsigned int nodeB){
    for(unsigned int i = 1; i < size; i++){
        if(tour[i-1] == nodeA && tour[i] == nodeB) return true;
    }
    return false;
}

bool Tour::visited(unsigned int node) const{
    return v[node];
}

void Tour::printTour(){
    for(unsigned int i = 0; i < size - 1; i++){
        cout << "(" << tour[i] << "," << candidateLists->nodeDistance(tour[i], tour[i+1]) << ") -> ";
    }
    cout << "(" << tour[size - 1] << "," << candidateLists->nodeDistance(tour[size - 1], tour[0]) << ")" << endl;
}

unsigned int Tour::getNodePositionInTour(unsigned int node){
    return position[node];
}


void swap(unsigned int * arr, unsigned int i, unsigned int j){
    unsigned int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}