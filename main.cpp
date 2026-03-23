#include "mbed.h"
#include "mbed2/299/drivers/LocalFileSystem.h"
//Later change pins to right settings
DigitalOut Stepper_Motor_1_Speed(p5);
DigitalOut Stepper_Motor_1_Direction(p6);
DigitalOut Stepper_Motor_2_Speed(p7);
DigitalOut Stepper_Motor_2_Direction(p8);
DigitalIn US_Sensor_Echo(p9);
DigitalOut US_Sensor_Trig(p10);
DigitalIn IR_SensorsL1(p11);
DigitalIn IR_SensorsL2(p12);
DigitalIn IR_SensorsL3(p13);
DigitalIn IR_SensorsR1(p14);
DigitalIn IR_SensorsR2(p15);
DigitalIn IR_SensorsR3(p16);


Timer t;
void check_movement(){
   //void check_movement(){
    // Read all 6 IR sensors into a bitmask [L1,L2,L3,R1,R2,R3]
   // uint8_t sensors = (IR_SensorsL1 << 5) |
   //                   (IR_SensorsL2 << 4) |
   //                   (IR_SensorsL3 << 3) |
   //                   (IR_SensorsR1 << 2) |
   //                   (IR_SensorsR2 << 1) |
   //                   (IR_SensorsR3);

    //bool left_wall  = (sensors >> 3) & 0x07;  // any of L1, L2, L3
    //bool right_wall =  sensors       & 0x07;  // any of R1, R2, R3

    //if (left_wall && !right_wall) {
      //  turn_right();               // Too close to left wall
    //} else if (!left_wall && right_wall) {
      //  turn_left(Last_Choice);     // Too close to right wall
    //} else {
      //  forwards();                 // Centred — go straight
    //}
//}
        t.start();
        t.stop();
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
void maze_location(bool Last_Choice,int Distance_From_End){
    if (Last_Choice == false){
        Distance_From_End = Distance_From_End - 1;
    }
    Last_Choice = false;
}

volatile int us_distance_mm = 0;

void check_us_distance(bool ECHO){
    uint32_t echo_time = 0; //supposed to have values range from 0 - 49000
    
    //Trigger pulse function 
    US_Sensor_Trig = 0;
    wait_us(2);
    US_Sensor_Trig = 1;
    wait_us(10);
    US_Sensor_Trig = 0;
    //Wait for echo high
    while(!ECHO);
    t.reset();
    t.start();

    //Wait for echo low
    while(ECHO){
    t.stop();
    }
    echo_time = t.read_us();

    //Speed of sound = 0.343 mm/us
    us_distance_mm = (echo_time*343)/2000;
    if(us_distance_mm < 80){
    t.stop();
}
}


int main(){
    bool Last_Choice = false;
    bool ECHO = false;
    int No_of_Consecutive_Steps = 0;
    int Distance_from_End = 8;
    while (1){
        if (Distance_from_End > 0) {
            check_us_distance(true);
            check_movement();
            maze_location(Last_Choice, Distance_from_End);
        }
        else {
        t.reset();
        }
    }
}

