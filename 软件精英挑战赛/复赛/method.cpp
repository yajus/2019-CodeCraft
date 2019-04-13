#include "method.h"

using namespace std;

/*道路信息初始化*/
Road::Road(int id_, int length_, int max_speed_, int from_id_, int to_id_,
           int channels_, bool is_duplex_) {
    id = id_;
    length = length;
    max_speed = max_speed_;
    from_id = from_id_;
    to_id = to_id_;
    channels_num = channels_;
    is_duplex = is_duplex_;

}
/**根据ID*/

/*输入道路的流*/
Road::Road(string s) {
    stringstream ss;
    ss << s;
    ss >> id >> length >> max_speed >> channels_num
       >> from_id >> to_id >> is_duplex;
}

/* 获取可用的车道索引 */
int Road::getAvailableChannelIndex(vector<queue<Car *>> channels) {
    int index = 0;
    for (auto channel = channels.begin(); channel != channels.end(); channel++) {
        if (channel->back()->at_road_position > 0) {//判断队尾的车的位置是否大于0
            /*路的逻辑是 -------------->(尾部是0 逐渐增大至 队头)*/
            return index; //返回车的channel的索引
        }
    }
    return -1;
}

/* 车辆驶入道路的逻辑 */
void Road::addCar(Car *car, int last_cross_id) {
// 根据车辆驶来的方向来确认要驶入的道路
    vector<queue<Car *>> *target_channels = &s2e_channels;
    int direction = 1;
    if (last_cross_id != from_id) {
        if (!is_duplex) throw runtime_error("道路不是双向车道！");
        target_channels = &e2s_channels;
        direction = -1;
    }
// TODO: 处理车辆驶入道路的逻辑
// 1. 判断车道空闲情况，注意调用该函数时，目标车道一定是空闲的，否则将会抛出错误
    int availableChannelIndex = this->getAvailableChannelIndex(*target_channels);
// int availableChannelIndex = 0;
    if (availableChannelIndex < 0)
        throw runtime_error("不合法的调度，目标道路无空闲");
// 2. 车辆驶入新道路
    target_channels->at(availableChannelIndex).push(car);
// 3. 更改车辆所在道路以及车道
    car->changeRoad(this, availableChannelIndex, direction);
    target_channels->at(0).push(car);
}//???

Car::Car(int id_, int from_id_, int to_id_, int max_speed_, int plan_time_, int priority_, int preset_) {
    id = id_;
    from_id = from_id_;
    to_id = to_id_;
    max_speed = max_speed_;
    plan_time = plan_time_;
    priority = priority_;
    preset = preset_;
}

/**
* 从流中生成类实例，字符串流必须包含与类构造函数相匹配的参数数目
*/
Car::Car(string s) {
    stringstream ss;
    ss << s;
    ss >> id >> from_id >> to_id >> max_speed >> plan_time >> priority >> preset;
}

void Car::updateStatus(int new_status) {
    status = new_status;
}

void Car::changeSpeed(int new_s) {
    if (new_s > max_speed)
        throw runtime_error("The new speed cannot larger than max speed!");
    speed = new_s;
}

void Car::changeRoad(Road *road_, int channel_, int direction) {
    if (channel_ >= road_->channels_num) //
        throw runtime_error("Target road channels wrong!");
    at_road = road_;
    at_channel_id = channel_;
    at_channel_direction = direction;
}

void Car::changePosition(int new_p) {
    if (new_p > at_road->length) throw runtime_error("车辆所在位置超出车道限制！");
    at_road_position = new_p;
}

/* 车辆往前行驶一个时间单位 */
void Car::move() {
// 有两种情况，一种是车辆在车道内部行驶，另一种是车辆经过路口，进入下一条路
// 车辆开始行驶时，必须处于等待行驶状态，行驶过后，变为终止状态
    if (status != 1) throw runtime_error("只有处于等待状态的车辆才能行驶！");
// 开始判断车辆是否需要驶出路口
    Car *front_car = getFrontCar();

    int speed = min(max_speed, at_road->max_speed);

    this->current_time += 1;
    cout << this->id << " " << this->current_time << endl;
    if (front_car != NULL) {
        // 存在前车
        if (front_car->status == 1) {
            // 前车处于等待行驶状态，则本车保持不动
            this->updateStatus(1);
        } else if (front_car->status == 2) {
            // 前车处于终止状态，则本车向前行驶，并将自身状态设置为终止状态
            this->could_move_distance = min(could_move_distance,
                                            front_car->at_road_position - this->at_road_position - 1);//能移动的长度
            this->changePosition(this->at_road_position + could_move_distance);
            this->updateStatus(2); // 车辆移动完毕，处于终止状态
        } else {
            throw runtime_error("道路存在不明情况的前车");
        }
    } else {
        // 不存在前车
        if (at_road->length - this->at_road_position <= speed) {
            // 出道路
            // 当该车辆要行驶出路口时，将车辆状态设置为等待行驶状态
            // 因为路口处的规则比较复杂，需要单独进行处理
            this->could_move_distance = at_road->length - this->at_road_position;//说明到路口了
            this->changePosition(this->at_road_position + could_move_distance);
            this->updateStatus(1);

        } else {
            // 不出道路，直接处理车辆状态即可
            this->could_move_distance = this->at_road_position + speed;//能够移动的长度
            this->changePosition(this->at_road_position + could_move_distance);
            this->updateStatus(2);
        }
    }
}

Car *Car::getFrontCar() {
    auto *channels = at_channel_direction > 0 ? &at_road->s2e_channels : &at_road->e2s_channels;//查看方向
    auto *channel = &channels->at(at_channel_id);
    Car *result;
// 如果车辆是车道中最靠前的一辆车，则返回NULL，表示没有前车
    if (this->id == channel->back()->id) return NULL;
// 否则就搜索前车
    for (auto it = channel->back(); (it->id != this->id)&&(it>= channel->front()); it--) {
        result = it;
        // 一直搜索到队首，都搜索不到，则报错
        if (it == channel->front())
            throw runtime_error("车辆状态与所在道路状态不一致，请检查！");
    }
    return result;
}

Cross::Cross(int id_, int top_road_id_, int right_road_id_, int bottom_road_id_,
             int left_road_id_) {
    id = id_;
    top_road_id = top_road_id_;
    right_road_id = right_road_id_;
    bottom_road_id = bottom_road_id_;
    left_road_id = left_road_id_;
}

Cross::Cross(string s) {
    stringstream ss;
    ss << s;
    ss >> id >> top_road_id >> right_road_id >> bottom_road_id >> left_road_id;
}


Pre_set_car::Pre_set_car(int id_, int plan_time_, vector<int> path_) {
    id = id_;
    plan_time = plan_time_;
    path = path_;
}

Pre_set_car::Pre_set_car(string s) {
    stringstream ss;
    ss << s;
    ss >> id >> plan_time;
    for (int d; ss >> d; path.push_back(d)) {};
}


/* 车辆入库操作使用指针，来保证全局状态统一 */
int Cross::addCar(Car *c) {
    car_port.push(c);
    return car_port.size();
}


vector<string> &Traffic::split(const string &str, const string &delimiters, vector<string> &elems, bool skip_empty) {
    string::size_type pos, prev = 0;
    while ((pos = str.find_first_of(delimiters, prev)) != string::npos) {
        if (pos > prev) {
            if (skip_empty && 1 == pos - prev) break;
            elems.emplace_back(str, prev, pos - prev);
        }
        prev = pos + 1;
    }
    if (prev < str.size()) {
        elems.emplace_back(str, prev, str.size() - prev);
    }
    return elems;
}

/*根据输入流文件初始化road,car,cross,perset的实例*/
/*preset是否会多余后续会考虑*/
void Traffic::initTraffic(string car_path, string road_path, string cross_path, string pre_set_car) {
// 构建路网的基本实例
    cars = initInstance<Car>(car_path);  //将对象塞进vector容器中
    roads = initInstance<Road>(road_path);
    crosses = initInstance<Cross>(cross_path);
    pre_set_cars = initInstance<Pre_set_car>(pre_set_car);
    priority_cars = get_priority_or_unpriority_car(cars, 1);
    un_priority_cars = get_priority_or_unpriority_car(cars, 0);
    buildIndex();
    builTimeIndex(pre_set_cars);
    update_preset_car(cars);
    init_preset_road_weight();
}


void Traffic::buildIndex() {
    car_id2index = buildMapIndex<Car>(cars, false);
    road_id2index = buildMapIndex<Road>(roads, true);//按照ID升序,为了调度
    cross_id2index = buildMapIndex<Cross>(crosses, true);//按照ID升序,为了调度
    road_ID_dict = build_ID_dict<Road>(roads);
    car_ID_dict = build_ID_dict<Car>(cars);
    cross_ID_dict = build_ID_dict<Cross>(crosses);
    portCarsToPort();
    computeAvglen();
}


void Traffic::builTimeIndex(vector<Pre_set_car> pre_set_car) {
    for (auto p:pre_set_car)
        timeIndex[p.plan_time]++;
}

template<typename T>
unordered_map<int, T> Traffic::build_ID_dict(vector<T> &cross) {
    unordered_map<int, T> ID_dict;
    for (unsigned int i = 0; i < cross.size(); i++)
        ID_dict.insert(pair<int, T>(cross[i].id, cross[i]));
    return ID_dict;
}

/*计算道路长度,没有必要的了,估计*/
void Traffic::computeAvglen() {
    int s = 0;
    for (auto r:roads) {
        s += r.length;
    }
    avg_len = s / roads.size();
}


/* 将车辆停靠在对应的车库中去 */
void Traffic::portCarsToPort() {
    for (auto c = cars.begin(); c != cars.end(); c++) {
        if (c->from_id) {
            auto the_cross_index = cross_id2index.find(c->from_id);
            if (the_cross_index != cross_id2index.end()) {
                crosses[the_cross_index->second].addCar(&*c);
            }
        }
    }
}

/**核心调度算法*/
/* 生成车辆的路径规划 */
void Traffic::getPathOfCar(Car *car) {
    struct CrossD {
        Cross *cross = NULL;  //路口信息
        CrossD *prior_crossd = NULL;//下一条优先的道路
        Road *road = NULL;  //当前的路
        double min_d = __DBL_MAX__;  //路口权重
        int t;
    };
    vector<CrossD> G(int(crosses.size()), CrossD());//初始化所有路口信息,用vector容器存储
// CrossD[crosses.size()] G;
    unordered_map<int, CrossD *> S;//字典,路口ID 和信息
    unordered_map<int, CrossD *> Q;
// 定义比较函数
    auto cmp = [Q](pair<const int, CrossD *> a, pair<const int, CrossD *> b) {
        return a.second->min_d < b.second->min_d;
    };//比较哪个的权重大

// 1. 初始化列表
    for (int i = 0; i < int(crosses.size()); i++) {
        G[i].cross = &crosses[i];//当前路口
        G[i].t = car->plan_time;//车辆的出发时间
        if (crosses[i].id == car->from_id) {
            G[i].min_d = 0;
        }
        Q[crosses[i].id] = &G[i];
    }

// 2. 开始处理
    int last_q_size = Q.size(); // 判断死锁
    while (!Q.empty()) {
        auto min_iter = min_element(Q.begin(), Q.end(), cmp);/*Q的权重最小值*/
        auto u = min_iter->second;
        Q.erase(min_iter->first);//删除该元素
        S[u->cross->id] = &*u;
        vector<Road *> adjs = getAdjRoadOfCross(u->cross);//获得路口的连通道路的ID
        // 遍历u的邻居结点，松弛对应的值
        for (auto it = adjs.begin(); it != adjs.end(); it++) {//四条道路遍历
            // 首先剔除掉已经在S中的结点
            // cout << u->cross->id << ' ' << (*it)->from_id << endl;
            if (S.find((*it)->from_id) != S.end() &&
                S.find((*it)->to_id) != S.end())
                continue;
            // cout << "re: " << u->cross->id << ' ' << (*it)->from_id << endl;
            // 然后剔除不支持逆行的路口，即出发点不是u，并且不是双通路
            if ((*it)->from_id != u->cross->id && !(*it)->is_duplex) continue;
            // 然后对剩下的路口进行计算
            int d_t = getTimeCostOf(car, *it, u->cross);/*路口时间长度*/
            double new_d = u->min_d + getWeightOfRange(u->t, u->t + d_t, (*it)->id, car);//路口权重
            // 找到该路口对应的目标节点
            int target_cross_id = (*it)->from_id != u->cross->id ?
                                  (*it)->from_id : (*it)->to_id;
            CrossD *v = Q[target_cross_id];
            // 如果新的权重值小于目标节点权重值，则更新对应权重
            if (new_d < v->min_d) {
                v->min_d = new_d;
                v->prior_crossd = &*u;
                v->road = *it;
                v->t = u->t + d_t;
            }

        }

        if (last_q_size == int(Q.size())) {
            throw runtime_error("路径生成失败！");
        };
        last_q_size = Q.size();
    }
// 3. 此时S中所有点的权重均已更新完毕，生成对应的路径
    vector<Road *> path;
    auto target_cross = S.find(car->to_id);
    if (target_cross == S.end()) throw runtime_error("路径生成错误！");
    CrossD *the_crossd = target_cross->second;
    while (the_crossd->prior_crossd != NULL) {
        path.push_back(the_crossd->road);
        the_crossd = the_crossd->prior_crossd;
    }
    reverse(path.begin(), path.end());
    car->path = path; // 更新车辆的路径

}

void Traffic::getAllCarPath() {
    /*按照车辆的速度*/
    sort(cars.begin(), cars.end(), [](Car &a, Car &b) {
        return a.max_speed > b.max_speed;
    });

    int i = 0;
    int d_t = 1;
    int index = 0;
    //改变时间
//    line++;
    //时间片
    int j = 1;
    int start = 1;//表示当前可供选择的最小时间起点
    int batch_size = ceil(cars.size() / epoch);
    vector<Car>::iterator it;
    for (it = cars.begin(); it != cars.end(); it++) {
        if (it->preset)
            continue;
        if (j > batch_size) {
            if (timeIndex[start] > batch_size)
                start++;
            j = start;
        }
        if (timeIndex[j] > batch_size)
            j++;
        it->plan_time = max(it->plan_time, j);
        timeIndex[j]++;
        j++;
    }
    //根据出发时间排序
    /*根据优先级来排序,时间和id来排序*/
    sort(cars.begin(), cars.end(), [](Car &a, Car &b) {
        cout<<"aid:"<<a.id<<endl;
        cout<<"bid:"<<b.id<<endl;
        return (
                ((a.priority > b.priority)) ||
                ((a.priority == b.priority) && (a.plan_time < b.plan_time)) ||
                ((a.priority == b.priority) && (a.plan_time == b.plan_time) && (a.id < b.id))
        );
    });
    while (no_dead_lock)//非死锁
    {
        total_time++;
        /*drive just current road()*/
//        for (auto it = cars.begin(); it != cars.end(); it++) {
//            getPathOfCar(&*it);
//        }
        //优先车辆速度排序


        for (it = cars.begin(); it != cars.end(); it++) {
            if (!it->preset) {
                it->plan_time += d_t;
                getPathOfCar(&*it);
//            cout << "update " << endl;
                updateWeightsByPath(&*it);
                cout << ++index << endl;
            }
        }
        cout << "check path" << endl;
        checkPath();
    }
}

/*获得车下一条路的方向*/
int Traffic::direction(Car &car) {
    vector<Road *>::iterator it = car.path.end();
    Road *current = *(it - 1);//当前路径
    Road *next = *it;
    //判断方向,也就是当前道路的终点是下一条道路的起点
    if (!current->is_duplex)
        return current->to_id;//返回的是当前终点
    if (!next->is_duplex)
        return next->from_id;//下一条路起点
    if (current->is_duplex && next->is_duplex)//两条路都是双向的
    {
        if (current->from_id == next->from_id || current->from_id == next->to_id)
            return current->from_id;
        else if (current->to_id == next->from_id || current->to_id == next->to_id)
            return current->to_id;

    }
    return -1;
}

/*驱动每辆车终止或者等待状态*/
void Traffic::driveCurrentRoad() {
    queue<Car *> new_queue;
    for (auto r:roads) {
        if (r.is_duplex)/*双向的*/
        {
            for (auto c:r.s2e_channels)/*进入正向车道 from_cross -> to_cross,c是队列了*/
            {
                while (!c.empty()) {
                    c.front()->move();//每辆车进行自己的move
                    new_queue.push(c.front());//临时queue作为桥梁进行辅助
                }
                while (!new_queue.empty()) {
                    c.push(new_queue.front());//c重新恢复回来
                    new_queue.pop();
                }
            }
            for (auto c:r.e2s_channels)/*进入正向车道 to_cross -> from_cross,c是队列了*/
            {
                while (!c.empty()) {
                    c.front()->move();//每辆车进行自己的move
                    new_queue.push(c.front());//临时queue作为桥梁进行辅助
                }
                while (!new_queue.empty()) {
                    c.push(new_queue.front());//c重新恢复回来
                    new_queue.pop();
                }
            }
        } else {//单向的车道的话就遍历一个就可以了
            for (auto c:r.s2e_channels)/*进入正向车道 from_cross -> to_cross,c是队列了*/
            {
                while (!c.empty()) {
                    c.front()->move();//每辆车进行自己的move
                    new_queue.push(c.front());//临时queue作为桥梁进行辅助
                }
                while (!new_queue.empty()) {
                    c.push(new_queue.front());//c重新恢复回来
                    new_queue.pop();
                }
            }
        }
    }
}

/*优先车辆先上路并且在对应的道路上行驶*/
void Traffic::driveCarInitList(int t, bool priority) {/*当前时间片*/
    if (priority) {
        for (auto p:priority_cars) {
            if (p.plan_time == t) {
                p.move();
            }
        }
    } else {
        for (auto p:cars) {
            if (p.plan_time == t && !p.priority)
                p.move();
        }
    }
}

/*创建优先队列*/
void Traffic::createCartSequence() {
    vector<Car *> priority_car;//创建临时变量
    for (auto r:roads) {
        if (r.is_duplex) {
            for (auto c:r.s2e_channels) {/*进入正向车道 from_cross -> to_cross,c是队列了*/
                if (c.front()->at_road_position == r.length)
                    priority_car.push_back(c.front());//加入
                sort(priority_car.begin(), priority_car.end(), Car::cmpAtRoad());//排序
                /*优先车辆入队列*/
                for (auto p:priority_car)
                    r.s2e_priority_queue.push(p);
            }
            for (auto c:r.e2s_channels) {/*进入正向车道 from_cross -> to_cross,c是队列了*/
                if (c.front()->at_road_position == r.length)
                    priority_car.push_back(c.front());//加入
                sort(priority_car.begin(), priority_car.end(), Car::cmpAtRoad());//排序
                /*优先车辆入队列*/
                for (auto p:priority_car)
                    r.e2s_priority_queue.push(p);
            }
        } else {
            for (auto c:r.s2e_channels) {/*进入正向车道 from_cross -> to_cross,c是队列了*/
                if (c.front()->at_road_position == r.length)
                    priority_car.push_back(c.front());//加入
                sort(priority_car.begin(), priority_car.end(), Car::cmpAtRoad());//排序
                /*优先车辆入队列*/
                for (auto p:priority_car)
                    r.s2e_priority_queue.push(p);
            }
        }
    }
}

bool Traffic::moveToNextRoad(Car &car)
{
    if (car.status == 3)/*到达终点*/
        return true;
    /*一般来说是path vector的倒数第二条*/
    int current_cross_id = direction(car);//获得下一条道路的起点
    //获得下一条路
    Road *nextroad = *car.path.end();
    Road *preroad = *(car.path.end()-1);

    /*判断是不是满了*/

    if (nextroad->from_id == current_cross_id)//判断方向 from -->to
    {
        //遍历每个channel
        int index = 0;
        for (auto c:nextroad->s2e_channels) {
            if (c.size() == nextroad->length ||
                c.back()->status == 2 && c.back()->at_road_position == 0)//如果满了或者车的状态都是终止状态
                index++;
            else if (c.size() < nextroad->length ||
                     c.back()->status == 2 && c.back()->at_road_position > 0)//道路没有满,并且最后一辆车的位置不止最后面,且处于种植状态
            {
                /*车辆进入新的车道*/
                int speed = min(car.max_speed, nextroad->max_speed);//进入下一车道的速度
                if (speed > car.could_move_distance) {
                    preroad->s2e_channels[car.at_channel_id].pop();
                    preroad->s2e_priority_queue.pop();//全部出队
                    car.changePosition(speed - car.could_move_distance);//更新位置
                    car.changeRoad(nextroad, index, 1);//进入新的道路,且是正向的
                    car.updateStatus(2);
                    c.push(&car);
                }
                return true;//退出
            } else if (c.size() < nextroad->length || c.back()->status == 1) {
                car.updateStatus(2);//结束
                return false;
            }


            if (index == nextroad->s2e_channels.size()) {
                return true;//如果车道整个塞满了
            }
        }
    } else if (nextroad->to_id == current_cross_id) {
        int index = 0;
        for (auto c:nextroad->e2s_channels) {
            if (c.size() == nextroad->length ||
                c.back()->status == 2 && c.back()->at_road_position == 0)//如果满了或者车的状态都是终止状态
                index++;
            else if (c.size() < nextroad->length ||
                     c.back()->status == 2 && c.back()->at_road_position > 0)//道路没有满,并且最后一辆车的位置不止最后面,且处于种植状态
            {
                /*车辆进入新的车道*/
                int speed = min(car.max_speed, nextroad->max_speed);//进入下一车道的速度
                if (speed > car.could_move_distance) {
                    preroad->e2s_channels[car.at_channel_id].pop();
                    preroad->e2s_priority_queue.pop();
                    car.changePosition(speed - car.could_move_distance);//更新位置
                    car.changeRoad(nextroad, index, -1);//进入新的道路,且是反向的
                    car.updateStatus(2);
                }
                return true;
            } else if (c.size() < nextroad->length || c.back()->status == 1) {//为等待状态
                car.updateStatus(2);//结束
                return false;
            }

        }
    }
    return true;
}


/* 设置某条路在某个时刻的权重 */
void Traffic::setWeightOf(int t, int road_id, double w, Car *car) {
    auto t_weights = id_time_weights.find(t);
    if (t_weights != id_time_weights.end()) {
        auto id_weights = t_weights->second.find(road_id);
        if (id_weights != t_weights->second.end()) {
            id_weights->second += w;
        }
    }
    id_time_weights[t][road_id] = w + getWeightOf(t, road_id, car);
//    cout << t << " " << road_id << " " << id_time_weights[t][road_id] << endl;
}

/*根据预置的车辆来设置道路权重*/
void Traffic::init_preset_road_weight() {
    vector<Pre_set_car>::iterator car = pre_set_cars.begin();
    for (; car != pre_set_cars.end(); car++) {
        if (car->path.empty())throw runtime_error("车辆路径为空,不合法!");
        int t = car->plan_time;

        int car_id = car->id;//确认carID;
        std::unordered_map<int, Car>::iterator car_it;
        car_it = car_ID_dict.find(car->id);//索引到固定的项
        vector<int> value;
        for (auto p:car->path) {
            unordered_map<int, Road>::iterator road_it;
            road_it = road_ID_dict.find(p);
            double time_cost = getTimeCostOf_pre(&(*car_it).second, &(*road_it).second);

            for (int i = 0; i < time_cost; i++) {
                t += 1;
                // 道路权重是时间开销/车道数量
                double w = time_cost / (*road_it).second.channels_num;
                setWeightOf(t, p, w, &(*car_it).second);
            }

        }
    }
}

/* 根据车辆路径更新权重 */
void Traffic::updateWeightsByPath(Car *car) {
    if (car->path.empty()) throw runtime_error("车辆的路径为空！");
    int t = car->plan_time; // 开始时间
//  cout<<car->priority<<endl;
    int time_cost;
    int next_cross_id;
    vector<Road *>::iterator p = car->path.begin();
    for (; p != car->path.end() - 1; p++) {/*p是road对象*/
        int next_cross_id;
        int current_road_id = (*p)->id;
//        cout << "road ID " << current_road_id << endl;
        int next_road_id = (*(p + 1))->id;//下一条路的ID
        /**确定路口*/

        if ((*(p + 1))->is_duplex && (*(p + 1))->is_duplex)/*如果两条路都是双向的*/{

            if ((*p)->to_id == (*(p + 1))->from_id || (*p)->from_id == (*(p + 1))->from_id)
                next_cross_id = (*(p + 1))->from_id;
            if ((*p)->to_id == (*(p + 1))->to_id || (*p)->from_id == (*(p + 1))->to_id)
                next_cross_id = (*(p + 1))->to_id;
        } else if ((*p)->is_duplex && (*(p + 1))->is_duplex == 0)

            next_cross_id = (*(p + 1))->from_id;
        else if ((*p)->is_duplex == 0)
            next_cross_id = (*p)->to_id;

//        cout << (*p)->id << endl;
//        cout << "下一条路的Id" << endl;
//        cout << (*(p + 1))->id << endl;
//        cout << "next corss id " << next_cross_id << endl;

        std::unordered_map<int, Cross>::iterator cross_it;
        cross_it = cross_ID_dict.find(next_cross_id);//索引下个节点的对象上

        time_cost = getTimeCostOf(car, *p, &(*cross_it).second);//获得这个路口的平均权重
        for (int i = 0; i < time_cost; i++) {
            t += 1;
            // 道路权重是时间开销/车道数量
            double w = time_cost / (*p)->channels_num;
            setWeightOf(t, (*p)->id, w, car);
        }
    }
    /*否则就直接计算最后一条路的权重*/
    time_cost = getTimeCostOf_pre(car, *p);
    for (int i = 0; i < time_cost; i++) {
        t += 1;
        // 道路权重是时间开销/车道数量
        double w = time_cost / (*p)->channels_num;
        setWeightOf(t, (*p)->id, w, car);
    }

}


/* 获取某条路在某个时间段的平均权重 */
double Traffic::getWeightOfRange(int from_time, int to_time, int road_id, Car *car) {
    double w = 0.0;
    int t = to_time - from_time + 1;
    while (from_time <= to_time) {
        w += getWeightOf(from_time, road_id, car);
        from_time++;
    }
    // return w;
    return t > 0 ? w / t : w;
}

/* 获取某条路在某个时刻的权重 */
double Traffic::getWeightOf(int t, int road_id, Car *car) {
    auto t_weights = id_time_weights.find(t);
    if (t_weights != id_time_weights.end()) {
        auto id_weights = t_weights->second.find(road_id);
        if (id_weights != t_weights->second.end()) {
            return id_weights->second;
        }
    }
    Road *r = getRoadById(road_id);
    return getTimeCostOf_pre(car, r);
}


/* 根据id获取路 */
Road *Traffic::getRoadById(int road_id) {
    auto index = road_id2index.find(road_id);
    if (index == road_id2index.end() ||
        index->second >= int(roads.size()))
        throw runtime_error("查询路出错！");
    return &roads.at(index->second);
}

/* 根据id获取车 */
Car *Traffic::getCarById(int car_id) {
    auto index = car_id2index.find(car_id);
    if (index == car_id2index.end() ||
        index->second >= int(cars.size())) {
        throw runtime_error("查询车辆出错！");
    };
    return &cars.at(index->second);
}

/* 根据id获取路口 */
Cross *Traffic::getCrossById(int cross_id) {
    auto index = cross_id2index.find(cross_id);
    if (index == cross_id2index.end() ||
        index->second >= int(crosses.size())) {
        throw runtime_error("查询路口出错！");
    }
    return &crosses.at(index->second);
}

int Traffic::getSpeedOf(Car *car, Road *road) {
    return min(car->max_speed, road->max_speed);
}

int Traffic::getTimeCostOf_pre(Car *car, Road *road) {
    int speed = getSpeedOf(car, road);
    return ceil(double(road->length) / double(speed));
}

int Traffic::getTimeCostOf(Car *car, Road *road, Cross *cross) {
    int speed = getSpeedOf(car, road);
    //对着这个路口的下一个路口的四条路进行预测//
    int t = car->current_time;//当前时间片
    //判断起点
    int next_cross_id;
    int current_road_id = road->id;

    if (road->from_id == cross->id)
        next_cross_id = road->to_id;//下一个节点,要研究的就是下一个节点
    else
        next_cross_id = road->from_id;

    std::unordered_map<int, Cross>::iterator cross_it;
    cross_it = cross_ID_dict.find(next_cross_id);//索引下歌节点的对象上
    vector<int> next_cross_average_weight;

    if ((*cross_it).second.top_road_id != -1 && (*cross_it).second.top_road_id != current_road_id)
        next_cross_average_weight.push_back(id_time_weights[t][(*cross_it).second.top_road_id]);
    if ((*cross_it).second.right_road_id != -1 && (*cross_it).second.right_road_id != current_road_id)
        next_cross_average_weight.push_back(id_time_weights[t][(*cross_it).second.right_road_id]);
    if ((*cross_it).second.bottom_road_id != -1 && (*cross_it).second.bottom_road_id != current_road_id)
        next_cross_average_weight.push_back(id_time_weights[t][(*cross_it).second.bottom_road_id]);
    if ((*cross_it).second.left_road_id != -1 && (*cross_it).second.left_road_id != current_road_id)
        next_cross_average_weight.push_back(id_time_weights[t][(*cross_it).second.left_road_id]);

    double average;
    if (next_cross_average_weight.size())
        average = std::accumulate(std::begin(next_cross_average_weight),
                                  std::end(next_cross_average_weight), 0.0) / next_cross_average_weight.size();
    double cost = double(average / road->channels_num) * c1 + ((road->length) / double(speed)) * c2;
//        cout<<"更新后的"<<t<<" "<<road->id<<" "<<cost<<endl;
    return ceil(cost);//调参
}

/*通过路口ID获得可以行走的路的ID*/
vector<Road *> Traffic::getAdjRoadOfCross(Cross *cross) {
    vector<Road *> result;
    int ids[] = {
            cross->top_road_id,
            cross->right_road_id,
            cross->bottom_road_id,
            cross->left_road_id
    };
    for (int i = 0; i < 4; ++i) {
        if (ids[i] > -1) {
            result.push_back(getRoadById(ids[i]));
        }
    }
    return result;
}

vector<Car> Traffic::get_priority_or_unpriority_car(vector<Car> cars, bool priority) {
    vector<Car> priority_cars;
    for (auto p:cars)
        if (p.priority == priority)
            priority_cars.push_back(p);
    return priority_cars;
}

vector<string> Traffic::path2string() {
    vector<string> result;
    for (auto it = cars.begin(); it != cars.end(); it++) {
        if (!it->preset) {
//        cout<<it->id<<" no preset"<<endl;
            stringstream tmp;
            tmp << "(" << it->id << ", " << it->plan_time << ", ";
            int N = it->path.size();
            for (auto r:it->path) {
                tmp << r->id;
                if (r->id != it->path[N - 1]->id) {
                    tmp << ", ";
                }
            }
            tmp << ")";
            result.push_back(tmp.str());
        }
    }
    return result;
}


void Traffic::checkPath() {
    for (auto it:cars) {
        auto last_road = it.path[it.path.size() - 1];
        if (last_road->to_id != it.to_id && last_road->from_id != it.to_id) {
            throw runtime_error("路径生成失败！");
        }
    }
}

/*初始化函数,读入文件的出路,不需要管*/
template<class TrafficInstance>
vector<TrafficInstance> Traffic::initInstance(string file_path) {
    vector<TrafficInstance> instances;

    ifstream the_file;
    the_file.open(file_path);

    if (!the_file.is_open()) return instances;

    string temp_string;
    while (getline(the_file, temp_string)) {
        // 跳过注释行
        if (temp_string[0] == '#') continue;

        // 过滤掉不需要的字符
        string filtered_string;
//        for (auto s:temp_string) {
//            if (s != '(' && s != ')' && s != ',') {
//                filtered_string += s;
//            }
//        }
        temp_string.erase(temp_string.length() - 1, 1);
        temp_string.erase(0, 1);

        vector<string> result;

        int i = 0;
        split(temp_string, ",", result);
        for (auto s:result) {
            if (++i == 2 || i == 3) {
                filtered_string += " " + s;
            } else
                filtered_string += s;
        }


        // 生成类实例
        instances.push_back(TrafficInstance(filtered_string));
    }

    the_file.close();
    return instances;
}

template<typename T>
unordered_map<int, int> Traffic::buildMapIndex(vector<T> &vs, bool sorted) {
    if (sorted)
        sort(vs.begin(), vs.end(), [](T &a, T &b) {
            return a.id < b.id;
        });

    unordered_map<int, int> tmp_id2index;
    for (int i = 0; i < int(vs.size()); i++) {
        tmp_id2index[vs[i].id] = i;
    }
    return tmp_id2index;
}

void Traffic::update_preset_car(vector<Car> &car) {
    std::unordered_map<int, Car>::iterator car_map_it;
    std::unordered_map<int, Road>::iterator road_map_it;
    std::vector<Pre_set_car>::iterator pre_set_car_it;
    std::vector<int>::iterator road_it;
    for (pre_set_car_it = pre_set_cars.begin(); pre_set_car_it != pre_set_cars.end(); pre_set_car_it++) {
        int car_id = (*pre_set_car_it).id;
        int index = car_id2index[car_id];
        car[index].plan_time = (*pre_set_car_it).plan_time;
        for (road_it = (*pre_set_car_it).path.begin(); road_it != (*pre_set_car_it).path.end(); road_it++) {
            road_map_it = road_ID_dict.find(*road_it);
            car[index].path.push_back(&((*road_map_it).second));
        }

    }
}
