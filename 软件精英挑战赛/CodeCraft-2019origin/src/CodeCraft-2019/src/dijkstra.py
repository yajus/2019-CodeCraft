import copy

from Graph import graph
import os

class dijkstra():
    def method(self, graph, src):
        if graph == None:
            return None
        # 定点集合
        nodes = [i for i in range(len(graph))]  # 获取顶点列表，用邻接矩阵存储图
        # 顶点是否被访问
        visited = []
        visited.append(src)
        # 初始化dis
        dis = {src: 0}  # 源点到自身的距离为0
        for i in nodes:
            dis[i] = graph[src][i]
        path = {src: {src: []}}  # 记录源节点到每个节点的路径
        k = pre = src
        while nodes:
            temp_k = k
            mid_distance = float('inf')  # 设置中间距离无穷大
            for v in visited:
                for d in nodes:
                    if graph[src][v] != float('inf') and graph[v][d] != float('inf') :  # 有边

                        new_distance = graph[src][v] + graph[v][d]
                        if new_distance <= mid_distance:

                            mid_distance = new_distance
                            graph[src][d] = new_distance  # 进行距离更新
                            k = d
                            pre = v
            if k != src and temp_k == k:
                break
            dis[k] = mid_distance  # 最短路径
            path[src][k] = [i for i in path[src][pre]]
            # print(path)
            path[src][k].append(k)

            visited.append(k)
            # print(k)
            nodes.remove(k)
            # print(nodes)
        return dis, path




import Data

if __name__ == '__main__':

    G = graph()
    G.add_edge()
    D = dijkstra()
    tt = Data.data()
    tt.readCar()
    ID = tt.carID
    From = tt.carFrom
    To = tt.carTo
    with open('way.txt', 'w') as f:
        for i in range(len(ID)):
            id = int(ID[i])
            FROM = int(From[i])
            TO = int(To[i])

            graph_list = copy.deepcopy(G.adj)
            # print(graph_list)
            distance, path = D.method(graph_list, FROM-1)
            # print(distance)
            # print(path)
            pp = path[FROM-1]
            # print(pp)
            pp = pp.get(TO-1)
            length = distance.get(TO-1)
            pp = [i+1 for i in pp]
            f.write('{} ---> {} :length {}  way: {} {}'.format(FROM,TO,distance,pp,'\n'))
            print(length,' ',pp)