#include "DataUnit.h"

#define __null 1
#define __notnull 0

class IntDataUnit : public DataUnit{
public:
    IntDataUnit(int showLength);
    IntDataUnit(int showLength, bool canNull);
    ~IntDataUnit();
    bool readData(char* input, int length, int& pos);
    bool writeData(char* output, int &pos);
    bool changeData(char* input);
    int getShowLength();
    string getType();

private:
    int showLength;
}