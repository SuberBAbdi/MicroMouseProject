#include "mbed.h"
//busin best choice need to discuss combination,       pins are temp change later
DigitalOut Stepper_Motor_1_Speed(p5);
DigitalOut Stepper_Motor_1_Direction(p6);
DigitalOut Stepper_Motor_2_Speed(p7);
DigitalOut Stepper_Motor_2_Direction(p8);
DigitalIn US_Sensor_Echo(p9);
DigitalOut US_Sensor_Trig(p10);
BusIn IR_Sensors(p11,p12,p13,p14,p15,p16,p17);

Timer t;
void check_US_sensor_distance(    int Distance_from_Front_Wall;){
    t.reset();
    //add US distance code here
    US_Sensor_Trig = 1;
    wait_us(10000);
    US_Sensor_Trig = 0;
    t.start();
    while (US_Sensor_Echo == 0){}
    t.stop();
    //add calcs here
    if (Distance_from_Front_Wall <= 100){
        pause_on_spot();

    }
}
//ask about speed an whatnot on monday
void forwards(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 1;
}
void turn_left(bool Last_Choice){
    Last_Choice = true;
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 1;
    void
}
void turn_right(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
}
void backwards(bool Last_Choice){
    Last_Choice = true;
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 0;
}
void pause_on_spot(){
    wait_us(10000);
    check_movement();
}
void maze_location(bool Last_Choice){
    if (Last_Choice == false){
        int Distance_From_End = Distance_From_End - 1;
    }
    Last_Choice = false;
}
void check_movement(){
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
    int Distance_from_Front_Wall;
    while (1){
        if (Distance_from_End > 0) {
            check_US_sensor_distance();
            check_movement();
            maze_location();
        }
        else {
        //reset
        }
    }
}