#include "RM/rm.h"
#include <iostream>
using namespace std;

int main() {
    FileManager* file_manager = new FileManager();
    RM_Manager* rm_manager = new RM_Manager(file_manager);

}