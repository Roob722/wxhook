#pragma once



//102 ��¼״̬  103 ���˷�����Ϣ
typedef void(__stdcall*cbtSign)(const char * pstr);

typedef void(__stdcall*cbtSelete)(const char * pstr);


typedef void(__stdcall*cbtGetFriend)(const char * pstr);

typedef void(__stdcall*cbtRecvMsg)(const char * pstr);

typedef void(__stdcall*cbtDBkey)(byte pKey[16]);

extern "C"
{
	
	// �ر�ע��    pwDllPath ��β��Ҫ�� \\�ܣ� ���� "D:\��΢�ٳ���\sendCard\Release"
	BOOL SetEnv(const wchar_t* pwDllPath, PVOID pSign, PVOID pSelete, PVOID pGetFriend, PVOID pRecvmsg, PVOID pDBkey);

	
	//����ǰ����MuOpen.exe ����1��  ���Ƕ࿪΢��
	//������ ���ӵ���΢  ������΢pid �� С��4��ʾʧ��
	int WxStart(const wchar_t* pPP);//�������ӵ�΢��

	//���ӵ�΢��   
	BOOL WxAttach(int dwPid);


	BOOL GetState(int dwPid);//��ȡ��Ϣ״̬����¼��Ϣ

	
	//Ѱ��
	BOOL FindPhone(int npid, const wchar_t* pwPhone);
	//����
	BOOL AddFriend(int npid, const wchar_t* pwV3, const wchar_t* pwVerify);


	//����ƬpwWXID  ��΢����uid,  �ⲿȺ uid ����
	//pwCard  788de uid
	// SendCardMsg(g_pid, L"wxid_2tn9xxw1v98y22", L"wxid_1mxipy7a3g4h21");
	BOOL SendCardMsg(int npid, const wchar_t* pwWXID, const wchar_t* pwCard);

	//��ͼƬ
	//SendImage(g_pid, L"wxid_2tn9xxw1v98y22", L"D:\\1111.PNG");
	BOOL SendImage(int npid, const wchar_t* pwWXID, const wchar_t* pwPath);

	//������ pAtWXID ��0 
	//SendText(g_pid, L"wxid_2tn9xxw1v98y22", L"234242342423434442",0);
	VOID SendText(int dwPid, const wchar_t* pwwxid, const wchar_t* pwText, const wchar_t* pAtWXID = 0);

	//������
	//SendLink(g_pid, L"wxid_2tn9xxw1v98y22", L"����1111",L"https://baike.baidu.com/item/1/31661?fr=aladdin", 
	//L"https://bkimg.cdn.bcebos.com/pic/8435e5dde71190ef76c61d6d79518a16fdfaaf51ca64?x-bce-process=image/resize,m_lfit,w_536,limit_1/format,f_jpg",
//	L"qweqweqwwrweripweiruoweuioruoiweuiorwuioruoiouioui");
	BOOL SendLink(int npid, const wchar_t* pwWXID, const wchar_t* pwTitle,
		const wchar_t* pwURL, const wchar_t* pwImage, const wchar_t* pwDesc);


	//���� ͼƬ�ֽڴ�С �ҵ�ͼƬ
	//psource ����ͼƬ�ľ���·��
     //pdest ����ͼƬ��·��
	//û�ҵ�����0�� �ҵ�����ͼƬ��
	const char* FindSizeFile(const wchar_t * pPath, DWORD fsize);


	//���ܱ���ͼƬ 
	//psource ����ͼƬ�ľ���·��
	//pdest ����ͼƬ��·��
	//�ɹ�����1�� ʧ�ܷ���0
	int DePic(const wchar_t *psource, const wchar_t *pdest);

	

	//��ȡ����������Ϣ�� �ᴥ���ص��� MsgType == 102
	//GetWxFriInfo(g_pid, L"wxid_2tn9xxw1v98y22");
	VOID GetWxFriInfo(int dwPid, const wchar_t* pwwxid);
	

	//��ȡȫ��  ����\Ⱥ\���ں�\ ��΢����\ Ⱥ �б� �ᴥ���ص��� MsgType == 102
	//GetAllWxFri(g_pid);
	BOOL GetAllWxFri(int dwPid);
	

	//��ȡȺ��Ա �ᴥ���ص�   MsgType == 104
	//GetGrpMember(g_pid, L"17804851601@chatroom");
	BOOL GetGrpMember(int npid, const wchar_t* pwGroup);
	
	
	//BOOL SendFileMsg(int npid, const wchar_t* pwWXID, const wchar_t* pwPath);//�����ļ�
	BOOL GetState1(int dwPid);
	const wchar_t* GetVer(const wchar_t* path);
	const wchar_t* GetProcessPath(int ipid);
}	

