#include <cstdint>
#include <cstdlib>
#include "CandidateLists.h"
#include "utility.h"
#include <vector>
#include <iostream>
#define DISTANCE_SPACE 2
#define SPACE_FOR_VARIABLE 1
#define BIT_WIDTH 31
#define nodeListIndex(n, ts) (ts * DISTANCE_SPACE + SPACE_FOR_VARIABLE) * n
#define tierIndex(t) (CL_TIER_SIZE * DISTANCE_SPACE + SPACE_FOR_VARIABLE) * numberOfNodes * t
#define nodeIndex(i)  i + SPACE_FOR_VARIABLE
#define distanceIndex(i,ts) ts + i + SPACE_FOR_VARIABLE
#define getExactDistanceIndex(t,n,i,ts) tierIndex(t) + nodeListIndex(n, ts) + distanceIndex(i,ts)
#define getExactNodeIndex(t,n,i,ts) tierIndex(t) + nodeListIndex(n, ts) + nodeIndex(i)
using namespace std;

int compare(const void* p1, const void* p2){
    return ((Node*)p1)->distance - ((Node*)p2)->distance;
}

CandidateLists::CandidateLists(unsigned int numberOfNodes, unsigned int ** distances){
    numberOfTiers = numberOfNodes / CL_TIER_SIZE;
    remainderTierSize = numberOfNodes % CL_TIER_SIZE;
    if(remainderTierSize){
        numberOfTiers++;
    } else{
        remainderTierSize = CL_TIER_SIZE;
    }
    this->numberOfNodes  = numberOfNodes;
    
    Node** es = new Node*[numberOfNodes];
    this->distances = distances;
    #pragma omp parallel for
    for(unsigned int i = 0; i < numberOfNodes; i++){
        es[i] = new Node[numberOfNodes];
        for(unsigned int j = 0; j < numberOfNodes; j++){
            es[i][j] = Node(j, distances[i][j]);
        }
    }

    #pragma omp parallel for
    for(unsigned int i = 0; i < numberOfNodes; i++){
        qsort(es[i], numberOfNodes, sizeof(Node), compare);
    }
    
    totalSize = numberOfNodes*DISTANCE_SPACE* numberOfNodes + SPACE_FOR_VARIABLE*numberOfNodes*numberOfTiers;
    cLists = new unsigned int[totalSize];


    unsigned int tier_size = CL_TIER_SIZE;
    
    for(unsigned int t = 0; t < numberOfTiers; t++){
        unsigned int indexStart = t*tier_size;
            if(t == numberOfTiers - 1){
                tier_size = remainderTierSize;
            }
    #pragma omp parallel for
    for(unsigned int n = 0; n < numberOfNodes; n++){
            cLists[tierIndex(t) + nodeListIndex(n,tier_size)] = tier_size;
            for(unsigned int j = 0; j < tier_size; j++){
                cLists[getExactNodeIndex(t,n,j,tier_size)] = es[n][indexStart+j].nodeId;
                cLists[getExactDistanceIndex(t,n,j,tier_size)] = es[n][indexStart+j].distance;
                }
        }
    }
    for(unsigned int i = 0; i < numberOfNodes; i++){
        delete[] es[i];
    }
    delete[] es;
}

CandidateLists::~CandidateLists(){
    delete [] cLists;
}

unsigned int CandidateLists::getTierSize(unsigned int tier) const 
{   
    if(tier == numberOfTiers - 1)
        return remainderTierSize;
    return CL_TIER_SIZE;
} 

unsigned int CandidateLists::nodeDistance(unsigned int nodeA, unsigned int nodeB) const{
    return distances[nodeA][nodeB];
}

//TODO: Throw Exception if index's are out of bounds
unsigned int CandidateLists::getNode(unsigned int tier, unsigned int node, unsigned int index) const {
    unsigned int tier_size = CL_TIER_SIZE;
    if(tier == numberOfTiers - 1)
        tier_size = remainderTierSize;
    return cLists[getExactNodeIndex(tier, node, index, tier_size)];
}

//TODO: Throw Exception if index's are out of bounds
unsigned int CandidateLists::getDistance(unsigned int tier, unsigned int node, unsigned int index) const {
    unsigned int tier_size = CL_TIER_SIZE;
    if(tier == numberOfTiers - 1)
        tier_size = remainderTierSize;
    return cLists[getExactDistanceIndex(tier,node,index, tier_size)];
}

//TODO: Throw Exception if index's are out of bounds
unsigned int CandidateLists::getNumberLeftInTier(unsigned int tier, unsigned int node) const {
    unsigned int tier_size = CL_TIER_SIZE;
    if(tier == numberOfTiers - 1)
        tier_size = remainderTierSize;
    return cLists[tierIndex(tier) + nodeListIndex(node, tier_size)];
}

//TODO: Throw Exception if index's are out of bounds
const unsigned int * CandidateLists::getTierList(unsigned int tier, unsigned int node) const{
    unsigned int tier_size = CL_TIER_SIZE;
    if(tier == numberOfTiers - 1)
        tier_size = remainderTierSize;
    return cLists + tierIndex(tier) + nodeListIndex(node,tier_size) + 1;
}


void CandidateLists::printCandidateLists(){
    unsigned int tier_size = CL_TIER_SIZE;
    for(unsigned int t = 0; t < numberOfTiers; t++){
        if(t == numberOfTiers - 1)
                tier_size = remainderTierSize;
        for(unsigned int i = 0; i < numberOfNodes; i++){
            for(unsigned int j = 0; j < tier_size; j++){
                cout << " (" << getNode(t,i,j) << "," << getDistance(t,i,j)  << ") ";
            }
            cout << "\n";
        }
    }
    cout << endl;
}


