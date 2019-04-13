# -*- coding: utf-8 -*-
# @Time    : 2019-03-17 16:30
# @Author  : LXF
# @Site    : 
# @File    : CodeCraft-2019.py
# @Software: PyCharm

from Road_schedule import *
from sys import argv
import getData



if __name__ == '__main__':
    carfile, roadfile, crossfile, output_file = argv[1], argv[2], argv[3], argv[4]
    getData.getCarInfo(carfile)
    getData.getRoadInfo(roadfile)
    getData.getCrossInfo(crossfile)
    # starttime = datetime.datetime.now()
    create_coordinate()
    run(output_file)
    # endtime = datetime.datetime.now()
    # print(endtime - starttime)
