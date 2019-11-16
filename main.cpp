//
//  main.cpp
//  EDF and RM Simulator
//
//  Created by Toby Yu on 2019/11/12.
//  Copyright © 2019 Toby Yu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <math.h>

using namespace std;

struct Task
{
    int TID;
    int Phase;
    int Period;
    int WCET;
    int RDeadline;
    float Utilization;
};

struct Job
{
    int release_time;
    int remain_execution_time;
    int absolute_deadline;
    int TID; //屬於哪個Task的工作
    int Period;
};

int Total_Task_Number = 0;  // 總共的 task 個數
int Total_Job_Number = 0; // 總共的 job 個數
int Miss_Deadline_Job_Number=0;  // 錯過 deadline 的工作總數
int LCM = 0;  // 週期的最小公倍數
int MaxPH = 0;  // 最大 Phase time
int Clock = 0;  // current clock
struct Task task[1000];
struct Job job[1000];
bool schedulable_EDF = true;
bool schedulable_RM = true;
list <int> Q; // 系統的工作序列 Q
list <int> EDF_Q; // EDF 工作序列
list <int> RM_Q; // RM 工作序列

int gcd(int m, int n)
{
    while(n != 0)
    {
        int r = m % n;
        m = n;
        n = r;
    }
    return m;
}

int lcm(int m, int n)
{
    return m * n / gcd(m, n);
}

void showlist(list <int> g)
{
    cout << "List is:";
    list <int> :: iterator it;
    for(it = g.begin(); it != g.end(); ++it)
        cout << *it << " ";
    cout << endl;
}

int main(int argc, const char * argv[])
{
    fstream file;
    string filename = "/Users/toby/Library/Mobile Documents/com~apple~CloudDocs/Collage Classes/Real time system/EDF and RM Simulator/EDF and RM Simulator/test1.txt";
    file.open(filename.c_str());
    if(!file)
    {
        cout << "檔案無法開啟" << endl;
        return 0;
    }
    
    string line; // 暫存讀入的資料
    
    // MARK: 存入資料到 Task struct
    for(Total_Task_Number = 0; getline(file, line); Total_Task_Number++)
    {
        string substr;
        stringstream line_ss(line);
        vector<int> result;
        for (int j = 0; line_ss >> j;)
        {
            result.push_back(j);
            if (line_ss.peek() == ',')
                line_ss.ignore();
        }
        int number = 0;
        task[Total_Task_Number].TID = Total_Task_Number;
        task[Total_Task_Number].Phase = result[number++];
        if (task[Total_Task_Number].Phase > MaxPH)
            MaxPH = task[Total_Task_Number].Phase;
        task[Total_Task_Number].Period = result[number++];
        task[Total_Task_Number].RDeadline = result[number++];
        task[Total_Task_Number].WCET = result[number++];
        task[Total_Task_Number].Utilization = 0;
    }
    
    cout << "TID, " << "phase time, " << "period, " << "relative deadline, " << "execution time, " << "utilization" << endl;
    
    for (size_t i = 0; i < 3; i++)
    {
        cout << task[i].TID << ", ";
        cout << task[i].Phase << ", ";
        cout << task[i].Period << ", ";
        cout << task[i].RDeadline << ", ";
        cout << task[i].WCET << ", ";
        cout << task[i].Utilization << endl;
    }
    
    // MARK: 計算 period 的 LCM
    LCM = task[0].Period;
    for(int i = 1; i < Total_Task_Number; i++)
    {
        LCM = lcm(LCM, task[i].Period);
    }
    cout << "lcm number: " << LCM << endl;
    
    // MARK: Schedulability test
    for (int i = 0; i < Total_Task_Number; i++)
    {
        if(task[i].WCET / min(task[i].Period, task[i].RDeadline) <= 1)
            schedulable_EDF = false;
        if(task[i].WCET / min(task[i].Period, task[i].RDeadline) <= Total_Task_Number * (pow(2, 1 / Total_Task_Number) - 1))
            schedulable_RM = false;
    }
    
    Q.clear();
    
    // MARK: 開始 clock 計算
    while(Clock < (LCM + MaxPH))
    {
        if(!Q.empty()) // 有工作則檢查所有工作是否可排 (Step 6)
        {
            list <int> :: iterator it;
            for(it = Q.begin(); it != Q.end(); ++it)
            {
                if(job[*it].absolute_deadline - Clock - job[*it].remain_execution_time <= 0)
                {
                    cout << "Job number" << *it << "can't be finished" << endl;
                    Miss_Deadline_Job_Number++;
                    Q.remove(*it);
                }
            }
        }
        
        for(int i = 0; i < Total_Task_Number ; i++) // Step 7
        {
            if(Clock - task[i].Phase == 0)
            {
                Q.push_back(Total_Job_Number);  // 在 Queue 中存入對應到該 job 的編號
                job[Total_Job_Number].release_time = Clock;
                job[Total_Job_Number].remain_execution_time = task[i].WCET;
                job[Total_Job_Number].absolute_deadline = Clock - task[i].WCET;
                job[Total_Job_Number].TID = task[i].TID;
                job[Total_Job_Number++].Period = task[i].Period;
            }
        }
        
        // Step 8
        list <int> :: iterator it;
        int shortest_period = 0;
        int RM_PID = 0;
        for(it = Q.begin(); it != Q.end(); ++it) //RM
        {
            if(job[*it].Period > shortest_period)
            {
                shortest_period = job[*it].Period;
                RM_PID = *it;
            }
        }
        if(RM_PID != 0)
        {
            if(job[RM_PID].remain_execution_time > 0)
                job[RM_PID].remain_execution_time--;
            else
                Q.remove(RM_PID);
        }
        shortest_period = 0;
        RM_PID = 0;
        
        Clock++;
    }
    
    // MARK: 程式結束
    file.close();
    return 0;
}
