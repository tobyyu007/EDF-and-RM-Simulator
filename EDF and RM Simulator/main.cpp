//
//  main.cpp
//  EDF and RM Simulator
//
//  Created by Toby on 2019/11/10.
//  Copyright © 2019 Toby Yu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <math.h>
#include <limits.h>
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
    cout << "List is: ";
    list <int> :: iterator it;
    for(it = g.begin(); it != g.end(); ++it)
        cout << *it << " ";
    cout << endl;
}

void EDF();
void RM();



int main()
{
    cout << "RM 排程" << endl;
    RM();
    puts("");
    cout << "EDF 排程" << endl;
    EDF();
}


// MARK: RM
void RM()
{
    int Total_Task_Number = 0;  // 總共的 task 個數
    int Total_Job_Number = 0; // 總共的 job 個數
    int Miss_Deadline_Job_Number=0;  // 錯過 deadline 的工作總數
    int LCM = 0;  // 週期的最小公倍數
    int MaxPH = 0;  // 最大 Phase time
    int Clock = 0;  // current clock
    struct Task task[1000];  // 儲存 struct task 資訊
    struct Job job[1000];  // 儲存 struct job 資訊
    list <int> waitingQ; // RM 的待執行工作序列
    vector <int> RM_Q; // RM 排班執行順序
    
    fstream file;
    string filename = "/Users/toby/Library/Mobile Documents/com~apple~CloudDocs/Collage Classes/Real time system/EDF and RM Simulator/EDF and RM Simulator/test1.txt";
    file.open(filename.c_str());
    if(!file)
    {
        cout << "檔案無法開啟" << endl;
    }
    
    string line; // 暫存讀入的資料
    
    // MARK: RM - Step 1
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
    
    for (size_t i = 0; i < Total_Task_Number; i++)
    {
        cout << task[i].TID << ", ";
        cout << task[i].Phase << ", ";
        cout << task[i].Period << ", ";
        cout << task[i].RDeadline << ", ";
        cout << task[i].WCET << ", ";
        cout << task[i].Utilization << endl;
    }
    
    // MARK: RM - Step 2
    LCM = task[0].Period;
    for(int i = 1; i < Total_Task_Number; i++)
    {
        LCM = lcm(LCM, task[i].Period);
    }
    cout << "lcm number: " << LCM << endl;
    
    // MARK: RM - Schedulability test
    float schedulability_test = 0;
    for (int i = 0; i < Total_Task_Number; i++)
    {
        float temp = task[i].WCET;
        schedulability_test += temp / min(task[i].Period, task[i].RDeadline);;
    }
    
    
    if(schedulability_test > 1)
        cout << "使用 EDF 可能不能排程" << endl;
    if(schedulability_test > Total_Task_Number * ((pow(2, 1 / Total_Task_Number) - 1)))
        cout << "使用 RM 可能不能排程" << endl;
    
    // MARK: RM - Step 3
    waitingQ.clear();
    
    // MARK: RM - Step 4
    Clock = 0;
    
    // MARK: RM - Step 5
    while(Clock < (LCM + MaxPH))
    {
        // MARK: RM - Step 6
        if(!waitingQ.empty()) // 有工作則檢查所有工作是否可排
        {
            vector<int> jobToRemove; // 儲存不能在 deadline 前完成 job 的號碼
            for(list <int> :: iterator it = waitingQ.begin(); it != waitingQ.end(); ++it) // 找出哪些 job 無法在 deadline 完成
            {
                if(job[*it].absolute_deadline - Clock - job[*it].remain_execution_time < 0)
                {
                    cout << "Job T" << *it << " 不能在截限時間內完成" << endl;
                    Miss_Deadline_Job_Number++;
                    jobToRemove.push_back(*it);
                }
            }
            for(int i = 0; i < jobToRemove.size(); i++) // 移除 job
            {
                waitingQ.remove(jobToRemove[i]);
            }
            jobToRemove.clear();
        }
        
        // MARK: RM - Step 7
        for(int i = 0; i < Total_Task_Number ; i++)
        {
            if((Clock - task[i].Phase) % task[i].Period == 0)
            {
                waitingQ.push_back(Total_Job_Number);  // 在 Queue 中存入對應到該 job 的編號
                job[Total_Job_Number].release_time = Clock;
                job[Total_Job_Number].remain_execution_time = task[i].WCET;
                job[Total_Job_Number].absolute_deadline = task[i].RDeadline + Clock;
                job[Total_Job_Number].TID = task[i].TID;
                job[Total_Job_Number++].Period = task[i].Period;
            }
        }
        
        //showlist(Q);
        
        // MARK: RM - Step 8
        
        // RM
        int shortest_period = INT_MAX;
        int RM_PID = -1;
        for(list <int> :: iterator it = waitingQ.begin(); it != waitingQ.end(); ++it)
        {
            if(job[*it].Period < shortest_period)
            {
                shortest_period = job[*it].Period;
                RM_PID = *it;
            }
            else if (job[*it].Period == shortest_period)  // 當擁有相同 period 時，TID 小的先執行
            {
                if(job[*it].TID < RM_PID)
                    RM_PID = *it;
            }
        }
        
        if(RM_PID != -1) // 在該時間點，有工作會執行
        {
            if(--job[RM_PID].remain_execution_time <= 0)
                waitingQ.remove(RM_PID);
            RM_Q.push_back(job[RM_PID].TID);
            cout << Clock << " T" << job[RM_PID].TID << endl;
        }
        else
        {
            RM_Q.push_back(-1);
            cout << Clock << endl;
        }
        shortest_period = INT_MAX;
        RM_PID = -1;
         
        Clock++;
    }
    
    // MARK: RM - 程式結束
    file.close();
}


// MARK: EDF
void EDF()
{
    int Total_Task_Number = 0;  // 總共的 task 個數
    int Total_Job_Number = 0; // 總共的 job 個數
    int Miss_Deadline_Job_Number=0;  // 錯過 deadline 的工作總數
    int LCM = 0;  // 週期的最小公倍數
    int MaxPH = 0;  // 最大 Phase time
    int Clock = 0;  // current clock
    struct Task task[1000];  // 儲存 struct task 資訊
    struct Job job[1000];  // 儲存 struct job 資訊
    list <int> waitingQ; // RM 的待執行工作序列
    list <int> waitingQ_EDF; // EDF 的待執行工作序列
    vector <int> EDF_Q; // EDF 排班執行順序
    
    fstream file;
    string filename = "/Users/toby/Library/Mobile Documents/com~apple~CloudDocs/Collage Classes/Real time system/EDF and RM Simulator/EDF and RM Simulator/test1.txt";
    file.open(filename.c_str());
    if(!file)
    {
        cout << "檔案無法開啟" << endl;
    }
    
    string line; // 暫存讀入的資料
    
    // MARK: EDF - Step 1
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
    
    for (size_t i = 0; i < Total_Task_Number; i++)
    {
        cout << task[i].TID << ", ";
        cout << task[i].Phase << ", ";
        cout << task[i].Period << ", ";
        cout << task[i].RDeadline << ", ";
        cout << task[i].WCET << ", ";
        cout << task[i].Utilization << endl;
    }
    
    // MARK: EDF - Step 2
    LCM = task[0].Period;
    for(int i = 1; i < Total_Task_Number; i++)
    {
        LCM = lcm(LCM, task[i].Period);
    }
    cout << "lcm number: " << LCM << endl;
    
    // MARK: EDF - Schedulability test
    float schedulability_test = 0;
    for (int i = 0; i < Total_Task_Number; i++)
    {
        float temp = task[i].WCET;
        schedulability_test += temp / min(task[i].Period, task[i].RDeadline);;
    }
    
    
    if(schedulability_test > 1)
        cout << "使用 EDF 可能不能排程" << endl;
    if(schedulability_test > Total_Task_Number * ((pow(2, 1 / Total_Task_Number) - 1)))
        cout << "使用 RM 可能不能排程" << endl;
    
    // MARK: EDF - Step 3
    waitingQ.clear();
    
    // MARK: EDF - Step 4
    Clock = 0;
    
    // MARK: EDF - Step 5
    while(Clock < (LCM + MaxPH))
    {
        // MARK: EDF - Step 6
        if(!waitingQ.empty()) // 有工作則檢查所有工作是否可排
        {
            vector<int> jobToRemove; // 儲存不能在 deadline 前完成 job 的號碼
            for(list <int> :: iterator it = waitingQ.begin(); it != waitingQ.end(); ++it) // 找出哪些 job 無法在 deadline 完成
            {
                if(job[*it].absolute_deadline - Clock - job[*it].remain_execution_time < 0)
                {
                    cout << "Job T" << *it << " 不能在截限時間內完成" << endl;
                    Miss_Deadline_Job_Number++;
                    jobToRemove.push_back(*it);
                }
            }
            for(int i = 0; i < jobToRemove.size(); i++) // 移除 job
            {
                waitingQ.remove(jobToRemove[i]);
            }
            jobToRemove.clear();
        }
        
        // MARK: EDF - Step 7
        for(int i = 0; i < Total_Task_Number ; i++)
        {
            if((Clock - task[i].Phase) % task[i].Period == 0)
            {
                waitingQ.push_back(Total_Job_Number);  // 在 Queue 中存入對應到該 job 的編號
                job[Total_Job_Number].release_time = Clock;
                job[Total_Job_Number].remain_execution_time = task[i].WCET;
                job[Total_Job_Number].absolute_deadline = task[i].RDeadline + Clock;
                job[Total_Job_Number].TID = task[i].TID;
                job[Total_Job_Number++].Period = task[i].Period;
            }
        }
        
        //showlist(waitingQ);
        
        // MARK: EDF - Step 8
         
        // EDF
        int earliest_deadline = INT_MAX;
        int EDF_PID = -1;
        for(list <int> :: iterator it = waitingQ.begin(); it != waitingQ.end(); ++it)
        {
            if(job[*it].absolute_deadline - Clock < earliest_deadline)
            {
                earliest_deadline = job[*it].absolute_deadline - Clock;
                EDF_PID = *it;
            }
            else if (job[*it].absolute_deadline - Clock == earliest_deadline)  // 當擁有相同 deadline 時，TID 小的先執行
            {
                if(job[*it].TID < EDF_PID)
                    EDF_PID = *it;
            }
        }
        
        
        if(EDF_PID != -1) // 在該時間點，有工作會執行
        {
            if(--job[EDF_PID].remain_execution_time <= 0)
                waitingQ.remove(EDF_PID);
            EDF_Q.push_back(job[EDF_PID].TID);
            cout << Clock << " T" << job[EDF_PID].TID << endl;
        }
        else
        {
            EDF_Q.push_back(-1); // 表示該時間點沒有工作執行
            cout << Clock << endl;
        }
        earliest_deadline = INT_MAX;
        EDF_PID = -1;
        
         
        Clock++;
    }
    
    // MARK: EDF - 程式結束
    file.close();
}
