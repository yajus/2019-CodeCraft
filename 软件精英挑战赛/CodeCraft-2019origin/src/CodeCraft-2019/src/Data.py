# import os
# data_path = './1-map-training-1/'
#
# class data():
#     def __init__(self,crossfile=data_path+'cross.txt', roadfile=data_path+'road.txt'):
#         self.crossfile = crossfile
#         self.roadfile = roadfile
#         self.crossID = []
#         self.connect_roadID_1 = []
#         self.connect_roadID_2 = []
#         self.connect_roadID_3 = []
#         self.connect_roadID_4 = []
#         self.roadID = []
#         self.roadLenght = []
#         self.roadSpeed = []
#         self.raodChannel = []
#         self.roadFrom = []
#         self.roadTo = []
#         self.roadIsDuplex = []
#
#
#     def readCross(self):
#         for line in open(self.crossfile).readlines():
#             if line[0]=='#':
#                 pass
#             else:
#                 # (1, 5000, 5007, -1, -1)
#                 line = line.replace(line[0],'')
#                 line = line.replace(line[-1],'')
#                 split_line = line.split(', ')
#                 # print(split_line)
#                 self.crossID.append(split_line[0])
#                 self.connect_roadID_1.append(split_line[1])
#                 self.connect_roadID_2.append(split_line[2])
#                 self.connect_roadID_3.append(split_line[3])
#                 self.connect_roadID_4.append(split_line[4])
#
#     def readRoad(self):
#         for line in open(self.roadfile).readlines():
#             if line[0]=='#':
#                 pass
#             else:
#                 # (1, 5000, 5007, -1, -1)
#                 line = line.replace(line[0],'')
#                 line = line.replace(line[-1],'')
#                 split_line = line.split(', ')
#                 self.roadID.append(split_line[0])
#                 self.roadLenght.append(split_line[1])
#                 self.roadSpeed.append(split_line[2])
#                 self.raodChannel.append(split_line[3])
#                 self.roadFrom.append(split_line[4])
#                 self.roadTo.append(split_line[5])
#                 self.roadIsDuplex.append(split_line[6])
#
#
#

import os

data_path = './1-map-training-1/'


class data():
    def __init__(self, crossfile='map/cross.txt', roadfile='map/road.txt',carfile='map/car.txt'):
        self.crossfile = crossfile
        self.roadfile = roadfile
        self.carfile = carfile
        self.crossID = []
        self.connect_roadID_1 = []
        self.connect_roadID_2 = []
        self.connect_roadID_3 = []
        self.connect_roadID_4 = []
        self.roadID = []
        self.roadLenght = []
        self.roadSpeed = []
        self.raodChannel = []
        self.roadFrom = []
        self.roadTo = []
        self.roadIsDuplex = []
        self.carID = []
        self.carFrom = []
        self.carTo = []

    def readCross(self):
        for line in open(self.crossfile).readlines():
            if line[0] == '#':
                pass
            else:
                # (1, 5000, 5007, -1, -1)
                line = line.replace(line[0], '')
                line = line.replace(line[-1], '')
                line = line.replace(')','')
                split_line = line.split(', ')
                # print(split_line)
                self.crossID.append(split_line[0])
                self.connect_roadID_1.append(split_line[1])
                self.connect_roadID_2.append(split_line[2])
                self.connect_roadID_3.append(split_line[3])
                self.connect_roadID_4.append(split_line[4])

    def readRoad(self):
        for line in open(self.roadfile).readlines():
            if line[0] == '#':
                pass
            else:
                # (1, 5000, 5007, -1, -1)
                line = line.replace(line[0], '')
                line = line.replace(line[-1], '')
                line = line.replace(')', '')
                split_line = line.split(', ')
                # print(split_line)
                self.roadID.append(split_line[0])
                self.roadLenght.append(split_line[1])
                self.roadSpeed.append(split_line[2])
                self.raodChannel.append(split_line[3])
                self.roadFrom.append(split_line[4])
                self.roadTo.append(split_line[5])
                self.roadIsDuplex.append(split_line[6])

    def readCar(self):
        for line in open(self.carfile).readlines():
            if line[0] == '#':
                pass
            else:
                #(id,from,to,speed,planTime)
                line = line.replace(line[0], '')
                line = line.replace(line[-1], '')
                line = line.replace(')', '')
                split_line = line.split(', ')
                self.carID.append(split_line[0])
                self.carFrom.append(split_line[1])
                self.carTo.append(split_line[2])

