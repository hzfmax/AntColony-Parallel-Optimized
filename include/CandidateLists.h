#include <cstdint>
#ifndef CL_TIER_SIZE
#define CL_TIER_SIZE 10
#endif

typedef uint_fast32_t u_int;

class CandidateLists{
public:
  
    CandidateLists() = delete;
    CandidateLists(u_int numberOfNodes, u_int ** distances);
    u_int getNode(u_int tier, u_int node, u_int index) const;
    u_int getDistance(u_int tier, u_int node, u_int index) const;
    u_int getNumberLeftInTier(u_int tier, u_int node) const;
    u_int getNumberOfTiers() const{return numberOfTiers;} 
    u_int getTierSize()const {return CL_TIER_SIZE;} 
    u_int getMarkedFlag() const;
    void markNode(u_int tier, u_int node, u_int index);
    bool isValueMarked(u_int value) const;
    void reset();


private:
    u_int * cLists; // 1D array indexed like a 2D array
    u_int totalSize;
    u_int numberOfTiers;
    u_int numberOfNodes;
    u_int currentUnmarkedTier = 0;
    //u_int getExactNodeIndex(u_int t,u_int n,u_int i) const;
    //u_int getExactDistanceIndex(u_int t,u_int n,u_int i) const;
                // A particular node's candidate list will include all nodes and distances associated with that that Node. Nodes will be ordered shortest distance to longest. I will split the Node's list into sub N/M Tiers
                // where N is number of nodes and M is number of elements considered by the algorithm in one step. Tier 0 is considered first by the search algorithms. Only if none of Tier 0 will we procceed to Tier 1.
                
                    // A node's list data organization: [ [numLeftInTier ], [node0], [node1], [node2]....., [distance0], [distance1], [distance2]....]       


                // Node's that have been used will have their last bit switched to 1 and tested against. When we reset the candidate list, we switch this bit back to 0, this will preserve our data without having to recompute it. 
                // It does bound our List's to nodes of half the unsigned length of 32 bit - which is ~2 billion. This is way larger then most any traveling salesman problem so it should work well.
}

