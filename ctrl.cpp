#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.h>
#include <unistd.h>

using namespace std;

// 定义GPIO引脚
const int MOTOR_A1 = 17;  // 电机A1控制引脚
const int MOTOR_A2 = 18;  // 电机A2控制引脚
const int MOTOR_B1 = 27;  // 电机B1控制引脚
const int MOTOR_B2 = 22;  // 电机B2控制引脚
const int SERVO_PIN = 23; // 舵机控制引脚

// 初始化GPIO
gpiod_chip* chip;
gpiod_line* motorA1;
gpiod_line* motorA2;
gpiod_line* motorB1;
gpiod_line* motorB2;
gpiod_line* servo;

void setup() {
    // 打开GPIO芯片
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        exit(1);
    }

    // 获取GPIO线
    motorA1 = gpiod_chip_get_line(chip, MOTOR_A1);
    motorA2 = gpiod_chip_get_line(chip, MOTOR_A2);
    motorB1 = gpiod_chip_get_line(chip, MOTOR_B1);
    motorB2 = gpiod_chip_get_line(chip, MOTOR_B2);
    servo = gpiod_chip_get_line(chip, SERVO_PIN);

    // 设置GPIO线为输出
    gpiod_line_request_output(motorA1, "motorA1", 0);
    gpiod_line_request_output(motorA2, "motorA2", 0);
    gpiod_line_request_output(motorB1, "motorB1", 0);
    gpiod_line_request_output(motorB2, "motorB2", 0);
    gpiod_line_request_output(servo, "servo", 0);
}

void setMotorSpeed(gpiod_line* motor1, gpiod_line* motor2, int speed) {
    if (speed > 0) {
        gpiod_line_set_value(motor1, 1);
        gpiod_line_set_value(motor2, 0);
    } else if (speed < 0) {
        gpiod_line_set_value(motor1, 0);
        gpiod_line_set_value(motor2, 1);
    } else {
        gpiod_line_set_value(motor1, 0);
        gpiod_line_set_value(motor2, 0);
    }
}

void setServoAngle(int angle) {
    // 简单的PWM控制舵机角度
    // 这里假设舵机的角度范围是0到180度
    int pulseWidth = 500 + (angle * 2000 / 180);
    gpiod_line_set_value(servo, 1);
    std::this_thread::sleep_for(std::chrono::microseconds(pulseWidth));
    gpiod_line_set_value(servo, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(20 - pulseWidth / 1000));
}

void cleanup() {
    // 释放GPIO线
    gpiod_line_release(motorA1);
    gpiod_line_release(motorA2);
    gpiod_line_release(motorB1);
    gpiod_line_release(motorB2);
    gpiod_line_release(servo);

    // 关闭GPIO芯片
    gpiod_chip_close(chip);
}

int main() {
    setup();

    // 控制小车前进
    setMotorSpeed(motorA1, motorA2, 1);  // 电机A正转
    setMotorSpeed(motorB1, motorB2, 1);  // 电机B正转
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 控制小车后退
    setMotorSpeed(motorA1, motorA2, -1); // 电机A反转
    setMotorSpeed(motorB1, motorB2, -1); // 电机B反转
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 控制小车停止
    setMotorSpeed(motorA1, motorA2, 0);  // 电机A停止
    setMotorSpeed(motorB1, motorB2, 0);  // 电机B停止

    // 控制舵机角度
    setServoAngle(90);  // 舵机转到90度
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setServoAngle(0);   // 舵机转到0度
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setServoAngle(180); // 舵机转到180度
    std::this_thread::sleep_for(std::chrono::seconds(1));

    cleanup();
    return 0;
}
