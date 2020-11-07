#include "Tour.h"
#include <iostream>
#define distanceIndex(i) numberOfNodes + i
#define nodeIndex(i) i
using namespace std;

Tour::Tour(unsigned int nodes, CandidateLists * cl): numberOfNodes(nodes), candidateLists(cl){
    tour = new unsigned int [(numberOfNodes+1)*2];
    v = new bool[numberOfNodes];
}

Tour::~Tour(){
    delete[] tour;
    delete[] v;
}

bool Tour::nextUnvisitedNode(Node & n) {
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
                n.nodeId = currentTierList[lastSearchedIndex];
                n.distance = candidateLists->getDistance(currentSearchingTier,nodeId,lastSearchedIndex);
                return true;
            }
        }
        currentSearchingTier++;
        currentTierList = candidateLists->getTierList(currentSearchingTier,nodeId);
    }
    return false;
}

// TODO: throw an error if trying to add to something when no room - default constructor will do this
void Tour::add(unsigned int node, unsigned int distance){
    if(isTourComplete())
        return; // TO DO: Make this throw an exception instead;

    tour[nodeIndex(size)] = node;
    tour[distanceIndex(size)] = distance;
    totalTourDistance += distance;
    size++; 
    v[node] = true;
    searching = false;
    if(size == numberOfNodes)
        add(getNode(0), candidateLists->findNodesDistance(node,getNode(0) ));
}

void Tour::add(Node n){
    add(n.nodeId, n.distance);
}

unsigned int Tour::getNode(unsigned int index) {
    if(index >= size)
        return -1; // TO DO: Throw an exception instead;
    return tour[nodeIndex(index)];
}
unsigned int Tour::getDistance(unsigned int index) {
    if(index >= size)
        return -1; // TO DO: Throw an exception instead;
    return tour[distanceIndex(index)];
}
unsigned long Tour::getTotalTourDistance(){
    return totalTourDistance;
}
bool Tour::isTourComplete(){
    return size == numberOfNodes + 1;
}
void Tour::reset(){
    for(unsigned int i = 0; i < numberOfNodes; i++)
        v[i] = false;
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
        cout << "(" << getNode(i) << "," << getDistance(i) << ") -> ";
    }
    cout << "(" << getNode(size - 1) << "," << getDistance(size - 1) << ")\n";
}