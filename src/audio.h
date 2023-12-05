#pragma once
#include <Eigen/Dense>
class Audio {
public:
    Audio(int channel_num);
    ~Audio();
private:
    int channel_num;
    Eigen::VectorXf* channels;
};