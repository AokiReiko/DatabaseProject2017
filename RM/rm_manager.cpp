#include "rm.h"

/* RM_Manager handles the creation, deletion, opening, and closing of files of records
in the RM component. Your program should create exactly one instance of this
class, and all requests for RM component file management should be directed 
to that instance. */
RM_Manager::RM_Manager(FileManager* FM) {
    this->FM = FM;
 }// Constructor
RM_Manager::~RM_Manager () {

}// Destructor

RC RM_Manager::CreateFile (const char *fileName, int recordSize) {
    if (fileName == NULL) return -1;
    //判断recordsize是否合法
    int recordNum = calcRecNumPerPage(recordSize);
    if (recordNum <= 0) {
        return -1;
    }



    BufPageManager* BPM = new BufPageManager(this->FM);


}// Create a new file
RC RM_Manager::DestroyFile (const char *fileName) {
    
}// Destroy a file
RC RM_Manager::OpenFile (const char *fileName, RM_FileHandle &fileHandle) {

}// Open a file
RC RM_Manager::CloseFile (RM_FileHandle &fileHandle) {

} // Close a file