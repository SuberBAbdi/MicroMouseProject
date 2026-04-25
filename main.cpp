#include "mbed.h"
//Later change pins to right settings
DigitalOut Stepper_Motor_1_Step(p21); // Left Motor
DigitalOut Stepper_Motor_1_Direction(p22); // Left Motor
DigitalOut Stepper_Motor_2_Step(p23); // Right Motor
DigitalOut Stepper_Motor_2_Direction(p24); // Right Motor
DigitalIn US_Sensor_Echo(p12); // Ultrasonic Sensor
DigitalOut US_Sensor_Trig(p13); // Ultrasonic Sensor
BusIn Left_Sensors (p8,p7,p6); // Left Bus Sensor
BusIn Right_Sensors (p11,p10,p9); // Right Bus Sensor

Timer t;

volatile int us_distance_mm = 0;

// North = 0 ,East = 1, South = 2, West = 3
int number_of_maze_walls[8][8];
int mouse_position_x = 0; // location of the mouse
int mouse_position_y = 0; // location of the mouse

int number_of_maze_cells[8][8];
int orientation = 0; // direction of the mouse, assume starting North always


void share_walls(int x, int y); // forward declaration

double check_us_distance(){
    uint32_t echo_time = 0; //supposed to have values range from 0 - 49000

    //Trigger pulse function 
    US_Sensor_Trig = 0;
    wait_us(2);
    US_Sensor_Trig = 1;
    wait_us(10);
    US_Sensor_Trig = 0;
    //Wait for echo high
    while(!US_Sensor_Echo){};
    t.reset();
    t.start();

    //Wait for echo low
    while(US_Sensor_Echo){
    }
    t.stop(); 

    echo_time = t.elapsed_time().count();

    //Speed of sound = 0.343 mm/us
    us_distance_mm = (echo_time*343)/2000;
    if(us_distance_mm < 80){
        t.stop();
    }
    return (double)us_distance_mm;
}

int check_left_sensors(){
    int left_wall_value = Left_Sensors.read();
    if (left_wall_value == 7){
        return 1;
    }
    // can add elseif for alignment issues but need hardware to test
    else {
        return 0;
    }
}

int check_right_sensors(){
    int right_wall_value = Right_Sensors.read();
    if (right_wall_value == 7){
        return 1;
    }
    // can add elseif for alignment issues but need hardware to test
    else {
        return 0;
    }
}

bool wall_in_front(){
    return check_us_distance() < 80;
}
bool wall_to_left(){
    return check_left_sensors() == 1;
}
bool wall_to_right(){
    return check_right_sensors() == 1;
}

void update_walls(int orientation, bool front, bool left, bool right){
    if (orientation == 0){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Mark North Wall, Front is North (1)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Mark West Wall, Left is West (8)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Mark East Wall, Right is East (2)
    }
    else if (orientation == 1){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Front is East (2)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Left is North (1)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Right is South (4)
    }
    else if (orientation == 2){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Front is South (4)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Left is East (2)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Right is West (8)
    }
    else if (orientation == 3){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Front is West (8)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Left is South (4)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Right is North (1)
    }
    share_walls(mouse_position_x, mouse_position_y); //share walls with neighbours
}

void share_walls(int x, int y){
    // If this cell has a North wall, the cell above gets a South wall
    if((number_of_maze_walls[x][y] & 1) && y < 7)
        number_of_maze_walls[x][y+1] |= 4;
    // If this cell has an East wall, the cell to the right gets a West wall
    if((number_of_maze_walls[x][y] & 2) && x < 7)
        number_of_maze_walls[x+1][y] |= 8;
    // If this cell has a South wall, the cell below gets a North wall
    if((number_of_maze_walls[x][y] & 4) && y > 0)
        number_of_maze_walls[x][y-1] |= 1;
    // If this cell has a West wall, the cell to the left gets an East wall
    if((number_of_maze_walls[x][y] & 8) && x > 0)
        number_of_maze_walls[x-1][y] |= 2;
}

void setup_perimeter_walls(){
    for(int i = 0; i < 8; i++){
        number_of_maze_walls[i][0] |= 4;  // South wall on entire bottom row
        number_of_maze_walls[i][7] |= 1;  // North wall on entire top row
        number_of_maze_walls[0][i] |= 8;  // West wall on entire left column
        number_of_maze_walls[7][i] |= 2;  // East wall on entire right column
    }
}

void flood_fill_setup(){  
    // Initialise all cells to 0
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            number_of_maze_cells[x][y] = 0;
        }
    }
    setup_perimeter_walls(); //load outer walls before BFS runs so it doesnt go past outer walls
    
    bool visited[8][8] = {};
    // Stores cells in queue
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;

    number_of_maze_cells[7][7] = 0;
    visited[7][7] = true;
    queue_x[tail] = 7; queue_y[tail] = 7; tail++;

    // Process till empty 
    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;
       
        int next_val = number_of_maze_cells[cx][cy] + 1;

        // Check North neighbour, if not visited assign Lee number and add to queue
        if(cy < 7 && !visited[cx][cy+1]){
            number_of_maze_cells[cx][cy+1] = next_val;
            visited[cx][cy+1] = true;
            queue_x[tail] = cx; queue_y[tail] = cy+1; tail++;
        }
        // Check East neighbour
        if(cx < 7 && !visited[cx+1][cy]){
            number_of_maze_cells[cx+1][cy] = next_val;
            visited[cx+1][cy] = true;
            queue_x[tail] = cx+1; queue_y[tail] = cy; tail++;
        }
        // Check South
        if(cy > 0 && !visited[cx][cy-1]){
            number_of_maze_cells[cx][cy-1] = next_val;
            visited[cx][cy-1] = true;
            queue_x[tail] = cx; queue_y[tail] = cy-1; tail++;
        }
        // Check West
        if(cx > 0 && !visited[cx-1][cy]){
            number_of_maze_cells[cx-1][cy] = next_val;
            visited[cx-1][cy] = true;
            queue_x[tail] = cx-1; queue_y[tail] = cy; tail++;
        }
    }
}

void flood_fill_update(){
    // Reset all cells, walls will block propagation as they are discovered
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            number_of_maze_cells[x][y] = 999;
        }
    }

    // Set goal cell [7][7] to 0 and add to queue
    number_of_maze_cells[7][7] = 0;
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;
    queue_x[tail] = 7; queue_y[tail] = 7; tail++;

    // Process queue until empty
    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;

        int next_val = number_of_maze_cells[cx][cy] + 1;

        // Check North - skip if north wall exists
        if(cy < 7 && !(number_of_maze_walls[cx][cy] & 1) && number_of_maze_cells[cx][cy+1] == 999){
            number_of_maze_cells[cx][cy+1] = next_val;
            queue_x[tail] = cx; queue_y[tail] = cy+1; tail++;
        }
        // Check East 
        if(cx < 7 && !(number_of_maze_walls[cx][cy] & 2) && number_of_maze_cells[cx+1][cy] == 999){
            number_of_maze_cells[cx+1][cy] = next_val;
            queue_x[tail] = cx+1; queue_y[tail] = cy; tail++;
        }
        // Check South 
        if(cy > 0 && !(number_of_maze_walls[cx][cy] & 4) && number_of_maze_cells[cx][cy-1] == 999){
            number_of_maze_cells[cx][cy-1] = next_val;
            queue_x[tail] = cx; queue_y[tail] = cy-1; tail++;
        }
        // Check West
        if(cx > 0 && !(number_of_maze_walls[cx][cy] & 8) && number_of_maze_cells[cx-1][cy] == 999){
            number_of_maze_cells[cx-1][cy] = next_val;
            queue_x[tail] = cx-1; queue_y[tail] = cy; tail++;
        }
    }
}

void steps(){ // expand on loops when testing hardware
    Stepper_Motor_1_Step = 1; 
    Stepper_Motor_2_Step = 1;
    wait_us(1000);
    Stepper_Motor_1_Step = 0; 
    Stepper_Motor_2_Step = 0;
    wait_us(1000);
}

void forwards(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 1;
    for (int i = 0; i < 400; i++){
        steps();
    }
}

void turn_left(){  
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 1;
    for (int i = 0; i < 400; i++){
        steps();
    }
}

void turn_right(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
    for (int i = 0; i < 400; i++){
        steps();
    }
}

void u_turn(){
    //u_turn is now double the steps of turn_right to rotate 180 degrees
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
    for (int i = 0; i < 800; i++){
        steps();
    }
}

//choose best direction using flood fill Lee numbers
int choose_direction(){
    int best_val = 9999;
    int best_dir = -1; // N=0, E=1, S=2, W=3

    // Check North - no north wall and in bounds
    if(mouse_position_y < 7 && !(number_of_maze_walls[mouse_position_x][mouse_position_y] & 1)){
        if(number_of_maze_cells[mouse_position_x][mouse_position_y+1] < best_val){
            best_val = number_of_maze_cells[mouse_position_x][mouse_position_y+1];
            best_dir = 0;
        }
    }
    // Check East
    if(mouse_position_x < 7 && !(number_of_maze_walls[mouse_position_x][mouse_position_y] & 2)){
        if(number_of_maze_cells[mouse_position_x+1][mouse_position_y] < best_val){
            best_val = number_of_maze_cells[mouse_position_x+1][mouse_position_y];
            best_dir = 1;
        }
    }
    // Check South
    if(mouse_position_y > 0 && !(number_of_maze_walls[mouse_position_x][mouse_position_y] & 4)){
        if(number_of_maze_cells[mouse_position_x][mouse_position_y-1] < best_val){
            best_val = number_of_maze_cells[mouse_position_x][mouse_position_y-1];
            best_dir = 2;
        }
    }
    // Check West
    if(mouse_position_x > 0 && !(number_of_maze_walls[mouse_position_x][mouse_position_y] & 8)){
        if(number_of_maze_cells[mouse_position_x-1][mouse_position_y] < best_val){
            best_val = number_of_maze_cells[mouse_position_x-1][mouse_position_y];
            best_dir = 3;
        }
    }
    return best_dir;
}

void clamp_position(){                              //forcesposition values to stay in the range
    if(mouse_position_x < 0) mouse_position_x = 0; //this show if x goes wrong like -1 this snaps it back to 0
    if(mouse_position_x > 7) mouse_position_x = 7;
    if(mouse_position_y < 0) mouse_position_y = 0;
    if(mouse_position_y > 7) mouse_position_y = 7; //8 or hugher snaps back to 7 
}

int main(){
   
    flood_fill_setup();

    while(1){
        wait_us(10000);
        bool front = wall_in_front();
        bool left = wall_to_left();
        bool right = wall_to_right();
        wait_us(10000);
        update_walls(orientation, front, left, right);
        wait_us(10000);

        // Check if goal reached
        if((mouse_position_x == 7) && (mouse_position_y == 7)){
            break;
        }

        //use flood fill to pick best direction
        int target_dir = choose_direction();

        // Work out how many right turns needed to face target direction
        int turns_needed = (target_dir - orientation + 4) % 4;

        if(turns_needed == 0){
            // Already facing correct direction, move forward
            forwards();
        }
        else if(turns_needed == 1){
            turn_right();
            forwards(); // turn then move forward into the cell
        }
        else if(turns_needed == 2){
            u_turn();
            forwards();
        }
        else if(turns_needed == 3){
            turn_left();
            forwards();
        }

        // Update orientation and position after moving
        orientation = target_dir;
        if(orientation == 0) mouse_position_y++;
        else if(orientation == 1) mouse_position_x++;
        else if(orientation == 2) mouse_position_y--;
        else if(orientation == 3) mouse_position_x--;

        clamp_position();

        flood_fill_update();
    }
}
