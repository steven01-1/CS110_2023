#include "lcd/lcd.h"
#include "pictures.h"
#include "utils.h"
#include <string.h>

void Inp_init(void) {
  rcu_periph_clock_enable(RCU_GPIOA); // Enable clock for GPIOA
  rcu_periph_clock_enable(RCU_GPIOC); // Enable clock for GPIOC

  gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
            | GPIO_PIN_6); // Initialize GPIOA pins as input with pull-down
  gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ,
            GPIO_PIN_13); // Initialize GPIOC pin 13 as input with pull-down
}

void IO_init(void) {
  Inp_init(); // Initialize input ports
  Lcd_Init(); // Initialize LCD
}

void Start() {
  u16 stringcolor = 0xFFFF; // Set string color to white
  LCD_Clear(BLACK); // Clear the LCD screen with black color
  LCD_ShowString(50, 30, "Welcome!", stringcolor); // Display "Welcome!" string at coordinates
                                                   // (50,30) with the specified string color
  while (1) {
    LCD_Clear(BLACK); // Clear the LCD screen with black color
    u16 dotcolor;
    dotcolor = rand() % (0xFFFF); // Generate a random color
    for (int i = 0; i < 50; i++) {
      u16 dotx = rand() % 160; // Generate a random x-coordinate within the LCD boundaries
      u16 doty = rand() % 80; // Generate a random y-coordinate within the LCD boundaries
      LCD_DrawPoint_big(
      dotx, doty,
      dotcolor); // Draw a big point at the generated coordinates with the random color
    }
    LCD_ShowString(50, 30, "Welcome!", stringcolor); // Display "Welcome!" string at coordinates
                                                     // (50,30) with the specified string color
    delay_1ms(250); // Delay for 250 milliseconds
    if (Get_Button(JOY_LEFT) || Get_Button(JOY_RIGHT)
        || Get_Button(JOY_DOWN)) // Check if any of the specified buttons are pressed
    {
      break; // Exit the while loop
    }
  }
  return;
}

u16 level; // Variable to store the selected level
u16 box_number; // Variable to store the selected number of boxes

void Level_selection() {
  u16 string_color = 0xFFFF; // Set string color to white
  u16 level_max = 3; // Maximum number of levels
  u16 box_max = 6; // Maximum number of boxes
  level = 0; // Set initial level to 0
  box_number = 0; // Set initial number of boxes to 0
  LCD_Clear(BLACK); // Clear the LCD screen with black color
  delay_1ms(300); // Delay for 300 milliseconds
  while (1) {
    LCD_ShowString(0, 0, "Pick your level",
                   string_color); // Display "Pick your level" string at coordinates (0,0) with
                                  // the specified string color
    LCD_ShowNum(120, 0, level + 1, 2,
                string_color); // Display the selected level number at coordinates (120,0) with
                               // the specified string color
    LCD_ShowString(0, 20, "Number of box",
                   string_color); // Display "Number of box" string at coordinates (0,20) with
                                  // the specified string color
    LCD_ShowNum(120, 20, box_number + 1, 2,
                string_color); // Display the selected number of boxes at coordinates (120,20)
                               // with the specified string color
    LCD_ShowString(0, 40, "Press BOOT0 to start",
                   string_color); // Display "Press BOOT0 to start" string at coordinates (0,40)
                                  // with the specified string color
    if (Get_Button(JOY_RIGHT)) // Check if JOY_RIGHT button is pressed
    {
      box_number = (box_number + 1) % box_max; // Increment the box_number and wrap around to 0
                                               // if it exceeds the maximum number of boxes
      delay_1ms(300); // Delay for 300 milliseconds
      continue; // Continue to the next iteration of the while loop
    }
    if (Get_Button(JOY_LEFT)) // Check if JOY_LEFT button is pressed
    {
      box_number = (box_number + box_max - 1)
                   % box_max; // Decrement the box_number and wrap around to the maximum number
                              // of boxes - 1 if it becomes negative
      delay_1ms(300); // Delay for 300 milliseconds
      continue; // Continue to the next iteration of the while loop
    }
    if (Get_Button(JOY_DOWN)) // Check if JOY_DOWN button is pressed
    {
      level =
      (level + level_max - 1) % level_max; // Decrement the level and wrap around to the maximum
                                           // number of levels - 1 if it becomes negative
      delay_1ms(300); // Delay for 300 milliseconds
      continue; // Continue to the next iteration of the while loop
    }
    if (Get_Button(JOY_CTR)) // Check if JOY_CTR button is pressed
    {
      level = (level + 1) % level_max; // Increment the level and wrap around to 0 if it exceeds
                                       // the maximum number of levels
      delay_1ms(300); // Delay for 300 milliseconds
      continue; // Continue to the next iteration of the while loop
    }
    if (Get_BOOT0()) // Check if BOOT0 button is pressed
    {
      break; // Exit the while loop
    }
  }
  return;
}

struct Point {
  u16 x; // x-coordinate
  u16 y; // y-coordinate
};

struct MAP {
  u16 target; // Target value
  u16 val; // Current value
};
/* Space = 0, Box = 1, Obstacle = 2 */

struct Point obstacle[3][200]; // Array to store obstacle coordinates (WHITE)
struct Point box[3][20]; // Array to store box coordinates (YELLOW)
struct Point target[3][20]; // Array to store target coordinates (BLUE)
struct Point player; // Player coordinates

struct MAP map[20][10]; // Map grid
u16 scoreboard[3][3]; // Scoreboard grid

const u16 Obstacle_Level[3] = {39, 67, 44}; // Array to store obstacle levels
const int player_pos[3][2] = {
{4, 4}, {4, 2}, {8, 6}}; // Array to store initial player positions for each level
const u16 Score_Max = 0X3f3f3f3f; // Maximum score value

void Data_init() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      map[i][j].val = 0;
    }
  }
  // leyel 1
  obstacle[0][0].y = 0;
  obstacle[0][0].x = 4;
  obstacle[0][1].y = 0;
  obstacle[0][1].x = 5;
  obstacle[0][2].y = 0;
  obstacle[0][2].x = 6;
  obstacle[0][3].y = 0;
  obstacle[0][3].x = 7;

  obstacle[0][4].y = 1;
  obstacle[0][4].x = 1;
  obstacle[0][5].y = 1;
  obstacle[0][5].x = 2;
  obstacle[0][6].y = 1;
  obstacle[0][6].x = 3;
  obstacle[0][7].y = 1;
  obstacle[0][7].x = 4;
  obstacle[0][8].y = 1;
  obstacle[0][8].x = 7;

  obstacle[0][9].y = 2;
  obstacle[0][9].x = 1;
  obstacle[0][10].y = 2;
  obstacle[0][10].x = 4;
  obstacle[0][11].y = 2;
  obstacle[0][11].x = 7;
  obstacle[0][12].y = 2;
  obstacle[0][12].x = 8;
  obstacle[0][13].y = 2;
  obstacle[0][13].x = 9;

  obstacle[0][14].y = 3;
  obstacle[0][14].x = 0;
  obstacle[0][15].y = 3;
  obstacle[0][15].x = 1;
  obstacle[0][16].y = 3;
  obstacle[0][16].x = 4;
  obstacle[0][17].y = 3;
  obstacle[0][17].x = 9;
  obstacle[0][18].y = 3;
  obstacle[0][18].x = 10;

  obstacle[0][19].y = 4;
  obstacle[0][19].x = 0;
  obstacle[0][20].y = 4;
  obstacle[0][20].x = 10;

  obstacle[0][21].y = 5;
  obstacle[0][21].x = 0;
  obstacle[0][22].y = 5;
  obstacle[0][22].x = 4;
  obstacle[0][23].y = 5;
  obstacle[0][23].x = 10;

  obstacle[0][24].y = 6;
  obstacle[0][24].x = 0;
  obstacle[0][25].y = 6;
  obstacle[0][25].x = 1;
  obstacle[0][26].y = 6;
  obstacle[0][26].x = 2;
  obstacle[0][27].y = 6;
  obstacle[0][27].x = 3;
  obstacle[0][28].y = 6;
  obstacle[0][28].x = 4;
  obstacle[0][29].y = 6;
  obstacle[0][29].x = 10;

  obstacle[0][30].y = 7;
  obstacle[0][30].x = 4;
  obstacle[0][31].y = 7;
  obstacle[0][31].x = 5;
  obstacle[0][32].y = 7;
  obstacle[0][32].x = 9;
  obstacle[0][33].y = 7;
  obstacle[0][33].x = 10;

  obstacle[0][34].y = 8;
  obstacle[0][34].x = 5;
  obstacle[0][35].y = 8;
  obstacle[0][35].x = 9;

  obstacle[0][36].y = 9;
  obstacle[0][36].x = 5;
  obstacle[0][37].y = 9;
  obstacle[0][37].x = 6;
  obstacle[0][38].y = 9;
  obstacle[0][38].x = 9;
  // box
  box[0][0].y = 3;
  box[0][0].x = 3;
  box[0][1].y = 4;
  box[0][1].x = 6;
  box[0][2].y = 4;
  box[0][2].x = 8;
  box[0][3].y = 6;
  box[0][3].x = 6;
  box[0][4].y = 6;
  box[0][4].x = 8;
  box[0][5].y = 7;
  box[0][5].x = 7;
  // target
  target[0][0].x = 2;
  target[0][0].y = 3;
  target[0][1].x = 6;
  target[0][1].y = 4;
  target[0][2].x = 6;
  target[0][2].y = 6;
  target[0][3].x = 8;
  target[0][3].y = 4;
  target[0][4].x = 8;
  target[0][4].y = 6;
  target[0][5].x = 7;
  target[0][5].y = 7;
  // ===============================================================
  // level 2
  for (int i = 0; i <= 19; ++i) {
    obstacle[1][i].x = i;
    obstacle[1][i].y = 0;
  }

  obstacle[1][20].x = 0;
  obstacle[1][20].y = 1;
  obstacle[1][21].x = 19;
  obstacle[1][21].y = 1;

  obstacle[1][22].x = 0;
  obstacle[1][22].y = 2;
  obstacle[1][23].x = 8;
  obstacle[1][23].y = 2;
  obstacle[1][24].x = 9;
  obstacle[1][24].y = 2;
  obstacle[1][25].x = 10;
  obstacle[1][25].y = 2;
  obstacle[1][26].x = 11;
  obstacle[1][26].y = 2;
  obstacle[1][27].x = 19;
  obstacle[1][27].y = 2;

  obstacle[1][28].x = 0;
  obstacle[1][28].y = 3;
  obstacle[1][29].x = 1;
  obstacle[1][29].y = 3;
  obstacle[1][30].x = 5;
  obstacle[1][30].y = 3;
  obstacle[1][31].x = 6;
  obstacle[1][31].y = 3;
  obstacle[1][32].x = 7;
  obstacle[1][32].y = 3;
  obstacle[1][33].x = 11;
  obstacle[1][33].y = 3;
  obstacle[1][34].x = 12;
  obstacle[1][34].y = 3;
  obstacle[1][35].x = 13;
  obstacle[1][35].y = 3;
  obstacle[1][36].x = 17;
  obstacle[1][36].y = 3;
  obstacle[1][37].x = 18;
  obstacle[1][37].y = 3;
  obstacle[1][38].x = 19;
  obstacle[1][38].y = 3;

  obstacle[1][39].x = 1;
  obstacle[1][39].y = 4;
  obstacle[1][40].x = 2;
  obstacle[1][40].y = 4;
  obstacle[1][41].x = 5;
  obstacle[1][41].y = 4;
  obstacle[1][42].x = 13;
  obstacle[1][42].y = 4;
  obstacle[1][43].x = 17;
  obstacle[1][43].y = 4;

  obstacle[1][44].x = 2;
  obstacle[1][44].y = 5;
  obstacle[1][45].x = 5;
  obstacle[1][45].y = 5;
  obstacle[1][46].x = 13;
  obstacle[1][46].y = 5;
  obstacle[1][47].x = 14;
  obstacle[1][47].y = 5;
  obstacle[1][48].x = 17;
  obstacle[1][48].y = 5;

  obstacle[1][49].x = 2;
  obstacle[1][49].y = 6;
  obstacle[1][50].x = 5;
  obstacle[1][50].y = 6;
  obstacle[1][51].x = 14;
  obstacle[1][51].y = 6;
  obstacle[1][52].x = 17;
  obstacle[1][52].y = 6;

  obstacle[1][53].x = 2;
  obstacle[1][53].y = 7;
  obstacle[1][54].x = 5;
  obstacle[1][54].y = 7;
  obstacle[1][55].x = 14;
  obstacle[1][55].y = 7;
  obstacle[1][56].x = 17;
  obstacle[1][56].y = 7;

  obstacle[1][57].x = 2;
  obstacle[1][57].y = 8;
  obstacle[1][58].x = 5;
  obstacle[1][58].y = 8;
  obstacle[1][59].x = 14;
  obstacle[1][59].y = 8;
  obstacle[1][60].x = 15;
  obstacle[1][60].y = 8;
  obstacle[1][61].x = 16;
  obstacle[1][61].y = 8;
  obstacle[1][62].x = 17;
  obstacle[1][62].y = 8;

  obstacle[1][63].x = 2;
  obstacle[1][63].y = 9;
  obstacle[1][64].x = 3;
  obstacle[1][64].y = 9;
  obstacle[1][65].x = 4;
  obstacle[1][65].y = 9;
  obstacle[1][66].x = 5;
  obstacle[1][66].y = 9;
  // boxes
  box[1][0].x = 2;
  box[1][0].y = 2;
  box[1][1].x = 4;
  box[1][1].y = 5;
  box[1][2].x = 4;
  box[1][2].y = 7;
  box[1][3].x = 14;
  box[1][3].y = 2;
  box[1][4].x = 15;
  box[1][4].y = 3;
  box[1][5].x = 16;
  box[1][5].y = 5;
  // target
  target[1][0].x = 4;
  target[1][0].y = 1;
  target[1][1].x = 5;
  target[1][1].y = 1;
  target[1][2].x = 6;
  target[1][2].y = 1;
  target[1][3].x = 12;
  target[1][3].y = 1;
  target[1][4].x = 13;
  target[1][4].y = 1;
  target[1][5].x = 14;
  target[1][5].y = 1;
  // ========================================================================
  // level 3
  for (int i = 0; i <= 8; ++i) {
    obstacle[2][i].x = i;
    obstacle[2][i].y = 0;
  }

  obstacle[2][9].x = 0;
  obstacle[2][9].y = 1;
  obstacle[2][10].x = 8;
  obstacle[2][10].y = 1;
  obstacle[2][11].x = 0;
  obstacle[2][11].y = 2;
  obstacle[2][12].x = 2;
  obstacle[2][12].y = 2;
  obstacle[2][13].x = 3;
  obstacle[2][13].y = 2;
  obstacle[2][14].x = 5;
  obstacle[2][14].y = 2;
  obstacle[2][15].x = 6;
  obstacle[2][15].y = 2;
  obstacle[2][16].x = 8;
  obstacle[2][16].y = 2;
  obstacle[2][17].x = 9;
  obstacle[2][17].y = 2;
  obstacle[2][18].x = 10;
  obstacle[2][18].y = 2;

  obstacle[2][19].x = 0;
  obstacle[2][19].y = 3;
  obstacle[2][20].x = 10;
  obstacle[2][20].y = 3;
  obstacle[2][21].x = 11;
  obstacle[2][21].y = 3;

  obstacle[2][22].x = 0;
  obstacle[2][22].y = 4;
  obstacle[2][23].x = 1;
  obstacle[2][23].y = 4;
  obstacle[2][24].x = 11;
  obstacle[2][24].y = 4;

  obstacle[2][25].x = 1;
  obstacle[2][25].y = 5;
  obstacle[2][26].x = 2;
  obstacle[2][26].y = 5;
  obstacle[2][27].x = 3;
  obstacle[2][27].y = 5;
  obstacle[2][28].x = 5;
  obstacle[2][28].y = 5;
  obstacle[2][29].x = 6;
  obstacle[2][29].y = 5;
  obstacle[2][30].x = 8;
  obstacle[2][30].y = 5;
  obstacle[2][31].x = 9;
  obstacle[2][31].y = 5;
  obstacle[2][32].x = 11;
  obstacle[2][32].y = 5;

  obstacle[2][33].x = 3;
  obstacle[2][33].y = 6;
  obstacle[2][34].x = 11;
  obstacle[2][34].y = 6;

  for (int i = 3; i <= 11; ++i) {
    obstacle[2][32 + i].x = i;
    obstacle[2][32 + i].y = 7;
  }
  // boxes
  box[2][0].x = 3;
  box[2][0].y = 3;
  box[2][1].x = 5;
  box[2][1].y = 3;
  box[2][2].x = 7;
  box[2][2].y = 3;
  box[2][3].x = 4;
  box[2][3].y = 4;
  box[2][4].x = 6;
  box[2][4].y = 4;
  box[2][5].x = 8;
  box[2][5].y = 4;
  // target
  target[2][0].x = 4;
  target[2][0].y = 3;
  target[2][1].x = 6;
  target[2][1].y = 3;
  target[2][2].x = 8;
  target[2][2].y = 3;
  target[2][3].x = 3;
  target[2][3].y = 4;
  target[2][4].x = 5;
  target[2][4].y = 4;
  target[2][5].x = 7;
  target[2][5].y = 4;
}

void Game_init() {
  LCD_Clear(BLACK); // Clear the LCD screen with black color
  for (u16 i = 0; i < box_number + 1; i++) {
    // fill boxes
    LCD_Fill(box[level][i].x * 8, box[level][i].y * 8, box[level][i].x * 8 + 7,
             box[level][i].y * 8 + 7,
             YELLOW); // Fill the box at the specified coordinates with yellow color
    map[box[level][i].x][box[level][i].y].val =
    1; // Set the value of the corresponding map position to 1 (box)
  }
  for (u16 i = 0; i < box_number + 1; i++) {
    // fill targets
    if (map[target[level][i].x][target[level][i].y].val
        == 1) // Check if the target position is occupied by a box
    {
      LCD_Fill(target[level][i].x * 8, target[level][i].y * 8, target[level][i].x * 8 + 7,
               target[level][i].y * 8 + 7,
               GREEN); // Fill the target at the specified coordinates with green color
    }
    else {
      LCD_Fill(target[level][i].x * 8, target[level][i].y * 8, target[level][i].x * 8 + 7,
               target[level][i].y * 8 + 7,
               BLUE); // Fill the target at the specified coordinates with blue color
    }
    map[target[level][i].x][target[level][i].y].target =
    1; // Set the target flag of the corresponding map position to 1 (target)
  }
  // fill obstacles
  for (u16 i = 0; i < Obstacle_Level[level]; i++) {
    LCD_Fill(obstacle[level][i].x * 8, obstacle[level][i].y * 8, obstacle[level][i].x * 8 + 7,
             obstacle[level][i].y * 8 + 7,
             WHITE); // Fill the obstacle at the specified coordinates with white color
    map[obstacle[level][i].x][obstacle[level][i].y].val =
    2; // Set the value of the corresponding map position to 2 (obstacle)
  }
  // player
  player.x = player_pos[level][0]; // Set the player's x-coordinate based on the current level
  player.y = player_pos[level][1]; // Set the player's y-coordinate based on the current level
  LCD_ShowPicture(
  player.x * 8, player.y * 8, player.x * 8 + 7, player.y * 8 + 7, 64,
  role_pic); // Show the player's picture at the specified coordinates on the LCD
}

u16 Game_play() {
  u16 step = 0; // Initialize the step count to zero
  LCD_ShowNum((level == 1 ? 60 : 90), (level == 1 ? 60 : 0), step, 3,
              WHITE); // Show the step count on the LCD at the specified coordinates

  while (1) {
    int moved = 0; // Flag to track if the player moved
    int x = (int)player.x; // Get the current x-coordinate of the player
    int y = (int)player.y; // Get the current y-coordinate of the player
    int tx = 0; // Temporary x-coordinate for calculating new player position
    int ty = 0; // Temporary y-coordinate for calculating new player position
    int dx = 0; // Change in x-coordinate for player movement
    int dy = 0; // Change in y-coordinate for player movement

    if (Get_Button(JOY_RIGHT)) {
      dx = 1;
      dy = 0;
      moved = 1;
    }
    if (Get_Button(JOY_LEFT)) {
      dx = -1;
      dy = 0;
      moved = 1;
    }
    if (Get_Button(JOY_DOWN)) {
      dx = 0;
      dy = 1;
      moved = 1;
    }
    if (Get_Button(JOY_CTR)) {
      dx = 0;
      dy = -1;
      moved = 1;
    }

    if (moved == 0) {
      continue; // If no movement occurred, continue to the next iteration
    }

    tx = x + dx; // Calculate the new x-coordinate of the player
    ty = y + dy; // Calculate the new y-coordinate of the player

    if (tx < 0 || tx > 19 || ty < 0 || ty > 9 || map[tx][ty].val == 2) {
      continue; // If the new position is outside the boundaries or is an obstacle, continue to
                // the next iteration
    }

    if (map[tx][ty].val == 1) // If the new position contains a box
    {
      int box_tx = tx + dx; // Calculate the new x-coordinate of the box
      int box_ty = ty + dy; // Calculate the new y-coordinate of the box

      if (map[box_tx][box_ty].val != 0 || box_tx < 0 || box_tx > 19 || box_ty < 0
          || box_ty > 9) {
        continue; // If the box cannot be moved to the new position, continue to the next
                  // iteration
      }

      if (map[x][y].target == 0) {
        LCD_Fill((u16)x * 8, (u16)y * 8, (u16)x * 8 + 7, (u16)y * 8 + 7, BLACK);
      }
      else {
        LCD_Fill((u16)x * 8, (u16)y * 8, (u16)x * 8 + 7, (u16)y * 8 + 7, BLUE);
      }

      // player
      LCD_ShowPicture((u16)tx * 8, (u16)ty * 8, (u16)tx * 8 + 7, (u16)ty * 8 + 7, 64, role_pic);

      if (map[box_tx][box_ty].target == 0) {
        LCD_Fill((u16)box_tx * 8, (u16)box_ty * 8, (u16)box_tx * 8 + 7, (u16)box_ty * 8 + 7,
                 YELLOW);
      }
      else {
        LCD_Fill((u16)box_tx * 8, (u16)box_ty * 8, (u16)box_tx * 8 + 7, (u16)box_ty * 8 + 7,
                 GREEN);
      }

      map[box_tx][box_ty].val = 1; // Update the value of the box in the new position
      map[tx][ty].val = 0; // Update the value of the box in the old position
      player.x = (u16)tx; // Update the x-coordinate of the player
      player.y = (u16)ty; // Update the y-coordinate of the player
      step++; // Increment the step count
      LCD_ShowNum((level == 1 ? 60 : 90), (level == 1 ? 60 : 0), step, 3,
                  WHITE); // Show the updated step count on the LCD
    }
    else if (map[tx][ty].val == 0) // If the new position is empty
    {
      if (map[x][y].target == 0) {
        LCD_Fill((u16)x * 8, (u16)y * 8, (u16)x * 8 + 7, (u16)y * 8 + 7, BLACK);
      }
      else {
        LCD_Fill((u16)x * 8, (u16)y * 8, (u16)x * 8 + 7, (u16)y * 8 + 7, BLUE);
      }

      // player
      LCD_ShowPicture((u16)tx * 8, (u16)ty * 8, (u16)tx * 8 + 7, (u16)ty * 8 + 7, 64, role_pic);

      player.x = (u16)tx; // Update the x-coordinate of the player
      player.y = (u16)ty; // Update the y-coordinate of the player
      step++; // Increment the step count
      LCD_ShowNum((level == 1 ? 60 : 90), (level == 1 ? 60 : 0), step, 3,
                  WHITE); // Show the updated step count on the LCD
    }

    int finished = 1; // Flag to track if all targets are filled

    for (int i = 0; i < box_number + 1; i++) {
      if (map[target[level][i].x][target[level][i].y].val != 1) {
        finished = 0; // If any target is not filled, set the flag to 0
        break;
      }
    }

    if (finished == 1) {
      break; // If all targets are filled, exit the loop
    }

    delay_1ms(300); // Delay for 300 milliseconds before the next iteration
  }

  return step; // Return the final step count
}

void Finished(u16 step) {
  LCD_Clear(BLACK); // Clear the LCD screen
  LCD_ShowString(0, 0, "You finished with",
                 WHITE); // Display a string on the LCD at the specified coordinates
  LCD_ShowNum(0, 20, step, 3,
              WHITE); // Show the step count on the LCD at the specified coordinates
  LCD_ShowString(30, 20, "moves",
                 WHITE); // Display a string on the LCD at the specified coordinates
  delay_1ms(3000); // Delay for 3000 milliseconds (3 seconds)
  LCD_Clear(BLACK); // Clear the LCD screen
  LCD_ShowNum(0, 20, 1, 1, WHITE); // Show a number on the LCD at the specified coordinates
  LCD_ShowNum(0, 40, 2, 1, WHITE); // Show a number on the LCD at the specified coordinates
  LCD_ShowNum(0, 60, 3, 1, WHITE); // Show a number on the LCD at the specified coordinates
  LCD_ShowString(24, 0, "LEVEL1",
                 WHITE); // Display a string on the LCD at the specified coordinates
  LCD_ShowNum(80, 0, 2, 4, WHITE); // Show a number on the LCD at the specified coordinates
  LCD_ShowNum(120, 0, 3, 4, WHITE); // Show a number on the LCD at the specified coordinates

  if (step < scoreboard[level][0]) // Check if the current step count is lower than the first
                                   // score in the scoreboard
  {
    scoreboard[level][2] = scoreboard[level][1];
    scoreboard[level][1] = scoreboard[level][0];
    scoreboard[level][0] = step;
  }
  else if (step < scoreboard[level][1]) // Check if the current step count is lower than the
                                        // second score in the scoreboard
  {
    scoreboard[level][2] = scoreboard[level][1];
    scoreboard[level][1] = step;
  }
  else if (step < scoreboard[level][2]) // Check if the current step count is lower than the
                                        // third score in the scoreboard
  {
    scoreboard[level][2] = step;
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (scoreboard[i][j] != 0x3F3F3F3F) // Check if the score is not equal to a specific value
                                          // (indicating an empty score)
      {
        LCD_ShowNum(40 + i * 40, 20 + j * 20, scoreboard[i][j], 4,
                    WHITE); // Show a score on the LCD at the specified coordinates
      }
      else {
        LCD_ShowNum(40 + i * 40, 20 + j * 20, 0, 4,
                    WHITE); // Show a zero on the LCD at the specified coordinates
      }
    }
  }

  while (1) {
    if (Get_Button(JOY_DOWN) || Get_Button(JOY_RIGHT)
        || Get_Button(JOY_LEFT)) // Check if any of the specified buttons are pressed
    {
      return; // Exit the function
    }
  }
}

int main(void) {
  u16 moves;
  // init OLED
  IO_init();
  LCD_Clear(WHITE);
  // start picture
  LCD_ShowPicture(0, 0, 159, 79, 12800, start_pic);
  delay_1ms(880);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      scoreboard[i][j] = Score_Max;
    }
  }
  // main loop
  while (1) {
    Start();
    Level_selection();
    Data_init();
    Game_init();
    moves = Game_play();
    Finished(moves);
  }
}
