#include "IntDataUnit.h"

IntDataUnit::IntDataUnit(int showLength){
    this.size = 4;
    data = (char*)(new int);
    memset(data,0,size+1);
    this.isNullable = true;
    this.isNull = false;
    this.showLength = showLength;
}

IntDataUnit::IntDataUnit(int showLength, bool canNull){
    IntDataUnit(showLength);
    this.isNullable = canNull;
}

IntDataUnit::~IntDataUnit(){
}

int getRealSize(){
    return this.size+1;
}

bool readData(char* input, int length, int& pos){
    if (length != 4) return false;
    memcpy(data,input,this.size);
    pos += this.size;
    return true;
}

bool writeData(char* output, int &pos){
    memcpy(output,data,this.size);
    position += this.size;
    return true;
}

bool changeData(char* input){
    
}