#include <stdio.h>
#include "write_results.h"
void writeResults(const char* header,const char * result) {
   FILE * resultsFile = fopen("_temp_output.csv", "r");
   if(resultsFile == NULL){
      resultsFile = fopen("_temp_output.csv", "w");
      fprintf(resultsFile, "%s\n", header);
   };
   fclose(resultsFile);
   resultsFile = fopen("_temp_output.csv", "a");
   fprintf(resultsFile, "%s\n", result);
   fclose(resultsFile);
}