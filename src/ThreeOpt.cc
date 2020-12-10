
#include "ThreeOpt.h"
#include "CandidateLists.h"
#include <iostream>
using namespace std;
// Code inspired and derived from http://tsp-basics.blogspot.com/2017/04/3-opt-with-neighbor-lists-and-dlb.html

ThreeOpt::ThreeOpt(Tour* tour, CandidateLists* cl) {
    this->tour = tour;
    this->cl = cl;
    this->dontLookBit = new bool[cl->getNumberOfNodes()];
    clearDLB();
}

long ThreeOpt::gainFrom2Opt(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2){
    long delLength =  cl->distance(x1, x2) + cl->distance(y1,y2);
    long addLength =  cl->distance(x1, y1) + cl->distance(x2,y2);
    return delLength - addLength;
}

long ThreeOpt::gainFrom3Opt(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2, unsigned int z1, unsigned int z2, OptCase c){
    long addLength = 0;
    switch(c){
        case case6:
            addLength = cl->distance(x1, y2) + cl->distance(z1, y1) + cl->distance(x2, z2);
            break;
        case case7:
            addLength = cl->distance(x1, y2) + cl->distance(z1, x2) + cl->distance(y1, z2);
            break;
        case case1:
            break;
    }

     long delLength = cl->distance(x1, x2) + cl->distance(y1, y2) + cl->distance(z1, z2);
     return delLength - addLength;
}

void ThreeOpt::makeMove(const Move& move){

    unsigned int N = cl->getNumberOfNodes();
    switch(move.optCase){
        case case1:    
            tour->reverse((move.k+1) % N, move.i);   
            break;
        case case6:
            tour->reverse((move.k+1) % N, move.i);
            tour->reverse((move.j+1) % N, move.k);
            break;
        case case7:
            tour->reverse((move.k+1) % N, move.i); 
            tour->reverse((move.i+1) % N, move.j);
            tour->reverse((move.j+1) % N, move.k);
            break;
    }
    
}

bool ThreeOpt::oneCity3Opt(unsigned int start){
    bool improved = false;
    unsigned int N = cl->getNumberOfNodes();
    unsigned int i, j;
    long gainExpected = 0;
    Move goodMove(0,0,0,case1,0);
    for(int toggle = 0; toggle < 2; toggle++){
        if(toggle)
            i  = (N + start - 1) % N;
        else
            i = start;
        unsigned int x1 = tour->getNode(i);
        unsigned int x2 = tour->getNode((i+1) % N);

        for(unsigned int neighborIndex1 = 0; neighborIndex1 < cl->size(); neighborIndex1++){
            unsigned int y2 = cl->getNode(x1,neighborIndex1);
            j = (tour->getNodePositionInTour(y2) + N - 1) % N;
            unsigned int y1 = tour->getNode(j);

            if(y1 != x1 && y1 != x2){
                gainExpected = gainFrom2Opt(x1,x2,y1,y2);
                if (gainExpected > goodMove.gain){
                    improved = true;
                    goodMove.set(i,j,j,case1, gainExpected);
                }
            }

            for(unsigned int neighborIndex2 = 0; neighborIndex2 < cl->size(); neighborIndex2++){
                unsigned int z1_6 = cl->getNode(y1,neighborIndex2);
                unsigned int k_6 = tour->getNodePositionInTour(z1_6);
                unsigned int z2_6 = tour->getNode((k_6 + 1) % N);
                

                unsigned int z2_7 = z1_6;
                unsigned int k_7 = (tour->getNodePositionInTour(z2_7) + N - 1) % N;
                unsigned int z1_7 = tour->getNode(k_7);

                if (between(i,j,k_6)){
                    gainExpected = gainFrom3Opt(x1,x2,y1,y2,z1_6,z2_6,case6);

                    if (gainExpected > goodMove.gain){
                        improved = true;
                        goodMove.set(i,j,k_6,case6, gainExpected);
                    }            

                }

                if (between(i,j,k_7)){
                    gainExpected = gainFrom3Opt(x1,x2,y1,y2,z1_7,z2_7,case7);

                    if (gainExpected > goodMove.gain){
                        improved = true;
                        goodMove.set(i,j,k_7,case7, gainExpected);
                    }            
                }
            }
        }
        
    }
    if (improved){
        dontLookBit[goodMove.i] = false;
        dontLookBit[goodMove.j] = false;
        dontLookBit[goodMove.k] = false;
        dontLookBit[(goodMove.i + 1) % N] = false;
        dontLookBit[(goodMove.j + 1) % N] = false;
        dontLookBit[(goodMove.k + 1) % N] = false;
        makeMove(goodMove);
    }
    return improved;
}

void ThreeOpt::clearDLB(){
    for(unsigned int i = 0; i < cl->getNumberOfNodes(); i++){
        dontLookBit[i] = false;
    }
}

bool ThreeOpt::between(unsigned int a, unsigned int x, unsigned int b){
  if (b > a)
    return (x > a) && (x < b);
  else if (b < a)
    return (x > a) || (x < b);
  return false;
}

void ThreeOpt::optimize(){
    bool optimal = false;
    
    while(!optimal){
        optimal = true;
        for(unsigned int tourIndex = 0; tourIndex < cl->getNumberOfNodes(); tourIndex++){
            unsigned int node = tour->getNode(tourIndex);

            if(!dontLookBit[node]) {
                if (oneCity3Opt(tourIndex)) 
                    optimal = false;
                else
                    dontLookBit[node] = true;
                }
        }
    }
}