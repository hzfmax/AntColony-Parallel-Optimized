#include <cstdint>
#include "CandidateLists.h"
#include "AntColony.h"
#include "Tour.h"
#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.h"
constexpr unsigned int CANDIDATE_LIST_SIZE = 25U;


unsigned int ** initDistanceArray(){
    unsigned int ** distances = new  unsigned int * [CANDIDATE_LIST_SIZE];
    for(unsigned int i = 0; i < CANDIDATE_LIST_SIZE; i++){
        distances[i] = new unsigned int [CANDIDATE_LIST_SIZE];
        for(unsigned int j = 0; j < CANDIDATE_LIST_SIZE; j++){
            unsigned int k = (i+1)*100 - j;
            distances[i][j] = k;
        }
    }
    return distances;
}

void deleteDistance(unsigned int ** d){
    for(unsigned int i = 0; i < CANDIDATE_LIST_SIZE; i++){
        delete[] d[i];
    }
    delete[] d;
}

TEST_CASE("Candidate Lists initialization", "[CandidateLists]"){
    unsigned int ** distancesMock = initDistanceArray();
    CandidateLists cl((unsigned int)CANDIDATE_LIST_SIZE, distancesMock);
    cl.printCandidateLists();

    REQUIRE(cl.getTierSize(0) == 10);
    REQUIRE(cl.getNumberLeftInTier(0,0) == 10);
    REQUIRE(cl.getNumberOfTiers() == 3);
    REQUIRE(cl.getNode(1,8,9) == 5);
    REQUIRE(cl.getDistance(1,8,9) == 895);

    deleteDistance(distancesMock);
}

TEST_CASE("Tour initilization testing", "[Tour]"){
    unsigned int ** distancesMock = initDistanceArray();
    CandidateLists * cl = new CandidateLists((unsigned int)CANDIDATE_LIST_SIZE, distancesMock);
    Tour t(10, cl);
    for(int i = 0; i < 8; i++){
        t.add(i, i*10);
    }
    CHECK(!t.isTourComplete());

    CHECK(t.getSize() == 8);
    CHECK(t.getTotalTourDistance() == 280);
    t.add(10, 22);
    t.add(11, 54);
    t.printTour();
    CHECK(t.getSize() == 11);
    CHECK(t.isTourComplete());
    
    CHECK(t.getNode(8) == 10);
    CHECK(t.getDistance(8) == 22);
    deleteDistance(distancesMock);
    delete cl;
}

TEST_CASE("Tour reset tests", "[Tour]"){
    unsigned int ** distancesMock = initDistanceArray();
    CandidateLists * cl = new CandidateLists((unsigned int)CANDIDATE_LIST_SIZE, distancesMock);
    Tour t(10, cl);

    for(unsigned int i = 0; i < 8; i++){
        t.add(i, i*10);
    }
    t.add(10, 22);
    t.add(11, 54);
    t.reset();

    t.add(4,33);
    t.add(55,77);
    t.printTour();
    CHECK(t.getTotalTourDistance() == 110);
    CHECK(t.getSize() == 2);

    deleteDistance(distancesMock);
    delete cl;
}


TEST_CASE("Tour nextUnvisitedNode tests", "[Tour]"){
    unsigned int ** distancesMock = initDistanceArray();
    CandidateLists * cl = new CandidateLists((unsigned int)CANDIDATE_LIST_SIZE, distancesMock);
    Tour t(10, cl);
    
    Node n;
    t.add(0,0);
    while(!t.isTourComplete()){
        t.nextUnvisitedNode(n);
        t.add(n);
    }

    t.printTour();


    deleteDistance(distancesMock);
    delete cl;
}


   





