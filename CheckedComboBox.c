#include <Windows.h>

WNDPROC DefLBProc;

LRESULT CALLBACK LBCheckedCBProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
    int iIndex;
    BOOL fNonClient;

    switch(uMsg){
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        fNonClient=HIWORD(SendMessage(hwnd,LB_ITEMFROMPOINT,0,lParam));
        if(fNonClient)
            break;

        iIndex=LOWORD(SendMessage(hwnd,LB_ITEMFROMPOINT,0,lParam));
        SendMessage(hwnd,LB_SETITEMDATA,iIndex,!SendMessage(hwnd,LB_GETITEMDATA,iIndex,0));
        InvalidateRect(hwnd,NULL,FALSE);
        return 0;
    }
    return CallWindowProc(DefLBProc,hwnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
    static HWND hwndCheckedCB,hwndLBCheckedCB;
    COMBOBOXINFO cbi;

    LPDRAWITEMSTRUCT lpdis;
    HDC hdc;
    RECT rcCheckBox,rcText;
    WCHAR szBuf[128];
    UINT uState;

    switch(uMsg){
    case WM_CREATE:
        hwndCheckedCB=CreateWindow(L"COMBOBOX",NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
            10,10,200,200,
            hwnd,(HMENU)1,((LPCREATESTRUCT)lParam)->hInstance,NULL);

        cbi.cbSize=sizeof(COMBOBOXINFO);
        SendMessage(hwndCheckedCB,CB_GETCOMBOBOXINFO,0,(LPARAM)&cbi);
        hwndLBCheckedCB=cbi.hwndList;
        SetWindowText(cbi.hwndItem,L"Checked combo box");
        DefLBProc=(WNDPROC)GetWindowLong(hwndLBCheckedCB,GWL_WNDPROC);
        SetWindowLong(hwndLBCheckedCB,GWL_WNDPROC,(LONG)LBCheckedCBProc);

        SendMessage(hwndCheckedCB,CB_ADDSTRING,0,(LPARAM)L"First");
        SendMessage(hwndCheckedCB,CB_ADDSTRING,0,(LPARAM)L"Second");
        SendMessage(hwndCheckedCB,CB_ADDSTRING,0,(LPARAM)L"Third");
        return 0;

    case WM_DRAWITEM:
        lpdis=(LPDRAWITEMSTRUCT)lParam;
        if(lpdis->itemState & ODS_COMBOBOXEDIT){
            hdc=lpdis->hDC;
            rcText=lpdis->rcItem;
            DrawText(hdc,L"Checked combo box",-1,&rcText,DT_VCENTER);
            return 0;
        }

        hdc=lpdis->hDC;
        rcCheckBox=lpdis->rcItem;
        rcText=lpdis->rcItem;

        rcCheckBox.right=rcCheckBox.left+rcCheckBox.bottom-rcCheckBox.top;
        rcText.left=rcCheckBox.right;

        uState=DFCS_BUTTONCHECK;
        if(SendMessage(hwndLBCheckedCB,LB_GETITEMDATA,lpdis->itemID,0))
            uState |= DFCS_CHECKED;
        DrawFrameControl(hdc,&rcCheckBox,DFC_BUTTON,uState);
        SendMessage(hwndLBCheckedCB,LB_GETTEXT,lpdis->itemID,(LPARAM)szBuf);
        DrawText(hdc,szBuf,-1,&rcText,DT_VCENTER);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR szCmdLine,int iCmdShow){
    HWND hwnd;
    MSG msg;
    WNDCLASS wc;

    wc.style=CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc=WndProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInstance;
    wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName=NULL;
    wc.lpszClassName=L"ANKE";
    if(!RegisterClass(&wc))
        return -1;
    
    hwnd=CreateWindow(L"ANKE",L"Checked Combo Box",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        NULL,NULL,hInstance,NULL);
    if(!hwnd)
        return -1;
    
    while(GetMessage(&msg,NULL,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}