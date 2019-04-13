//
//  main.cpp
//  CodeCraft-2019
//
//  Created by apple on 2019/3/21.
//  Copyright © 2019年 apple. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <set>
#include <vector>
#define eps 1e-8
using namespace std;
const int NMax = 9999;
const int maxnum = 200;
const int maxint = 999999;
static int carline=0;
static int roadline=0;
static int crossline=0;
//static int c[12000][100][100];   // 记录图的两点间路径长度
static int cardata[500000][8]={0};
static int roaddata[500000][8]={0};
static int crossdata[500000][8]={0};
static int CarRoadID[500000][1002]={0};
static int dist[50000][100]={0};     // 表示当前点到源点的最短路径长度
static int prev1[50000][100]={0};     // 记录当前点的前一个结点
static int frequency[1000][12000]={0};
static int road_car[1000] = {0};//每条路的车辆数
static int outline=0;
static double Phe[NMax][NMax];//边对应的信息素浓度
int graph[500][100];  //图

int sameNum,samePhe[NMax];//每次去寻找信息素最多的边，如初始情况，信息素量都相同时，要
//随机的去从中选取边。
int bugNum,bugTry[NMax];//出错情况下进行的选择
const double bugP = 0.90;//每一次操作的出错概率
double Max;//用来选取最多信息素的边
bool Passed[NMax];//用来判断城市是否已经经过，是否可以选取
const double Dis = 0.1;//每次信息素 消失的速率
double **pheromone; //信息素
double  **herustic; //启发式的值
double **info;// info = pheromone ^ delta * herustic ^ beta 根据蚁群算法的论文来的公式，不多逼逼
double pheromone_0;//pheromone初始值，这里是1 / (avg * N)其中avg为图网中所有边边权的平均数,根据论文来的
double  alpha = 0.1;//evaporation parameter，挥发参             数，每次信息素要挥发的量
const int delta = 1;
const int beta = 6;// delta 和 beta分别表示pheromones和herustic的比重
//所有数据从第1行第1列开始存放
//cross
//(id,roadId,roadId,roadId,roadId)
//car
//(id,from,to,speed,planTime)
//road
//(id,length,speed,channel,from,to,isDuplex)
//v为起点 n为节点数 dist到不同点去距离 prev不同点的前节点 c表示图,outline当前点

template <typename T>
T sort_4_num(T a,T b,T c,T d) //四个数排序
{
    vector<T> list;
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.push_back(d);
    sort(list.begin(),list.end());
    return list[0];
}


void Dijkstra(int n,int v,int outline,int c[100][100])
{
    int PointToRoad(int start,int end);
    bool s[maxnum];    // 判断是否已存入该点到S集合中
    for(int i=1; i<=n; ++i)
    {
        dist[outline][i] = c[v][i];
        //起点权值刷新start
        if(int((cardata[outline][5]+3)/4)>2)
        {
            int road1,road2,road3,road4;
            int point1=0,point2 = 0,point3= 0,point4= 0;
            road1=crossdata[v][2];
            road2=crossdata[v][3];
            road3=crossdata[v][4];
            road4=crossdata[v][5];
            if(road1!=1)
            {
                int truejudge=0;
                if( roaddata[road1-5000+1][5]!=v&&roaddata[road1-5000+1][7]==1)
                {
                    truejudge=1;
                    point1=roaddata[road1-5000+1][5];
                }
                else if(roaddata[road1-5000+1][6]!=v)
                {
                    truejudge=1;
                    point1=roaddata[road1-5000+1][6];
                }
                int one1=0;
                int x=int((cardata[outline][5]+3)/4);
                one1+=frequency[x-1][road1-5000+1];
                one1+=frequency[x-2][road1-5000+1];

                int bandwidth=roaddata[road1-5000+1][4];
                int speedlimit=min(roaddata[road1-5000+1][3],cardata[outline][4]);
                one1/=(bandwidth*speedlimit);
                if(truejudge==1)
                    dist[outline][point1]=one1;

                //补充start
                if(point1!=0)
                {
                    int one=0,two=0,three=0,four=0;
                    int deliver=0;
                    int j=point1;
                    int y=road1;
                    if(crossdata[j][5]!=1&&crossdata[j][5]!=y)
                    {
//                        cout<<crossdata[j][5]<<endl;
                        deliver++;
                        one+=frequency[x-1][crossdata[j][5]-5000+1];
                        one+=frequency[x-2][crossdata[j][5]-5000+1];

                        int bandwidth=roaddata[crossdata[j][5]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][5]-5000+1][3],cardata[outline][4]);
                        one/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][2]!=1&&crossdata[j][2]!=y)
                    {
                        deliver++;
                        two+=frequency[x-1][crossdata[j][2]-5000+1];
                        two+=frequency[x-2][crossdata[j][2]-5000+1];
                        int bandwidth=roaddata[crossdata[j][2]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][2]-5000+1][3],cardata[outline][4]);
                        two=two/(bandwidth*speedlimit);
                    }
                    if(crossdata[j][3]!=1&&crossdata[j][3]!=y)
                    {
                        deliver++;
                        three+=frequency[x-1][crossdata[j][3]-5000+1];
                        three+=frequency[x-2][crossdata[j][3]-5000+1];
                        int bandwidth=roaddata[crossdata[j][3]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][3]-5000+1][3],cardata[outline][4]);
                        three/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][4]!=1&&crossdata[j][4]!=y)
                    {
                        deliver++;
                        four+=frequency[x-1][crossdata[j][4]-5000+1];
                        four+=frequency[x-2][crossdata[j][4]-5000+1];
                        int bandwidth=roaddata[crossdata[j][4]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][4]-5000+1][3],cardata[outline][4]);
                        four/=(bandwidth*speedlimit);
                    }
                    //                    four/=deliver;
                    int average=0;
                    average+=(one+two+three+four)/deliver;
                    dist[outline][point1]+=average+road_car[road1-5000+1];//加上当前车道的车的数目
                }
                //补充end
            }
            point2=0;
            if(road2!=1)
            {
                int truejudge=0;
                if( roaddata[road2-5000+1][5]!=v&&roaddata[road2-5000+1][7]==1)
                {
                    truejudge=1;
                    point2=roaddata[road2-5000+1][5];
                }
                else if(roaddata[road2-5000+1][6]!=v)
                {
                    truejudge=1;
                    point2=roaddata[road2-5000+1][6];
                }
                int two1=0;
                int x=int((cardata[outline][5]+3)/4);
                two1+=frequency[x-1][road2-5000+1];
                two1+=frequency[x-2][road2-5000+1];
                int bandwidth=roaddata[road2-5000+1][4];
                int speedlimit=min(roaddata[road2-5000+1][3],cardata[outline][4]);
                two1/=(bandwidth*speedlimit);
                if( truejudge==1)
                    dist[outline][point2]=two1;

                //补充start
                if(point2!=0)
                {
                    int one=0,two=0,three=0,four=0;
                    int deliver=0;
                    int j=point2;
                    int y=road2;
                    if(crossdata[j][5]!=1&&crossdata[j][5]!=y)
                    {
                        deliver++;
                        one+=frequency[x-1][crossdata[j][5]-5000+1];
                        one+=frequency[x-2][crossdata[j][5]-5000+1];
                        int bandwidth=roaddata[crossdata[j][5]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][5]-5000+1][3],cardata[outline][4]);
                        one/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][2]!=1&&crossdata[j][2]!=y)
                    {
                        deliver++;
                        two+=frequency[x-1][crossdata[j][2]-5000+1];
                        two+=frequency[x-2][crossdata[j][2]-5000+1];
                        int bandwidth=roaddata[crossdata[j][2]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][2]-5000+1][3],cardata[outline][4]);
                        two=two/(bandwidth*speedlimit);
                    }
                    if(crossdata[j][3]!=1&&crossdata[j][3]!=y)
                    {
                        deliver++;
                        three+=frequency[x-1][crossdata[j][3]-5000+1];
                        three+=frequency[x-2][crossdata[j][3]-5000+1];
                        int bandwidth=roaddata[crossdata[j][3]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][3]-5000+1][3],cardata[outline][4]);
                        three/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][4]!=1&&crossdata[j][4]!=y)
                    {
                        deliver++;
                        four+=frequency[x-1][crossdata[j][4]-5000+1];
                        four+=frequency[x-2][crossdata[j][4]-5000+1];
                        int bandwidth=roaddata[crossdata[j][4]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][4]-5000+1][3],cardata[outline][4]);
                        four/=(bandwidth*speedlimit);
                    }
                    //                    four/=deliver;
                    int average=0;
                    average+=(one+two+three+four)/deliver;
                    dist[outline][point2]+=average+road_car[road2-5000+1];
                }
                //补充end

            }
            point3=0;
            if(road3!=1)
            {
                int truejudge=0;
                if( roaddata[road3-5000+1][5]!=v&&roaddata[road3-5000+1][7]==1)
                {
                    truejudge=1;
                    point3=roaddata[road3-5000+1][5];
                }
                else if(roaddata[road3-5000+1][6]!=v)
                {
                    truejudge=1;
                    point3=roaddata[road3-5000+1][6];
                }
                int three1=0;
                int x=int((cardata[outline][5]+3)/4);
                three1+=frequency[x-1][road3-5000+1];
                three1+=frequency[x-2][road3-5000+1];
                int bandwidth=roaddata[road3-5000+1][4];
                int speedlimit=min(roaddata[road3-5000+1][3],cardata[outline][4]);
                three1/=(bandwidth*speedlimit);
                if( truejudge==1)
                    dist[outline][point3]=three1;

                //补充start
                if(point3!=0)
                {
                    int one=0,two=0,three=0,four=0;
                    int deliver=0;
                    int j=point3;
                    int y=road3;
                    if(crossdata[j][5]!=1&&crossdata[j][5]!=y)
                    {
                        deliver++;
                        one+=frequency[x-1][crossdata[j][5]-5000+1];
                        one+=frequency[x-2][crossdata[j][5]-5000+1];
                        int bandwidth=roaddata[crossdata[j][5]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][5]-5000+1][3],cardata[outline][4]);
                        one/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][2]!=1&&crossdata[j][2]!=y)
                    {
                        deliver++;
                        two+=frequency[x-1][crossdata[j][2]-5000+1];
                        two+=frequency[x-2][crossdata[j][2]-5000+1];
                        int bandwidth=roaddata[crossdata[j][2]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][2]-5000+1][3],cardata[outline][4]);
                        two=two/(bandwidth*speedlimit);
                    }
                    if(crossdata[j][3]!=1&&crossdata[j][3]!=y)
                    {
                        deliver++;
                        three+=frequency[x-1][crossdata[j][3]-5000+1];
                        three+=frequency[x-2][crossdata[j][3]-5000+1];
                        int bandwidth=roaddata[crossdata[j][3]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][3]-5000+1][3],cardata[outline][4]);
                        three/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][4]!=1&&crossdata[j][4]!=y)
                    {
                        deliver++;
                        four+=frequency[x-1][crossdata[j][4]-5000+1];
                        four+=frequency[x-2][crossdata[j][4]-5000+1];
                        int bandwidth=roaddata[crossdata[j][4]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][4]-5000+1][3],cardata[outline][4]);
                        four/=(bandwidth*speedlimit);
                    }
                    //                    four/=deliver;
                    int average=0;
                    average+=(one+two+three+four)/deliver;
                    dist[outline][point3]+=average+road_car[road3-5000+1];
                }
                //补充end

            }
            point4=0;
//            cout<<road4<<endl;
            if(road4!=1)
            {
                int truejudge=0;
                if( roaddata[road4-5000+1][5]!=v&&roaddata[road4-5000+1][7]==1)
                {
                    truejudge=1;
                    point4=roaddata[road4-5000+1][5];
                }
                else if(roaddata[road4-5000+1][6]!=v)
                {
                    truejudge=1;
                    point4=roaddata[road4-5000+1][6];
                }
                int four1=0;
                int x=int((cardata[outline][5]+3)/4);
                four1+=frequency[x-1][road4-5000+1];
                four1+=frequency[x-2][road4-5000+1];
                int bandwidth=roaddata[road4-5000+1][4];
                int speedlimit=min(roaddata[road4-5000+1][3],cardata[outline][4]);
                four1/=(bandwidth*speedlimit);
                if( truejudge==1)
                    dist[outline][point4]=four1;

                //补充start
                if(point4!=0)
                {
                    int one=0,two=0,three=0,four=0;
                    int deliver=0;
                    int j=point4;
                    int y=road4;
                    if(crossdata[j][5]!=1&&crossdata[j][5]!=y)
                    {
                        deliver++;
                        one+=frequency[x-1][crossdata[j][5]-5000+1];
                        one+=frequency[x-2][crossdata[j][5]-5000+1];
                        int bandwidth=roaddata[crossdata[j][5]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][5]-5000+1][3],cardata[outline][4]);
                        one/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][2]!=1&&crossdata[j][2]!=y)
                    {
                        deliver++;
                        two+=frequency[x-1][crossdata[j][2]-5000+1];
                        two+=frequency[x-2][crossdata[j][2]-5000+1];
                        int bandwidth=roaddata[crossdata[j][2]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][2]-5000+1][3],cardata[outline][4]);
                        two=two/(bandwidth*speedlimit);
                    }
                    if(crossdata[j][3]!=1&&crossdata[j][3]!=y)
                    {
                        deliver++;
                        three+=frequency[x-1][crossdata[j][3]-5000+1];
                        three+=frequency[x-2][crossdata[j][3]-5000+1];
                        int bandwidth=roaddata[crossdata[j][3]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][3]-5000+1][3],cardata[outline][4]);
                        three/=(bandwidth*speedlimit);
                    }
                    if(crossdata[j][4]!=1&&crossdata[j][4]!=y)
                    {
                        deliver++;
                        four+=frequency[x-1][crossdata[j][4]-5000+1];
                        four+=frequency[x-2][crossdata[j][4]-5000+1];
                        int bandwidth=roaddata[crossdata[j][4]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][4]-5000+1][3],cardata[outline][4]);
                        four/=(bandwidth*speedlimit);
                    }
                    //                    four/=deliver;
                    int average=0;
                    average+=(one+two+three+four)/deliver;
                    dist[outline][point4]+=average+road_car[road4-5000+1];
                }
                //补充end
            }
        }
//起点权值刷新end
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
//        road_car[chosenroad-5000+1] +=1; //新路车加1
//        int pre_prepoint = prev1[outline][prepoint];//前前一条路
//        int pre_road = PointToRoad(pre_prepoint,prepoint);
//        road_car[pre_road-5000+1] -=1;  //旧路车减1

        // 更新dist
        for(int j=1; j<=n; ++j)
            if((!s[j]) && c[u][j]<maxint)
            {
                int newdist;
                //修改边权限制条件
//                cout<<cardata[outline][5]<<endl;
                if(int((cardata[outline][5]+3)/4)>2)
                {
                    int x=int((cardata[outline][5]+3)/4);
//                    cout<<x<<endl;
                    int y=PointToRoad(u, j);
                    int pretwo=(frequency[x-1][y-5000+1]+frequency[x-2][y-5000+1]);
                    //new调整start
                    int one=0,two=0,three=0,four=0;
                    int deliver=0;
                    if(crossdata[j][5]!=1&&crossdata[j][5]!=y)
                    {
                        deliver++;
                        one+=frequency[x-1][crossdata[j][5]-5000+1];
                        one+=frequency[x-2][crossdata[j][5]-5000+1];
                        int car_num = road_car[crossdata[j][5]];
                        int bandwidth=roaddata[crossdata[j][5]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][5]-5000+1][3],cardata[outline][4]);
                        one/=(bandwidth*speedlimit+car_num/bandwidth);
                    }
                    if(crossdata[j][2]!=1&&crossdata[j][2]!=y)
                    {
                        deliver++;
                        two+=frequency[x-1][crossdata[j][2]-5000+1];
                        two+=frequency[x-2][crossdata[j][2]-5000+1];
                        int car_num = road_car[crossdata[j][2]];
                        int bandwidth=roaddata[crossdata[j][2]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][2]-5000+1][3],cardata[outline][4]);
                        two=two/(bandwidth*speedlimit+car_num/bandwidth);
                    }
                    if(crossdata[j][3]!=1&&crossdata[j][3]!=y)
                    {
                        deliver++;
                        three+=frequency[x-1][crossdata[j][3]-5000+1];
                        three+=frequency[x-2][crossdata[j][3]-5000+1];
                        int car_num = road_car[crossdata[j][3]];
                        int bandwidth=roaddata[crossdata[j][3]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][3]-5000+1][3],cardata[outline][4]);
                        three/=(bandwidth*speedlimit+car_num/bandwidth);
                    }
                    if(crossdata[j][4]!=1&&crossdata[j][4]!=y)
                    {
                        deliver++;
                        four+=frequency[x-1][crossdata[j][4]-5000+1];
                        four+=frequency[x-2][crossdata[j][4]-5000+1];
                        int car_num = road_car[crossdata[j][4]];
                        int bandwidth=roaddata[crossdata[j][4]-5000+1][4];
                        int speedlimit=min(roaddata[crossdata[j][4]-5000+1][3],cardata[outline][4]);
                        four/=(bandwidth*speedlimit+car_num/bandwidth);
                    }
//                    four/=deliver;
                    int average=0;
                    average+=(one+two+three+four)/deliver;
//                    cout<<average<<endl;
//                    pretwo+=four;
                    //new调整end
//                    int bandwidth=(roaddata[y-5000+1][2]*roaddata[y-5000+1][4]);
                    int bandwidth=roaddata[y-5000+1][4];
                    int speedlimit=min(roaddata[y-5000+1][3],cardata[outline][4]);
                    newdist=dist[outline][u]+(1000*pretwo/(bandwidth*speedlimit))+average;
//                    cout<<(pretwo/(bandwidth*speedlimit))<<endl;
                }
                else
                    newdist = dist[outline][u] + c[u][j];
//
                if(newdist < dist[outline][j])
                {
                    dist[outline][j] = newdist;
                    prev1[outline][j] = u;
                }
                int prepoint=prev1[outline][j];
                int chosenroad=PointToRoad(prepoint, j);
                road_car[chosenroad-5000+1] +=1; //新路车加1
                int pre_prepoint = prev1[outline][prepoint];//前前一条路
                int pre_road = PointToRoad(pre_prepoint,prepoint);
                road_car[pre_road-5000+1] -=1;  //旧路车减1
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

void carin(char* nn)
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


void roadin(char* nn)
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


void crossin(char* nn)
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
            if(cardata[i][4]<cardata[j][4]&&cardata[i][5]>cardata[j][5])
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
int get_total_road()
{
    int total_num = 0;
    for(int i=0;i<=roadline;i++)
        total_num+=roaddata[i][4]*(roaddata[i][7]+1);
    return  total_num;

}

void ChangeStartTime()
{
    int i=1;
    int j=1;
    int point=1;
    int total_num = get_total_road();
//    cout<<total_num<<endl;
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
        if(j>total_num)//调参啊调参
        {
            j=1;
        }

        cardata[point][5]=max(j,cardata[point][5]);
//        cout<<cardata[point][5]<<endl;
        j++;
        point++;
    }
    carsort1();

}

//蚁群算法



/*
 * param:
 * N节点的个数，从cross文件中读取,也就是行数
 *
 */
//void init(int N,int **dis) {//初始化//注意这里是图的初始化
//
//    double power(double,int);
//    double alpha = 0.1;//evaporation parameter，挥发参             数，每次信息素要挥发的量
//    int delta = 1;
//    int beta = 6;// delta 和 beta分别表示pheromones和herustic的比
//    int m = N;
//    int total_road_lenght = 0;//用于计算初始信号量
//
//    pheromone = new double *[N + 5];
//    herustic = new double *[N + 5];
//    info = new double *[N + 5];
//    int *r1 = new int[N + 5]; //r1:ant k(这里是车)的起点
//    int *r = new int[N + 5];  //r终点，这里可能待会读取的时候通过参数传入
//    int *s = new int[N + 5];  //s是下一个节点，根据的启发函数的值来选取
//    int *J = new set<int>[N + 5];   //J是ant(k)还未走过的节点,防止重复节点
////    empty.clear();
//
//    //初始化整幅图
//
//    for (int i = 1; i <= crossline; ++i)
//        for (int j = 1; j <= crossline; ++j) {
//            graph[i][j] = maxint;
//            if (roaddata[i][7] == 1)       // 双向
//            {
//                graph[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
//                graph[roaddata[i][6]][roaddata[i][5]] = roaddata[i][2];
//                total_road_lenght += roaddata[i][2] * 2;
//            } else// 单向
//            {
//                graph[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
//                total_road_lenght += roaddata[i][2];
//            }
//        }
//
//    //初始化信号量
//    pheromone_0 = 1/(total_road_lenght/roadline*crossline);//total_road_lenght/roadline是每条边的权值,crosslines是节点数目
//
//    for (int i = 1; i <=crossline; i ++){
//        pheromone[i] = new double[N + 5];
//        herustic[i] = new double[N + 5];
//        info[i] = new double[N + 5];
////        empty.insert(i);//??需要后面来更新
//        for (int j = 1; j <=crossline; j ++){
//            pheromone[i][j] = pheromone_0;
//            herustic[i][j] = 1 / (graph[i][j] + eps);//加一个小数eps，防止被零除
//            info[i][j] = power(pheromone[i][j], delta) *power(herustic[i][j], beta);
//        }
//    }
////    best_so_far.clean();
//    int iteration = 0;//迭代次数
//    int MAX = N * N;  //
//}
//
//double power(double x, int y){//快速幂，计算x ^ y，时间复杂度O(logn)
//    double ans = 1;
//    while (y){
//        if (y & 1) ans *= x;
//        x *= x;
//        y >>= 1;
//    }
//    return ans;
//}

//void reset(){
//    for (int i = 0; i < N; i ++)
//        for (int j = 0; j < N; j ++){
//            info[i][j] = power(pheromone[i][j], delta) *power(herustic[i][j], beta);
//        }//选择公式
//}
//
//void Ant_Colony_Optimization()
//{
//
//}

int main(int a,char* argv[])
{//car road cross answer
    clock_t first,finish;
    first = clock();
    char* carargv=argv[1];
    char* roadargv=argv[2];
    char* crossargv=argv[3];
//    string carargv="/Users/apple/Downloads/1-map-training-2/car.txt";
//    string roadargv="/Users/apple/Downloads/1-map-training-2/road.txt";
//    string crossargv="/Users/apple/Downloads/1-map-training-2/cross.txt";
    int total_road_lenght=0;
    crossin(crossargv);
    roadin(roadargv);
    carin(carargv);
    carsort();
    ChangeStartTime();
    cout<<crossdata[1][5]<<endl;
    cout<<crossdata[2][5]<<endl;
    cout<<crossdata[3][5]<<endl;
    cout<<crossdata[4][5]<<endl;
    cout<<crossdata[5][5]<<endl;
    cout<<crossdata[6][5]<<endl;

    // 各数组都从下标1开始

//    cout<<crossline<<endl;
    // 初始化c[][]为maxintt//图的初始化
    for(outline=1;outline<=carline;outline++)
    {
        for(int i=1; i<=crossline; ++i)
            for(int j=1; j<=crossline; ++j)
                graph[i][j] = maxint;

        //根据道路建图
        for(int i=1; i<=roadline; ++i)
        {
            if(roaddata[i][7] ==1)       // 双向
            {
                graph[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
                graph[roaddata[i][6]][roaddata[i][5]] = roaddata[i][2];
                total_road_lenght+= roaddata[i][2]*2;
            }
            else// 单向
            {
                graph[roaddata[i][5]][roaddata[i][6]] = roaddata[i][2];
                total_road_lenght+=roaddata[i][2];
            }
        }
        //初始化信号量
        pheromone_0 = 1/(total_road_lenght/roadline*crossline);//total_road_lenght/roadline是每条边的权值,crosslines是节点数目

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
        Dijkstra(crossline, start,outline,graph);
        searchPath(start,end,outline);
    }
    ofstream outfile;
    char* answer=argv[4];
//    string answer="/Users/apple/Desktop/软件精英挑战赛/2019软挑-初赛-SDK/SDK/SDK_C++/config/answer.txt";
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
    finish = clock();
    cout<<double(finish-first)/CLOCKS_PER_SEC<<endl;
}