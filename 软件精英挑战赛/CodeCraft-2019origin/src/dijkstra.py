import copy

from Graph import graph
import os
import Data
import getData

class dijkstra():
    def __init__(self):
        self.recordprenum=0;
        self.viewrow=[]
        self.view = []  # 从0开始
        self.methoddata = Data.data()
        self.methoddata.readCross()
        self.methoddata.readRoad()
        self.methoddata.readCar()
    def method(self, graph, src,destination,carnum):#src destination 都为节点id减一 node_road里面需要的是id

        if graph == None:
            return None
        # 定点集合
        nodes = [i for i in range(len(graph))]  # 获取顶点列表，用邻接矩阵存储图
        # 顶点是否被访问
        visited = []
        visited.append(src)
        nodes.remove(src)
        # 初始化dis
        dis = {src: 0}  # 源点到自身的距离为0
        for i in nodes:
            dis[i] = graph[src][i]
        path = {src: {src: [src]}}  # 记录源节点到每个节点的路径
        k = pre = src
        # getData.getRoadInfo()
        node_road = getData.node_road#ID而不是下标
        # viewnum=1
        lunci=int(self.methoddata.usable[carnum]/4+1)
        graph_ = copy.deepcopy(graph)
        while nodes:
            temp_k = k
            mid_distance = float('inf')  # 设置中间距离无穷大
            for v in visited:
                for d in nodes:
                    # print(visited,nodes)

                    if graph[src][v] != float('inf') and graph_[v][d] != float('inf') :  # 有边
                        cost = graph[v][d]
                        if(lunci>2):
                            # print(graph[v][d])
                            roadnum=node_road[int(v)+1,int(d)+1]
                            roadnum-=5000
                            mspeed=min(int(self.methoddata.roadSpeed[roadnum]),int(self.methoddata.carSpeed[carnum]))
                            time=graph[v][d]/mspeed
                            # print(lunci)
                            # print(len(self.view))
                            pretwo=(int(self.view[lunci-2][roadnum]+self.view[lunci-3][roadnum]))
                            wide=(int(self.methoddata.roadChannel[roadnum]) * int(self.methoddata.roadLength[roadnum]))
                            # print("pretwo")
                            # print(pretwo)
                            # print("wide")
                            # print(wide)
                            # print("time")
                            # print(time)
                            cost=graph[v][d]*(1+time*pretwo/wide)
                        new_distance = graph[src][v] + cost
                        if new_distance <= mid_distance:
                            mid_distance = new_distance
                            graph[src][d] = new_distance  # 进行距离更新
                            k = d
                            pre = v
            if k != src and temp_k == k:
                break
            dis[k] = mid_distance  # 最短路径
            path[src][k] = [i for i in path[src][pre]]
            path[src][k].append(k)
            visited.append(k)
            # print(k)
            nodes.remove(k)
            # print(nodes)
        # 记录用图的创建
        # print(path)
        if(self.recordprenum!=lunci):
            print(lunci)
            print(self.recordprenum)
            self.recordprenum=lunci
            self.view.append(self.viewrow)
            self.viewrow = []  # 从0开始
            for views in range(len(self.methoddata.roadFrom)):
                self.viewrow.append(0)
        pa = path[src]
        pa = pa.get(destination)
        # print(pa)
        pa = [point + 1 for point in pa]
        for j in range(len(pa) - 1):
            myway = node_road[int(pa[j]), int(pa[j + 1])]-5000
            self.viewrow[myway] += 1
        # view.append(viewrow)
        return dis, path






if __name__ == '__main__':

    G = graph()
    G.add_edge()
    D = dijkstra()
    tt = Data.data()
    tt.readCar()
    tt.readCross()
    tt.readRoad()
    ID = tt.carID
    From = tt.carFrom
    To = tt.carTo
    # getData.getRoadInfo()
    node_road = getData.node_road
    print(node_road)
    # print(node_road)
    # usable=[i for i in range(1,len(ID)+1)]

    # #排序交换 ID usable From To
    # for i in range(len(From)):
    #     for j in range(i + 1, len(From)):
    #         # print(j)
    #         if (int(ID[i]) > int(ID[j])):
    #             k = ID[i]
    #             ID[i] = ID[j]
    #             ID[j] = k
    #             k = usable[i]
    #             usable[i]=usable[j]
    #             usable[j]=k
    #             k=From[i]
    #             From[i]=From[j]
    #             From[j]=k
    #             k=To[i]
    #             To[i]=To[j]
    #             To[j]=k

    with open('way.txt', 'w') as f:
        for i in range(len(ID)):
        # for i in range(1000):
            id = int(ID[i])
            FROM = int(From[i])
            TO = int(To[i])
            graph_list = copy.deepcopy(G.adj)
            # print(graph_list)
            distance, path = D.method(graph_list, FROM-1,TO-1,i)
            pp = path[FROM-1]
            pp = pp.get(TO-1)
            length = distance.get(TO-1)
            pp = [i+1 for i in pp]
            f.write("(" + str(id) + "," + str(tt.usable[i]))
            # for i in range(len(pp)-1):
            for j in range(len(pp)-1):
                way = node_road[int(pp[j]), int(pp[j + 1])]
                f.write(","+str(way))
            print(i)
            f.write(")\n")
            print(id, ' ',length,' ', )
            # print(length,' ',pp)
        print(D.view)