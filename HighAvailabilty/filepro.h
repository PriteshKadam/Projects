
#include"stdafx.h"

   WIN32_FIND_DATA FileData; 
   HANDLE hSearch; 
   HWND hdlg,hWnd;

   static HWND  hwndInit,hwndMonitor,hwndRecover,hwndExit;
   static HWND hWndEditSrc,hWndEditDest,hStatic1,hStatic2,hWndSet ;
  
   static TCHAR szDestPath[MAX_PATH]=  TEXT("D:\\BACKUP"); 
  
   static TCHAR szSrcPath[MAX_PATH]=TEXT("C:\\TC");

   static TCHAR szNewPath[MAX_PATH];
   static TCHAR szOrigPath[MAX_PATH];
   static TCHAR  init_path[MAX_PATH];
   static TCHAR  szAddPath[MAX_PATH];
   static  TCHAR   szRemovePath[MAX_PATH];
  

   BOOL fFinished = FALSE; 
   
   
   static HINSTANCE hMyInstance;
   static int flag = 0;
   int ret;

	
	HANDLE		m_hDirectory;

	OVERLAPPED	m_Overlapped;

    FILE_NOTIFY_INFORMATION * pInfo;

	BYTE m_Buffer[200];

	BYTE m_BackupBuffer[100];
    DWORD dwBytes;
    SHFILEOPSTRUCT s = { 0 };

	

void initiate()
{
   fFinished = FALSE; 
 //return;
// Create a new directory. 

 /*  if (!CreateDirectory(szDestPath, NULL)) 
   { 
      printf("Could not create new directory.\n"); 
      //return;
   }*/

ret=SHCreateDirectoryEx(NULL,szSrcPath,NULL);
switch(ret)
{
	
case ERROR_SUCCESS:
	  MessageBox(NULL,L"SOURCE_FILE_DOES_NOT_EXIST",L"H.A.S",MB_OK);
	  ZeroMemory(&s,sizeof(s));
	  s.wFunc = FO_DELETE;
	  s.pTo = NULL;
	  s.pFrom = szSrcPath;
	  s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	  SHFileOperation( &s );
     return;
	 

case ERROR_BAD_PATHNAME:
	MessageBox(NULL,L"BAD_SOURCE_PATHNAME",L"H.A.S",MB_OK);
     return;

case ERROR_PATH_NOT_FOUND:
	MessageBox(NULL,L"SOURCE_PATH_NOT_FOUND",L"H.A.S",MB_OK);
     return;

case ERROR_ALREADY_EXISTS:
	 break;
}


ret=SHCreateDirectoryEx(NULL,szDestPath,NULL);


switch(ret)
{

case ERROR_SUCCESS:
	//MessageBox(NULL,L"SUCCESS",L"H.A.S",MB_OK);
    break;


case ERROR_BAD_PATHNAME:
	MessageBox(NULL,L"BAD_PATHNAME",L"H.A.S",MB_OK);
     return;

case ERROR_PATH_NOT_FOUND:
	MessageBox(NULL,L"PATH_NOT_FOUND",L"H.A.S",MB_OK);
     return;

case ERROR_ALREADY_EXISTS:
	 ret=MessageBox(NULL,L"DESTINATION_DIRECTORY_ALREADY_EXISTS. Overwrite??",L"H.A.S",MB_YESNO);
	 if(ret==IDYES)
	 {
      ZeroMemory(&s,sizeof(s));
	  s.wFunc = FO_DELETE;
	  s.pTo = NULL;
	  s.pFrom = szDestPath;
	  s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	  SHFileOperation( &s );
	  SHCreateDirectoryEx(NULL,szDestPath,NULL);
		 break;
	 }
	 else
		return;

case ERROR_CANCELLED:
	return;
}

  lstrcpy(szNewPath,szDestPath);
  lstrcat(szNewPath, TEXT("\\"));

// Start searching for files/folders in the current directory/drive. 

  lstrcpy(szOrigPath,szSrcPath);
  lstrcat(szOrigPath, TEXT("\\*.*"));
   
   //hSearch = FindFirstFile(TEXT("C:\\TC\\*.*"), &FileData); 
  hSearch = FindFirstFile(szOrigPath, &FileData);

   if (hSearch == INVALID_HANDLE_VALUE) 
   { 
      printf("No text files found.\n"); 
      return;
   } 
   
 
// Copy each file to the new directory 

	
   while (!fFinished) 
   { 
	 //  TCHAR szOrigPath[MAX_PATH] = TEXT("C:\\TC\\");
	   ZeroMemory(&init_path[0],MAX_PATH);
       lstrcpy(init_path,szSrcPath);
	   lstrcat(init_path, TEXT("\\"));

       ZeroMemory(&szNewPath[0],MAX_PATH);
	   lstrcpy(szNewPath,szDestPath);
       lstrcat(szNewPath, TEXT("\\"));


       lstrcat(szNewPath, FileData.cFileName); 
	   lstrcat(init_path, FileData.cFileName); 
       
	   //SHFILEOPSTRUCT s = { 0 };
	   s.wFunc = FO_COPY;
	   s.pTo = szNewPath;
	   s.pFrom = init_path;
	   s.fFlags = FOF_SIMPLEPROGRESS|FOF_NOERRORUI|FOF_NOCONFIRMATION;
	   s.lpszProgressTitle=TEXT("INITIATING BACKUP");
	   //Copy folders(initiate backup)
	   SHFileOperation( &s );
	   if(s.fAnyOperationsAborted==TRUE)
		   break;
	
 
      if (!FindNextFile(hSearch, &FileData)) 
      {
         if (GetLastError() == ERROR_NO_MORE_FILES) 
         { 
            MessageBox(NULL,L"INIT SUCCESSFUL",L"H.A.S",MB_OK); 
            fFinished = TRUE; 
         } 
         else 
         { 
           MessageBox(NULL,L"INIT_ERROR",L"H.A.S",MB_OK); 
            return;
         } 
      }
   } 
 
// Close the search handle. 
 
   FindClose(hSearch);
}


void BeginRead(HANDLE hDir)
{
BOOL success = ReadDirectoryChangesW(hDir,&m_Buffer[0],/*m_Buffer.size()*/MAX_PATH,
                    TRUE,FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|
					FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE|
					FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_ATTRIBUTES
					|FILE_NOTIFY_CHANGE_LAST_ACCESS,&dwBytes,NULL,NULL);

  printf("ERROR:%d",GetLastError());
}



void monitor()
{
 char* pBase = (char*)&m_Buffer[0];

 FILE_NOTIFY_INFORMATION& fni = (FILE_NOTIFY_INFORMATION&)*pBase;
 
 int ch;

 while(1)
      {
 
  lstrcpy(szNewPath,szDestPath);
  lstrcat(szNewPath, TEXT("\\"));

  lstrcpy(szOrigPath,szSrcPath);
  lstrcat(szOrigPath, TEXT("\\"));
   
   ZeroMemory(&init_path[0],MAX_PATH);
   ZeroMemory(&szAddPath[0],MAX_PATH);
   ZeroMemory(&szRemovePath[0],MAX_PATH);
  // ZeroMemory(&szDestPath[0],MAX_PATH);

   lstrcpy(init_path,szOrigPath);
   lstrcpy(szAddPath,szNewPath);
   lstrcpy(szRemovePath,szNewPath);
 

  HANDLE hDir = CreateFile(szSrcPath,FILE_LIST_DIRECTORY,
                           FILE_SHARE_READ| FILE_SHARE_WRITE| FILE_SHARE_DELETE,
                           NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
                           NULL);                 
   


   ZeroMemory(&m_Buffer[0],MAX_PATH);
   ZeroMemory(&s,sizeof(s));

   BeginRead(hDir);

   pBase = (char*)&m_Buffer[0];

   fni = (FILE_NOTIFY_INFORMATION&)*pBase;

    switch(fni.Action)
      {
     case FILE_ACTION_RENAMED_OLD_NAME:
		  
		  pInfo = (FILE_NOTIFY_INFORMATION *)m_Buffer;
		  if (pInfo->NextEntryOffset > 0)
            {
                  // move to the next record
                  pInfo = (FILE_NOTIFY_INFORMATION *)
                                          (((CHAR *) pInfo) + pInfo->NextEntryOffset);
			   lstrcat(szAddPath, pInfo->FileName);
               lstrcat(init_path, pInfo->FileName);
               s.wFunc = FO_COPY;
	           s.pTo = szAddPath;
	           s.pFrom = init_path;
			   s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
			   SHFileOperation( &s );
  
               ZeroMemory(&szAddPath[0],MAX_PATH);
               lstrcpy(szAddPath,szNewPath);
			   lstrcat(szAddPath, fni.FileName);

			   ZeroMemory(&s,sizeof(s));
			   s.wFunc = FO_DELETE;
	           s.pTo = NULL;
	           s.pFrom = szAddPath;
	           s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
			   SHFileOperation( &s );
			  }
            else
                  pInfo = NULL;
	      break;

	 case FILE_ACTION_ADDED:
          
		   lstrcat(szAddPath, fni.FileName);
           lstrcat(init_path, fni.FileName);
             s.wFunc = FO_COPY;
	         s.pTo = szAddPath;
	         s.pFrom = init_path;
			 s.fFlags = FOF_SILENT;
			 SHFileOperation( &s );

             break;
  
     case FILE_ACTION_REMOVED:
	          ch=MessageBox(NULL,L"REMOVE BACKUP ENTRY..?",L"HAS",MB_YESNO);
	          if(ch==IDYES)
	           {   
		     
		        lstrcat(szRemovePath, fni.FileName);
		         s.wFunc = FO_DELETE;
	             s.pFrom = szRemovePath;
			     s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
			     SHFileOperation( &s );
				 do
                 {
				    pInfo = (FILE_NOTIFY_INFORMATION *)m_Buffer;
				    if (pInfo->NextEntryOffset > 0)
                    {
                      // move to the next record
                       pInfo = (FILE_NOTIFY_INFORMATION *)
                                          (((CHAR *) pInfo) + pInfo->NextEntryOffset);
				      }
				       else
                       pInfo = NULL;
					}while(NULL != pInfo);

				//printf("ERROR:%d",GetLastError());
			  }
			  break;

	 case FILE_ACTION_MODIFIED:
              // MessageBox(NULL,L"File Added..",L"HAS",MB_OK);
			   lstrcat(szAddPath, fni.FileName);
               lstrcat(init_path, fni.FileName);
               s.wFunc = FO_COPY;
	           s.pTo = szAddPath;
	           s.pFrom = init_path;
			   s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
			   SHFileOperation( &s );
           	   break;

      
	}
  }
}


void recover()
{

fFinished=0;

ret=SHCreateDirectoryEx(NULL,szSrcPath,NULL);

switch(ret)
{

/*case ERROR_SUCCESS:
	MessageBox(NULL,L"SUCCESS",L"H.A.S",MB_OK);
    break;*/


case ERROR_BAD_PATHNAME:
	MessageBox(NULL,L"BAD_PATHNAME",L"H.A.S",MB_OK);
     return;

case ERROR_PATH_NOT_FOUND:
	MessageBox(NULL,L"PATH_NOT_FOUND",L"H.A.S",MB_OK);
     return;

case ERROR_ALREADY_EXISTS:
	 ZeroMemory(&s,sizeof(s));
	  s.wFunc = FO_DELETE;
	  s.pTo = NULL;
	  s.pFrom = szSrcPath;
	  s.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	  SHFileOperation( &s );
	  SHCreateDirectoryEx(NULL,szSrcPath,NULL);
	  break;
	 
case ERROR_CANCELLED:
	return;
}

  lstrcpy(szNewPath,szSrcPath);
  lstrcat(szNewPath, TEXT("\\"));

// Start searching for files/folders in the current directory/drive. 

  lstrcpy(szOrigPath,szDestPath);
  lstrcat(szOrigPath, TEXT("\\*.*"));
   
   //hSearch = FindFirstFile(TEXT("C:\\TC\\*.*"), &FileData); 
  hSearch = FindFirstFile(szOrigPath, &FileData);

   if (hSearch == INVALID_HANDLE_VALUE) 
   { 
      MessageBox(NULL,L"INVALID_HANDLE_VALUE",L"H.A.S",MB_OK);
      return;
   } 
   
 
// Copy each file to the new directory 

	
   while (!fFinished) 
   { 
	 
	   ZeroMemory(&init_path[0],MAX_PATH);
       lstrcpy(init_path,szDestPath);
	   lstrcat(init_path, TEXT("\\"));
       
	   ZeroMemory(&szNewPath[0],MAX_PATH);
	   lstrcpy(szNewPath,szSrcPath);
       lstrcat(szNewPath, TEXT("\\"));
       
	   lstrcat(szNewPath, FileData.cFileName); 
	   lstrcat(init_path, FileData.cFileName); 
    
	   s.wFunc = FO_COPY;
	   s.pTo = szNewPath;
	   s.pFrom = init_path;
	   s.fFlags = FOF_SIMPLEPROGRESS|FOF_NOERRORUI|FOF_NOCONFIRMATION;
	   s.lpszProgressTitle=TEXT("RECOVERING DATA...");
	
	   SHFileOperation( &s );
	   if(s.fAnyOperationsAborted==TRUE)
		   break;
	
 
      if (!FindNextFile(hSearch, &FileData)) 
      {
         if (GetLastError() == ERROR_NO_MORE_FILES) 
         { 
            MessageBox(NULL,L"RECOVERY SUCCESSFUL",L"H.A.S",MB_OK); 
            fFinished = TRUE; 
         } 
         else 
         { 
           MessageBox(NULL,L"RECOVERY_ERROR",L"H.A.S",MB_OK); 
            return;
         } 
      }
   } 
 
// Close the search handle. 
 
   FindClose(hSearch);
}

