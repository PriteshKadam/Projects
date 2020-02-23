#include "filepro.h"



LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("HIGH AVAILABILITY SYSTEM") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
     
     wndclass.style         = 0;//CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
	 wndclass.hIcon         = LoadIcon (NULL, IDI_INFORMATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;

     wndclass.hbrBackground = (HBRUSH) GetStockObject (SYSTEM_FONT) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName, TEXT ("HIGH AVAILABILITY SYSTEM"),
                          WS_MINIMIZEBOX|WS_SYSMENU,100,100,400,250,
                          NULL, NULL, hInstance, NULL) ;
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hParentwnd, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	 hWnd=hParentwnd;
     static HBRUSH hbr;
     static RECT  rect ;
     static int   cxChar, cyChar ;
     HDC          hdc ;
     PAINTSTRUCT  ps ;
    
     
     switch (message)
     {
     case WM_CREATE :
		 hMyInstance=((LPCREATESTRUCT)lParam)->hInstance;
        
               hwndInit = CreateWindow(TEXT("button"),TEXT("INITIATE BACKUP"),
				                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									   10, 48,160, 28,hParentwnd,(HMENU)ID_BUTTON1,
									  hMyInstance , NULL) ;
                
			  
			    hwndMonitor = CreateWindow(TEXT("button"),TEXT("START MONITORING"),
					                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									 10, 80,160, 28,hParentwnd,(HMENU)ID_BUTTON2,
									 hMyInstance, NULL) ;
                

				
				hwndRecover=CreateWindow (TEXT("button"),TEXT("RECOVER"),
										WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
									    10, 112,160, 28, hParentwnd,(HMENU)ID_BUTTON3,
										hMyInstance, NULL) ;

				hwndExit=CreateWindow (TEXT("button"),TEXT("EXIT SYSTEM"),
										WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
										10, 144,160, 28, hParentwnd,(HMENU)ID_BUTTON3,
										hMyInstance, NULL) ;


				   hbr=CreateSolidBrush(RGB(0,155,155));
		          SetClassLong(hWnd,GCL_HBRBACKGROUND,(LONG)hbr);

				

          return 0 ;

          
     case WM_PAINT :

          hdc = BeginPaint (hParentwnd, &ps) ;
          SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
       
      
         
		  if(flag==1)
		  {
		        ShowWindow(hwndInit,SW_HIDE);
				EnableWindow(hwndInit,FALSE);
			
				ShowWindow(hwndMonitor,SW_HIDE);
				EnableWindow(hwndMonitor,FALSE);

				ShowWindow(hwndExit,SW_HIDE);
                EnableWindow(hwndExit,FALSE);
		  }

		if(flag==2)
		{       
                ShowWindow(hStatic1,SW_HIDE);
				EnableWindow(hStatic1,FALSE);

                ShowWindow(hStatic2,SW_HIDE);
				EnableWindow(hStatic2,FALSE);
				
				ShowWindow(hWndEditSrc,SW_HIDE);
				EnableWindow(hWndEditSrc,FALSE);
				
				ShowWindow(hWndEditDest,SW_HIDE);
				EnableWindow(hWndEditDest,FALSE);
				
				ShowWindow(hWndSet,SW_HIDE);
				EnableWindow(hWndSet,FALSE);



				ShowWindow(hwndInit,SW_SHOW);
				EnableWindow(hwndInit,TRUE);
				
				ShowWindow(hwndMonitor,SW_SHOW);
				EnableWindow(hwndMonitor,TRUE);
				
				ShowWindow(hwndExit,SW_SHOW);
                EnableWindow(hwndExit,TRUE);

				flag=0;
				InvalidateRect (hParentwnd, NULL, TRUE) ;

		}
		     
		  EndPaint (hParentwnd, &ps) ;
          return 0 ;
          
     case WM_DRAWITEM :
     case WM_COMMAND :
          if((HWND)lParam==hwndInit)
		  {   
			
                flag=1;
				InvalidateRect (hParentwnd, NULL, TRUE) ;

				hStatic1 = CreateWindow(TEXT("STATIC"),TEXT("SOURCE"),
										WS_CHILD|WS_VISIBLE|SS_CENTER ,
					                    8, 48,160, 28,hParentwnd,NULL,hMyInstance,NULL);


	        	hStatic2 = CreateWindow(TEXT("STATIC"),
					                    TEXT("DESTINATION"),
										WS_CHILD|WS_VISIBLE|SS_CENTER ,
										8, 80,160, 28,hWnd,NULL,hMyInstance,NULL);

                hWndEditSrc=CreateWindow (TEXT("edit"), 
                                   NULL,
                                   WS_CHILD | WS_VISIBLE |WS_BORDER,
                                   200, 48,160, 28,hParentwnd,(HMENU)ID_EDIT1,
                                   hMyInstance, NULL) ;
					
			  
				 hWndEditDest=CreateWindow ( TEXT("edit"), 
                                   NULL,
                                   WS_CHILD | WS_VISIBLE |WS_BORDER,
                                   200, 80,160, 28,hParentwnd,(HMENU)ID_EDIT2,
                                   hMyInstance, NULL) ;

				 hWndSet=CreateWindow (TEXT("button"),TEXT("SET"),
										WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
										150,140,100, 28, hParentwnd,(HMENU)ID_BUTTON3,
										hMyInstance, NULL) ;


				 SetWindowText(hWndEditSrc,TEXT("C:\\TC"));
			     SetWindowText(hWndEditDest,TEXT("D:\\BACKUP"));

				InvalidateRect (hParentwnd, NULL, TRUE) ;
			
			return 0;

		  }
            
		  
		  if((HWND)lParam==hWndSet)
		  {
			  GetWindowText(hWndEditSrc,szSrcPath,80);
			  GetWindowText(hWndEditDest,szDestPath,80);
			 
			  flag=2;

			  InvalidateRect(hParentwnd, NULL, TRUE) ;
   
			  initiate();
				
		  }


          if((HWND)lParam==hwndMonitor)
		  {
                ShowWindow(hParentwnd,SW_HIDE);
				EnableWindow(hParentwnd,FALSE);

			    monitor();
				
				ShowWindow(hParentwnd,SW_SHOW);
				EnableWindow(hParentwnd,TRUE);

		  }

		  if((HWND)lParam==hwndRecover)
		  {
             ShowWindow(hParentwnd,SW_HIDE);
			 EnableWindow(hParentwnd,FALSE); 

			 recover();

			 ShowWindow(hParentwnd,SW_SHOW);
			 EnableWindow(hParentwnd,TRUE);

		  }


          if((HWND)lParam==hwndExit)
		  {
			  PostQuitMessage (0) ;
			  return 0;
		  }

		 
          break ;

          
     case WM_DESTROY :
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hParentwnd, message, wParam, lParam) ;
}






