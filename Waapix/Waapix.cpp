/* * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *	Copyright (C) 2011 by Andreas Bank, andreas.mikael.bank@gmail.com
 *	last edited: 19-09-2011 by Andreas
 *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include "stdafx.h"
#include "Waapix.h"
#include "VapixObject.h"
#include "HttpReader.h"

#define WAAPIX_VERSION			"0.01A" //the application version
#define WIDTH					476
#define HEIGHT					533
#define MIN_WIDTH				476
#define MIN_HEIGHT				533
#define SOCKET_READ_BUFFER_SIZE	1048576

HINSTANCE			hInst;
void				myRegisterClass(HINSTANCE hInstance, char* className, WNDPROC classFunction, int hMenu);
BOOL				showMainWnd(HINSTANCE, int);
void				showAboutBox(void);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	aboutBoxProc(HWND, UINT, WPARAM, LPARAM);
void				connectFunction(HWND);
char*				itoc(int);	// int to char* function
char*				errorh(void); //error handler
char*				strToLower(char* a_str); //convert string to lowercase
char*				strToUpper(char* a_str); //convert string to uppercase
char*				myItoa(int theInt); //same as char* itoc()
char*				makeStringWinCompatible(char* theString); //replaces \n with \r\n
void				TEST_BUTTON(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // delete me
void				openInBrowser(char* httpAddress);	// opens internet address in the default browser
void				openInVLC(char *filePath);
void				addAttribute(); //adds an attribute to the attributes and to the listbox
void				removeAttribute(); //^^opposite
HWND				hWnd; //handle to the main window, for MessageBoxes and other weird stuff
char*				errmsg; //the socket error message (string)
HttpReader*			httpReader; //the object that handles the HTTP replies
Vapix*				vapix; //vapix object containing all the functions
SHELLEXECUTEINFO	execInfo;	// contains info for starting the browser

/**
 * Yet another app gets born!
 */
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	HACCEL hAccelTable;
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		MessageBox(NULL, "WSAStartup (sockets) failed.", "A FREAKING SERIOUS ERROR", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
	}
	InitCommonControls();
	myRegisterClass(hInstance, "myWinClass", (WNDPROC)WndProc, IDC_WAAPIX);
	myRegisterClass(hInstance, "myAboutBoxClass", (WNDPROC)aboutBoxProc, 0);

	//creating the VAPIX object
	vapix = new Vapix();

	//struct for opening the browser
	memset(&execInfo, '\0', sizeof(execInfo));
	execInfo.nShow = SW_SHOWNORMAL;
	execInfo.cbSize = sizeof(execInfo);

	//creating the http handler
	httpReader = new HttpReader();

	if (!showMainWnd (hInstance, nCmdShow)) {
		MessageBox(NULL, strcat(itoc((int)GetLastError()), "\r\n\r\nError creating main window"), "Cataclysmic error!", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	//use for language files? or make own and remove this?
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WAAPIX);

	//doing the same shit that's been done for 30 years
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (IsDialogMessage(hWnd, &msg) != 0)
			continue;
		else if (IsDialogMessage(hWnd, &msg) != 0)
			continue;
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//free up memory
	delete vapix;
	delete httpReader;

	//gerrarre!!!
	return (int) msg.wParam;
}

/**
 * Register window class, this way ensures Win95 compatibility
 */
void myRegisterClass(HINSTANCE hInstance, char* className, WNDPROC classFunction, int classHMenu) {
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= classFunction;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDC_MYICON);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= (LPCTSTR)classHMenu;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	if (RegisterClassEx(&wcex) == 0) {
		MessageBox(NULL, strcat("Error registering window class.\r\n\r\nErr nr.: ", itoc((int)GetLastError())), "2012 domomsday kinda error", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
	}
}

/**
 * Create and show the about window
 */
void showAboutBox(HWND parentHandle) {
	HWND hWndControl;
	HFONT DisplayFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, "Verdana");
	HWND hWndAB = CreateWindow("myAboutBoxClass", "About", WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION, CW_USEDEFAULT, 0, 300, 150, parentHandle, NULL, hInst, NULL);
	if (hWndAB == NULL) {
		MessageBox(NULL, itoc((int)GetLastError()), "döh", MB_OK);
		return;
	}
	char* staticText = (char*)malloc(sizeof(char)*200);
	sprintf(staticText, "Waapix v%s\nCopyright (C) 2011 by Andreas Bank,\nandreas.mikael.bank@gmail.com", WAAPIX_VERSION);
	hWndControl = CreateWindow("STATIC", staticText, WS_VISIBLE | WS_CHILD, 30, 10, 300, 100, hWndAB, NULL, hInst, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl = CreateWindow("BUTTON", "OK?", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 210, 75, 70, 30, hWndAB, (HMENU) IDC_BUTTON1, hInst, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	ShowWindow(hWndAB, SW_SHOWNORMAL);
	UpdateWindow(hWndAB);
}

/**
 * Create and show the main window
 */
BOOL showMainWnd(HINSTANCE hInstance, int nCmdShow) {
	HWND hWndControl;
	HFONT DisplayFont;
	hInst = hInstance;
	char* title = (char*)malloc(sizeof(char)*80);
	sprintf(title, "Waapix, the VAPIX tool (Waapix %s, VAPIX %s, HttpReader %s)", WAAPIX_VERSION, vapix->getVersion(), httpReader->getVersion());
	hWnd = CreateWindow("myWinClass", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);
	free(title);
	if (!hWnd) {
		return FALSE;
	}

	// creating a font to be used by all controls and stuff
	DisplayFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, "Microsoft Sans Serif");

	//protocol (combobox can't be filled at this time since no function has been selected)
	hWndControl	= CreateWindow("STATIC", "Protocol:", WS_VISIBLE | WS_CHILD, 12, 9, 49, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN, 15, 25, 67, 21, hWnd, (HMENU)ID_CBPROTOCOLS, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//address
	hWndControl	= CreateWindow("STATIC", "DUT host/IP:", WS_VISIBLE | WS_CHILD, 85, 9, 121, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl	= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "localhost", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 88, 25, 121,20, hWnd, (HMENU) ID_EDIT_ADDRESS, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	
	//port
	hWndControl		= CreateWindow("STATIC", "Port:", WS_VISIBLE | WS_CHILD, 212, 9, 29, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl		= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_NUMBER, 215, 25, 61, 20, hWnd, (HMENU) ID_EDIT_PORT, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	
	//username
	hWndControl		= CreateWindow("STATIC", "DUT's username:", WS_VISIBLE | WS_CHILD, 12, 49, 127, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl		= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "root", WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_NUMBER, 15, 65, 127, 20, hWnd, (HMENU) ID_USERNAME, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	
	//password
	hWndControl		= CreateWindow("STATIC", "DUT's password:", WS_VISIBLE | WS_CHILD, 146, 49, 127, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl		= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "pass", WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_NUMBER, 149, 65, 127, 20, hWnd, (HMENU) ID_PASSWORD, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//functions
	hWndControl		= CreateWindow("STATIC", "Function:", WS_VISIBLE | WS_CHILD, 12, 88, 51, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, 15, 104, 261, 21, hWnd, (HMENU)ID_CBFUNCTIONS, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	// filling the combobox with the vapix-object functions
	char* functions[VAPIX_FUNCTIONS_COUNT];
	vapix->getFunctions(functions);
	for(int i = 0; i<VAPIX_FUNCTIONS_COUNT; i++) {
        SendMessage(hWndControl, CB_ADDSTRING, 0, (LPARAM) functions[i]);
    }

	//action pane and OK - EXIT buttons
	hWndControl	= CreateWindow("BUTTON", "Ultimate actions", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 40, 131, 169, 51, hWnd, (HMENU) ID_GROUP1, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl	= CreateWindow("BUTTON", "&Send", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 6+40, 149, 75, 23, hWnd, (HMENU) ID_SHOW_BUTTON, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl		= CreateWindow("BUTTON", "E&xit", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, 87+40, 149, 75, 23, hWnd, (HMENU) ID_EXIT_BUTTON, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//final URL
	hWndControl	= CreateWindow("STATIC", "Final URL:", WS_CHILD | WS_VISIBLE, 13, 183, 432, 59, hWnd, (HMENU)ID_FINALURL, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	//TODO: to be removed and a char* in its place
	hWndControl	= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 13, 197, 432, 59, hWnd, (HMENU)ID_EDIT_TOSEND, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//attribute
	hWndControl	= CreateWindow("STATIC", "Attribute:", WS_VISIBLE | WS_CHILD, 302, 9, 46, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN, 305, 25, 67, 21, hWnd, (HMENU)ID_CBATTRIBUTE, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//value
	hWndControl	= CreateWindow("STATIC", "Value:", WS_VISIBLE | WS_CHILD, 375, 9, 37, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN, 378, 25, 67, 21, hWnd, (HMENU)ID_CBVALUE, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//add attribute button
	//TODO: position!
	hWndControl	= CreateWindow("BUTTON", "+", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, 400, 9, 37, 13, hWnd, (HMENU) ID_ATTRIBADD, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//added attributes
	hWndControl	= CreateWindow("LISTBOX", NULL, LBS_STANDARD | WS_VISIBLE | WS_CHILD | WS_TABSTOP, 305, 61, 140, 119, hWnd, (HMENU)ID_LBATTRIBUTES, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//remove attribute button
	//TODO: position!
	hWndControl	= CreateWindow("BUTTON", "-", WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, 420, 9, 37, 13, hWnd, (HMENU) ID_ATTRIBREMOVE, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	//DUT response
	hWndControl	= CreateWindow("STATIC", "DUT's response:", WS_VISIBLE | WS_CHILD, 12, 256, 200, 13, hWnd, (HMENU)IDC_STATIC, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);
	hWndControl	= CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "...", WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 15, 272, 430, 207, hWnd, (HMENU) ID_EDIT_INFO, hInstance, NULL);
	SendMessage(hWndControl, WM_SETFONT, (WPARAM) DisplayFont, 0);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetFocus(GetDlgItem(hWnd, ID_EDIT_ADDRESS));
	return TRUE;
}

/**
 * Callback function for the main window
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	HDC hDc;
	COLORREF myColor = RGB(150,150,200);
	switch (message) {
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		switch (wmId) {

		//protocol combobox
		case ID_CBPROTOCOLS:

			//on combobox selection change
			if(wmEvent == CBN_SELCHANGE) {
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				LRESULT dutProtocol = SendMessage(GetDlgItem(hWnd, ID_CBPROTOCOLS), CB_GETCURSEL, 0, 0);
				if(selection < 0)
					break;
				//set protocol for the function
				if(dutProtocol == -1)
					vapix->getFunction(selection)->setProtocol(vapix->getFunction(selection)->setProtocol(dutProtocol));
				else
					vapix->getFunction(selection)->setProtocol(dutProtocol);
				//emulate ID_EDIT_PORT EN_CHANGE message to update ID_EDIT_TOSEND text
				SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(ID_EDIT_PORT, EN_CHANGE), 0); 
			}
			break;

		//function combobox
		case ID_CBFUNCTIONS:

			//on combobox selection change
			if(wmEvent == CBN_SELCHANGE) {
				//clear the response edit box
				SetWindowText(GetDlgItem(hWnd, ID_EDIT_INFO), NULL);
				//reset dependent comboboxes
				SendMessage(GetDlgItem(hWnd, ID_CBPROTOCOLS), CB_RESETCONTENT, 0, 0);
				SendMessage(GetDlgItem(hWnd, ID_CBATTRIBUTE), CB_RESETCONTENT, 0, 0);
				SendMessage(GetDlgItem(hWnd, ID_CBVALUE), CB_RESETCONTENT, 0, 0);
				SendMessage(GetDlgItem(hWnd, ID_LBATTRIBUTES), LB_RESETCONTENT, 0, 0);
				//get selection index
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				//set hostname/ip in the vapix object
				char* dutAddress = (char*)malloc(sizeof(char)*200);
				memset(dutAddress, '\0', 200);
				GetWindowText(GetDlgItem(hWnd, ID_EDIT_ADDRESS), dutAddress, 200);
				vapix->setHostname(dutAddress);
				//set port
				char* dutPort = (char*)malloc(sizeof(char)*6);
				memset(dutPort, '\0', 6);
				GetWindowText(GetDlgItem(hWnd, ID_EDIT_PORT), dutPort, 6);
				if(atoi(dutPort) > 0)
					vapix->setPort(selection, atoi(dutPort));
				else
					vapix->setPort(selection, atoi(vapix->getFunction(selection)->getProtocol()[1]));
				//remove attribute objects
				for(int i=0; i<VAPIX_FUNCTIONS_COUNT; i++) {
					vapix->getFunction(i)->removeAllAttributes();
				}
				//then get the function string and set it in the edit box
				SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selection));
				//fill the protocols combobox
				int vpSize = 0;
				char*** validProtocols = vapix->getFunction(selection)->getValidProtocols(&vpSize);
				for(int i = 0; i<vpSize; i++) {
					SendMessage(GetDlgItem(hWnd, ID_CBPROTOCOLS), CB_ADDSTRING, 0, (LPARAM) validProtocols[i][0]);
				}
				//fill the attributes combobox with the vapix attributes for the selected function
				int vaSize = 0;
				VapixAttribute** vapixAttributes = vapix->getFunction(selection)->getValidAttributes(&vaSize);
				for(int i = 0; i<vaSize; i++) {
					SendMessage(GetDlgItem(hWnd, ID_CBATTRIBUTE), CB_ADDSTRING, 0, (LPARAM) vapixAttributes[i]->getName());
				}
				free(dutAddress);
				free(dutPort);
			}
			break;

		//attribute combobox
		case ID_CBATTRIBUTE:

			//on combobox selection change
			if(wmEvent == CBN_SELCHANGE) {
				//reset dependent comboboxes
				SendMessage(GetDlgItem(hWnd, ID_CBVALUE), CB_RESETCONTENT, 0, 0);
				//get selection index
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				LRESULT selection2 = SendMessage(GetDlgItem(hWnd, ID_CBATTRIBUTE), CB_GETCURSEL, 0, 0);
				//fill the values combobox with the vapix values for the selected attribute in the selected function
				int vvSize = 0;
				char** vapixAttributeValues = vapix->getFunction(selection)->getValidAttribute(selection2)->getValidValues(&vvSize);
				for(int i = 0; i<vvSize; i++) {
					SendMessage(GetDlgItem(hWnd, ID_CBVALUE), CB_ADDSTRING, 0, (LPARAM) vapixAttributeValues[i]);
				}
				//update the "send" string
				SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selection));
			}
			break;

		//add attribute button
		case ID_ATTRIBADD:
			addAttribute();
			break;
		//remove attribute button
		case ID_ATTRIBREMOVE:
			removeAttribute();
			break;
		case ID_EDIT_ADDRESS:
			if(wmEvent == EN_CHANGE) {
				char* dutAddress = (char*)malloc(sizeof(char)*200);
				memset(dutAddress, '\0', 200);
				GetWindowText(GetDlgItem(hWnd, ID_EDIT_ADDRESS), dutAddress, 200);
				vapix->setHostname(dutAddress);
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				if(selection >= 0) {
					SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selection));
				}
				free(dutAddress);
			}
			break;
		case ID_EDIT_PORT:
			if(wmEvent == EN_CHANGE) {
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				if(selection < 0)
					break;
				char* dutPort = (char*)malloc(sizeof(char)*6);
				memset(dutPort, '\0', 6);
				GetWindowText(GetDlgItem(hWnd, ID_EDIT_PORT), dutPort, 6);
				//empty field means use protocol default port
				if(dutPort[0] == '\0')
					vapix->setPort(selection, atoi(vapix->getFunction(selection)->getProtocol()[1]));
				else
					vapix->setPort(selection, atoi(dutPort));
				SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selection));
				free(dutPort);
			}
			break;
		case ID_SHOW_BUTTON:
			{
				//get selection index
				LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
				if(selection < 0) {
					MessageBox(hWnd, "A function must be selected before making a request!", "Info", MB_ICONEXCLAMATION);
					break;
				}

				//check if the function is returning text or binary
				int opener = vapix->getFunction(selection)->getOpener();
				switch(opener) {
				case 0:
					//open locally
					connectFunction(hWnd);
					break;
				case 1:
					//open in browser
					{
						char* temp = (char*)malloc(sizeof(char)*100);
						//get URI
						GetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), temp, 100);
						//open URI in the default browser
						openInBrowser(temp);
						free(temp);
					}
					break;
				case 2:
					//open in external video player (VLC)
					{
						//TODO: make it add user:password@ to URI
						char* temp = (char*)malloc(sizeof(char)*100);
						//get URI
						GetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), temp, 100);
						//open URI in VLC
						openInVLC(temp);
						free(temp);
					}
					break;
				default:
					{
						LRESULT selectedFunction = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
						char* tmpStr = (char*)malloc(sizeof(char)*200);
						memset(tmpStr, '\0', 200);
						sprintf(tmpStr, "The selected function has an opener value (int VapixObject::VapixFunction::opener = %d)\nthat is not recognised by this version of Waapix(v%s).", vapix->getFunction(selectedFunction)->getOpener(), WAAPIX_VERSION);
						MessageBox(hWnd, tmpStr, "Error activating function", MB_ICONEXCLAMATION);
						free(tmpStr);
					}
				}
			}
			break;
		case ID_EXIT_BUTTON:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			showAboutBox(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wmId, wmEvent);
		}
		break;
	case WM_DESTROY:
		WSACleanup();
		PostQuitMessage(0);
		break;
	case WM_CTLCOLORSTATIC:
		hDc = (HDC)(wParam); 
		SetBkMode(hDc, TRANSPARENT);
		return (INT_PTR)GetStockObject(NULL_BRUSH);
		break;
	case WM_SIZING:
		{
			RECT clientRect, *currentRect = (RECT*)lParam;
			BOOL processed = FALSE;
			GetClientRect(hWnd, &clientRect);
			if (currentRect->right - currentRect->left < MIN_WIDTH) {
				currentRect->right = currentRect->left + MIN_WIDTH;
				processed = TRUE;
			}
			if (currentRect->bottom - currentRect->top < MIN_HEIGHT) {
				currentRect->bottom = currentRect->top + MIN_HEIGHT;
				processed = TRUE;
			}
			if (processed) return TRUE;
		}
		break;
	case WM_SIZE:
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			SetWindowPos(GetDlgItem(hWnd, ID_EDIT_INFO), HWND_NOTOPMOST, 15, 272, clientRect.right-30, clientRect.bottom-272-15, SWP_NOZORDER);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/**
 * Callback function for the About window
 */
LRESULT CALLBACK aboutBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_BUTTON1) {
				DestroyWindow(hWnd);
			}
			break;
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkMode(hdcStatic, TRANSPARENT);
			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/**
 * Connect function, Windows Sockets
 * TODO: move to separate class/file
 * and maybe switch to HTTP/1.1
 */
void connectFunction(HWND hWnd) {
	//SOCKET_READ_BUFFER_SIZE is 1048576 (1MB)
	struct sockaddr_in dest_addr, my_addr;
	struct hostent *hostInfo;
	char* hWndTxbAddressTxt = (char*)malloc(sizeof(char)*100);
	char* hWndTxbPortTxt = (char*)malloc(sizeof(char)*6);
	char* tmpCharPtr = (char*)malloc(sizeof(char)*SOCKET_READ_BUFFER_SIZE);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(hWndTxbAddressTxt, '\0', 100);
	memset(hWndTxbPortTxt, '\0', 6);
	memset(tmpCharPtr, '\0', SOCKET_READ_BUFFER_SIZE);
	SendMessage(GetDlgItem(hWnd, ID_EDIT_ADDRESS), WM_GETTEXT, 100, (LPARAM) hWndTxbAddressTxt);

	//fun part! SOCKETS & ADDRESSES!
	//TODO: make it check the address vadility first!
	hostInfo = gethostbyname(hWndTxbAddressTxt);
	//TODO: change to prioritise the 171.x.x.x net
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(0);
	memset(&(my_addr.sin_zero), '\0', sizeof(my_addr.sin_zero));
	dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)hostInfo->h_addr_list[0])));
	dest_addr.sin_family = AF_INET;
	//read port number
	SendMessage(GetDlgItem(hWnd, ID_EDIT_PORT), WM_GETTEXT, 6, (LPARAM) hWndTxbPortTxt);
	//set port number
	dest_addr.sin_port = htons(atoi(hWndTxbPortTxt));
	memset(&(dest_addr.sin_zero), '\0', sizeof(dest_addr.sin_zero));
	sprintf(tmpCharPtr, "Connecting to %s on port %s", hWndTxbAddressTxt, hWndTxbPortTxt);
	SendMessage(GetDlgItem(hWnd, ID_EDIT_INFO), WM_SETTEXT, 0, (LPARAM) tmpCharPtr);

	//lets try this shit now!
	if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		sprintf(tmpCharPtr, "Ragnarok sized error!\r\nFailed to connect to %s on port %s.\r\n%s.", hWndTxbAddressTxt, hWndTxbPortTxt, errorh());
		SendMessage(GetDlgItem(hWnd, ID_EDIT_INFO), WM_SETTEXT, 0, (LPARAM) tmpCharPtr);
		free(hWndTxbAddressTxt);
		free(hWndTxbPortTxt);
		free(tmpCharPtr);
		return;
	}
	else {
		sprintf(tmpCharPtr, "Connected to %s on port %s", hWndTxbAddressTxt, hWndTxbPortTxt);
		SendMessage(GetDlgItem(hWnd, ID_EDIT_INFO), WM_SETTEXT, 0, (LPARAM) tmpCharPtr);
	}
	memset(tmpCharPtr, '\0', SOCKET_READ_BUFFER_SIZE);
	char* requestString = (char*)malloc(sizeof(char)*200);
	memset(requestString, '\0', 200);
	LRESULT selectedFunction = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
	//set hostname to vapix object too
	//TODO: this should be done elswhere alot earlier
	vapix->setHostname(hWndTxbAddressTxt);
	vapix->setPort(selectedFunction, atoi(hWndTxbPortTxt));
	// TODO: requestString = make it take the EditBox string and remove the http://address[:port]
	requestString = vapix->getFunctionCode(selectedFunction);
	sprintf(tmpCharPtr, "GET /%s HTTP/1.0\r\nHost: %s\r\nContent-type: charset=ISO-8859-1\r\rConnection: Close\r\n\r\n", requestString, hWndTxbAddressTxt);
	send(sock, tmpCharPtr, (int)strlen(tmpCharPtr), 0);
	//clear buffer for receiving
	memset(tmpCharPtr, '\0', SOCKET_READ_BUFFER_SIZE);
	//cool! now lets see what we got
	recv(sock, tmpCharPtr, SOCKET_READ_BUFFER_SIZE, 0);
	//tmpCharPtr = makeStringWinCompatible(tmpCharPtr);
	//fill the edit box with the results
	SendMessage(GetDlgItem(hWnd, ID_EDIT_INFO), WM_SETTEXT, 0, (LPARAM)tmpCharPtr);

	//that's it folks
	if (sock != NULL) {
		shutdown(sock, 0);
		closesocket(sock);
	}
	free(hWndTxbAddressTxt);
	free(hWndTxbPortTxt);
	free(tmpCharPtr);
	free(requestString);
}

/**
 * Integer to Char, shortened
 */
char* itoc(int number) {
	char* returnChar = (char*)calloc(sizeof(char), 10);
	sprintf(returnChar, "%d", number);
	return returnChar;
}

/**
 * Error translator for the Socketing
 */
char* errorh(void) {
	 //	I only proccess the usual
	 //	socket errors for this application
	int errnr = WSAGetLastError();
	switch(errnr) {
		case 10040:
			errmsg = (char*) calloc(sizeof(char), strlen("Message too long"));
			strcpy(errmsg, "Message too long");
			break;
		case 10048:
			errmsg = (char*) calloc(sizeof(char), strlen("Address already in use"));
			strcpy(errmsg, "Address already in use");
			break;
		case 10049:
			errmsg = (char*) calloc(sizeof(char), strlen("Cannot assign requested address"));
			strcpy(errmsg, "Cannot assign requested address");
			break;
		case 10054:
			errmsg = (char*) calloc(sizeof(char), strlen("Connection reset by peer"));
			strcpy(errmsg, "Connection reset by peer");
			break;
		case 10061:
			errmsg = (char*) calloc(sizeof(char), strlen("Connection refused"));
			strcpy(errmsg, "Connection refused");
			break;
		case 10060:
			errmsg = (char*) calloc(sizeof(char), strlen("Connection timed out"));
			strcpy(errmsg, "Connection timed out");
			break;
		default:
			errmsg = (char*) calloc(sizeof(char), sizeof(errnr));
			_itoa(errnr, errmsg, 2);
			return errmsg;
	}
	return errmsg;
}

/**
 * String to lowercase
 */
char* strToLower(char *a_str) {
	for (int i=0; a_str[i] != '\0'; i++) {
		 a_str[i] = tolower((unsigned char) a_str[i]);
	}
	return a_str;
}

/**
 * String to uppercase
 */
char* strToUpper(char *a_str) {
	for (int i=0; a_str[i] != '\0'; i++) {
		 a_str[i] = toupper((unsigned char) a_str[i]);
	}
	return a_str;
}

/**
 * Again... Integer to string....
 * I suffer form memory loss at times!
 */
char *myItoa(int theInt) {
	char *temp = (char*)malloc(sizeof(char)*64);
	return itoa(theInt, temp, 10);
}

/**
 * replace all single '\n' with "\r\n"
 * and erase the "ï»¿" (Byte-order-Mark)
 * note: char **theString might be more suitable
 * for free(*theString)-ing the memory
 */
char* makeStringWinCompatible(char *theString) {
	char* newString, * tmpStr;
	int stringLength = strlen(theString);
	BOOL atLeastOneFound = FALSE;
	//newline occurrences
	int charOccurrences = 0;
	//last time we found a \n and copied chars up to it
	int lastCopy = 0;
	tmpStr = (char*)malloc(sizeof(char)*stringLength); //min should be the original string length
	memset(tmpStr, '\0', stringLength);

	//find number of single \n occurrences
	for(int i=0; i<stringLength; i++) {
		//and while we're here erase the "ï»¿" (Byte-order-Mark)
		if(theString[i] == 'ï' && theString[i+1] == '»' && theString[i+2] == '¿') {
			//write over the 3 BoM characters
			strcpy(&theString[i], &theString[i+3]);
			//after removing the BoM "it is only logical" to shorten the string by 3
			stringLength = stringLength - 3;
		}
		if(theString[i] == '\n' && theString[i-1] != '\r')
			charOccurrences++;
	}

	//allocate mem for the new string now that we know how much it will need
	newString = (char*)malloc(sizeof(char)*(stringLength+charOccurrences));
	memset(newString, '\0', sizeof(char)*(stringLength+charOccurrences));

	//begin the search and rescue operation
	//TODO: fix the logic, \r comes before \n, you think opposite here!
	//or is it \n that needs to be removed on windows?... Im too tired now...
	for(int i=0; i<stringLength; i++) {
		if(theString[i] == '\n' && theString[i-1] != '\r') {
			atLeastOneFound = TRUE;
			//clear buffer
			memset(tmpStr, '\0', 1024);
			//copy from last \n (excluded) to current \n (excluded)
			strncpy(tmpStr, &theString[lastCopy], i-lastCopy);
			strcat(newString, tmpStr);
			strcat(newString, "\r\n");
			//set lastCopy to current position but skip \n char
			lastCopy = i+1;
		}
	}
	free(tmpStr);
	if(atLeastOneFound) {
		free(theString);
		return newString;
	}
	else {
		free(newString);
		return theString;
	}
}

/**
 * Opens the passed string in the appropriet application
 * NOTE: This function passes the string to a windows shell
 * that handles requests (in this case "open") and determines
 * what application to use to perform the operation.
 * At this moment I can't really see any risk in not checking
 * if it is a valid http address or not.
 */
void openInBrowser(char* httpAddress) {
	execInfo.lpVerb = "open";
	execInfo.lpFile = httpAddress;
	ShellExecuteEx(&execInfo);
}

/**
 * Starts VLC with the given address
 */
void openInVLC(char *stream) {
	//TODO: make check for VLC, find where it is installed, and use appropriate path
	//TIPS: registry: HKLM\Software\VideoLAN\VLC\,
	// http://www.daniweb.com/software-development/cpp/threads/71751
	// http://msdn.microsoft.com/en-us/library/ms724911.aspx
	execInfo.lpVerb = "open";
	execInfo.lpFile = "C:\\Program Files (x86)\\VideoLAN\\VLC\\vlc.exe";
	execInfo.lpParameters = stream;
	ShellExecuteEx(&execInfo);
}

/**
 * Adds an attribute to the 'VapixAttribute** VapixObject::VapixFunction::attributes'
 * array and to the ID_LBATTRIBUTES ListBox
 */
void addAttribute() {
	//find out what is selected
	LRESULT selection = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
	LRESULT selectedValue = SendMessage(GetDlgItem(hWnd, ID_CBVALUE), CB_GETCURSEL, 0, 0);
	if(selection < 0) {
		MessageBox(hWnd, "No attribute selected!", "Info", MB_ICONEXCLAMATION);
		return;
	}
	if(selectedValue < 0) {
		MessageBox(hWnd, "A value must be selected before adding the attribute!", "Info", MB_ICONEXCLAMATION);
		return;
	}
	//make some text buffers
	char* attributeName = (char*)malloc(sizeof(char)*200);
	memset(attributeName, '\0', 200);
	char* attributeValue = (char*)malloc(sizeof(char)*200);
	memset(attributeValue, '\0', 200);
	//fill the text buffers with the name
	GetWindowText(GetDlgItem(hWnd, ID_CBATTRIBUTE), attributeName, 200);
	//and the value
	GetWindowText(GetDlgItem(hWnd, ID_CBVALUE), attributeValue, 200);
	//create the attribute
	VapixAttribute* attribute = new VapixAttribute(attributeName, "");
	attribute->addValidValue(attributeValue);
	attribute->setValue(attributeValue);
	//check if attribute already exists
	if(vapix->getFunction(selection)->attributeExists(attribute)>=0) {
		//ask whether to add anyway
		int result = MessageBox(hWnd, "This attribute has already been added to the attributes list.\nDo you wish to update its value?", "Attribute exists", MB_YESNO);
		//on NO don't do anything
		if(result < 0)
			return;
		//on YES update the added attribute
		//TODO: update it
		//IDEA: maybe remove it and let the code below add it again with the new value?
		MessageBox(hWnd, "TODO: update it", "TODO", MB_OK);
		return;
	}
	//add the attribute to the list and to the VapixFunction object
	//IDEA: maybe clear the attribute and value cb selections?
	vapix->getFunction(selection)->addAttribute(attribute);
	char* lbString = (char*)malloc(sizeof(char)*200);
	memset(lbString, '\0', 200);
	strcat(lbString, attribute->getString());
	SendMessage(GetDlgItem(hWnd, ID_LBATTRIBUTES), LB_ADDSTRING, 0, (LPARAM) lbString);
	SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selection));
	free(attributeName);
	free(attributeValue);
	free(attribute);
	free(lbString);
}

/**
 * Removes an attribute from the 'VapixAttribute** VapixObject::VapixFunction::attributes'
 * array and from the ID_LBATTRIBUTES ListBox
 */
void removeAttribute() {
	//find out what is selected
	LRESULT selectedFunction = SendMessage(GetDlgItem(hWnd, ID_CBFUNCTIONS), CB_GETCURSEL, 0, 0);
	LRESULT selectedAttribute = SendMessage(GetDlgItem(hWnd, ID_LBATTRIBUTES), LB_GETCURSEL, 0, 0);
	if(selectedAttribute >= 0) {
		//and remove it
		SendMessage(GetDlgItem(hWnd, ID_LBATTRIBUTES), LB_DELETESTRING, (WPARAM)selectedAttribute, 0);
		int tmp = 0;
		vapix->getFunction(selectedFunction)->removeAttribute(vapix->getFunction(selectedFunction)->getAttributes(&tmp)[selectedAttribute]);
		SetWindowText(GetDlgItem(hWnd, ID_EDIT_TOSEND), vapix->getBrowserFunctionString(selectedFunction));
	}
}