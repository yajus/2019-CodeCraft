//
//  main.cpp
//  CodeCraft-2019
//
//  Created by apple on 2019/3/21.
//  Copyright © 2019年 apple. All rights reserved.
//

#include <iostream>
#include <fstream>
using namespace std;
const int maxnum = 200;
const int maxint = 999999;
static int carline=0;
static int roadline=0;
static int crossline=0;
//static int c[12000][100][100];   // 记录图的两点间路径长度
static int cardata[100000][8]={0};
static int roaddata[100000][8]={0};
static int crossdata[100000][8]={0};
static int CarRoadID[100000][1002]={0};
static int dist[12000][100]={0};     // 表示当前点到源点的最短路径长度
static int prev1[12000][100]={0};     // 记录当前点的前一个结点
static int frequency[1000][12000]={0};
static int outline=0;
//所有数据从第1行第1列开始存放
//cross
//(id,roadId,roadId,roadId,roadId)
//car
//(id,from,to,speed,planTime)
//road
//(id,length,speed,channel,from,to,isDuplex)
//v为起点 n为节点数 dist到不同点去距离 prev不同点的前节点 c表示图
void Dijkstra(int n,int v,int outline,int c[100][100])
{
    int PointToRoad(int start,int end);
    bool s[maxnum];    // 判断是否已存入该点到S集合中
    for(int i=1; i<=n; ++i)
    {
        dist[outline][i] = c[v][i];
        s[i] = 0;     // 初始都未用过该点
        if(dist[outline][i] == maxint)
            prev1[outline][i] = 0;
        else
            prev1[outline][i] = v;
    }
    dist[outline][v] = 0;
    s[v] = 1;
    
    // 依次将未放入S集合的结点中，取dist[]最小值的结点，放入结合S中
    // 一旦S包含了所有V中顶点，dist就记录了从源点到所有其他顶点之间的最短路径长度
    for(int i=2; i<=n; ++i)
    {
        int tmp = maxint;
        int u = v;
        // 找出当前未使用的点j的dist[j]最小值
        for(int j=1; j<=n; ++j)
            if((!s[j]) && dist[outline][j]<tmp)
            {
                u = j;              // u保存当前邻接点中距离最小的点的号码
                tmp = dist[outline][j];
            }
        s[u] = 1;    // 表示u点已存入S集合中
        
        //铺路
        int prepoint=prev1[outline][u];
        int chosenroad=PointToRoad(prepoint, u);
        frequency[int((cardata[outline][5]+3)/4)][chosenroad-5000+1]+=1;
        
        // 更新dist
        for(int j=1; j<=n; ++j)
            if((!s[j]) && c[u][j]<maxint)
            {
                int newdist;
                //修改边权限制条件
                if(int((cardata[outline][5]+3)/4)>2)
                {
                    int x=int((cardata[outline][5]+3)/4);
                    int y=PointToRoad(u, j);
                    int pretwo=(frequency[x-1][y-5000+1]+frequency[x-2][y-5000+1]);
//                    int bandwidth=(roaddata[y-5000+1][2]*roaddata[y-5000+1][4]);
                    int bandwidth=roaddata[y-5000+1][4];
                    int speedlimit=min(roaddata[y-5000+1][3],cardata[outline][4]);
                    newdist=dist[outline][u]+(pretwo/(bandwidth*speedlimit));
                }
                else
                newdist = dist[outline][u] + c[u][j];
//
                if(newdist < dist[outline][j])
                {
                    dist[outline][j] = newdist;
                    prev1[outline][j] = u;
                }
            }
    }
}

//起点 终点
int PointToRoad(int start,int end)
{
    int result=-1;
    int i;
    int yes=0;
    for(i=1;i<=roadline;i++)
    {
        if(start==roaddata[i][5]&&end==roaddata[i][6])
        {
            yes=1;
            break;
        }
        if(start==roaddata[i][6]&&end==roaddata[i][5]&&roaddata[i][7]==1)
        {
            yes=1;
            break;
        }
    }
    if(yes==1)
    {
    result=roaddata[i][1];
    }
    return result;
}

//v 起点 u 终点 carnum第几辆车
void searchPath(int v, int u,int carnum)
{
    int que[maxnum];
    int tot = 1;
    que[tot] = u;
    tot++;
    int tmp = prev1[outline][u];
    while(tmp != v)
    {
        que[tot] = tmp;
        tot++;
//        cout<<tot<<endl;
        tmp = prev1[outline][tmp];
    }
    que[tot] = v;
    int pa=1;
    for(int i=tot; i>1; --i)
    {
        int result=PointToRoad(que[i], que[i-1]);
        CarRoadID[carnum][pa]=result;
        pa++;
    }
//        if(i != 1)
//            cout << que[i] << " -> ";
//        else
//            cout << que[i] << endl;
}

void carin(string nn)
{
    string str;
    ifstream carfile;
    carfile.open(nn);
    getline(carfile,str);
    if(carfile.is_open())
    {
        carline=1;//数据从第1行开始存放
        while(!carfile.eof())
        {
        getline(carfile,str);
//        cout<<str<<endl;
//        cout<<str<<endl;
        int i=0;
        int sum=0;
            
        //提取数字start
        int point=1;//数据从第1列开始存放
        while (i < str.length())
        {
            if ('0' <= str.at(i) && str.at(i) <= '9')
                sum = sum * 10 + (str.at(i) - '0');
            else if(sum>0)
            {
                cardata[carline][point]=sum;
                point++;
                sum=0;
            }
            i++;
        }
        ++carline;
        //提取数字end
            
        }
//        cout<<"line"<<line;
//        for(int i=1;i<=line-1;i++)
//        {for(int j=1;j<=5;j++)
//            {
//                cout<<cardata[i][j]<<" ";
//            }
//            cout<<endl;
//        }
    }
    else
    {
        cout<<"wrong";
    }
    carfile.close();
    carline--;
}


void roadin(string nn)
{
    string str;
    ifstream roadfile;
    roadfile.open(nn);
    getline(roadfile,str);
    if(roadfile.is_open())
    {
        roadline=1;//数据从第1行开始存放
        while(!roadfile.eof())
        {
            getline(roadfile,str);
//            cout<<str<<endl;
            int i=0;
            int sum=0;
            
            //提取数字start
            int point=1;//数据从第1列开始存放
            while (i < str.length())
            {
                if ('0' <= str.at(i) && str.at(i) <= '9')
                    sum = sum * 10 + (str.at(i) - '0');
                else if(sum>0)
                {
                    roaddata[roadline][point]=sum;
                    point++;
                    sum=0;
                }
                i++;
            }
            ++roadline;
            //提取数字end
            
        }
//                cout<<"line"<<roadline;
//                for(int i=1;i<=roadline-1;i++)
//                {for(int j=1;j<=7;j++)
//                    {
//                        cout<<roaddata[i][j]<<" ";
//                    }
//                    cout<<endl;
//                }
    }
    else
    {
        cout<<"wrong";
    }
    roadfile.close();
    roadline--;
}


void crossin(string nn)
{
    string str;
    ifstream crossfile;
    crossfile.open(nn);
    getline(crossfile,str);
    if(crossfile.is_open())
    {
        crossline=1;//数据从第1行开始存放
        while(!crossfile.eof())
        {
            getline(crossfile,str);
//            cout<<str<<endl;
            int i=0;
            int sum=0;
            
            //提取数字start
            int point=1;//数据从第1列开始存放
            while (i < str.length())
            {
                if ('0' <= str.at(i) && str.at(i) <= '9')
                    sum = sum * 10 + (str.at(i) - '0');
                else if(sum>0)
                {
                    crossdata[crossline][point]=sum;
                    point++;
                    sum=0;
                }
                i++;
            }
            ++crossline;
            //提取数字end
            
        }
//                cout<<"line"<<crossline;
//                for(int i=1;i<=crossline-1;i++)
//                {for(int j=1;j<=5;j++)
//                    {
//                        cout<<crossdata[i][j]<<" ";
//                    }
//                    cout<<endl;
//                }
    }
    else
    {
        cout<<"wrong";
    }
    crossfile.close();
    crossline--;
}

void carsort()
{
    for (int i=1;i<=carline;i++)
        for(int j=i+1;j<=carline;j++)
        {
            if(cardata[i][4]<cardata[j][4])
                swap(cardata[i],cardata[j]);
        }
}

void carsort1()
{
    for (int i=1;i<=carline;i++)
        for(int j=i+1;j<=carline;j++)
        {
            if(cardata[i][5]>cardata[j][5])
                swap(cardata[i],cardata[j]);
        }
}

void ChangeStartTime()
{
    int i=1;
    int j=1;
    int point=1;
//    while(point<=carline)//版本1
//    {
//        if(i>70)//调参啊调参
//        {
//            i=1;
//            j+=4;
//        }
//        cardata[point][5]=max(j,cardata[point][5]);
//        point++;
//        i++;
//    }
    while(point<=carline)
    {
        if(j>500)//调参啊调参
        {
            j=1;
        }
        cardata[point][5]=max(j,cardata[point][5]);
        j++;
        point++;
    }
    carsort1();
        
}

int main(int a,char* argv[])
{//car road cross answer
    
    int c[100][100];
    string carargv=argv[1];
    string roadargv=argv[2];
    string crossargv=argv[3];
    crossin(crossargv);
    roadin(roadargv);
    carin(carargv);
    carsort();
    ChangeStartTime();
    // 各数组都从下标1开始

//    cout<<crossline<<endl;
    // 初始化c[][]为maxintt//图的初始化
    for(outline=1;outline<=carline;outline++)
    {
    for(int i=1; i<=crossline; ++i)
        for(int j=1; j<=crossline; ++j)
            c[i][j] = maxint;
    
    //根据道路建图
    for(int i=1; i<=roadline; ++i)
    {
        if(roaddata[i][7] ==1)       // 双向
        {
            c[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
             c[roaddata[i][6]][roaddata[i][5]] = roaddata[i][2];
        }
        else// 单向
        {
            c[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
        }
    }

    for(int i=1; i<=crossline; ++i)
        dist[outline][i] = maxint;
//    for(int i=1; i<=crossline; ++i)
//    {
//        for(int j=1; j<=crossline; ++j)
//            printf("%8d", c[i][j]);
//        printf("\n");
//    }

        int start=cardata[outline][2];
        int end=cardata[outline][3];
        Dijkstra(crossline, start,outline,c);
        searchPath(start,end,outline);
    }
    ofstream outfile;
    string answer=argv[4];
    outfile.open(answer);
    for(int i=1;i<=carline;i++)
    {
        outfile<<"("<<cardata[i][1]<<","<<cardata[i][5]<<",";
        for(int j=1;;j++)
        {
            if(CarRoadID[i][j]==0)
                break;
            else
            {
                outfile<<CarRoadID[i][j];
            }
            if(CarRoadID[i][j+1]!=0)
                outfile<<",";
        }
        outfile<<")"<<endl;
    }
    // 最短路径长度
//    cout << "源点到最后一个顶点的最短路径长度: " << dist[crossline] << endl;//距离升序
//    // 路径
//    cout << "源点到最后一个顶点的路径为: ";
//    searchPath(prev, 2, crossline);
}
