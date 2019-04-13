# -*- coding: utf-8 -*-
import os
from getData import CarInfo


##车的信息
class Car:
    def __init__(self,id):
        self.id = id
        self.start = CarInfo[id][0]
        self.to = CarInfo[id][1]
        self.speed = CarInfo[id][2]
        self.planTime = CarInfo[id][3] #出发时间
        self.current_speed = self.speed
        ##优先级
        self.priority = 0
        ##车辆花费时间
        self.costTime = 0
        ##车之前的道路编号
        self.lastRoad = 0
        #之前车channel的id
        self.lastRoad_channel_id = 0
        ##小车现在的道路编号
        self.currentRoad = 0
        ##当前道路的起点,也是上一条道路的终点
        self.currentRoad_start = 0
        #当前车channel的id
        self.currentRoad_channel_id = 0
        ##当前道路的终点
        self.currentRoad_end = 0
        ##车是否到达终点,标记
        self.flag = 0
        #从起点到当前节点的距离,用于A*
        self.start_current = 0


        #记录已经走过的道路
        self.roads = []
        self.cross= []
        #记录小车开始行驶的时间
        self.start_time = self.planTime

    def add_Cross(self,cross_id):
        self.cross.append(cross_id)

    def add_Route(self,road_id):
        self.roads.append(road_id)


if __name__ == '__main__':
    a=Car(10000)
    a.add_Route(1)
    print(a.roads)