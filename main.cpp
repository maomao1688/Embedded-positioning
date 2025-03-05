#include<iostream>
#include<fstream>
#include<windows.h>
#include<stdlib.h>
#include <chrono>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include"MultiGateSolutionByDistance.h"

#pragma comment(lib,"MultiGateSolutionByDistance.lib")

using namespace std;

class TimerWheel {
public:
    using Task = std::function<void()>;

    explicit TimerWheel(size_t wheel_size, int interval_ms)
        : wheel_size_(wheel_size),
        interval_ms_(interval_ms),
        wheel_(wheel_size),
        current_index_(0) {}

    ~TimerWheel() {
        Stop();
    }

    void Start() {
        if (running_) {
            return;
        }
        running_ = true;
        thread_ = std::thread([this]() {
            while (running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
                Tick();
            }
            std::cout << "timer oooops!" << std::endl;
            });
        thread_.detach();
    }

    void Stop() {
        if (!running_) {
            return;
        }
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    void AddTask(int timeout_ms, Task task) {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t ticks = timeout_ms / interval_ms_;
        size_t index = (current_index_ + ticks) % wheel_size_;
        size_t allindex = index;
        for (size_t i = 1; allindex < wheel_size_; i++)
        {
            allindex = index * i;
            if (allindex >= wheel_size_)
                break;
            wheel_[allindex].push_back(task);
        }

    }

private:
    void Tick() {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& tasks = wheel_[current_index_];
        for (const auto& task : tasks) {
            task();
        }
        //tasks.clear();
        current_index_ = (current_index_ + 1) % wheel_size_;
    }

private:
    size_t wheel_size_;
    int interval_ms_;
    std::vector<std::list<Task>> wheel_;
    size_t current_index_;
    bool running_ = false;
    std::thread thread_;
    std::mutex mutex_;
};

void putouttime() {
    SYSTEMTIME localtime;
    GetLocalTime(&localtime); //获取时间
    cout << localtime.wHour << ":";//输出
    cout << localtime.wMinute << ":";
    cout << localtime.wSecond << ":";
    cout << localtime.wMilliseconds << endl;
    return;
}

int time_ms = 0;
int i = 0;
int sum = 0;
void print_time_ms() {      //打印时间方便观察程序运行过程，便于debug，实际算法中不需要
    SYSTEMTIME localtime;
    GetLocalTime(&localtime); //获取时间
    int a = localtime.wSecond * 1000 + localtime.wMilliseconds;
    cout << a - time_ms << "\t" << i << "\t" << sum << endl;
    i++;
    sum += a - time_ms;
    time_ms = a;
    return;
}

void error(string s) {
    cout << s << endl;
    system("pause");
}

struct XY       //坐标
{
    int x;      //x坐标
    int y;      //y坐标
};

//全局变量
vector<UserInfo_t>userinfo_lobal_1, userinfo_lobal_2;
XY a, b;                    //两个锚点的坐标(全局变量)
string a_name, b_name;      //两个锚点的名称(全局变量)

void getinstance1() {   //获取所有标签到第一个锚点的距离（当前锚点）
    userinfo_lobal_1.clear();
    getMasterInstance(userinfo_lobal_1);
    cout << "get1 ";
    //print_time_ms();
    return;
}
void getinstance2() {       //获取所有标签到第二个锚点的距离
    userinfo_lobal_2.clear();
    getSlaveInstance(b_name, userinfo_lobal_2);
    cout << "get2 ";
    //print_time_ms();
    return;
}

XY coordinate_calculation(XY a, XY b, double l_ac, double l_bc, bool ifpass) { //已知ab两点坐标以及c点到ab两点距离，求c点坐标
    double l_ab = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    double cosA = (l_ac * l_ac + l_ab * l_ab - l_bc * l_bc) / (2 * l_ac * l_ab);
    double sinA = sqrt(1 - cosA * cosA);
    XY c;
    if (!ifpass) c.y = l_ac * sinA;
    else c.y = -l_ac * sinA;
    c.x = a.x + l_ac * cosA;
    return c;

}

//全局变量
int righti;                 //将在开门区域的用户的i记下来
string user_Crossgate;      //正在通过闸门的用户（同时只有一个）   
bool ifopen = false;          //此时门是否开着（是否有过闸用户）

void Logic() {
    //首先获取当前时间的用户位置信息
    //getinstance1();
    //getinstance2();
    cout << "Logic" << endl;
    //处理得到的定位数据，计算是否在开闸门区域
    XY c;
    int counter = 0;            //计数器，判断是否同时有多个用户在开门区域内，若有，则不能开门
    bool ifpass = false;        //标志某个用户是否正在通过，以判断y坐标为正还是负

    for (int i = 0; i < userinfo_lobal_1.size(); i++) {     //遍历此次获取到的所有用户位置信息
        cout << "ifopen:" << ifopen << endl;
        if (ifopen) {
            if (userinfo_lobal_1[i].userId.compare(user_Crossgate) == 0) ifpass = true;
            else ifpass = false;
        }
        c = coordinate_calculation(a, b, userinfo_lobal_1[i].distance, userinfo_lobal_2[i].distance, ifpass);
        cout << userinfo_lobal_1[i].userId << ":" << c.x << ", " << c.y << endl;
        //判断有几个用户在开门区域
        if (((c.x >= a.x && c.x <= b.x) || (c.x <= a.x && c.x >= b.x)) && (c.y - a.y >= 30 && c.y - a.y <= 50)) {   //当用户靠近闸机前 30-50CM 时（假定两个锚点y坐标相同）
            counter++;
            righti = i;
        }
        //若有过闸用户，判断过闸用户是否已经通过闸门进入计费区域
        if (ifopen) {
            if (userinfo_lobal_1[i].userId.compare(user_Crossgate) == 0) {
                if (c.y < -100) {   //通过闸门进入计费区域
                    genPurchaseVoucher(user_Crossgate);
                    cout << "生成计费凭证" << endl;
                    ifopen = false;     //用户已通过，没有过闸用户
                }
            }
        }
    }
    cout << "counter:" << counter << endl;
    //若只有一个用户在开门区域,并且此时没有正在过闸用户，则开门,并将此用户的名称记录下来
    if (counter == 1 && !ifopen) {
        if (openGate(userinfo_lobal_1[righti].userId) != 0) error("openGate error!");
        cout << "opengate" << endl;
        ifopen = true;
        user_Crossgate = userinfo_lobal_1[righti].userId;
    }
}




int main() {
    //从config.ini文件读取锚点信息
    fstream ifs;
    ifs.open("config.ini");
    string tmp;
    int counter = 0;        //计数器，判断是第几个锚点
    if (ifs.is_open()) {
        while (getline(ifs, tmp)) {
            if (tmp[0] != '#') {
                int i = tmp.find('=');
                int j = tmp.find(',');
                if (counter == 0) {
                    a_name = tmp.substr(0, i);
                    a.x = stoi(tmp.substr(i + 1, j - i - 1));
                    a.y = stoi(tmp.substr(j + 1, tmp.size() - j - 1));
                    cout << "锚点a:" << a_name << " " << a.x << "," << a.y << endl;
                }
                if (counter == 1) {
                    b_name = tmp.substr(0, i);
                    b.x = stoi(tmp.substr(i + 1, j - i - 1));
                    b.y = stoi(tmp.substr(j + 1, tmp.size() - j - 1));
                    cout << "锚点b:" << b_name << " " << b.x << "," << b.y << endl;
                }
                counter++;
            }

        }
    }

    //初始化
    if (initialize() != 0) error("initialize error!");

    /* vector<UserInfo_t>userinfo1, userinfo2;
     getMasterInstance(userinfo1);
     getSlaveInstance(anotherAnchor, userinfo2);
     for (int i = 0; i < userinfo1.size(); i++) {
         cout << userinfo1[i].userId << "\t" << userinfo1[i].distance << endl;
     }
     for (int i = 0; i < userinfo2.size(); i++) {
         cout << userinfo2[i].userId << "\t" << userinfo2[i].distance << endl;
     }
     */
    putouttime();       //观察程序运行时间，便于debug，无实际意义


    //通过时间轮实现每250ms调用一次函数，这里调用的函数需要为void()类型，因此变量采用全局变量
    static TimerWheel timer(10, 250);
    timer.Start();
    //经测试，循环从大概第500ms开始，间隔为250ms，偶尔有大间隔（大概500ms左右）
    timer.AddTask(250, getinstance1);
    timer.AddTask(250, getinstance2);
    timer.AddTask(250, Logic);
    //三个任务各自独立
    std::this_thread::sleep_for(std::chrono::seconds(10));
    timer.Stop();

    return 0;
}