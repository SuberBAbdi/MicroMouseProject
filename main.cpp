#include "mbed.h"
DigitalOut Stepper_Motor_1_Speed(p4);
DigitalOut Stepper_Motor_1_Direction(p6);
DigitalOut Stepper_Motor_2_Speed(p7);
DigitalOut Stepper_Motor_2_Direction(p8);
DigitalIn US_Sensor_Echo(p9);
DigitalOut US_Sensor(p9);
AnalogIn IR_Sensor_1(p10);
AnalogIn IR_Sensor_2(p11);
AnalogIn IR_Sensor_3(p12);
AnalogIn IR_Sensor_4(p13);
AnalogIn IR_Sensor_5(p14);
AnalogIn IR_Sensor_6(p15);
AnalogIn IR_Sensor_7(p16);




int main(){
    int Last_Choice = 1;
    int No_of_Consecutive_Steps = 0;
    int Distance_from_End = 8;
    while (1){
        if (Distance_from_End > 0) {
            wait (Delay);
            void check_direction_choices();
            if (Last_Choice = 1){
                int Distance_From_End = Distance_From_End - 1;
            }
        }
    }
}
void check_direction_choices(){
    if (IR_Sensor)
}
void forwards(){}
void turn_left(){}
void turn_right(){}
void backwards(){}
void flood_fill(){}
