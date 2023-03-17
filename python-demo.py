import ctypes
from ctypes import *
from time import sleep
import psutil
import PySimpleGUI as sg
import threading
import re

login_add = {}
course_pid = []
val_data = []
sg.theme('black')
sg.theme('dark red')
sg.theme('dark green 7')
dll = ctypes.cdll.LoadLibrary('.//Control.dll')


@WINFUNCTYPE(None, c_char_p) #登录信息回调
def cbtSign(msg):
    get_msg = msg.decode('utf-8')
    login_info(get_msg)


@WINFUNCTYPE(None, c_char_p)
def cbtSelete(msg):
    print('2',msg)


@WINFUNCTYPE(None, c_char_p)
def cbtGetFriend(msg):
    print('3', msg)


@WINFUNCTYPE(None, c_char_p) #查询回调
def cbtRecvMsg(msg):
    get_msg = msg.decode('utf-8')
    pid = re.findall('@sdk\|@"iPid":(\d+)@sdk\|@',get_msg)
    v3 = re.findall('@sdk\|@"v3":"(.*?)"@sdk\|@',get_msg)
    print(pid,v3)
    dll.AddFriend(pid[0],v3[0],'测试')
    print('添加好友已发送')


dll.SetEnv('.//',cbtSign,cbtSelete,cbtGetFriend,cbtRecvMsg,'')



def login_info(pstr):
    login_mation = {}
    cut_str = pstr.split('@sdk|@')
    for s in cut_str:
        r = s.replace('"','')
        k = r.split(':')
        if k[0]:
            login_mation[k[0]] = k[1]
    login_add[int(login_mation['iPid'])] = login_mation['SignWxid']
    print(login_add)


def to_attach():
    course = psutil.pids()
    for pid in course:
        sleep(0.01)
        try:
            p = psutil.Process(pid)
            name = p.name()
            if name == 'WeChat.exe':
                course_pid.append(pid)
            else:
                pass
        except Exception as e:
            print(e)
    for v in course_pid:
        sleep(1)
        dll.WxAttach(v)
        print('附加成功')
    print('附加完成..')


def get_stat():
    to_attach()
    print('获取登录信息')
    for i in course_pid:
        while True:
            sleep(2)
            if i in login_add:
                break
            else:
                dll.GetState(i)
    print('登录信息获取完成')


def find_phone():

    dll.FindPhone(11111,'13400804312')

menu_def = [['&Application', ['E&xit']],
            ['&Help', ['&About']]]
right_click_menu_def = [[], ['Edit Me', 'Versions', 'Nothing', 'More Nothing', 'Exit']]
graph_right_click_menu_def = [[], ['Erase', 'Draw Line', 'Draw', ['Circle', 'Rectangle', 'Image'], 'Exit']]
input_layout = [
    [
     sg.Btn('附加进程', key='OLAY',  size=(9, 1)),
        sg.Button('添加好友', key='FRIEND', size=(9, 1))
    ],
]

logging_layout = [[sg.Text("Receive：")],
                  [sg.Multiline(size=(60, 15), font='Courier 8', expand_x=True, expand_y=True, write_only=True,
                                reroute_stdout=True, reroute_stderr=True, echo_stdout_stderr=True, autoscroll=True,
                                auto_refresh=True)]
                  # [sg.Output(size=(60,15), font='Courier 8', expand_x=True, expand_y=True)]
                  ]

layout = [[sg.MenubarCustom(menu_def, key='-MENU-', font='Courier 15', tearoff=True)],
          [sg.Text('请勿用于违法犯罪行为!', size=(38, 1), justification='center', font=("Helvetica", 16),
                   relief=sg.RELIEF_RIDGE, k='-TEXT HEADING-', enable_events=True)]]
layout += [[sg.TabGroup([[sg.Tab('功能选项', input_layout),

                          sg.Tab('运行日志', logging_layout)]], key='-TAB GROUP-', expand_x=True, expand_y=True),

            ]]
layout[-1].append(sg.Sizegrip())
windows = sg.Window('WxHook', layout, right_click_menu=right_click_menu_def, right_click_menu_tearoff=True,
                   grab_anywhere=True, resizable=True, margins=(0, 0), use_custom_titlebar=True, finalize=True,
                   keep_on_top=True)
windows.set_min_size(windows.size)


while True:
    event, value = windows.Read()
    if event == 'OLAY':
        threading.Thread(target=get_stat,daemon=True).start()
    elif event == 'FRIEND':
        threading.Thread(target=find_phone,daemon=True).start()
    elif event == sg.WINDOW_CLOSED:
        break





