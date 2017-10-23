#ifndef RMINTERNAL_H
#define RMINTERNAL_H
#include "../filesystem/utils/pagedef.h"
typedef int PageNum;
typedef int SlotNum;
#define RM_PAGEHEADER_SIZE sizeof(RM_PageHeader)
#define RM_FILEHEADER_SIZE sizeof(RM_FileHeader)

#define RM_INVALID_RECORD -1
#define RM_INVALID_RECORD_SIZE -2
#define RM_INVALIDRID -3
#define RM_NO_FREE_PAGES -4

// RM_FileHeader: Header for each file
struct RM_FileHeader {
  int recordSize;           // record size in file
  int numRecordsPerPage;    // calculated max # of recs per page
  int numPages;             // number of pages
  PageNum firstFreePage;    // pointer to first free object

  int bitmapOffset;         // location in bytes of where the bitmap starts
                            // in the page headers
  int bitmapSize;           // size of bitmaps in the page headers
  
};
struct RM_PageHeader {
    int nextFreePage;
    int recordsNum;
};
int calcRecNumPerPage(int recordSize);
int calcBitmapSize(int recordSize);


#endif RMINTERNAL_H