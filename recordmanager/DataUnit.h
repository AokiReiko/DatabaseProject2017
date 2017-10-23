#ifndef DATABASETYPE_H
#define DATABASETYPE_H
#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;
#define IS_NULL 1
#define IS_NOT_NULL 0

class DataUnit{
public:
    DataUnit();
    char* getData();
    ~DataUnit();
    void showData(string hint);
    bool getIsNullable();
    bool getIsNull();
    int getSize();
    virtual bool readData(char* input, int length, int& pos) = 0;
    virtual bool writeData(char* output, int &pos) = 0;
    virtual bool changeData(char* input) = 0;
    virtual bool printData(char* output) = 0;
    virtual string getType() = 0;

protected:
    int size;
    char* data;
    bool isNullable;
    bool isNull;
};

#endif