#include "Tour.h"
#include <iostream>

using namespace std;

void swap(unsigned int * arr, unsigned int i, unsigned int j);

Tour::Tour(unsigned int nodes, const CandidateLists * cl): numberOfNodes(nodes), candidateLists(cl){
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
    unsigned int nextNode = 0;
    if(!searching){
        searching = true;
        lastSearchedIndex = 0;
    }

    for(unsigned int i = lastSearchedIndex; i < numberOfNodes; i++ ){
        nextNode = candidateLists->getNode(nodeId,i);
        if(!visited(nextNode)){
            lastSearchedIndex = i;
            break;
            }
    }
    return nextNode;
}

void Tour::add(unsigned int node){
    if(isTourComplete())
        throw range_error("Tour.add(node): Tour is full, cannot add any more items to it. node: "s + std::to_string(node)); // TO DO: Make this throw an exception instead;
    position[node] = size;
    tour[size] = node;
    size++; 
    v[node] = true;
    changed = true;
    searching = false;
}

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
    if(index >= numberOfNodes)
        throw range_error("Tour.getNode(index): index out of bounds: "s + std::to_string(index));
    return tour[index];
}

unsigned long Tour::getTotalTourDistance(){
    totalTourDistance = 0;
    if(changed){
    for(unsigned int i = 1; i < size; i++){
        totalTourDistance += candidateLists->distance(tour[i-1], tour[i]);
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
        cout << "(" << tour[i] << "," << candidateLists->distance(tour[i], tour[i+1]) << ") -> ";
    }
    cout << "(" << tour[size - 1] << "," << candidateLists->distance(tour[size - 1], tour[0]) << ")" << endl;
}

unsigned int Tour::getNodePositionInTour(unsigned int node){
    return position[node];
}


void swap(unsigned int * arr, unsigned int i, unsigned int j){
    unsigned int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}