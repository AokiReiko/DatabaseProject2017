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
    if (fileName == NULL || recordSize <= 0) return -1;
    //判断recordsize是否合法
    int recordNum = calcRecNumPerPage(recordSize);
    int bitmapSize = calcBitmapSize(recordSize);
    if (recordNum <= 0 ) {
        return -1;
    }

    bool canCreatd = FM->createFile(fileName);
    if (!canCreatd) return -1;
    int fileID, index;
    FM->openFile(fileName, fileID);
    BufPageManager* BPM = new BufPageManager(this->FM);
    BufType buff = BPM->allocPage(fileID, 0, index, true);

    RM_FileHeader fheader;
    fheader.recordSize = recordSize;
    fheader.numRecordsPerPage = recordNum;
    fheader.numPages = 1;
    fheader.firstFreePage = -1;
    fheader.bitmapOffset = RM_FILEHEADER_SIZE;
    fheader.bitmapSize = bitmapSize;
    memcpy(buff, &fheader, RM_FILEHEADER_SIZE);
    BPM->markDirty(index);
    BPM->close();
    delete BPM;
    FM->closeFile(fileID);//是否需要判断成功？

    return 0;

}// Create a new file
RC RM_Manager::DestroyFile (const char *fileName) {
    remove(fileName);
}// Destroy a file
RC RM_Manager::OpenFile (const char *fileName, RM_FileHandle &fileHandle) {
    if (fileHandle.hasFileOpened) {
        this->CloseFile(fileHandle);
    }
    int id;
    //文件名不合法或无法打开则返回-1
    if ( fileName == NULL || (!FM->openFile(fileName, id)))
        return -1;
    fileHandle.initialize(FM, id, fileName);
}// Open a file
RC RM_Manager::CloseFile (RM_FileHandle &fileHandle) {
    if (FM->closeFile(fileHandle.fileId)) {
        fileHandle.hasFileOpened = false;
        return 0;
    }
    return -1;
} // Close a file