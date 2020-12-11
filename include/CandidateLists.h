#ifndef CANDIDATE_LIST_H
#define CANDIDATE_LIST_H
#include <cstdint>
#include <vector>
constexpr unsigned int CL_TIER_SIZE = 25U;

struct Node{
    Node() = default;
    Node(unsigned int n, unsigned int d) : nodeId(n), distance(d){};
    unsigned int nodeId;
    unsigned int distance;
};

class CandidateLists{
public:
    CandidateLists() = delete;
    CandidateLists(unsigned int nodes, unsigned int candidateListSize, const std::vector<std::pair<float,float>> &symmetricCoordinates);
    unsigned int getNode(unsigned int node, unsigned int index) const;
    unsigned int size() const {return _size;};
    unsigned int getNumberOfNodes() const {return numberOfNodes;};
    unsigned int distance(unsigned int nodeA, unsigned int nodeB) const;
    ~CandidateLists();

private:
    unsigned int numberOfNodes;
    unsigned int _size;
    unsigned int ** cLists; 
    unsigned int ** distances;
     
   
};

#endif