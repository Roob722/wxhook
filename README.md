```cpp
#pragma once



//102 登录状态  103 加人返回信息
typedef void(__stdcall*cbtSign)(const char * pstr);

typedef void(__stdcall*cbtSelete)(const char * pstr);


typedef void(__stdcall*cbtGetFriend)(const char * pstr);

typedef void(__stdcall*cbtRecvMsg)(const char * pstr);

typedef void(__stdcall*cbtDBkey)(byte pKey[16]);

extern "C"
{
	
	// 特别注意    pwDllPath 结尾不要带 \\杠， 比如 "D:\企微再出发\sendCard\Release"
	BOOL SetEnv(const wchar_t* pwDllPath, PVOID pSign, PVOID pSelete, PVOID pGetFriend, PVOID pRecvmsg, PVOID pDBkey);

	
	//启动前调用MuOpen.exe 休眠1秒  就是多开微信
	//启动且 附加到企微  返回企微pid ， 小于4表示失败
	int WxStart(const wchar_t* pPP);//启动附加到微信

	//附加到微信   
	BOOL WxAttach(int dwPid);


	BOOL GetState(int dwPid);//获取信息状态，登录信息

	
	//寻人
	BOOL FindPhone(int npid, const wchar_t* pwPhone);
	//加人
	BOOL AddFriend(int npid, const wchar_t* pwV3, const wchar_t* pwVerify);


	//发名片pwWXID  企微好友uid,  外部群 uid 都行
	//pwCard  788de uid
	// SendCardMsg(g_pid, L"wxid_2tn9xxw1v98y22", L"wxid_1mxipy7a3g4h21");
	BOOL SendCardMsg(int npid, const wchar_t* pwWXID, const wchar_t* pwCard);

	//发图片
	//SendImage(g_pid, L"wxid_2tn9xxw1v98y22", L"D:\\1111.PNG");
	BOOL SendImage(int npid, const wchar_t* pwWXID, const wchar_t* pwPath);

	//发文字 pAtWXID 填0 
	//SendText(g_pid, L"wxid_2tn9xxw1v98y22", L"234242342423434442",0);
	VOID SendText(int dwPid, const wchar_t* pwwxid, const wchar_t* pwText, const wchar_t* pAtWXID = 0);

	//发链接
	//SendLink(g_pid, L"wxid_2tn9xxw1v98y22", L"标题1111",L"https://baike.baidu.com/item/1/31661?fr=aladdin", 
	//L"https://bkimg.cdn.bcebos.com/pic/8435e5dde71190ef76c61d6d79518a16fdfaaf51ca64?x-bce-process=image/resize,m_lfit,w_536,limit_1/format,f_jpg",
//	L"qweqweqwwrweripweiruoweuioruoiweuiorwuioruoiouioui");
	BOOL SendLink(int npid, const wchar_t* pwWXID, const wchar_t* pwTitle,
		const wchar_t* pwURL, const wchar_t* pwImage, const wchar_t* pwDesc);


	//根据 图片字节大小 找到图片
	//psource 加密图片的绝对路径
     //pdest 解密图片的路径
	//没找到返回0， 找到返回图片名
	const char* FindSizeFile(const wchar_t * pPath, DWORD fsize);


	//解密保存图片 
	//psource 加密图片的绝对路径
	//pdest 解密图片的路径
	//成功返回1， 失败返回0
	int DePic(const wchar_t *psource, const wchar_t *pdest);

	

	//获取单个好友信息。 会触发回调， MsgType == 102
	//GetWxFriInfo(g_pid, L"wxid_2tn9xxw1v98y22");
	VOID GetWxFriInfo(int dwPid, const wchar_t* pwwxid);
	

	//获取全部  好友\群\公众号\ 企微好友\ 群 列表。 会触发回调， MsgType == 102
	//GetAllWxFri(g_pid);
	BOOL GetAllWxFri(int dwPid);
	

	//获取群成员 会触发回调   MsgType == 104
	//GetGrpMember(g_pid, L"17804851601@chatroom");
	BOOL GetGrpMember(int npid, const wchar_t* pwGroup);
	
	
	//BOOL SendFileMsg(int npid, const wchar_t* pwWXID, const wchar_t* pwPath);//发送文件
	BOOL GetState1(int dwPid);
	const wchar_t* GetVer(const wchar_t* path);
	const wchar_t* GetProcessPath(int ipid);
}	

```
