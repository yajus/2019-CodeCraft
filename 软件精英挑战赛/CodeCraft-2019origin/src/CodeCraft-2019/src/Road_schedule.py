# -*- coding: utf-8 -*-
import os
from Road import Road
from Car import Car
import getData
from getData import CarInfo, CrossInfo, RoadInfo
from queue import Queue
import datetime
import Graph
from time import sleep

##判断车道是不是满的


##地杰斯特拉
def dijkstra(graph, src):
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
        nodes.remove(k)
        # print(nodes)
    return dis, path

def car_to_new_road(carID, roadID):
    for i in range(Road(roadID).channel):
        current_num = Road(roadID).queue[i].qsize()
        if current_num < Road(roadID).length:
            ##不满的话就入队
            Road(roadID).queue[i].put(Car(carID))
            Car(carID).currentRoad_channel_id = i
            Car(carID).currentRoad = roadID
            ##之前的车道的弹出
            Road(Car(carID).lastRoad).queue[Car(carID).lastRoad_channel_id].pop()  # 这里也要改queue
            ##速度问题
            if Road(roadID).speed > Car(carID).speed:
                Car(carID).current_speed = Road(roadID).speed


def road_to_choice(carID, crossID,pre_crossID,crosses):
    '''
    选出当前路口节点的所有可选道路
    对每一条道路找到相对的道路节点
    用A*算法计算节点的启发函数
    返回最佳道路ID
    '''
    crossinfo = CrossInfo[crossID]
    turn_to_road = 0  # 最终选择的道路
    result = 100000  # 最终结果
    tmp_result = 0  # 暂时结果

    for road_id in crossinfo:
        if road_id!=-1:
            roadinfo = RoadInfo[road_id]
            road_from = roadinfo[3]
            road_to = roadinfo[4]
            next_cross_id = 0  # 当前选择道路的下一个路口节点
            # 判断当前选择道路的下一个路口节点
            if crossID == road_from:
                next_cross_id = road_to
            # 判断道路是否双向
            elif roadinfo[5] == 1:
                next_cross_id = road_from
            # 如果不是 则当前道路虽然联通 但没有这个方向的路
            else:
                continue
            if next_cross_id==pre_crossID or next_cross_id in crosses:
                continue
            tmp_result = heuristic(carID, next_cross_id)
            # print("第"+str(road_id)+"个方向的A*结果为："+str(tmp_result))
            if tmp_result < result:
                result = tmp_result
                turn_to_road = road_id
    # print("选择的路径为"+str(turn_to_road))
    if turn_to_road==0:
        return 0,0
        # turn_to_road=road_id
        # # with open("error.txt","a") as fp:
        # #     fp.write(str(carID)+"\n")
        # print("找不到可以走的路,重走:" + str(carID))
    return turn_to_road,next_cross_id


def heuristic(carID, crossID):
    '''
    用启发式函数计算当前道路节点到终点的距离
    can_to_end函数判断小车走这条路是否能够到达终点
    cal_distance函数计算两个节点之间的直线距离
    '''
    end_id = CarInfo[carID][1]  # 小车的终点节点
    a = 1  # 系数
    # 判断小车能否到达终点
    # can_to_end = can_to_end(crossID, end_id)
    can_to_end=1
    if can_to_end == 0:
        a = 1000  # 如果不能到达终点，则系数设为无穷大
    # 计算公式  当前小车已走距离 + 系数 * 下一节点到终点的直线距离
    result = Car(carID).start_current + a * cal_distance(crossID, end_id)
    return result


def cal_distance(crossID, end_id):
    '''
    计算两节点间直线距离
    公式 ((x1-x2)^2+(y1-y2)^2)
    coordinate是一个二维数组 每个节点一个一维数组，数组内为节点x坐标和节点Y坐标
    '''
    distance = pow((coordinate[crossID][0] - coordinate[end_id][0]), 2) + \
               pow((coordinate[crossID][1] - coordinate[end_id][1]), 2)
    return distance


def print_result():
    '''
    遍历小车
    将Car中的路径结果保存到txt中
    #(carId,StartTime,RoadId...)
    (1001, 1， 501, 502, 503, 516, 506, 505, 518, 508, 509, 524)
    '''
    with open("answer.txt", "w") as fp:
        # for id in range(10000, 10000 + len(CrossInfo)):
        for id in range(10000, 10100):
            car=Car(id)
            fp.write("(" + str(id) + ", " + str(Car(id).start_time))
            for roads in car.roads:
                fp.write(", " + roads)
            fp.write(")\n")


def create_coordinate():
    '''
    根据road.txt和cross.txt构建出及节点坐标
    '''
    finish_cross = []  # 已经处理的节点
    q = Queue()  # 创建队列来存放未处理的节点
    global coordinate
    coordinate = [[0] * 2 for i in range(len(CrossInfo) + 1)]  # 节点id从1开始
    coordinate[1][0] = 0
    coordinate[1][1] = 0  # 初始化零坐标点 以节点1的坐标为(0,0)
    current_point = 1
    finish_cross.append(current_point)
    for roads in range(4):  # 遍历当前节点的四个方向
        if (CrossInfo[1][roads] != -1):  # 判断该方向有没有道路
            road_id = CrossInfo[1][roads]  # 取出道路信息
            if current_point == RoadInfo[road_id][3]:
                next_point = RoadInfo[road_id][4]
            else:
                next_point = RoadInfo[road_id][3]  # 找当前节点连接的下一个节点
            if next_point not in finish_cross:  # 如果之前已经计算过该节点坐标 则跳过
                q.put(next_point)
                finish_cross.append(next_point)
            else:
                # 将新节点放入已完成数组
                continue
            if (int(roads) == 0):  # 根据道路在节点的哪个方向来决定新节点的坐标计算
                coordinate[next_point][0] = coordinate[1][0]  # x不变
                coordinate[next_point][1] = coordinate[1][1] + RoadInfo[road_id][0]  # y=y+长度
            elif (int(roads) == 1):
                coordinate[next_point][0] = coordinate[1][0] + RoadInfo[road_id][0]  # x=x+长度
                coordinate[next_point][1] = coordinate[1][1]  # y不变
            elif (int(roads) == 2):
                coordinate[next_point][0] = coordinate[1][0]
                coordinate[next_point][1] = coordinate[1][1] - RoadInfo[road_id][0]
            elif (int(roads) == 3):
                coordinate[next_point][0] = coordinate[1][0] - RoadInfo[road_id][0]
                coordinate[next_point][1] = coordinate[1][1]
    # print("遍历完初始节点")
    # print(finish_cross)
    # print(q.qsize())
    while not q.empty() and len(finish_cross) != len(CrossInfo):
        # if q.empty():
        #     q.put()
        current_point = q.get()
        # print("now dealing corss_id:  " + str(current_point))
        for roads in range(4):
            # print("当前找第" + str(roads) + "个方向的路径")
            if (CrossInfo[current_point][roads] != -1):
                road_id = CrossInfo[current_point][roads]
                # print(road_id)
                # print(RoadInfo[road_id][3], RoadInfo[road_id][4])
                if current_point == RoadInfo[road_id][3]:
                    next_point = RoadInfo[road_id][4]
                else:
                    next_point = RoadInfo[road_id][3]  # 找当前节点连接的节点
                # print(next_point)
                if next_point not in finish_cross:  # 如果之前已经计算过该节点坐标 则跳过
                    q.put(next_point)
                    finish_cross.append(next_point)
                    # print("放入新节点")
                else:
                    # print("这个节点找过了")
                    # 将新节点放入已完成数组
                    # print(finish_cross)
                    continue
                if (int(roads) == 0):
                    coordinate[next_point][0] = coordinate[current_point][0]  # x不变
                    coordinate[next_point][1] = coordinate[current_point][1] + RoadInfo[road_id][0]  # y=y+长度
                elif (int(roads) == 1):
                    coordinate[next_point][0] = coordinate[current_point][0] + RoadInfo[road_id][0]  # x=x+长度
                    coordinate[next_point][1] = coordinate[current_point][1]  # y不变
                elif (int(roads) == 2):
                    coordinate[next_point][0] = coordinate[current_point][0]
                    coordinate[next_point][1] = coordinate[current_point][1] - RoadInfo[road_id][0]
                elif (int(roads) == 3):
                    coordinate[next_point][0] = coordinate[current_point][0] - RoadInfo[road_id][0]
                    coordinate[next_point][1] = coordinate[current_point][1]


def run(output_file):
    # 对每辆车进行调度，得出行驶的路径，保存在Car的属性中
    G = Graph.graph()
    G.add_edge()
    node_road = getData.node_road
    # with open(output_file, "w") as fp:
    for id in range(10000, 10000 + len(CarInfo)):
    # for id in range(11403,  11405):
        # if id%1000 ==0:
        # print("处理到第"+str(id)+"辆车",end="")
        #写文件
        car=Car(id)
        # fp.write("(" + str(id) + ", " + str(Car(id).start_time))
        # print("对第"+str(id)+"辆车进行行驶")
        # print(Car(id).roads)
        carinfo = CarInfo[id]
        start_point = carinfo[0]
        end_point = carinfo[1]
        # print("第"+str(id)+"辆车的起点和终点： "+str(start_point),str(end_point))
        current_point = start_point
        pre_point = 0
        # print("经过的道路ID为： ",end="")
        # print("经过的节点ID为："+str(current_point)+" ",end="")
        # print("|")
        while (current_point != end_point):
            # 判断当前节点应该走哪条道路
            next_road,next_id = road_to_choice(id, current_point,pre_point,car.cross)
            if next_road==0 and next_id==0:

                graph = G.adj
                distance, path= dijkstra(graph,start_point-1)
                pp = path[start_point - 1]
                # print(pp)
                pp = pp.get(end_point - 1)
                length = distance.get(end_point - 1)
                pp = [i + 1 for i in pp]
                pp = [node_road[pp[i], pp[i + 1]] for i in range(len(pp) - 1)]
                # print (pp)
                car.roads=pp
                # print(car.id, ' ',car.roads)
                # print("用到dj斯特拉！！！")
                break
            # print("放入新道路："+str(next_road))
            # Car(id).roads.append(str(next_road))
            car.add_Cross(next_id)
            car.add_Route(next_road)
            # print(car.roads)
            # Car(id).roads.append(str(next_road))

            # 判断下一个节点
            pre_point = current_point
            if (current_point == RoadInfo[next_road][3]):
                current_point = RoadInfo[next_road][4]
            else:
                current_point = RoadInfo[next_road][3]

            # print(str(next_road)+" ",end="")    #当前走的道路
            # print(str(current_point)+" ",end="")  #当前走的节点
            # print("|", end="")
            # fp.write(", "+str(next_road))
        with open(output_file,"a") as fp:
            fp.write("("+str(id)+","+str(Car(id).start_time))
            for i in car.roads:
                fp.write(","+str(i))
            fp.write(")\n")
            print(car.id,' ',car.roads)
        # print(car.roads)
        # sleep(0.01)
        # fp.write(")\n")
        # print("")



if __name__ == '__main__':
    getData.getCarInfo(r"../1-map-training-1/car.txt")
    getData.getRoadInfo(r"../1-map-training-1/road.txt")
    getData.getCrossInfo(r"../1-map-training-1/cross.txt")
    starttime = datetime.datetime.now()
    create_coordinate()
    # for i in range(1, len(CrossInfo) + 1):
    #     print("第" + str(i) + "个节点的坐标： " + str(coordinate[i][0]), str(coordinate[i][1]))
    run(r"../answer/answer1.txt")
    # print_result()
    endtime = datetime.datetime.now()
    print(endtime - starttime)

