
/******************************************************************

$Archive: /MfcExt/Source/CoolControlsManager.h $
$Workfile: CoolControlsManager.h $
$Author: Bogdan Ledwig $
$Date: 99-04-26 22:11 $
$Revision: 13 $

*******************************************************************/

/*
 Written by Bogdan Ledwig 
  e-mail:   bogdanle@opole.mtl.pl
  web site: http://bogdanle.cjb.net/
 Copyright © 1999 Bogdan Ledwig. All Rights Reserved.

 This code may be used in compiled form in any way you wish. This
 file may be redistributed unmodified by any means PROVIDING it is 
 not sold for profit without the authors written consent, and 
 providing that this notice and the authors name is included. If the 
 source code is used in any commercial product then a statement 
 along the lines of "Portions Copyright (C) 1999 Bogdan Ledwig" 
 must be included in the startup banner, "About" box or printed 
 documentation. An email letting me know that you are using it would be 
 nice as well. 

 This software is provided "as is" without express or implied warranty. 
 Use it at you own risk! The author accepts no liability for any damages 
 to your computer or data these products may cause.

 History:
    1.0     1999-02-24   First public release
    1.1     1999-05-16   LVS_EX_HEADERDRAGDROP list control bug fixed (thanks to Vlad Bychkoff for pointing this out)
                         UNICODE support added
                         WH_CALLWNDPROCRET is no longer supported due to some weird problems with that
                         Added support for multiple UI threads - (thanks for Mike Walter for the code)
                         Class name has been changed to CCoolControlsManager (my own idea)
                         Added support for SysTabControl32
*/

#if !defined (__CoolControlsManager_h)
#define __CoolControlsManager_h

#include <afxtempl.h>   // For CMapPtrToPtr

//////////////////////////////////////////////////////////////////////
// CCoolControlsManager class

class CCoolControlsManager {
   public:

      enum ButtonStatesEnum {
         bsNormal,
         bsHover,
         bsChecked,
      };

      enum DrawStatesEnum {
         dsNormal, 
         dsHover,
         dsAlternate,
         dsFocus = 0x04,
         dsHoverMask = 0x05,
         dsDisabled = 0x08,
      };

      // Base class for all controls and dialogs
      class CCMCore {
         public:            
            void Subclass( HWND hWnd, WNDPROC wndNewProc );
            void Unsubclass();

            // New window procedure (must be implemented by derived classes)
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;
         
         protected:
            HWND m_hWnd;                          // Handle of the window            
            WNDPROC m_oldWndProc;                 // Address of original window procedure
      };

      // Class that holds important control informations and is
      // responsible for drawing the control
      class CCMControl : public CCMCore {

         // Construction/destruction
         public:            
            CCMControl();
         
         // Operations
         public:
            // Main drawing routine            
            void DrawBorder();   
            // Returns TRUE if control needs repainting 
            virtual BOOL NeedRedraw( const POINT& point );

            // Give acces to the control state
            void SetState( int nFlagRemove, int nFlagAdd, BOOL bRedraw = TRUE );
            int GetState() const;

            // New window procedure
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
            
            static HWND m_hWndOld;           // Previously 'hovered' or 'focused' window

         // Implementation
         protected:            
            // Paint the scrollbars if a window contains at least one
            void DrawScrollBars( HDC hDC, const RECT& rc );
            // Paint the scrollbar control or an embedded window scrollbar (horiz or vert)
            void DrawScrollBar( HDC hDC, const RECT& rc,
                                int nType, BOOL bScrollbarCtrl = FALSE );
            // Draw the scrollbar thumb
            void DrawScrollbarThumb( HDC hDC, const RECT& rc );

            // There are helper functions for drawing 3D frames
            static void Draw3dBorder( HDC hDC, const RECT& rc, 
                                      int nColor1, int nColor2,
                                      int nColor3, int nColor4 );                               
            static void Draw3dBorder( HDC hDC, const RECT& rc, 
                                      int nColor1, int nColor2,
                                      int nColor3, int nColor4,
                                      int nColor5, int nColor6 );
            static void FillSolidRect( HDC hDC, const RECT& rect, COLORREF clr );
            static void FillSolidRect( HDC hDC, int x, int y, int cx, int cy, COLORREF clr );
            static void Draw3dRect( HDC hDC, int x, int y, int cx, int cy,
                                    COLORREF clrTopLeft, COLORREF clrBottomRight );
            static void Draw3dRect( HDC hDC, const RECT& rect,
                                    COLORREF clrTopLeft, COLORREF clrBottomRight );

         // Overrides
         protected:                                                            
            // Prepares DC and RECT for further drawings
            virtual void PrepareDraw( HDC& hDC, RECT& rc );    

            // Drawing code which is specific to the control
            virtual void DrawControl( HDC hDC, const RECT& rc );            

            // Returns TRUE if control (or one of its children) has a focus
            virtual BOOL IsFocused();
            
            short m_nState;                       // Current control state
            short m_nOldState;                    // Previous control state
      };

      // Edit windows
      class CCMEdit : public CCMControl {         
         public:
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
      };

      // ComboBoxes (all styles are suported)
      class CCMComboBox : public CCMControl {
         public:         
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            virtual BOOL IsFocused();
      };

      // Date/Time pickers
      class CCMDateTime : public CCMComboBox {
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
      };

      // Pushbuttons
      class CCMPushButton : public CCMControl {
         public:
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
      };

      // Checkboxes
      class CCMCheckBox : public CCMPushButton {
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );

            // Check boxes and radio buttons are always drawn
            // in the same way regardless of focus   
            virtual BOOL IsFocused() { return FALSE; }
      };

      // Radiobuttons
      class CCMRadioButton : public CCMPushButton {
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawFrame( POINT* ptArr, int nColor, 
                            HDC hDC, int xOff, int yOff );
            virtual BOOL IsFocused() { return FALSE; }
      };

      // Spin Buttons
      class CCMUpDown : public CCMControl {
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawButton( HDC hDC, const RECT& rc );
      };

      // Edit control in simple combobox
      class CCMEditCombo : public CCMEdit {            
         protected:
            virtual void PrepareDraw( HDC& hDC, RECT& rc );
      };

      // Stand-alone scrollbar controls
      class CCMScrollBar : public CCMControl {
         public:
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
      };

      // Header control
      class CCMHeaderCtrl : public CCMControl {
         public:
            virtual BOOL NeedRedraw( const POINT& point );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawButton( HDC hDC, const RECT& rc, int nState );
         private:
            int m_nOldItem;        // Recently selected item
      };

      // Slider control
      class CCMTrackbar : public CCMControl {
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawThumb( HDC hDC, const RECT& rc );
      };

      // Toolbars
      class CCMToolbar : public CCMControl {
         public:            
            virtual BOOL NeedRedraw( const POINT& point );                     
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawButton( HDC hDC, const RECT& rc, int nState );            
         private:
            int m_nOldItem;        // Recently selected item
      };

      // IP Address control
      class CCMIPAddress : public CCMControl {
         public:
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
         protected:            
            virtual BOOL IsFocused();
      };

       // Tab control
      class CCMTabControl : public CCMControl {
         public:
            enum OrientationsEnum {
               tabTop,
               tabLeft,
               tabRight,
               tabBottom,
            };

            CCMTabControl() 
            {
               m_nOldItem = -1;
            }
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
            virtual BOOL NeedRedraw( const POINT& point );
         protected:
            virtual void DrawControl( HDC hDC, const RECT& rc );
            void DrawTab( HDC hDC, const RECT& rc, int nItem, int nState );            
            virtual BOOL IsFocused() { return FALSE; }
            OrientationsEnum GetOrientation() const;
         private:
            int m_nOldItem;         // Recently selected item
      };

      // Dialog (parent of controls)
      class CCMDialog : public CCMCore {
         public:
            virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
      };
      
   public:
      CCoolControlsManager();
      virtual ~CCoolControlsManager();
      
      void Install( HWND hWnd );            // Installs control manager for given window only
      void Uninstall( HWND hWnd );          // Removes all window controls from the map
      
      // Installs WH_CALLWNDPROC hook, which automatically 
      // handles all dialogs in the givent thread. If dwThreadID = -1, then
      // current thread is used
      void InstallHook( DWORD dwThreadID = -1, BOOL bDialogOnly = TRUE );
                                                         
      // Installs WH_CALLWNDPROC hook for 
      // all dialogs in system (valid only for a DLL)
      void InstallGlobalHook( HINSTANCE hInstance, BOOL bDialogOnly = TRUE );

      // Uninstals hook
      void UninstallHook( DWORD dwThreadID = -1 );                 
      
      // Adds single control to the map
      BOOL AddControl( HWND hWnd, LPCTSTR lpszClass = NULL  );
      // Removes single control from the map
      BOOL RemoveControl( HWND hWnd );      

      void AddDialog( HWND hWnd );          // Add dialog
      void RemoveDialog( HWND hWnd );       // Removes dialog window from the map

      void Enable( BOOL bEnable = TRUE );   // Temporary enable/disable control manager
      BOOL IsEnabled() const;               // Returns TRUE if control manager is enabled

      void TimerProc();                     // Timer procedure 

      static CMapPtrToPtr m_ctrlMap;        // Main control map      
      static CMapWordToPtr m_threadMap;     // Main thread map      

   protected:
      // Other class members      
      static BOOL m_bEnabled;               // TRUE if control manager is enabled
      BOOL m_bDialogOnly;                   // Process only controls in dialogs
      HOOKPROC m_hkWndProc;                 // Old WH_WNDPROC hook procedure
      UINT m_uTimerID;                      // Our timer identifier
      static CMapPtrToPtr m_dlgMap;         // Map for dialog window (in general - for owner of controls)

      // Give access to the protected members for these functions
      friend LRESULT CALLBACK CCM_CallWndProc( int nCode, WPARAM wParam, LPARAM lParam );
      friend static LRESULT CALLBACK CCM_DialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
      friend static LRESULT CALLBACK CCM_ControlProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
      friend static void CALLBACK CCM_TimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
};

//////////////////////////////////////////////////////////////////////////////////////////
// CCoolControlsManager inlines

inline
void CCoolControlsManager::Enable( BOOL bEnable )
{
   m_bEnabled = bEnable;
}

inline
BOOL CCoolControlsManager::IsEnabled() const
{
   return m_bEnabled;
}

/////////////////////////////////////////////////////////////////////////////
// CCMControl inlines

inline
void CCoolControlsManager::CCMControl::SetState( int nFlagRemove, int nFlagAdd, BOOL bRedraw )
{
   m_nState &= ~nFlagRemove;
   m_nState |= nFlagAdd;
   if ( bRedraw )
      DrawBorder();
}

inline
int CCoolControlsManager::CCMControl::GetState() const
{
   return m_nState;
}

// Gives access to the one and only CCoolControlsManager object
CCoolControlsManager& GetCtrlManager();

#endif // __CoolControlsManager_h
