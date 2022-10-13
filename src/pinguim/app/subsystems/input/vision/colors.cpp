#include "colors.hpp"

Colors::Colors() {
    this->robotMin = {cv::Scalar(0), cv::Scalar(0), cv::Scalar(0)};
    this->robotMax = {cv::Scalar(179, 255, 255), cv::Scalar(179, 255, 255), cv::Scalar(179, 255, 255)};

    this->allyHSVMin = cv::Scalar(0);
    this->allyHSVMax = cv::Scalar(179, 255, 255);

    this->enemyHSVMin = cv::Scalar(0);
    this->enemyHSVMax = cv::Scalar(179, 255, 255);

    this->ballHSVMin = cv::Scalar(0, 147, 0);
    this->ballHSVMax = cv::Scalar(23, 255, 255);

}

/* Getters */
cv::Scalar Colors::getAllyMin() {
    return this->allyHSVMin;
}

cv::Scalar Colors::getAllyMax() {
    return this->allyHSVMax;
}

cv::Scalar Colors::getRobotColorMin(int robot_id) {
    return this->robotMin[robot_id];
}

cv::Scalar Colors::getRobotColorMax(int robot_id) {
    return this->robotMax[robot_id];
}

cv::Scalar Colors::getEnemyMin() {
    return this->enemyHSVMin;
}

cv::Scalar Colors::getEnemyMax() {
    return this->enemyHSVMax;
}

cv::Scalar Colors::getBallMin() {
    return this->ballHSVMin;
}

cv::Scalar Colors::getBallMax() {
    return this->ballHSVMax;
}

/* Setters */
void Colors::setAllyMin(cv::Scalar hsv) {
    this->allyHSVMin = hsv;
}

void Colors::setAllyMax(cv::Scalar hsv) {
    this->allyHSVMax = hsv;
}

void Colors::setRobotColorMin(cv::Scalar hsv, int robot_id) {
    this->robotMin[robot_id] = hsv;
}

void Colors::setRobotColorMax(cv::Scalar hsv, int robot_id) {
    this->robotMax[robot_id] = hsv;
}

void Colors::setEnemyMin(cv::Scalar hsv) {
    this->enemyHSVMin = hsv;
}

void Colors::setEnemyMax(cv::Scalar hsv) {
    this->enemyHSVMax = hsv;
}

void Colors::setBallMin(cv::Scalar hsv) {
    this->ballHSVMin = hsv;
}

void Colors::setBallMax(cv::Scalar hsv) {
    this->ballHSVMax = hsv;
}
