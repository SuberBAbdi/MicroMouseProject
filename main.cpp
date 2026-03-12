#include "mbed.h"
//busin best choice need to discuss combination
DigitalOut Stepper_Motor_1_Speed(p5);
DigitalOut Stepper_Motor_1_Direction(p6);
DigitalOut Stepper_Motor_2_Speed(p7);
DigitalOut Stepper_Motor_2_Direction(p8);
DigitalIn US_Sensor_Echo(p9);
DigitalOut US_Sensor_Trig(p9);
BusIn IR_Sensors(p10,p11,p12,p13,p14,p15,p16);

void check_US_sensor_distance(){}
void forwards(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 1;
}
void turn_left(){
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 1;
}
void turn_right(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
}
void backwards(){
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 0;
}
void maze_location(){}
void flood_fill(){
    switch(IR_Sensors) {
            case 0x1: forward(); break;
            case 0x2: backwards(); break;
            case 0x3: turn_right(); break;
            case 0x4: turn_left(); break;
        }
}

int main(){
    bool Last_Choice = false;
    int No_of_Consecutive_Steps = 0;
    int Distance_from_End = 8;
    while (1){
        if (Distance_from_End > 0) {
            check_US_sensor_distance();
            flood_fill();
            if (Last_Choice == false){
                int Distance_From_End = Distance_From_End - 1;
            }
        }
    }
}