#ifndef TOUR_H
#define TOUR_H
#include <cstdint>
#include "CandidateLists.h"
class Tour{
public:
    Tour() = delete;
    Tour(unsigned int nodes, CandidateLists * cl);
    void add(unsigned int node, unsigned int distance);
    void add(Node n);
    unsigned int getNode(unsigned int index);
    unsigned int getDistance(unsigned int index);
    unsigned long getTotalTourDistance();
    unsigned int getCurrentSearchTier(){return currentSearchingTier;}
    bool isTourComplete();
    void reset();
    unsigned int getSize();
    void printTour();
    bool edgeInTour(unsigned int nodeA, unsigned int nodeB);
    bool visited(unsigned int node) const;
    bool nextUnvisitedNode(Node & n);
    ~Tour();
private:
    // We will hold both the Tour and the related distance in the second half of a 1D array to garantee memory locality based on access patterns.
    bool * v;
    unsigned int * tour;
    unsigned int numberOfNodes;
    CandidateLists * candidateLists;
    unsigned long totalTourDistance = 0;
    unsigned int size = 0;
    bool searching = false;
    unsigned int currentSearchingTier = 0;
    unsigned int lastSearchedIndex = 0;
    const unsigned int * currentTierList = nullptr;
};

#endif