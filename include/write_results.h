# ifndef WRITE_RESULST_H
# define WRITE_RESULST_H

// Writes out data to _temp_data.csv
// header will only be placed in the first line and is only used if the file doesn't exist
// result is the data to be written
void writeResults(const char* header,const char * result);

# endif