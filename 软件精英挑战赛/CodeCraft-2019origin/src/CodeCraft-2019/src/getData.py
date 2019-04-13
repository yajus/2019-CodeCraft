# -*- coding: utf-8 -*-
from operator import itemgetter

CarInfo = dict()
RoadInfo = dict()
CrossInfo = dict()
node_road = dict()
# txt =['car.txt','road.txt','cross.txt']
# data_path = './1-map-training-1/'

def getCarInfo(carfile):
    # for line in open(data_path+'car.txt').readlines():
    for line in open(carfile).readlines():
        if line[0]=='#':
            pass
        else:
            tuple_info = tuple(eval(line.strip()))
            dict_info = dict()
            key = tuple_info[0]
            item = tuple_info[1:]
            CarInfo[key] = item

def getRoadInfo(file_path):
    for line in open(file_path).readlines():
        if line[0]=='#':
            pass
        else:
            tuple_info = tuple(eval(line.strip()))

            key = tuple_info[0]
            item = tuple_info[1:]
            RoadInfo[key] = item
            key = (tuple_info[-3],tuple_info[-2])
            node_road[key] = tuple_info[0]
            if tuple_info[-1]==1:
                key = (tuple_info[-2],tuple_info[-3])
                node_road[key] = tuple_info[0]



def getCrossInfo(file_path):
    for line in open(file_path).readlines():
        if line[0]=='#':
            pass
        else:
            tuple_info = tuple(eval(line.strip()))
            dict_info = dict()
            key = tuple_info[0]
            item = tuple_info[1:]
            CrossInfo[key] = item





