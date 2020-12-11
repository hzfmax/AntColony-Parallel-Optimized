#include "Tour.h"
#include "CandidateLists.h"


enum OptCase {
    case1,   
    case6, 
    case7   
};

struct Move {
    Move(unsigned int ii, unsigned int jj, unsigned int kk, OptCase c, long g) : i(ii), j(jj), k(kk), optCase(c), gain(g){};
    void set(unsigned int ii, unsigned int jj, unsigned int kk, OptCase c, long g){
        i = ii;
        j = jj;
        k = kk;
        optCase = c;
        gain = g;
    }
    unsigned int i,j,k;
    OptCase optCase;
    long gain;
};

class ThreeOpt{
    public:
    ThreeOpt(Tour* tour, CandidateLists * cl);
    ~ThreeOpt();
    void optimize();
    
    private:
    long gainFrom2Opt(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2);
    long gainFrom3Opt(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2, unsigned int z1, unsigned int z2, OptCase c);
    void makeMove(const Move& move);
    bool oneCity3Opt(unsigned int start);
    void clearDLB();
    bool between(unsigned int a, unsigned int x, unsigned int b);
    Tour* tour;
    CandidateLists *cl;
    bool* dontLookBit;
   
};
