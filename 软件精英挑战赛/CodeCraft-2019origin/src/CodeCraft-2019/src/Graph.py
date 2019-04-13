# -*- coding: utf-8 -*-
import os
import Data
data_path = './1-map-training-1/'

class graph():
    def __init__(self):
        self.test=Data.data()
        self.test.readCross()
        self.test.readRoad()
        self.n_vertices =self.test.crossID
        self.adj = [[float('inf') for i in range(len(self.n_vertices))] for _ in self.n_vertices]
        self.roadFrom = self.test.roadFrom
        self.roadTo = self.test.roadTo
        self.roadLength = self.test.roadLenght
        self.roadIsDuplex = self.test.roadIsDuplex



    def add_edge(self):
        for i in range(len(self.test.roadTo)):

            self.adj[self.n_vertices.index(self.roadFrom[i])][self.n_vertices.index(self.roadTo[i])] \
                = int(self.roadLength[i])
            self.adj[self.n_vertices.index(self.roadFrom[i])][self.n_vertices.index(self.roadFrom[i])]=0
            self.adj[self.n_vertices.index(self.roadTo[i])][self.n_vertices.index(self.roadTo[i])]=0


            if(self.roadIsDuplex[i]=='1'):
                self.adj[self.n_vertices.index(self.roadTo[i])][self.n_vertices.index(self.roadFrom[i])] \
                    = int(self.roadLength[i])
                # print('in f')








