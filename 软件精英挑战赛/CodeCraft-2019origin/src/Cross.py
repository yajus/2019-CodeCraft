# -*- coding: utf-8 -*-
import os
from getData import CrossInfo


class Cross:
    def __init__(self, id):
        self.id = id
        self.RoadId_1 = CrossInfo[id][0]
        self.RoadId_2 = CrossInfo[id][1]
        self.RoadId_3= CrossInfo[id][2]
        self.RoadId_4 = CrossInfo[id][3]

        ##优先级

