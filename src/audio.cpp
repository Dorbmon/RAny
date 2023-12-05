#include "audio.h"

Audio::Audio(int channel_num) {
    this->channel_num = channel_num;
    this->channels = new Eigen::VectorXf[channel_num];
}

Audio::~Audio() {
    delete[] this->channels;
}