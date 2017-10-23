#ifndef RM_H
#define RM_H

#include "../filesystem/bufmanager/BufPageManager.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/utils/pagedef.h"
#include "rm_internal.h"
#include "../database.h"
#include <iostream>
class RM_FileHandle;
class RID;
class RM_Record;


/* handles the creation, deletion, opening, and closing of files of records
in the RM component. Your program should create exactly one instance of this
class, and all requests for RM component file management should be directed 
to that instance. */
class RM_Manager {
public:
    RM_Manager (FileManager* FM);            // Constructor
    ~RM_Manager ();                           // Destructor
    RC CreateFile (const char *fileName, int recordSize);  
                                                // Create a new file
    RC DestroyFile (const char *fileName);       // Destroy a file
    RC OpenFile (const char *fileName, RM_FileHandle &fileHandle);
                                                // Open a file
    RC CloseFile (RM_FileHandle &fileHandle);  // Close a file
private:
    FileManager* FM;
};
/*The RM_FileHandle class is used to manipulate the records in an open RM 
component file. To manipulate the records in a file, a client first creates 
an instance of this class and passes it to the RM_Manager::OpenFile method 
described above.*/
class RM_FileHandle {
friend class RM_Manager;
friend class RM_FileScan;
public:
    RM_FileHandle ();                                  // Constructor
    ~RM_FileHandle ();                                  // Destructor
    RC GetRec (const RID &rid, RM_Record &rec) const;
                                                           // Get a record
    RC InsertRec (const char *pData, RID &rid);       // Insert a new record,
                                                           //   return record id
    RC DeleteRec (const RID &rid);                    // Delete a record
    RC UpdateRec (const RM_Record &rec);              // Update a record
    RC GetNextRec (PageNum page, SlotNum slot, RM_Record &rec, BufType &buf, bool nextPage)；
                                                        //get next available record
    RC ForcePages (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
                                                           //   to disk
    bool readyForScan() const;
    RC initialize(FileManager* FM, int id, string filename);
    void setHeader(const RM_FileHeader& header);
    RC getPageHeaderAndBitmap(BufType buffer, char* &bitmap, struct RM_PageHeader* &pheader);
    RC getFirstZero(char* bitmap, int recordNum, int &slot);
    RC getNextOne(char* bitmap, int recordNum, int start, int &slot);
    RC setBitToOne(char* bitmap, int recordNum, int slot);
    RC setBitToZero(char* bitmap, int recordNum, int slot);
    RC checkIsOne(char* bitmap, int recordNum, int slot, bool &flag);

private:
    FileManager* FM;
    BufPageManager* BPM;
    int fileId;
    RM_FileHeader fheader;
    string filename;
    bool hasFileOpened;//FM默认打开一个文件，在打开别的文件初始化handler时要判断是否已经打开了
};
/*The RM_FileScan class provides clients the capability to perform scans over 
the records of an RM component file, where a scan may be based on a specified 
condition. As usual, the constructor and destructor methods are not described*/
class RM_FileScan {
public:
    RM_FileScan  ();                                // Constructor
    ~RM_FileScan ();                                // Destructor
    RC OpenScan (const RM_FileHandle &fileHandle,  // Initialize file scan
                    AttrType      attrType,
                    int           attrLength,
                    int           attrOffset,
                    CompOp        compOp,
                    void          *value,
                    ClientHint    pinHint = NO_HINT);
    RC GetNextRec   (RM_Record &rec);                  // Get next matching record
    RC CloseScan    ();                                // Terminate file scan

private:
    RC GetNumRecOnPage(BufType buff, int &numRecords);

    RM_FileHandle* fh;
    bool (*comparator) (void*, void*, AttrType, int);
    int offset;
    int length;
    AttrType type;
    CompOp comp_op;
    void* value;

    bool opened;
    bool ended;

    PageNum currentPage;
    SlotNum currentSlot;

    int numRecOnPage;
    int numRecScanned;


};


/*RID uniquely identifies a record within a given file, based on the record's
 page number in the file and slot number within that page. */
class RID {
public:
    RID ();                        // Default constructor
    ~RID ();                        // Destructor
    RID (PageNum pageNum, SlotNum slotNum);
                                             // Construct RID from page and
                                             //   slot number
    RC GetPageNum (PageNum &pageNum) const;  // Return page number
    RC GetSlotNum (SlotNum &slotNum) const;  // Return slot number
    bool isValidId();

private:
    PageNum pageNum;
    SlotNum slotNum;
};
/*The RM_Record class defines record objects. To materialize a record, a 
client creates an instance of this class and passes it to one of the 
RM_FileHandle or RM_FileScan methods that reads a record (described above).
RM_Record objects should contain copies of records from the buffer pool, 
not records in the buffer pool itself.*/
class RM_Record {
friend RM_FileHandle;
public:
    RM_Record  ();                     // Constructor
    ~RM_Record ();                     // Destructor

    RM_Record& operator= (const RM_Record &record);
    RC GetData    (char *&pData) const;   // Set pData to point to
                                        //   the record's contents
    RC GetRid     (RID &rid) const;       // Get the record id
    // Sets the record with an RID, data contents, and its size
    RC SetRecord (RID rec_rid, char *recData, int size);
private:
    RID rid;        // record RID
    char* data;    // pointer to record data. This is stored in the
                    // record object, where its size is malloced
    int size;       // size of the malloc
};
bool EQ(void* a, void* b, AttrType type, int length);
bool NE(void* a, void* b, AttrType type, int length);
bool LT(void* a, void* b, AttrType type, int length);
bool LE(void* a, void* b, AttrType type, int length);
bool GT(void* a, void* b, AttrType type, int length);
bool GE(void* a, void* b, AttrType type, int length);
#endif RM_H