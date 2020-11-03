#include <cstdint>
#include <cstdlib>
#include "CandidateLists.h"
typedef uint_fast32_t u_int;
#define DISTANCE_SPACE 2
#define SPACE_FOR_VARIABLE 1
#define BIT_WIDTH 32
#define nodeListIndex(n) (CL_TIER_SIZE * DISTANCE_SPACE + SPACE_FOR_VARIABLE) * n
#define tierIndex(t) (CL_TIER_SIZE * DISTANCE_SPACE + SPACE_FOR_VARIABLE) * numberOfNodes * t
#define nodeIndex(i)  i + SPACE_FOR_VARIABLE
#define distanceIndex(i) CL_TIER_SIZE + i + SPACE_FOR_VARIABLE
#define getExactDistanceIndex(t,n,i) tierIndex(t) + nodeListIndex(n) + distanceIndex(i)
#define getExactNodeIndex(t,n,i) tierIndex(t) + nodeListIndex(n) + nodeIndex(i)

struct pair{
    pair() = default;
    pair(u_int n, u_int d) : node(n), distance(d){};
    u_int node;
    u_int distance;
};

int compare(const void* p1, const void* p2){
    return ((pair*)p1)->distance - ((pair*)p2)->distance;
}

CandidateLists::CandidateLists(u_int numberOfNodes, u_int ** distances){
    numberOfTiers = numberOfNodes / CL_TIER_SIZE;
    this->numberOfNodes  = numberOfNodes;
    totalSize = (numberOfNodes*DISTANCE_SPACE + SPACE_FOR_VARIABLE) * numberOfNodes;
    pair** es = new pair*[numberOfNodes*numberOfNodes];

    for(u_int i = 0; i < numberOfNodes; i++){
        es[i] = new pair[numberOfNodes];
        for(u_int j = 0; j < numberOfNodes; j++){
            es[i][j] = pair(j, distances[i][j]);
        }
    }

    for(int i = 0; i < numberOfNodes; i++){
        qsort(es[i], numberOfNodes, sizeof(pair), compare);
    }
    u_int tier = 0;
    u_int total = 0;
    cLists = new u_int[totalSize];
    for(u_int i = 0; i < numberOfNodes; i++){
        for(u_int j = 0; j < numberOfNodes; j++){
            cLists[getExactNodeIndex(tier,i,j)] = es[i][j].node;
            cLists[getExactDistanceIndex(tier,i,j)] = es[i][j].distance;
            total++;

            if(total % CL_TIER_SIZE == 0){
                tier++;
                cLists[tierIndex(tier) + nodeListIndex(i)] = CL_TIER_SIZE;
            }
        }
    }
}

u_int CandidateLists::getNode(u_int tier, u_int node, u_int index) const {
    return cLists[getExactNodeIndex(tier, node, index)];
}

u_int CandidateLists::getDistance(u_int tier, u_int node, u_int index) const {
    return cLists[getExactDistanceIndex(tier,node,index)];
}
u_int CandidateLists::getNumberLeftInTier(u_int tier, u_int node) const {
    return cLists[tierIndex(tier) + nodeListIndex(node)];
}

u_int CandidateLists::getFirstUnmarkedTier(){
    return currentUnmarkedTier;
}

void CandidateLists::markNode(u_int tier, u_int node, u_int index){
    cLists[getExactNodeIndex(tier,node,index)] |= (1U << BIT_WIDTH);
    u_int countIndex = tierIndex(tier) + nodeListIndex(node);
    if(cLists[countIndex] - 1 > 0)
        cLists[countIndex]--;
    else if(cLists[countIndex] - 1 == 0){
        cLists[countIndex]--;
        currentUnmarkedTier++;
    }
}

inline bool CandidateLists::isValueMarked(u_int value) const {
    return value & (1U << BIT_WIDTH);
}

void CandidateLists::reset() {
    for(int i=0; i < totalSize; i++){
    cLists[i] &= ~(1U << BIT_WIDTH);
    }
    for(int i=0; i < numberOfNodes; i++){
        for(int t = 0; t < numberOfTiers; t++){
            cLists[tierIndex(tier) + nodeListIndex(node)] = CL_TIER_SIZE;
        }
    }
    currentUnmarkedTier = 0;
}


