#include <iostream>
#include <termios.h>
#include <chrono>
#include <thread>
#include <gpiod.h>
#include <unistd.h>
#include <map>

using namespace std;

// 定义GPIO引脚
const int MOTOR_A1_1 = 216;  // 电机A1控制引脚7 GPIO09
const int MOTOR_A1_2 = 50;  // 电机A1控制引脚11
const int MOTOR_A2_1 = 79;  // 电机A2控制引脚12
const int MOTOR_A2_2 = 14;  // 电机A2控制引脚13
const int MOTOR_B1_1 = 194;  // 电机B1控制引脚15
const int MOTOR_B1_2 = 232;  // 电机B1控制引脚16
const int MOTOR_B2_1 = 15;  // 电机B2控制引脚18
const int MOTOR_B2_2 = 16;  // 电机B2控制引脚19
// const int SERVO_PIN = 27; // 舵机控制引脚

// 初始化GPIO
gpiod_chip* chip;
gpiod_line* motorA1_1;
gpiod_line* motorA1_2;
gpiod_line* motorA2_1;
gpiod_line* motorA2_2;
gpiod_line* motorB1_1;
gpiod_line* motorB1_2;
gpiod_line* motorB2_1;
gpiod_line* motorB2_2;
// gpiod_line* servo;

int _getch() {
    struct termios oldt, newt;
    int ch;
    
    // 获取当前终端设置
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // 修改设置：禁用行缓冲和回显
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;   // 最小读取1个字符
    newt.c_cc[VTIME] = 0;  // 无超时
    
    // 应用新设置
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // 读取字符
    ch = getchar();
    
    // 恢复原始终端设置
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return ch;
}

void setup() {
    // 打开GPIO芯片
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        exit(1);
    }

    // 获取GPIO线
    motorA1_1 = gpiod_chip_get_line(chip, MOTOR_A1_1);
    motorA1_2 = gpiod_chip_get_line(chip, MOTOR_A1_2);
    motorA2_1 = gpiod_chip_get_line(chip, MOTOR_A2_1);
    motorA2_2 = gpiod_chip_get_line(chip, MOTOR_A2_2);
    motorB1_1 = gpiod_chip_get_line(chip, MOTOR_B1_1);
    motorB1_2 = gpiod_chip_get_line(chip, MOTOR_B1_2);
    motorB2_1 = gpiod_chip_get_line(chip, MOTOR_B2_1);
    motorB2_2 = gpiod_chip_get_line(chip, MOTOR_B2_2);
   //  servo = gpiod_chip_get_line(chip, SERVO_PIN);

    // 设置GPIO线为输出
    gpiod_line_request_output(motorA1_1, "motorA1_1", 0);
    gpiod_line_request_output(motorA1_2, "motorA1_2", 0);
    gpiod_line_request_output(motorA2_1, "motorA2_1", 0);
    gpiod_line_request_output(motorA2_2, "motorA2_2", 0);
    gpiod_line_request_output(motorB1_1, "motorB1_1", 0);
    gpiod_line_request_output(motorB1_2, "motorB1_2", 0);
    gpiod_line_request_output(motorB2_1, "motorB2_1", 0);
    gpiod_line_request_output(motorB2_2, "motorB2_2", 0);
    // gpiod_line_request_output(servo, "servo", 0);
}

/*
 * 10 is run 01 is reverse
 */
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
/*
void setServoAngle(int angle) {
    // 简单的PWM控制舵机角度
    // 这里假设舵机的角度范围是0到180度
    int pulseWidth = 500 + (angle * 2000 / 180);
    gpiod_line_set_value(servo, 1);
    std::this_thread::sleep_for(std::chrono::microseconds(pulseWidth));
    gpiod_line_set_value(servo, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(20 - pulseWidth / 1000));
}
*/

void cleanup() {
    // 释放GPIO线
    gpiod_line_release(motorA1_1);
    gpiod_line_release(motorA1_2);
    gpiod_line_release(motorA2_1);
    gpiod_line_release(motorA2_2);
    gpiod_line_release(motorB1_1);
    gpiod_line_release(motorB1_2);
    gpiod_line_release(motorB2_1);
    gpiod_line_release(motorB2_2);
    // gpiod_line_release(servo);

    // 关闭GPIO芯片
    gpiod_chip_close(chip);
}

int main() {
    setup();
    char input = 0;
    map<char, gpiod_line*> input_map;
    input_map['q'] = motorA1_1;
    input_map['w'] = motorA2_1;
    input_map['e'] = motorB1_1;
    input_map['r'] = motorB2_1;
    input_map[(char)motorA1_1] = motorA1_2;
    input_map[(char)motorA2_1] = motorA2_2;
    input_map[(char)motorB1_1] = motorB1_2;
    input_map[(char)motorB2_1] = motorB2_2;
    while(1) {
        input = _getch();
        
        setMotorSpeed(input_map[input], input_map[(char)input_map[input]], 1);  

        std::this_thread::sleep_for(std::chrono::seconds(1));

        setMotorSpeed(input_map[input], input_map[(char)input_map[input]], 0);  
    }

    // 控制小车前进
 //   setMotorSpeed(motorA1_1, motorA1_2, 1);  // 电机A1正转
 //   setMotorSpeed(motorA2_1, motorA2_2, 1);  // 电机A2正转
 //   setMotorSpeed(motorB1_1, motorB1_2, 1);  // 电机B1正转
 //   setMotorSpeed(motorB2_1, motorB2_2, 1);  // 电机B2正转
 //   std::this_thread::sleep_for(std::chrono::seconds(10));

    // 控制小车后退
 //   setMotorSpeed(motorA1_1, motorA1_2, -1);  // 电机A1逆转
 //   setMotorSpeed(motorA2_1, motorA2_2, -1);  // 电机A2逆转
 //   setMotorSpeed(motorB1_1, motorB1_2, -1);  // 电机B1逆转
 //   setMotorSpeed(motorB2_1, motorB2_2, -1);  // 电机B2逆转
    std::this_thread::sleep_for(std::chrono::seconds(10));
 //
 //   // 控制小车停止
 //   setMotorSpeed(motorA1_1, motorA1_2, 0);  // 电机A1停止
 //   setMotorSpeed(motorA2_1, motorA2_2, 0);  // 电机A2停止
 //   setMotorSpeed(motorB1_1, motorB1_2, 0);  // 电机B1停止
 //   setMotorSpeed(motorB2_1, motorB2_2, 0);  // 电机B2停止

    /*
    // 控制舵机角度
    setServoAngle(90);  // 舵机转到90度
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setServoAngle(0);   // 舵机转到0度
    std::this_thread::sleep_for(std::chrono::seconds(1));
    setServoAngle(180); // 舵机转到180度
    std::this_thread::sleep_for(std::chrono::seconds(1));
*/
    cleanup();
    return 0;
}
