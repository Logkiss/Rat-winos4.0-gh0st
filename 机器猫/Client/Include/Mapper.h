
#ifndef __IO_MAPPER__
#define __IO_MAPPER__

#define net_msg

class __declspec(novtable) CIOMessageMap
{
public:
	virtual bool ProcessIOMessage(IOType clientIO, ClientContext* pContext, DWORD dwSize) = 0;
};

#define BEGIN_IO_MSG_MAP() \
public: \
		bool ProcessIOMessage(IOType clientIO, ClientContext* pContext, DWORD dwSize = 0) \
		{ \
			bool bRet = false; 

#define IO_MESSAGE_HANDLER(msg, func) \
			if (msg == clientIO) \
				bRet = func(pContext, dwSize); 

#define END_IO_MSG_MAP() \
		return bRet; \
	}

#endif