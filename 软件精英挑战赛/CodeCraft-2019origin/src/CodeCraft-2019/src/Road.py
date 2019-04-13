# -*- coding: utf-8 -*-
import os
from getData import RoadInfo
from queue import Queue
class Road:
    def __init__(self,id):
        self.id = id
        self.length = RoadInfo[id][0]
        self.speed = RoadInfo[id][1]
        self.channel = RoadInfo[id][2]
        self.start = RoadInfo[id][3]
        self.to = RoadInfo[id][4]
        self.isDuplex = RoadInfo[id][5]
        ##优先级
        self.current_car_num = 0
        self.queue = [[] for i in range(self.isDuplex)] #加个判断道路是否双向
        for i in range(self.channel):
            if self.isDuplex:
                self.queue[0].append(Queue())
                self.queue[1].append(Queue())
            else:
                self.queue[0].append(Queue())