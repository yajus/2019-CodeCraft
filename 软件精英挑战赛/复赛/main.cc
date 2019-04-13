#include <iostream>
#include "method.h"

int main(int argc, char const *argv[]) {
//  if(argc < 2){
//    std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
//    exit(1);
//  }

//  std::string car_path(argv[1]);
//  std::string road_path(argv[2]);
//  std::string cross_path(argv[3]);
//  std::string answer_path(argv[4]);
  std::string car_path("/Users/apple/Desktop/软件精英挑战赛/复赛/2-map-training-1/car.txt");
  std::string road_path("/Users/apple/Desktop/软件精英挑战赛/复赛/2-map-training-1/road.txt");
  std::string cross_path("/Users/apple/Desktop/软件精英挑战赛/复赛/2-map-training-1/cross.txt");
  std::string pre_set_car("/Users/apple/Desktop/软件精英挑战赛/复赛/2-map-training-1/presetAnswer.txt");
  std::string answer_path("/Users/apple/Desktop/软件精英挑战赛/复赛/2-map-training-1/answer.txt");
  auto test = Traffic();
  test.initTraffic(car_path, road_path, cross_path,pre_set_car);

  test.getAllCarPath();

  ofstream out_file;
  out_file.open(answer_path, ios::out | ios::trunc);
  vector<string> paths = test.path2string();
  for (auto s:paths) {
    out_file << s << endl;
  }

  return 0;
}
