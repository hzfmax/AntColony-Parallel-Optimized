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
    CandidateLists(unsigned int numberOfNodes, unsigned int ** distances);
    unsigned int getNode(unsigned int tier, unsigned int node, unsigned int index) const;
    unsigned int getDistance(unsigned int tier, unsigned int node, unsigned int index) const;
    unsigned int getNumberLeftInTier(unsigned int tier, unsigned int node) const;
    unsigned int getNumberOfTiers() const{return numberOfTiers;} 
    unsigned int getTierSize(unsigned int tier) const; 
    const unsigned int * getTierList(unsigned int tier, unsigned int node) const;
    void printCandidateLists();
    unsigned int getNumberOfNodes(){return numberOfNodes;}
    unsigned int nodeDistance(unsigned int nodeA, unsigned int nodeB) const;
    ~CandidateLists();

private:
    unsigned int * cLists; // 1D array indexed like a 2D array
    unsigned int totalSize;
    unsigned int numberOfTiers;
    unsigned int numberOfNodes;
    unsigned int remainderTierSize;
    unsigned int ** distances;
    
    // A particular node's candidate list will include all nodes and distances associated with that that Node. Nodes will be ordered shortest distance to longest. I will split the Node's list into sub N/M Tiers
    // where N is number of nodes and M is number of elements considered by the algorithm in one step. Tier 0 is considered first by the search algorithms. Only if none of Tier 0 will we procceed to Tier 1.
    
        // A node's list data organization: [ [numLeftInTier ], [node0], [node1], [node2]....., [distance0], [distance1], [distance2]....]       
};

#endif