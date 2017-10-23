#include "DataUnit.h"

DataUnit::DataUnit(){
    size = 0;
    data = NULL;
}

DataUnit::~DataUnit(){
    size = 0;
    if (data != NULL){
        free(data);
        data = NULL;
    }
}

bool DataUnit::getIsNull(){
    return isNull;
}

bool DataUnit::getIsNullable(){
    return isNullable;
}

int DataUnit::getSize(){
    return size;
}

void DataUnit::showData(string hint){
    printf("Printing "+hint+"\n");
    for (int i=0;i<size;i++){
        printf("No. %d is : %c\n",i,data[i]);
    }
    printf("Print end.\n");
}