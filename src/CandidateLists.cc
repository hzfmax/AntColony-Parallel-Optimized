#include <cstdint>
#include <cstdlib>
#include "CandidateLists.h"
#include "utility.h"
#include <vector>
#include <iostream>
#include <utility>
#include <cmath>
#include <stdexcept>
#include <string>
#define remainderI
using namespace std;

int compare(const void* p1, const void* p2){
    return ((Node*)p1)->distance - ((Node*)p2)->distance;
}

unsigned int distance(unsigned int nodeA, unsigned int nodeB,  const std::vector<std::pair<float,float>> &symmetricCoordinates);

void initDistanceMatrix(unsigned int ** distances,  const std::vector<std::pair<float,float>> &symmetricCoordinates);

CandidateLists::CandidateLists(unsigned int nodes, unsigned int candidateListSize, const std::vector<std::pair<float,float>> &symmetricCoordinates) : numberOfNodes(nodes), _size(candidateListSize)
{
    if(nodes < candidateListSize){
        _size = nodes;
    }

    distances = new_2D_array<unsigned int>(numberOfNodes, numberOfNodes);
    cLists = new_2D_array<unsigned int>(numberOfNodes, numberOfNodes);

    initDistanceMatrix(distances, symmetricCoordinates);
    
    Node** es =  new_2D_array<Node>(numberOfNodes, numberOfNodes);
    #pragma omp parallel for
    for(unsigned int i = 0; i < numberOfNodes; i++){
        for(unsigned int j = 0; j < numberOfNodes; j++){
            es[i][j] = Node(j, distances[i][j]);
        }
    }

    #pragma omp parallel for
    for(unsigned int i = 0; i < numberOfNodes; i++){
        qsort(es[i], numberOfNodes, sizeof(Node), compare);
    }
    
    for(unsigned int i = 0; i < numberOfNodes; i++){
            for(unsigned int j = 0; j < numberOfNodes; j++){
                    cLists[i][j] = es[i][j].nodeId;
            }      
    }

    delete_2D_array<Node>(numberOfNodes, numberOfNodes, es);
}

CandidateLists::~CandidateLists(){
    delete_2D_array<unsigned int>(numberOfNodes, _size, distances);
    delete_2D_array<unsigned int>(numberOfNodes, _size, cLists);
}


unsigned int CandidateLists::distance(unsigned int nodeA, unsigned int nodeB) const{
    return distances[nodeA][nodeB];
}

//TODO: Throw Exception if index's are out of bounds
unsigned int CandidateLists::getNode(unsigned int node, unsigned int index) const {
    if(node >= numberOfNodes || index >= numberOfNodes){
        throw range_error("CandidateLists.getNode(node, index): node or index out of bounds - node: "s + std::to_string(node) + " index: " + std::to_string(index) + "");
    }
    return cLists[node][index];
}  

// Use Euclidian 2D to start
inline unsigned int distance(std::pair<float,float>& A, std::pair<float,float>& B)
{
    return floor(sqrt(pow((A.first - B.first), 2) + pow(A.second - B.second, 2)));
}

// Currently assumes symmetrical distances
void initDistanceMatrix(unsigned int ** distances,  const std::vector<std::pair<float,float>> &symmetricCoordinates){
    #pragma omp parallel for
    for(std::size_t i = 0; i < symmetricCoordinates.size(); i++){
        for(std::size_t j = 0; j < symmetricCoordinates.size(); j++){
            std::pair<float,float> A = symmetricCoordinates[i];
            std::pair<float,float> B = symmetricCoordinates[j];
            distances[i][j] = distance(A,B);
        }
    }
}
