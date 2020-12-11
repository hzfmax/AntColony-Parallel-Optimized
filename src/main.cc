#include "AntColony.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <ctype.h>
#include <sstream>
#include "timer.h"
using namespace std;

vector<pair<float, float>> readTspFile(const string & filename);

int main(int argc, char * argv[])
{
    if(argc < 2){
        cerr << "Missing required argumenets. Please provide a .tsp file for proccessing\n";
        return -1;
    }
    int iterations = 1;
    int numberOfPoints = 0;
    if(argc > 2)
        iterations = atoi(argv[2]);
     if(argc > 3)
         numberOfPoints =atoi(argv[3]);
    
    vector<pair<float, float>> tspData = readTspFile(argv[1]);
    if(numberOfPoints){
    tspData.resize(numberOfPoints);
    }
    initialize_timer();
    AntColony ac(tspData);
    start_timer();
    ac.optimize(iterations);
    stop_timer();

    cout << "The best tour length was: " << ac.getShortestTourLength() << " Found in :" << elapsed_time() << endl;
    char data [250];
    sprintf(data, "%lf", elapsed_time());
    writeResults("Time", data);
    return 0;
}

// TODO: Improve parsing robustness.
// TODO: create a TSP class that stores the file being read in with all it's associated data. 
// TODO: Improve that TSP class to allow generation of new tsp problems, and supporting async structures, ect and make my AntColony take in an object of that type to allow easy use.
vector<pair<float, float>> readTspFile(const string & filename)
{
    ifstream tspFile(filename);
    string line;
    stringstream stream;
    vector<pair<float, float>> tspData;
    if(!tspFile){
        cerr << "Bad file name.\n";
        exit(-1);
    }
    while(!isdigit(tspFile.peek())){
        getline(tspFile,line);
    }
    int temp;
    float x, y;
    while(getline(tspFile, line)){
        stream << line;
        stream >> temp >> x >> y;
        if(stream){
        tspData.push_back(make_pair(x,y));
        }
    }
    return tspData;
}