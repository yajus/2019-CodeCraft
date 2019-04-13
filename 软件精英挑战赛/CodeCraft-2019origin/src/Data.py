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

data_path = './1-map-training-2/'#该变量没有用到

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
        self.roadLength = []
        self.roadSpeed = []
        self.roadChannel = []
        self.roadFrom = []
        self.roadTo = []
        self.roadIsDuplex = []
        self.carID = []
        self.carFrom = []
        self.carTo = []
        self.carStartTime = []
        self.carSpeed=[]
        self.usable = []

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
                self.roadLength.append(split_line[1])
                self.roadSpeed.append(split_line[2])
                self.roadChannel.append(split_line[3])
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
                self.carStartTime.append(split_line[-1])
                self.carSpeed.append(split_line[3])
        #排序
        for i in range(len(self.carTo)):
            for j in range(i+1,len(self.carTo)):
                # print(j)
                if(int(self.carSpeed[i])<int(self.carSpeed[j])):
                    k=self.carID[i]
                    self.carID[i]=self.carID[j]
                    self.carID[j]=k
                    k = self.carTo[i]
                    self.carTo[i] = self.carTo[j]
                    self.carTo[j] = k
                    k = self.carFrom[i]
                    self.carFrom[i] = self.carFrom[j]
                    self.carFrom[j] = k
                    k = self.carStartTime[i]
                    self.carStartTime[i] = self.carStartTime[j]
                    self.carStartTime[j] = k
                    k = self.carSpeed[i]
                    self.carSpeed[i] = self.carSpeed[j]
                    self.carSpeed[j] = k

        #编辑出发时间
        ten=1
        num=1
        for i in range(1,len(self.carTo)+1):
            if ten>100:
                num+=4
                ten=1
            if num<int(self.carStartTime[i-1]):
                self.usable.append(int(self.carStartTime[i-1]))
            else:
                self.usable.append(num)
            ten += 1

        #顺序换回来
        # for i in range(len(self.carTo)):
        #     for j in range(i + 1, len(self.carTo)):
        #         # print(j)
        #         if (int(self.carID[i]) > int(self.carID[j])):
        #             k = self.carID[i]
        #             self.carID[i] = self.carID[j]
        #             self.carID[j] = k
        #             k = self.carTo[i]
        #             self.carTo[i] = self.carTo[j]
        #             self.carTo[j] = k
        #             k = self.carFrom[i]
        #             self.carFrom[i] = self.carFrom[j]
        #             self.carFrom[j] = k
        #             k = self.carStartTime[i]
        #             self.carStartTime[i] = self.carStartTime[j]
        #             self.carStartTime[j] = k
        #             k = self.usable[i]
        #             self.usable[i] = self.usable[j]
        #             self.usable[j] = k

        # print(self.carStartTime)



