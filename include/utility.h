#ifndef U_H
#define H


template<typename T>
T**  new_2D_array(int rows, int columns);

template<typename T>
void delete_2D_array(int rows, int columns, T ** arr);

template<typename T>
T**  new_2D_array(int rows, int columns){
    T* flat = new T[rows*columns];
    T** two_D = new T*[rows];
    for(int i = 0; i < rows; i++)
    {
        two_D[i] =  flat + i*rows;
    }
    return two_D;
}
template<typename T>
void delete_2D_array(int rows, int columns, T ** arr){
    delete[] arr[0];
    delete[] arr;
}

#endif 