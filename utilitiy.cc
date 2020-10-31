
template<typename T>
T**  new_2D_array(int rows, int columns){
    T temp = new T*[rows];
    for(int i = 0; i < columns; i++)
    {
        temp[i] = new T[columns]
    }
    return temp;
}
template<typename T>
void delete_2D_array(int rows, int columns, T ** arr){
    for(int i = 0; i < columns; i++){
        delete[] T[i];
    }
    delete[] T;
}