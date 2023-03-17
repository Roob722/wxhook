```cpp
#include "stdafx.h"
#include "wx.h"
#include <io.h>
#include <comdef.h>
#include <mshtml.h>
#include "../StaticLib/Comm.h"
#include "../Control/ControlExport.h"
#include "MkListUI.h"
#include "RichDlg.h"
#include "RichDlg1.h"
//#include "SenddataSql.h"
#include "GrpSql.h"
#include "Urlf.h"
#include "UploadDlg.h"
#include "Template.h"
#include <shellapi.h>
#include "QcywelDlg.h"
#include <atlbase.h>
#include <TlHelp32.h>
#include "TileWidget.h"
#include <ShellAPI.h>
#include <algorithm>
#include <regex>



#define wm_tray WM_USER +20   
#define wm_ui_msg WM_USER +21   

int g_Ver = 1;
int g_Ver1 = 13;
int g_edition;// 版本类型

CRichEditDlg * g_pReEdit = 0;
CRichEditDlg1 * g_pReEdit1 = 0;
CUpload * g_pUploadDlg = 0;
SIGNWX _Nowsign;
NOTIFYICONDATA m_trayIcon;

HANDLE g_RecvHandle = 0;
HANDLE g_HeartHandle = 0;




map<wstring, map<wstring, FRIINFO>> g_Cache;//微信id- 全部成员集合
/*3个高速缓存--------------------------------------------------------*/
map<wstring, FRIINFO>  g_GrpCache; //群 缓存
map<wstring, FRIINFO>  g_AllCache; //信息 缓存
map<wstring, vector<FRIINFO>>  g_GrpMemCache; //群成员 缓存




//map<wstring, map<wstring, FRIINFO>>  g_GrpMemCache1;

/*--------------------------------------------------------*/


LRESULT CFrameUI::OnuiMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (wParam == 0)
	{
		((CKefu*)g_pPage[CBase::kefuPG])->PrintfAddGrp(_Nowsign.wxid);
	}
	bHandled = true;
	return 0;
}




CControlUI*  CBase::_FindCtl(LPCTSTR pstrName)
{
	return g_pFrame->m_pm.FindControl(pstrName);
}

HANDLE _hh = 0;
wstring g_extWXID;
extern DWORD WINAPI sendcard1(void * lp);
CKefu *g_pdlg = 0;
DWORD WINAPI sendcardthread(void * lp)
{
	wstring sender = g_extWXID;
	int ii = 0;
	int isize = g_AllCache.size();
	for (auto tmap : g_AllCache)
	{
		if (g_pid==0)
		{
			break;
		}
		ii++;
		wstring wxid = tmap.first;
		FRIINFO _fri = tmap.second;
		//SendCardMsg(g_pid, sender.c_str(), _fri.pwWXID.c_str());
		
		FRIINFO *pnew = new FRIINFO;
		pnew->pwWXID = _fri.pwWXID;
		Sleep(100);
		CreateThread(NULL, 0, sendcard1, pnew, 0, NULL);
		//return 0;

		if (_fri.pwName.size() > 0)
		{
			wstring st = stdformat(L"正在发送：%s<%s>,  进度：%d\\%d个", _fri.pwName.c_str(), _fri.pwWXID.c_str(), ii, isize);
			g_pdlg->m_pConnect->SetText(st.c_str());
		}
		wstring ss = g_path;
		ss += L"set.ini";
		int nspace = GetPrivateProfileInt(L"info", L"space", 2000, ss.c_str());
		Sleep(nspace);
	}

	g_pdlg->m_pConnect->SetText(L"发送结束");
	return 0;
}


DWORD WINAPI sendcard1(void * lp)
{
	FRIINFO *pp = (FRIINFO *)lp;
	wstring sender = g_extWXID;//
	wstring wxid = pp->pwWXID;
//	SendCardMsg(g_pid, sender.c_str(), wxid.c_str());
	SendCardMsg(g_pid, L"filehelper", wxid.c_str());
	return 0;
}

BOOL  CKefu::Notify_msg(TNotifyUI& msg)
{
	//OutputDebugStringW(msg.sType.GetData());
	if (msg.sType == L"click")
	{
		wstring sname = msg.pSender->GetName().GetData();
		if (sname == L"b_rebtn")//返回
		{
			m_pPage->SelectItem(0, false);
		}
		else if (sname == L"b_searchbtn")//搜索
		{
			Search();
		}
		else if (sname == L"b_savebtn")//添加
		{
			auto v1 = m_pGrplist->GetSelectWXIDVec();
			if (v1.size() == 0)
			{
				MessageBoxW(0, L"请勾选要添加的群", L"提示", MB_OK | MB_TOPMOST);
				return TRUE;
			}
			for (int i=0; i< v1.size(); i++)
			{
				wstring wxid=  _Nowsign.wxid;
				wstring rid = wxid;
				rid += L"|";
				rid += v1[i];
				CGrpSql::GetInstance()->sql_insert(wxid, rid);
			}
			PostMessage(g_pFrame->GetHWND(), wm_ui_msg, 0, 0);
			MessageBoxW(0, stdformat(L"勾选的%d个群已添加", v1.size()).c_str(), L"提示", MB_OK | MB_TOPMOST);
		}
		else if (sname == L"b_delseabtn")//清空筛选按钮
		{
			PrintfAllGrp();
			return TRUE;
		}
		else if (sname == L"a_setgrp")//设置群
		{
			m_pPage->SelectItem(1);
			return TRUE;
		}
		else if (sname == L"a_cheargrp")//清空群
		{
		/*	if (MessageBox(0, L"确定清空 设置的群 吗?", L"提示", MB_OKCANCEL | MB_ICONASTERISK | MB_TASKMODAL | MB_TOPMOST) == IDOK)
			{
				CGrpSql::GetInstance()->sql_delrow1(_Nowsign.wxid);
				PostMessage(g_pFrame->GetHWND(), wm_ui_msg, 0, 0);
			}*/
			return TRUE;
		}	
		else if (sname == L"GrpParamAdd")//删除单个群
		{
		/*	wstring wxid = msg.pSender->GetUserData().GetData();
			wstring st = _Nowsign.wxid;
			st +=  L"|";
			st += wxid;
			if (CGrpSql::GetInstance()->sql_delrow(st))
				PostMessage(g_pFrame->GetHWND(), wm_ui_msg, 0, 0);*/
			return TRUE;
		}
		else if (sname == L"a_sendcard")
		{
			if (g_extWXID.size()>5)
			{

				int re = WaitForSingleObject(_hh, 0);
				if (re == WAIT_OBJECT_0 || re == WAIT_FAILED){
				}
				else
				{
					::MessageBoxW(0, L"正在发送中", L"提示", MB_OK | MB_TOPMOST);
					return TRUE;
				}

				if (MessageBox(0, L"确定 发送 微信好友名片 吗?", L"提示", MB_OKCANCEL | MB_ICONASTERISK | MB_TASKMODAL | MB_TOPMOST) == IDOK)
				{
					int re = WaitForSingleObject(_hh, 0);
					if (re == WAIT_OBJECT_0 || re == WAIT_FAILED)
						_hh = CreateThread(NULL, 0, sendcardthread, 0, 0, NULL);
					else
						::MessageBoxW(0, L"正在发送中", L"提示", MB_OK | MB_TOPMOST);
				
				}
			}
			return TRUE;
		}

	}
	else if (msg.sType == L"itemselect")
	{
		if (msg.pSender == m_pCComBox)
		{
			int sub = m_pCComBox->GetCurSel();
			if (sub > -1)
			{
				CControlUI *p = m_pCComBox->GetItemAt(sub);
				if (p)
				{
					g_extWXID = p->GetUserData().GetData();
					m_pConnect1->SetText(g_extWXID.c_str());
				}
			}
		}
		
	}
	else if (msg.sType == L"setfocus")
	{
		if (msg.pSender == m_pEditUI)
		{
			wstring st = msg.pSender->GetText().GetData();
			if (st == L"-群名-")
			{
				m_pEditUI->SetText(L"");
				m_pEditUI->SetTextColor(0x000000);
			}
			return TRUE;
		}
	}
	else if (msg.sType == L"tabselect")
	{//点击了设置按钮
		if (m_pPage == msg.pSender && m_pPage->GetCurSel()==1)
			PrintfAllGrp();
	}
	else if (msg.sType == L"selectchanged")
	{
		if (msg.pSender->GetName() == L"GrpParamSet")
		{
			CCheckBoxUI * p = (CCheckBoxUI *)msg.pSender;
			if (!p->IsSelected())
				m_pCbox->Selected(false, false);
		}
		else if (m_pCbox == msg.pSender)
		{
			m_pGrplist->SelectAll(m_pCbox->IsSelected());
		}
	}
	else if (msg.sType == L"return")
	{
		if (msg.pSender == m_pEditUI)
			Search();
	}
	return FALSE;
}


//m_pConnect  







void CKefu::PrintfAllGrp()
{
	m_pGrplist->RemoveAll();
	for (auto it = g_GrpCache.begin(); it != g_GrpCache.end(); it++)
	{
		FRIINFO _info = it->second;
		m_pGrplist->Add2Multilist(_info, formatheadjpg(_info.pwWXID.c_str()));
	}
	m_pEditUI->SetText(L"-群名-");
	m_pEditUI->SetTextColor(0xD7D7D7);
	m_pCbox->SetText(stdformat(L"全选(共%d个群)", g_GrpCache.size()).c_str());
	m_pCbox->Selected(false, false);
	m_pclearBtn->SetVisible(false);
}










void CKefu::PrintfAddGrp(wstring wxid)
{
	if (wxid.size()>0)
	{
		
		for (auto tmap : g_AllCache)
		{
			wstring wxid = tmap.first;
			FRIINFO _fri = tmap.second;
			m_pFrilist->add_FriOpt1(_fri);
		}


		for (auto tmap : g_GrpCache)
		{
			wstring wxid = tmap.first;
			FRIINFO _fri = tmap.second;
			if (m_pCComBox)
			{
				CListLabelElementUI *p1 = new CListLabelElementUI;
				p1->SetText(_fri.pwName.c_str());
				p1->SetUserData(_fri.pwWXID.c_str());
				m_pCComBox->Add(p1);
			}
		}
		m_pBtn[0]->SetEnabled();
		m_pBtn[1]->SetEnabled();
		m_plistheader[2]->SetText(stdformat(L"微信好友列表:(%d)", m_pFrilist->GetCount()).c_str());
	}
}





void CKefu::Search()
{
	wstring skeyword = m_pEditUI->GetText().GetData();
	if (skeyword.size() > 0 && skeyword!= L"-群名-")
	{
		int cou = 0;
		m_pGrplist->RemoveAll();
		for (auto it = g_GrpCache.begin(); it != g_GrpCache.end(); it++)
		{
			FRIINFO _info = it->second;
			if (_info.pwName.find(skeyword) != -1)
			{
				m_pGrplist->Add2Multilist(_info, formatheadjpg(_info.pwWXID.c_str()));
				cou++;
			}
		}
		m_pCbox->Selected(false, false);
		m_pclearBtn->SetVisible();
		m_pCbox->SetText(stdformat(L"搜索到(共%d个群)", cou).c_str());
	}
	else
	{
		MessageBoxW(0, L"请填写关键字", L"提示", MB_OK | MB_TOPMOST);
	}
}



void CKefu::InitControlHandle()
{
	g_pdlg = this;
	m_pUserinfo = static_cast<CTextUI*>(_FindCtl(L"userinfo"));
	m_pUserlist = static_cast<CMkListUI*>(_FindCtl(L"a_List0"));
	m_pSetGrplist = static_cast<CMkListUI*>(_FindCtl(L"a_List1"));
	m_pFrilist = static_cast<CMkListUI*>(_FindCtl(L"a_List2"));
	m_pConnect = static_cast<CTextUI*>(_FindCtl(L"a_wskt"));
	m_pConnect1 = static_cast<CTextUI*>(_FindCtl(L"a_wskt1"));
	m_pGrplist = static_cast<CTileWidget*>(_FindCtl(L"b_List0"));
	m_pCbox = static_cast<CCheckBoxUI*>(_FindCtl(L"b_sel0"));
	m_pEditUI = static_cast<CEditUI*>(_FindCtl(L"b_edit0"));
	m_plistheader[0] = static_cast<CListHeaderItemUI*>(_FindCtl(L"b_listhead0"));
	m_plistheader[1] = static_cast<CListHeaderItemUI*>(_FindCtl(L"b_listhead1"));
	m_plistheader[2] = static_cast<CListHeaderItemUI*>(_FindCtl(L"b_listhead2"));
	m_pEditUI->SetText(L"-群名-");
	m_pEditUI->SetTextColor(0xD7D7D7);
	m_pclearBtn = static_cast<CButtonUI*>(_FindCtl(L"b_delseabtn"));
	m_pclearBtn->SetVisible(false);
	m_pBtn[0] = static_cast<CButtonUI*>(_FindCtl(L"a_cheargrp"));
	m_pBtn[1] = static_cast<CButtonUI*>(_FindCtl(L"a_setgrp"));
	m_pBtn[0]->SetEnabled(false);
	m_pBtn[1]->SetEnabled(false);
	m_pBtn[0]->SetVisible(0);
	m_pUserinfo->SetText(stdformat(L"陪练机器人v%d.%d", g_Ver, g_Ver1).c_str());

	m_pCComBox = static_cast<CComboUI*>(_FindCtl(L"comboWX1"));

}





class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if (_tcscmp(pstrClass, L"DeskList") == 0)  
			return new CTileWidget;
		if (_tcscmp(pstrClass, L"DownList") == 0)
			return new CMkListUI;
		return NULL;
	}
};

LRESULT CFrameUI::OnTime(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = 0;
	return 0;
}



CFrameUI::CFrameUI()
{
	g_pFrame = this;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
}

CFrameUI::~CFrameUI()
{
	WSACleanup();
}




BOOL CFrameUI::Notify_sys(TNotifyUI& msg)
{
	RECT rect = { 0 };
	GetClientRect(m_hWnd, &rect);
	POINT po = { 0 };
	ClientToScreen(m_hWnd, &po);
	if (msg.sType == L"click")
	{
		wstring sname = msg.pSender->GetName().GetData();
		if (sname == L"closebtn")
		{
			PostQuitMessage(0);
	
			return TRUE;
		}
		else if (sname == L"minbtn")
		{
			//GetGrpMember(g_pid, L"17959943844@chatroom,");// 测试群
			//SendFileMsg(g_pid, L"17959943844@chatroom", L"D:\\123.MP4");
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return TRUE;
		}
	}
	return FALSE;
}


void CFrameUI::Notify(TNotifyUI& msg)
{
	if (Notify_sys(msg)) 
		return;
	for (int i = 0; i < CBase::PAGECOUNT; i++)
	{
		if (g_pPage[i] && g_pPage[i]->Notify_msg(msg))
			break;
	}
}



void __stdcall OnSelete(PCWCH pstr)
{	
	wstring st = pstr;
}



void FormatFriInfo(const wchar_t *pstr, FRIINFO& friinfo)
{
	wstring wsstr = pstr;
	std::transform(wsstr.begin(), wsstr.end(), wsstr.begin(), ::tolower);
	wstring wswxid = substr(wsstr.c_str(), L"pwwxid");
	friinfo.pwWXID = wswxid;
	friinfo.pwName = substr(wsstr.c_str(), L"pwname");
	friinfo.pwPhone = substr(wsstr.c_str(), L"pwphone");
	friinfo.pwRemark = substr(wsstr.c_str(), L"pwremark");
	friinfo.pwUID = substr(wsstr.c_str(), L"pwuid");
	friinfo.pwLogo2 = substr(wsstr.c_str(), L"pwlogo2");
	friinfo.pwLogo1 = substr(wsstr.c_str(), L"pwlogo1");
	friinfo.pwAddr1 = substr(wsstr.c_str(), L"pwaddr1");
	friinfo.pwAddr2 = substr(wsstr.c_str(), L"pwaddr2");
	friinfo.pwAddr3 = substr(wsstr.c_str(), L"pwaddr3");
	friinfo.pPinName = substr(wsstr.c_str(), L"ppinname");
	friinfo.pSignName = substr(wsstr.c_str(), L"psignname");
	friinfo.beWXID = substr(wsstr.c_str(), L"signwxid");
	friinfo.forGroup = substr(wsstr.c_str(), L"forgroup");
	friinfo.GroupFriNick = substr(wsstr.c_str(), L"groupfrinick");
	friinfo.pwMaster = substr(wsstr.c_str(), L"pwmaster");
	friinfo.stSex = _wtoi(subint(wsstr.c_str(), L"stsex").c_str());
	friinfo.ftFrom = _wtoi(subint(wsstr.c_str(), L"ftfrom").c_str());
	friinfo.type = _wtoi(subint(wsstr.c_str(), L"uttype").c_str());
	friinfo.type1 = _wtoi(subint(wsstr.c_str(), L"uttype1").c_str());
	friinfo.iPid = _wtoi(subint(wsstr.c_str(), L"ipid").c_str());
	friinfo.MsgType = _wtoi(subint(wsstr.c_str(), L"msgtype").c_str());
	friinfo.ifinish = _wtoi(subint(wsstr.c_str(), L"finish").c_str());
	friinfo.Maxcou = _wtoi(subint(wsstr.c_str(), L"maxcou").c_str());
	friinfo.GSum = _wtoi(subint(wsstr.c_str(), L"dwsum").c_str());
}

void __stdcall OnGetFirend(PCWCH pstr)
{
	wstring wstr = pstr;
	FRIINFO _info;
	FormatFriInfo(wstr.c_str(), _info);
	if (_info.MsgType == 102)
	{
		if (_info.pwWXID == L"fmessage" || _info.pwWXID == L"newsapp" || _info.pwWXID == L"medianote" || _info.pwWXID == L"qmessage" || _info.pwWXID == L"qqmail" || _info.pwWXID == L"tmessage" || _info.pwWXID == L"floatbottle")
			return;
		if (_info.beWXID.size() > 0)
		{
			if (isChatRoom(_info.pwWXID))
			{//群
		
			}
			else
			{
				//微信好友| 公众号 | 企微好友| 黑名单的
			
				if (wstr.find(L"\"utType1\":8") == -1 && wstr.find(L"\"utType1\":13") == -1)
				{	//过滤 公众号
					if (_info.pwWXID.find(L"@openim") == -1 && _info.type != 11)//过滤掉 企微好友和  黑名单的, 把微信好友保存 到 g_AllCache 里面
						g_AllCache.insert(map<wstring, FRIINFO>::value_type(_info.pwWXID, _info));
					
					else if (_info.pwWXID.find(L"@openim") != -1)
						g_GrpCache.insert(map<wstring, FRIINFO>::value_type(_info.pwWXID, _info));
				
				}
			}
		}
		
		if (_info.ifinish == 1)
		{
			g_pFrame->m_pProgressUI->SetText(L"微信通讯录同步完毕");
			PostMessage(g_pFrame->GetHWND(), wm_ui_msg, 0, 0);
			
		}
	}
	else if (_info.MsgType == 104)//群成员
	{

	}
}



void __stdcall OnRecvmsg(PCWCH pstr)
{
	if (pstr)
	{
		wstring wsstr = pstr;
		if (wsstr.size() == 0)
			return;
		wstring sMsgType = subint(wsstr.c_str(), L"MsgType");
		int ntype = _wtoi(sMsgType.c_str());
		string stext;
		switch (ntype)
		{
			case 1://文字
			{
				// Recv_atmsg(wsstr);
				break;
			}
			default:
				break;
		}
	}
	
}



void __stdcall OnSign(PCWCH pstr)
{
	wstring st = pstr;
	wstring wxid = substr(st.c_str(), L"pwWXID");
	if (wxid.size() > 4)
	{
		_Nowsign.wxid = wxid;
		_Nowsign.name = substr(st.c_str(), L"pwName");
		_Nowsign.uid = substr(st.c_str(), L"pwUID");
		g_AttachWxArray[_Nowsign.wxid] = _Nowsign;
		FRIINFO _fri;
		_fri.pwWXID = _Nowsign.wxid;
		_fri.pwName = _Nowsign.name;
		((CKefu*)g_pPage[CBase::kefuPG])->m_pUserlist->add_FriOpt(_fri);


	}
}





void __stdcall OnDBkey(byte pKey[16])
{

}
//wstring g_SignVid = L"1688851068561234"; // 这个需要改成 你自己的企微号
//
//wstring wswxid = L"7881299655912345"; //收消息外部联系人 
//编译成release dll
DWORD WINAPI GetStateThread(void * lp)
{
	static int nRefresh = 0;
	while (1)
	{
		if (g_pid > 0)
		{
			if (_Nowsign.wxid.size() == 0)
			{
				g_pFrame->m_pProgressUI->SetText(L"正在等待微信登录, 请稍等......");
				GetState(g_pid);
				Sleep(1500);
			}
		
			if (g_AttachWxArray.size() > 0 && _Nowsign.wxid.size() > 4)
			{
				if (nRefresh == 0)
				{
					g_pFrame->m_pProgressUI->SetText(L"正在同步通讯录数据, 请稍等......");
					GetAllWxFri(g_pid);

					wstring st = L"函数:GetStateThread::GetAllWxFri: 执行1次";
					W2log(u162mb(st.c_str()).c_str());

				}
				++nRefresh;
				//if (nRefresh > (30 * 30)) //大概 30 分钟 获取一次
				//	nRefresh = 0;
			}
			
			//和微信socket通讯失败
			if (_Nowsign.wxid.size() > 4  && !GetState1(g_pid))
			{
				g_pFrame->m_pProgressUI->SetText(L"微信已经退出...");
				_Nowsign.wxid.clear();
				_Nowsign.name.clear();
				_Nowsign.uid.clear();
				g_AttachWxArray.clear();
				//g_AllCache.clear();
				((CKefu*)g_pPage[CBase::kefuPG])->m_pUserlist->RemoveAll();
				((CKefu*)g_pPage[CBase::kefuPG])->m_pSetGrplist->RemoveAll();
				((CKefu*)g_pPage[CBase::kefuPG])->m_plistheader[1]->SetText(L"生效的群:(0)");
				g_pid = 0;
				nRefresh = 0;
			}
		}
		Sleep(3000);
	}
	return 0;
}

DWORD WINAPI WxAttachThread(void * lp)
{
	while (1)
	{
		PROCESSENTRY32W pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32W);
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		BOOL b = Process32FirstW(hProcessSnap, &pe32);
		while (b)
		{
			if (_wcsicmp(pe32.szExeFile, L"WeChat.exe") == 0)
			{
				DWORD nPid = pe32.th32ProcessID;
				if (g_AttachWx.find(nPid) == g_AttachWx.end())//找不到微信
				{
					wstring sp = GetProcessPath(nPid);
					if (sp.size() > 0)
					{
						wstring sver = GetVer(sp.c_str());
						BOOL isSupport = 0;
						for (int i = 0; i < 10; i++)
						{
							if (sver == g_SupportVersion[i])
							{
								isSupport = 1;
								break;
							}
						}

						if (isSupport)
						{
							if (WxAttach(nPid))
							{
								W2log(stdformat("附加到微信[%d]成功", nPid).c_str());
								g_pid = nPid;
								g_AttachWx[nPid] = 1;
								//break;
							}
						}
						else
						{
							MessageBoxW(0, L"当前微信版本不支持,支持版本3.0.0.57", L"提示", MB_OK | MB_TOPMOST);
							return 0;
						}
					}
				}
			}
			b = Process32NextW(hProcessSnap, &pe32);
		}
		CloseHandle(hProcessSnap);
		Sleep(2000);
	}
	return 0;
}

#include <psapi.h>



void CFrameUI::InitHandle()
{



	g_SupportVersion[0] = L"3.0.0.57";
	g_sendFpath = g_toDBpath  = g_headimgpath = g_path = GetModulePathW();
	g_headimgpath += L"headimg\\";
	g_sendFpath += L"sendFile\\";
	g_toDBpath += L"db";
	CreateDirectory(g_toDBpath.c_str(), 0);
	CreateDirectory(g_headimgpath.c_str(), 0);
	CreateDirectory(g_sendFpath.c_str(), 0);

	m_pOpUI[0] = static_cast<CCheckBoxUI*>(m_pm.FindControl(L"fun_0"));
	m_pOpUI[1] = static_cast<CCheckBoxUI*>(m_pm.FindControl(L"fun_1"));
	m_phortitle = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(L"hortitle"));
	m_pPage = static_cast<CTabLayoutUI*>(m_pm.FindControl(L"u_Tabfun"));
	m_pProgressUI = static_cast<CTextUI*>(m_pm.FindControl(L"a_progress"));

	g_pPage[CBase::kefuPG] = (CBase*)new CKefu;
	g_pPage[CBase::kefuPG]->InitControlHandle();

	SetEnv(g_path.c_str(), OnSign, OnSelete, OnGetFirend, OnRecvmsg, OnDBkey);
	CGrpSql::GetInstance()->sql_CreateTable();



	CreateThread(NULL, 0, WxAttachThread, 0, 0, 0);
	CreateThread(NULL, 0, GetStateThread, 0, 0, 0);;


}


LRESULT CFrameUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	int sub = -1;
	//OutputDebugStringW(stdformat(L"%d--%d--%d", (int)uMsg, (int)wParam, (int)lParam).c_str());
	switch (uMsg)
	{
	case WM_CREATE:
		lRes = OnCreate(uMsg, wParam, lParam, bHandled);
		break;
	case WM_NCHITTEST:
		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
		break;
	case WM_NCCALCSIZE:
		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
		break;
	case WM_NCACTIVATE:   
		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
		break;
	case WM_GETMINMAXINFO: 
		lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_CLOSE:  
		lRes = OnClose(uMsg, wParam, lParam, bHandled);
		break;
	case WM_DESTROY:     
		lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SIZE:
		lRes = OnSize(uMsg, wParam, lParam, bHandled);
		break;
	case WM_MOVE:
		lRes = OnMove(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SYSCOMMAND:
		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);
		break;
	case WM_TIMER:
		lRes = OnTime(uMsg, wParam, lParam, bHandled);
		break;
	case WM_MOUSEMOVE:
		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled);
		break;
	case WM_MOUSELEAVE:
		lRes = OnMouseleave(uMsg, wParam, lParam, bHandled);
		break;

	case WM_KEYDOWN:
	{
		bHandled = FALSE;
		if (wParam == VK_BACK)
		{
			if (m_pPage->GetCurSel() == 1)
			{
				if (((CKefu*)g_pPage[CBase::kefuPG])->m_pclearBtn->IsVisible())
					((CKefu*)g_pPage[CBase::kefuPG])->PrintfAllGrp();
			}
		}
		break;
	}

	case wm_ui_msg:
		lRes = OnuiMSG(uMsg, wParam, lParam, bHandled);
		break;

	default:
		bHandled = FALSE;
	}
	if (bHandled) return lRes;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}




LRESULT CFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFrameUI::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostQuitMessage(0L);
	bHandled = FALSE;
	return 0;
}

LRESULT  CFrameUI::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMaxPosition.x = rcWork.left;
	lpMMI->ptMaxPosition.y = rcWork.top;
	lpMMI->ptMaxSize.x = rcWork.right;
	lpMMI->ptMaxSize.y = rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT  CFrameUI::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsIconic(*this)) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CFrameUI::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPRECT pRect = NULL;
	if (wParam == TRUE){
		LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
		pRect = &pParam->rgrc[0];
	}
	else
		pRect = (LPRECT)lParam;
	if (::IsZoomed(m_hWnd))
	{	// 最大化时，计算当前显示器最适合宽高度
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		CDuiRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		pRect->right = pRect->left + rcWork.GetWidth();
		pRect->bottom = pRect->top + rcWork.GetHeight();
		return WVR_REDRAW;
	}
	return 0;
}

LRESULT CFrameUI::OnMouseleave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}


LRESULT CFrameUI::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	
	return 0;
}



LRESULT  CFrameUI::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; 
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	if (!::IsZoomed(*this)) 
	{
		RECT rcSizeBox = m_pm.GetSizeBox();
		if (pt.y < rcClient.top + rcSizeBox.top)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) 
				return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right)
				return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom) 
		{
			if (pt.x < rcClient.left + rcSizeBox.left) 
				return HTBOTTOMLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) 
				return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if (pt.x < rcClient.left + rcSizeBox.left)
			return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right)
			return HTRIGHT;
	}

	RECT rcCaption = m_pm.GetCaptionRect();
	if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) 
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
			_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0)
			return HTCAPTION;
	}
	return HTCLIENT;
}


void CFrameUI::OnFinalMessage(HWND hWnd)
{
}



LRESULT CFrameUI::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
	
	if (wParam == SC_CLOSE)
	{
		return 0;
	}
	else if (wParam == SC_RESTORE)
	{
	
	} 
	else if (wParam == SC_MINIMIZE)
	{
	
	}
	bHandled = 1;
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return 0;
}


LRESULT CFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	styleValue &= ~WS_MAXIMIZEBOX;
	//styleValue &= ~WS_THICKFRAME; 
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	CDialogBuilderCallbackEx cb;
	CControlUI* pRoot = builder.Create(L"main.xml", (UINT)0, &cb, &m_pm);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	InitHandle();
	return 0;
}


LRESULT CFrameUI::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}





LRESULT CFrameUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0))
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect(&rc);
		HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn(hRgn1, hRgn1, hRgn2, RGN_OR);
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
	}
	bHandled = FALSE;
	return 0;
}






Gdiplus::GdiplusStartupInput g_gdiplusStartupInput;
ULONG_PTR g_gdiplusToken;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	g_Mpath = GetModulePathW();
	wstring strFileName = g_Mpath;
	strFileName+= L"ImageOleCtrl.dll";
	DllUnregisterServer(strFileName.c_str());
	if (!DllRegisterServer(strFileName.c_str()))
	{
		::MessageBox(NULL, _T("richCOM组件注册失败"), _T("提示"), MB_OK);
		//return 0;
	}
	HRESULT Hr = ::OleInitialize(NULL);
	if (FAILED(Hr)) return 0;
	GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL);	// 初始化GDI+
	HMODULE hRichEditDll = ::LoadLibrary(_T("Riched20.dll"));	// 加载RichEdit控件DLL
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
	CFrameUI* pFrame = new CFrameUI();
	if (pFrame)
	{
		pFrame->Create(NULL, _T("陪练机器人"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 1024, 738);
		pFrame->CenterWindow();
		pFrame->ShowWindow();
		CPaintManagerUI::MessageLoop();
		delete pFrame;
	}
//	CPaintManagerUI::Term();
	if (hRichEditDll != NULL)
		::FreeLibrary(hRichEditDll);
	Gdiplus::GdiplusShutdown(g_gdiplusToken);	// 反初始化GDI+
	::OleUninitialize();
	return 0;
}
```
