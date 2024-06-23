/*************************************************************/
/*	SkinSharp API											 */
/*************************************************************/

#ifndef SKIN_H
#define SKIN_H

/**********************错误代码******************************/
#define		SRET_OK					0	//成功
#define		SRET_ERROR				1	//失败
#define		SRET_ERROR_READ			2	//皮肤文件读取错误	
#define		SRET_ERROR_PARAM		3	//参数错误
#define		SRET_ERROR_CREATE		4	//创建皮肤失败
#define		SRET_ERROR_FORMAT		5	//皮肤格式不正确
#define		SRET_ERROR_VERSION		6	//皮肤版本不兼容
#define		SRET_ERROR_PASSWORD		7	//皮肤密钥错误
#define		SRET_ERROR_INVALID		8	//换肤引擎无效
/************************************************************/


#define DECLSKINHAPI __declspec(dllimport) int  __stdcall

extern "C"
{
	/************************************************************************/
	/*	功能  :	加载程序当前目录下的文件名为skinh.she的皮肤进行换肤			*/
	/*	返回值:	成功返回0, 失败返回非0										*/
	/************************************************************************/
	DECLSKINHAPI SkinH_Attach();
	

	/************************************************************************/
	/*  功能  :	加载指定路径的皮肤进行换肤									*/
	/*	返回值: 成功返回0, 失败返回非0										*/
	/************************************************************************/
	DECLSKINHAPI SkinH_AttachEx(
						LPCTSTR strSkinFile,	//皮肤文件路径
						LPCTSTR strPassword		//皮肤密钥
						);


	/*
		功能:	加载指定路径的皮肤进行换肤并指定相应的色调，饱和度，亮度
		返回值: 成功返回0, 失败返回非0
	*/
	DECLSKINHAPI SkinH_AttachExt(
						LPCTSTR strSkinFile,	//皮肤文件路径
						LPCTSTR strPassword,	//皮肤密钥
						int nHue,				//色调，	取值范围-180-180, 默认值0
						int nSat,				//饱和度，	取值范围-100-100, 默认值0
						int nBri				//亮度，	取值范围-100-100, 默认值0
						);
	

	/*
		功能:	加载指定资源进行换肤并指定相应的色调，饱和度，亮度
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_AttachRes(
						LPBYTE	pShe,			//资源皮肤数据指针
						DWORD	dwSize,			//资源皮肤数据长度
						LPCTSTR strPassword,	//皮肤密钥
						int nHue,				//色调，	取值范围-180-180, 默认值0
						int nSat,				//饱和度，	取值范围-100-100, 默认值0
						int nBri				//亮度，	取值范围-100-100, 默认值0
						);
	

	/*
		功能:	加载指定皮肤资源进行换肤并指定相应的色调，饱和度，亮度
		返回值: 成功返回0, 失败返回非0
	*/
	DECLSKINHAPI SkinH_AttachResEx(
						LPCTSTR lpName,			//资源名
						LPCTSTR lpType,			//资源类型
						LPCTSTR strPassword,	//皮肤密钥
						int nHue,				//色调，	取值范围-180-180, 默认值0
						int nSat,				//饱和度，	取值范围-100-100, 默认值0
						int nBri				//亮度，	取值范围-100-100, 默认值0
						);

	/*
		功能:	卸载换肤	
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_Detach();




	/*
		功能:	卸载指定句柄的窗体或者控件的皮肤	
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_DetachEx(
						HWND hWnd				//指定卸载皮肤的窗体或控件的句柄
						);
	


	/*
		功能:	设置指定窗体的透明度
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetWindowAlpha(
						HWND hWnd,				//窗体的句柄 
						int nAlpha				//透明度
						);
	


	/*
		功能:	调整当前皮肤的色调，饱和度，亮度
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_AdjustHSV(
						int nHue,				//色调，	取值范围-180-180, 默认值0
						int nSat,				//饱和度，	取值范围-100-100, 默认值0
						int nBri				//亮度，	取值范围-100-100, 默认值0
						);
	



	/*
		功能:	获取指定窗口或控件在nX,nY处的颜色值 
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_GetColor(
						HWND hWnd,				//指定窗体或控件的句柄
						int nX,					//横坐标
						int nY					//纵坐标
						);
	
	


	/*
		功能:	指定窗体和控件的换肤类型
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_Map(
						HWND hWnd,				//指定窗体或控件的句柄 
						int nType				//换肤类型
						);
	//换肤类型
	#define TYPE_UNKNOWN				0		//未知类型
	#define TYPE_ANIMATE				1001	//动画控件 
	#define TYPE_CHECKBOX				1002	//复选框
	#define TYPE_COMBOBOX				1003	//组合框
	#define TYPE_COMBOLBOX				1004	//组合下拉框
	#define TYPE_CONTROLBAR				1005	//控件栏
	#define TYPE_DATETIME				1006	//日期控件
	#define TYPE_EDITBOX				1007	//文本框
	#define TYPE_GROUPBOX				1008	//分组框
	#define TYPE_HEADERCTRL				1009	//列头控件
	#define TYPE_HOTKEY					1010	//热键控件
	#define TYPE_IPADDRESS				1011	//IP地址控件
	#define TYPE_LABEL					1012	//标签控件
	#define TYPE_LISTBOX				1013	//列表框
	#define TYPE_LISTVIEW				1014	//列表视图
	#define TYPE_MDICLIENT				1015	//MDI客户区
	#define TYPE_MENU					1016	//菜单
	#define TYPE_MONTHCAL				1017	//月历控件
	#define TYPE_PICTURE				1018	//图片框
	#define TYPE_PROGRESS				1019	//进度条
	#define TYPE_PUSHBUTTON				1020	//普通按钮
	#define TYPE_RADIOBUTTON			1021	//单选框
	#define TYPE_REBAR					1022	//重组栏
	#define TYPE_RICHEDIT				1023	//富文本框
	#define TYPE_SCROLLBAR				1024	//滚动条
	#define TYPE_SCROLLCTRL				1025	//内置滚动条的控件
	#define TYPE_SPINCTRL				1026	//调节器
	#define TYPE_STATUSBAR				1027	//状态栏
	#define TYPE_TABCTRL				1028	//选择夹
	#define TYPE_TOOLBAR				1029	//工具栏
	#define TYPE_TOOLBARWND				1030	//MFC工具栏窗体
	#define TYPE_TRACKBAR				1031	//滑条控件
	#define TYPE_TREEVIEW				1032	//树形视图
	#define TYPE_WINDOW					1034	//标准窗体
	#define TYPE_COMCTRL				1036	//通用换肤
	#define TYPE_PAINTCTRL				1037	//通用换肤
	






	/*
		功能:	设置Aero特效
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetAero(
						int bAero				//1为开启特效,0为关闭特效 
						);
	



	/*
		功能:	设置Aero特效参数
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_AdjustAero(
						int nAlpha,				//透明度,   0-255, 默认值0
						int nShwDark,			//亮度,     0-255, 默认值0
						int nShwSharp,			//锐度,	    0-255, 默认值0 
						int nShwSize,			//阴影大小, 2-19,  默认值2
						int nX,					//水平偏移, 0-25,  默认值0 (目前不支持)
						int nY,					//垂直偏移, 0-25,  默认值0 (目前不支持)
						int nRed,				//红色分量, 0-255, 默认值 -1
						int nGreen,				//绿色分量, 0-255, 默认值 -1 
						int nBlue				//蓝色分量, 0-255, 默认值 -1 
						);
	



	/*
		功能:	设置窗体是否可以移动
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetWindowMovable(
						HWND hWnd,				//窗口句柄 
						BOOL bMovable			//0为不可移动, 1为可移动
						);
	



	/*
		功能:	设置控件的背景色(目前仅对单选框, 复选框, 分组框有效)
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetBackColor(
						HWND hWnd,				//控件句柄
						int nRed,				//红色分量
						int nGreen,				//绿色分量
						int nBlue				//蓝色分量
						);
	


	/*
		功能:	设置控件的文本颜色色(目前仅对单选框,复选框,分组框有效)
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetForeColor(
						HWND hWnd,				//控件句柄
						int nRed,				//红色分量
						int nGreen,				//绿色分量
						int nBlue				//蓝色分量
						);
	



	/*
		功能:	用于填充表格或者列表控件数据时，重复绘制影响执行效率问题
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_LockUpdate(
						HWND hWnd,				//指定窗体或控件的句柄 
						int bUpdate				//1为锁定绘制，0为解锁绘制 
						);
	



	/*
		功能:	设置菜单透明度
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetMenuAlpha(
						int nAlpha				//菜单透明度，取值范围 0 - 255
						);




	/*
		功能:	绘制指定设备上下文的元素
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_NineBlt(
						HDC hDtDC,				//目标设备上下文
						int left,				//左上角水平坐标
						int top,				//左上角垂直坐标
						int right,				//右下角水平坐标
						int bottom,				//右下角垂直坐标
						int nMRect				//元素id
						);

	/*
		功能:	设置标题菜单栏
		返回值: 成功返回0, 失败返回非0	
	*/
	DECLSKINHAPI SkinH_SetTitleMenuBar(
						HWND hWnd, 	//窗口句柄
						BOOL bEnable,	//是否设置 1为设置， 0 为取消 
						int nTMenuY, 	//菜单栏高度
						int nTopOffs, 	//顶部偏移
						int nRightOffs 	//右部偏移
						);

	
	/*
		功能:	设置控件的字体
		返回值: 成功返回0, 失败返回非0
	*/
	DECLSKINHAPI SkinH_SetFont(
						HWND hWnd,		//控件的句柄
						HFONT hFont		//字体句柄
						);
		
		
	/*
		功能:	设置控件的字体
		返回值: 成功返回0, 失败返回非0
	*/
	DECLSKINHAPI SkinH_SetFontEx(
						HWND hWnd,		//控件的句柄
						LPCTSTR szFace,	//字体名称
						int nHeight,	//字体高度
						int nWidth,		//字体宽度
						int nWeight,	//字体磅数
						int nItalic,	//是否斜体
						int nUnderline,	//是否下划线
						int nStrikeOut	//是否删除线
						);


	/*
		功能:	返回签名结果
		返回值: 已签名返回1，未签名返回0
	*/
	DECLSKINHAPI SkinH_VerifySign();


}
#endif