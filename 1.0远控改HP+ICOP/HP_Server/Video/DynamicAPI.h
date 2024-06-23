/*
typedef BOOL (WINAPI *CloseWindowT)(IN HWND hWnd);

class DynamicAPI
{
public:
      CloseWindowT pCloseWindowT;
	    void init_user32(){
		     pCloseWindowT=(CloseWindowT)GetProcAddress(LoadLibrary("USER32.dll"),"CloseWindow");
		}
};
*/

typedef BOOL (WINAPI* CloseWindowT)(IN HWND hWnd);

class DynamicAPI
{
public:
       CloseWindowT pCloseWindowT;
	   void init_user32()
	   {
		   //char chTemp[]={0x43,  0x6C,  0x6F,  0x73,  0x65,  0x57,  0x69,  0x6E,  0x64,  0x6F,  0x77,0};
           //pCloseWindowT=(CloseWindowT)GetProcAddress(LoadLibrary("USER32.dll"),chTemp);
		   char chTemp[]={0x43,  0x6C,  0x6F,  0x73,  0x65,  0x57,  0x69,  0x6E,  0x64,  0x6F,  0x77,  0};
		   //pCloseWindowT=(CloseWindowT)GetProcAddress(LoadLibrary("USER32.dll"),"CloseWindow");
		   pCloseWindowT=(CloseWindowT)GetProcAddress(LoadLibrary("USER32.dll"),chTemp);
	   }

};
