// @(#)root/io:$Id$
// Author: Elvin Sindrilaru   19/05/2011

/*************************************************************************
 * Copyright (C) 1995-2011, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TFilePrefetch
#define ROOT_TFilePrefetch

#ifndef ROOT_TFile
#include "TFile.h"
#endif
#ifndef ROOT_TThread
#include "TThread.h"
#endif
#ifndef ROOT_TFPBlock
#include "TFPBlock.h"
#endif
#ifndef ROOT_TSemaphore
#include "TSemaphore.h"
#endif
#ifndef ROOT_TMD5
#include "TMD5.h"
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif
#ifndef ROOT_TObjString
#include "TObjString.h"
#endif
#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif
#ifndef ROOT_TStopwatch
#include "TStopwatch.h"
#endif

#include <condition_variable>
#include <mutex>


class TFilePrefetch : public TObject {

private:
   TFile      *fFile;              // reference to the file
   TList      *fPendingBlocks;     // list of pending blocks to be read
   TList      *fReadBlocks;        // list of blocks read
   TThread    *fConsumer;          // consumer thread
   std::mutex fMutexPendingList;   // mutex for the pending list
   std::mutex fMutexReadList;      // mutex for the list of read blocks
   std::condition_variable fNewBlockAdded;  // signal the addition of a new pending block
   std::condition_variable fReadBlockAdded; // signal the addition of a new red block
   TSemaphore *fSemMasterWorker;   // semaphore used to kill the consumer thread
   TSemaphore *fSemWorkerMaster;   // semaphore used to notify the master that worker is killed
   TSemaphore *fSemChangeFile;     // semaphore used when changin a file in TChain
   TString     fPathCache;         // path to the cache directory
   TStopwatch  fWaitTime;          // time wating to prefetch a buffer (in usec)
   Bool_t      fThreadJoined;      // mark if async thread was joined

   static TThread::VoidRtnFunc_t ThreadProc(void*);  //create a joinable worker thread

public:
   TFilePrefetch(TFile*);
   virtual ~TFilePrefetch();

   void      ReadAsync(TFPBlock*, Bool_t&);
   void      ReadListOfBlocks();

   void      AddPendingBlock(TFPBlock*);
   TFPBlock *GetPendingBlock();

   void      AddReadBlock(TFPBlock*);
   Bool_t    ReadBuffer(char*, Long64_t, Int_t);
   void      ReadBlock(Long64_t*, Int_t*, Int_t);
   TFPBlock *CreateBlockObj(Long64_t*, Int_t*, Int_t);

   TThread  *GetThread() const;
   Int_t     ThreadStart();

   Bool_t    SetCache(const char*);
   Bool_t    CheckBlockInCache(char*&, TFPBlock*);
   char     *GetBlockFromCache(const char*, Int_t);
   void      SaveBlockInCache(TFPBlock*);

   Int_t     SumHex(const char*);
   Bool_t    BinarySearchReadList(TFPBlock*, Long64_t, Int_t, Int_t*);
   Long64_t  GetWaitTime();

   void      SetFile(TFile*);
   std::condition_variable &GetCondNewBlock() { return fNewBlockAdded; };
   void      WaitFinishPrefetch();

   ClassDef(TFilePrefetch, 0);  // File block prefetcher
};

#endif
