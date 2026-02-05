/*
SE 1850 Final Project "Squib Game"
Compile: gcc squib_game.c -lncurses -o squib_game
*/

#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

// Prototypes
void draw_menu();
void init();
void dead();
void traffic();
void cookie();     
void tugowar();
void hopscotch();
void squib();
void win();

// Helpers for cookie mini‑game
void add_line(int coords[][2], int pattern[], int *pattern_len, int y_to, int x_to);
void add_diagonal_line(int coords[][2], int pattern[], int *pattern_len, int y_to, int x_to);

// Global Variables
int count = 1;          
int death = 0;          
int i = 0;              
int lightIsRed = 1;     // Traffic light state (1=red, 0=green)
int rando[] = {11, 14, 19, 12, 16};  // Random intervals for light changes


int main() {
    init();
    while (1) {
        draw_menu();

        // Start global timer to track total completion time
        time_t start_time = time(NULL);

        // Run each minigame in sequence - if player dies, loop back to menu
        traffic();
        if (death == 1) { death--; continue; }
        cookie();
        if (death == 1) { death--; continue; }
        tugowar();
        if (death == 1) { death--; continue; }
        hopscotch();
        if (death == 1) { death--; continue; }
        squib();
        if (death == 1) { death--; continue; }

        // Calculate total time taken and display win screen
        time_t end_time = time(NULL);
        int total_seconds = (int)difftime(end_time, start_time);
        win(total_seconds);
        exit(0);
    }

    endwin();
    return 0;
}

void init() {
    initscr();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);    // Red for danger/failure (cookie game)
    init_pair(2, COLOR_GREEN, -1);  // Green for success/progress (cookie game)
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    srand((unsigned)time(NULL));
}

// Display main menu with ASCII logo and navigation options
void draw_menu() {
    const char *logo[] = {
        " _____  ___   __ __  ____  ____        ____   ____  ___ ___    ___ ",
        " / ___/ /   \\ |  T  Tl    j|    \\      /    T /    T|   T   T  /  _]",
        "(   \\_ Y     Y|  |  | |  T |  o  )    Y   __jY  o  || _   _ | /  [_ ",
        " \\__  T|  Q  ||  |  | |  | |     T    |  T  ||     ||  \\_/  |Y    _]",
        " /  \\ ||     ||  :  | |  | |  O  |    |  l_ ||  _  ||   |   ||   [_ ",
        " \\    |l     |l     | j  l |     |    |     ||  |  ||   |   ||     T",
        "  \\___j \\__,_j \\__,_j|____jl_____j    l___,_jl__j__jl___j___jl_____j"
    };

    const char *menu[] = {"Start Game", "Exit"};
    int highlight = 0, key;

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    clear();

    // Center and display ASCII logo
    for (int i = 0; i < (int)(sizeof(logo)/sizeof(logo[0])); i++)
        mvprintw(2 + i, (cols - (int)strlen(logo[i])) / 2, "%s", logo[i]);

    // Display menu options below logo
    int menu_row = 2 + (int)(sizeof(logo)/sizeof(logo[0])) + 3;
    while (1) {
        // Highlight selected menu option
        for (int i = 0; i < 2; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(menu_row + i, (cols - (int)strlen(menu[i])) / 2, "%s", menu[i]);
            attroff(A_REVERSE);
        }
        refresh();
        key = getch();
        
        // Navigate menu with arrow keys
        if (key == KEY_UP || key == KEY_DOWN)
            highlight = (highlight + 1) % 2;
        else if (key == 10) {  // Enter key pressed
            if (highlight == 1) {  // Exit selected
                clear();
                endwin();
                exit(0);
            }
            break;  // Start game selected
        }
    }
}

// Red light Green light - Cross the road without moving during red light
void traffic() {
    clear();
    printw("The objective of the first challenge is to cross a road before the timer runs out.\n"
           "However there is a stoplight, If you move on red, you die.\n\n"
           "Press any key to continue...");
    getch();
    clear();

    // Initialize game parameters
    int totalTime = 45;
    time_t startTime = time(NULL);
    int timeLeft = totalTime;
    int start_col = (COLS - 7) / 2;

    // Draw stoplight frame
    mvprintw(5, start_col, "+----+");
    mvprintw(8, start_col, "+----+");
    mvprintw(9, start_col, "+----+");
    mvprintw(12, start_col, "+----+");
    refresh();

    // Local helper function to toggle stoplight between red and green
    void updateBulbs() {
        static int topOn = 1;
        lightIsRed = topOn;

        char topLight = topOn ? '#' : ' ';
        char bottomLight = topOn ? ' ' : '#';

        // Draw red light (top)
        mvprintw(6, start_col, "| ");
        attron(COLOR_PAIR(1)); addch(topLight); addch(topLight); attroff(COLOR_PAIR(1));
        addstr(" |");

        mvprintw(7, start_col, "| ");
        attron(COLOR_PAIR(1)); addch(topLight); addch(topLight); attroff(COLOR_PAIR(1));
        addstr(" |");

        // Draw green light (bottom)
        mvprintw(10, start_col, "| ");
        attron(COLOR_PAIR(2)); addch(bottomLight); addch(bottomLight); attroff(COLOR_PAIR(2));
        addstr(" |");

        mvprintw(11, start_col, "| ");
        attron(COLOR_PAIR(2)); addch(bottomLight); addch(bottomLight); attroff(COLOR_PAIR(2));
        addstr(" |");

        refresh();
        topOn = !topOn;  // Toggle state for next call
    }

    // Initialize light with red
    updateBulbs();
    napms(1000);
    updateBulbs();  // Switch to green

    // Get random number of moves before light changes
    int ran = rando[rand() % 5];
    int colz = COLS / 8;
    int pos = colz;

    // Draw the crossing platform using dots
    while (i != (COLS * 3) / 4) {
        mvaddch(LINES / 2, colz, '.');
        colz++;
        i++;
    }
    i = 0;

    // Draw player character
    mvaddch((LINES / 2) - 2, pos, '@');
    mvaddch((LINES / 2) - 1, pos, '@');

    timeout(100);

    // Main game loop
    while (1) {
        // Update and display countdown timer
        time_t now = time(NULL);
        timeLeft = totalTime - (int)(now - startTime);
        mvprintw((LINES/2) + 10, COLS/2, "Time: %02d", timeLeft);
        refresh();
        if (timeLeft <= 0) { dead(); return; }

        int c = getch();

        // Player presses space to move forward
        if (c == ' ') {
            // Check if moving during red light (instant death)
            if (lightIsRed == 1) {
                dead();
                return;
            }
            
            // Move character forward
            pos += 2;
            count++;
            mvaddch((LINES / 2) - 2, pos - 2, ' ');
            mvaddch((LINES / 2) - 1, pos - 2, ' ');
            mvaddch((LINES / 2) - 2, pos, '@');
            mvaddch((LINES / 2) - 1, pos, '@');
            napms(50);

            // Check if player reached the end (win condition)
            if (pos >= ((COLS * 3) / 4) + 25) {
                clear();
                return;
            }

            // Check if it's time to change the light
            if (count == ran) {
                updateBulbs();  // Switch to red
                count = 0;
                time_t freezeStart = time(NULL);

                // Freeze period - light stays red for 3 seconds
                while (1) {
                    time_t now2 = time(NULL);
                    timeLeft = totalTime - (int)(now2 - startTime);
                    mvprintw((LINES/2)+10, COLS/2, "Time: %02d", timeLeft);
                    refresh();
                    if (timeLeft <= 0) { dead(); return; }

                    timeout(50);
                    int k = getch();

                    // If player moves during red light, they die
                    if (k == ' ') {
                        updateBulbs();
                        dead();
                        return;
                    }

                    // End freeze after 3 seconds
                    if (now2 - freezeStart >= 3) break;
                }

                updateBulbs();  // Switch back to green
                ran = rando[rand() % 5];  // Pick new random interval
            }
        }
    }
}

// Helper function for square cookie
void add_line(int coords[][2], int pattern[], int *pattern_len, int y_to, int x_to) {
    if (*pattern_len <= 0) return;
    
    // Move horizontally first
    while (coords[*pattern_len - 1][1] != x_to && *pattern_len < 79) {
        int cury = coords[*pattern_len - 1][0];
        int curx = coords[*pattern_len - 1][1];
        coords[*pattern_len][0] = cury;
        coords[*pattern_len][1] = curx + (x_to > curx ? 1 : -1);
        pattern[*pattern_len - 1] = (x_to > curx) ? 3 : 2; // right or left arrow
        (*pattern_len)++;
    }
    
    // Then move vertically
    while (coords[*pattern_len - 1][0] != y_to && *pattern_len < 79) {
        int cury = coords[*pattern_len - 1][0];
        int curx = coords[*pattern_len - 1][1];
        coords[*pattern_len][0] = cury + (y_to > cury ? 1 : -1);
        coords[*pattern_len][1] = curx;
        pattern[*pattern_len - 1] = (y_to > cury) ? 1 : 0; // down or up arrow
        (*pattern_len)++;
    }
}

// Another helper function: Diagonal path for triangle and diamond shapes
void add_diagonal_line(int coords[][2], int pattern[], int *pattern_len, int y_to, int x_to) {
    if (*pattern_len <= 0) return;
    int curx = coords[*pattern_len - 1][1];
    int cury = coords[*pattern_len - 1][0];
    int dx = (x_to > curx) ? 1 : ((x_to < curx) ? -1 : 0);
    int dy = (y_to > cury) ? 1 : ((y_to < cury) ? -1 : 0);

    // Move diagonally towards target point
    while (curx != x_to || cury != y_to) {
        if (curx != x_to) curx += dx;
        if (cury != y_to) cury += dy;
        coords[*pattern_len][0] = cury;
        coords[*pattern_len][1] = curx;

        // Determine arrow direction based on dominant axis
        if (dy != 0 && dx != 0) {
            pattern[*pattern_len - 1] =
                (abs(dy) >= abs(dx)) ? ((dy>0)?1:0) : ((dx>0)?3:2);
        } else if (dy != 0) {
            pattern[*pattern_len - 1] = (dy>0)?1:0;
        } else {
            pattern[*pattern_len - 1] = (dx>0)?3:2;
        }

        (*pattern_len)++;
        if (*pattern_len >= 79) break;
    }
}

// Cookie Cutter minigame: Trace a shape using arrow keys before time runs out
void cookie() {
    clear();
    nodelay(stdscr, FALSE);
    timeout(-1);

    // Display game instructions
    printw("Cookie Cutter Challenge!\n");
    printw("Trace the shape with arrow keys starting from '1'.\n");
    printw("You have 10 seconds and 3 mistakes max.\n\n");
    printw("Press any key to start...");
    getch();
    clear();

    // Randomly select one of three shapes
    int shape_choice = rand() % 3;
    char* shape_names[] = {"Triangle", "Square", "Hollow Diamond"};

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Display shape name and instructions
    mvprintw(1, max_x/2 - 10, "Shape: %s", shape_names[shape_choice]);
    mvprintw(2, max_x/2 - 20, "Find '1' and follow the arrows!");

    // Arrays to store the tracing pattern
    int pattern[80];      
    int coords[80][2];    
    int pattern_len = 0;
    char arrow_chars[] = {'^','v','<','>'};
    
    // Triangle shape generation
    if (shape_choice == 0) {
        int top_y = max_y/2 - 4;
        int top_x = max_x/2;
        int left_y = max_y/2 + 4;
        int left_x = max_x/2 - 8;
        int right_y = max_y/2 + 4;
        int right_x = max_x/2 + 8;

        coords[0][0] = left_y; coords[0][1] = left_x; pattern_len = 1;
        add_diagonal_line(coords, pattern, &pattern_len, top_y, top_x);
        add_diagonal_line(coords, pattern, &pattern_len, right_y, right_x);
        add_diagonal_line(coords, pattern, &pattern_len, left_y, left_x);
    } 
    // Square shape generation
    else if (shape_choice == 1) {
        int top = max_y/2 - 4, left = max_x/2 - 6;
        int bottom = top + 8, right = left + 12;

        coords[0][0] = top; coords[0][1] = left; pattern_len = 1;
        add_line(coords, pattern, &pattern_len, top, right);
        add_line(coords, pattern, &pattern_len, bottom, right);
        add_line(coords, pattern, &pattern_len, bottom, left);
        add_line(coords, pattern, &pattern_len, top, left);
    } 
    // Diamond shape generation
    else {
        int cy = max_y/2;
        int cx = max_x/2;
        int h = 8, w = 8;

        coords[0][0] = cy - h; coords[0][1] = cx; pattern_len = 1;
        add_diagonal_line(coords, pattern, &pattern_len, cy, cx + w);
        add_diagonal_line(coords, pattern, &pattern_len, cy + h, cx);
        add_diagonal_line(coords, pattern, &pattern_len, cy, cx - w);
        add_diagonal_line(coords, pattern, &pattern_len, cy - h, cx);
    }

    // Calculate position for starting "1" marker
    int start_y = coords[0][0];
    int start_x = coords[0][1];
    int first_dir = pattern[0];
    int one_y = start_y;
    int one_x = start_x;

    // Place "1" next to first arrow, opposite to movement direction
    switch(first_dir) {
        case 0: one_y += 1; break;  
        case 1: one_y -= 1; break; 
        case 2: one_x += 1; break;  
        case 3: one_x -= 1; break;  
    }

    // Draw starting "1" in green
    attron(COLOR_PAIR(2)|A_BOLD);
    mvprintw(one_y, one_x, "1");
    attroff(COLOR_PAIR(2)|A_BOLD);

    // Draw all arrows in the pattern (skip last to avoid overlap)
    for(int i=0; i<pattern_len-1; i++) {
        mvaddch(coords[i][0], coords[i][1], arrow_chars[pattern[i]]);
    }

    // Display game status information
    mvprintw(max_y-5,2,"Progress: 0/%d", pattern_len-1);
    mvprintw(max_y-4,2,"Errors: 0/3");
    mvprintw(max_y-3,2,"Time: 10");
    refresh();

    // game state variables
    int current_step = 0, errors = 0, max_errors = 3;
    time_t start_time = time(NULL);
    int time_limit = 10;

    nodelay(stdscr, TRUE);
    timeout(50);

    // Main gameplay loop
    while(current_step < pattern_len-1 && errors < max_errors) {
        // Update countdown timer
        time_t now = time(NULL);
        int elapsed = (int)(now - start_time);
        int time_left = time_limit - elapsed;
        mvprintw(max_y-3,2,"Time: %02d", time_left<0?0:time_left);
        refresh();

        if(time_left <= 0) { dead(); return; }

        // Check for arrow key input
        int ch = getch();
        int pressed = -1;
        if(ch==KEY_UP) pressed=0;
        else if(ch==KEY_DOWN) pressed=1;
        else if(ch==KEY_LEFT) pressed=2;
        else if(ch==KEY_RIGHT) pressed=3;
        if(pressed==-1) continue;

        // Check if correct arrow key was pressed
        if(pressed==pattern[current_step]){
            current_step++;
            mvprintw(max_y-5,2,"Progress: %d/%d", current_step, pattern_len-1);

            // Keep "1" visible
            attron(COLOR_PAIR(2)|A_BOLD);
            mvprintw(one_y, one_x, "1");
            attroff(COLOR_PAIR(2)|A_BOLD);

            // Turn completed arrows green to show progress
            for(int i=0; i<current_step && i<pattern_len-1; i++) {
                attron(COLOR_PAIR(2)|A_BOLD);
                mvaddch(coords[i][0], coords[i][1], arrow_chars[pattern[i]]);
                attroff(COLOR_PAIR(2)|A_BOLD);
            }
        } else {
            // Wrong key pressed - increment error count
            errors++;
            mvprintw(max_y-4,2,"Errors: %d/3", errors);
            attron(COLOR_PAIR(1));
            mvprintw(max_y-2,2,"WRONG KEY!");
            attroff(COLOR_PAIR(1));
        }
        refresh();
    }

    // Reset input settings
    nodelay(stdscr, FALSE);
    timeout(-1);
    
    // Check if player failed (too many errors)
    if(errors>=max_errors){ dead(); return; }

    // Display success message
    clear();
    mvprintw(max_y/2, max_x/2 - 10, "Cookie Cut Successfully!");
    refresh();
    napms(2000);
    clear();
}

// Tug of War minigame: Spam spacebar to pull rope toward your side
void tugowar(){
    clear();
    nodelay(stdscr, FALSE);
    timeout(-1);
    
    // Display game instructions
    printw("Tug of war! Spam space bar to outpull your enemy\n\nPress any key to continue...");
    napms(2500);
    getch();
    clear();
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calculate display positions
    int y_line  = max_y / 2;      // Rope horizontal line
    int y_people = y_line - 1;    // People position
    int y_arrow = y_line + 1;     // Center marker arrow

    int center = max_x / 2;
    int line_len = 31;            // Total rope length

    int start = center - line_len / 2;  // Left end of rope
    int end   = start + line_len - 1;   // Right end of rope

    // Display 3-2-1 countdown before game starts
    for (int i = 3; i >= 1; i--) {
        clear();

        // Draw left player (you)
        if (start - 3 >= 0)
            mvprintw(y_people, start - 3, "o<-");
        
        // Draw right player (computer)
        if (end + 1 < max_x - 2)
            mvprintw(y_people, end + 1, "->o");

        // Draw rope with end caps
        if (start >= 0 && start < max_x)
            mvaddch(y_line, start, '@');
        for (int x = start + 1; x <= end - 1; x++)
            if (x >= 0 && x < max_x)
                mvaddch(y_line, x, '-');
        if (end >= 0 && end < max_x)
            mvaddch(y_line, end, '@');

        // Draw middle marker on rope
        int mid = start + line_len / 2;
        if (mid >= 0 && mid < max_x)
            mvaddch(y_line, mid, '#');

        // Draw center arrow (win condition marker)
        mvaddch(y_arrow, center, '^');

        // Display countdown number
        mvprintw(1, center, "%d", i);

        refresh();
        napms(1000);
    }

    nodelay(stdscr, TRUE);

    // Main gameplay loop
    while (1) {
        getmaxyx(stdscr, max_y, max_x);
        y_line   = max_y / 2;
        y_people = y_line - 1;
        y_arrow  = y_line + 1;
        center   = max_x / 2;

        int c = getch();

        // Player presses space → pull rope left (toward player)
        // Otherwise opponent pulls rope right
        if (c == ' ') start--, end--;
        else start++, end++;

        // Check win/loss conditions
        if (start == center) {  // Opponent pulled rope to center → player loses
            dead();
            break;
        }
        if (end == center) {    // Player pulled rope to center → player wins
            break;
        }

        clear();

        if (start - 3 >= 0)
            mvprintw(y_people, start - 3, "o<-");
        if (end + 1 < max_x - 2)
            mvprintw(y_people, end + 1, "->o");

        if (start >= 0 && start < max_x)
            mvaddch(y_line, start, '@');
        if (end >= 0 && end < max_x)
            mvaddch(y_line, end, '@');

        for (int x = start + 1; x <= end - 1; x++)
            if (x >= 0 && x < max_x)
                mvaddch(y_line, x, '-');

        int mid_of_line = start + line_len / 2;
        if (mid_of_line >= 0 && mid_of_line < max_x)
            mvaddch(y_line, mid_of_line, '#');

        mvaddch(y_arrow, center, '^');

        refresh();
        napms(63);  
    }
}

// Hopscotch minigame: Choose correct platforms to jump on without falling
void hopscotch() {
    clear();
    nodelay(stdscr, FALSE);
    timeout(-1);
    
    // Display game instructions
    printw("You must get to the otherside without falling, you have to pick between 2 squares to jump to\n"
           "Use your arrow keys to pick a square\n\nPress any key to continue...");
    getch();
    napms(2500);
    clear();
    refresh();

    // Game parameters
    int rows = 4;  // Number of platform levels
    int cols = 2;  // Platforms per level

    int boxW = 6;  // Platform width
    int gap  = 4;  // Gap between platforms

    // Calculate center positions
    int centerX = COLS / 2;
    int startY  = LINES / 2 + 4;

    // Arrays to store platform positions
    int boxX[4][2];  // X positions of left and right platforms
    int boxY[4];     // Y positions of each row

    // Calculate all platform positions
    for (int r = 0; r < rows; r++) {
        boxY[r] = startY - r * 4;
        boxX[r][0] = centerX - (boxW + gap);  
        boxX[r][1] = centerX + gap;           
    }

    // Draw starting platform at bottom
    int platY = startY + 3;
    int platX = centerX - 5;
    mvprintw(platY/2, platX, "----------");
    mvprintw(platY,   platX, "----------");

    // Initialize player position
    int ax = centerX;
    int ay = platY - 1;

    // Draw all platforms
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int y = boxY[r];
            int x = boxX[r][c];
            mvprintw(y,     x, "+----+");
            mvprintw(y + 1, x, "|    |");
            mvprintw(y + 2, x, "+----+");
        }
    }

    mvaddch(ay, ax, '@');  // Draw player character
    refresh();

    int currentRow = -1;

    // Main gameplay loop
    while (1) {
        int choice = -1;
        int ch = getch();

        // Get player's platform choice
        if (ch == KEY_LEFT)  choice = 0;
        if (ch == KEY_RIGHT) choice = 1;
        if (choice == -1) continue;

        currentRow++;

        // Check if player reached the top (win condition)
        if (currentRow >= rows) {
            mvaddch(ay, ax, ' ');
            ay = boxY[0] - 2;
            ax = centerX;
            mvaddch(ay, ax, '@');
            clear();
            refresh();
            return;
        }

        // 20% chance the chosen platform is bad (will fall)
        int bad = (rand() % 100) < 20;
        int fallTile = bad ? choice : (choice ^ 1);  

        // Move player to chosen platform
        mvaddch(ay, ax, ' ');
        ax = boxX[currentRow][choice] + boxW/2;
        ay = boxY[currentRow] + 1;
        mvaddch(ay, ax, '@');
        refresh();
        napms(100);

        // Animate the falling platform
        int fyBase = boxY[currentRow];
        int fxBase = boxX[currentRow][fallTile];

        for (int d = 0; d < LINES - fyBase - 3; d++) {
            int fy = fyBase + d;

            // Draw platform at falling position
            mvprintw(fy,     fxBase, "+----+");
            mvprintw(fy + 1, fxBase, "|    |");
            mvprintw(fy + 2, fxBase, "+----+");
            refresh();
            napms(25);

            // Clear previous position
            if (d > 0) {
                mvprintw(fy - 1, fxBase, "      ");
                mvprintw(fy - 0, fxBase, "      ");
                mvprintw(fy - 2, fxBase, "      ");
            }
        }

        // Clean up fallen platform at bottom of screen
        mvprintw(LINES - 3, fxBase, "      ");
        mvprintw(LINES - 2, fxBase, "      ");
        mvprintw(LINES - 1, fxBase, "      ");
        refresh();

        // Check if player chose the bad platform (death)
        if (bad) {
            clear();
            dead();
            return;
        }
    }
}

// Quickdraw minigame: Press the displayed key faster than the computer
void squib(){
    clear();
    nodelay(stdscr, FALSE);
    timeout(-1);

    // Display game instructions
    printw("Quickdraw Challenge!\n");
    printw("When you see a key appear on screen, press it before the computer does!\n");
    printw("The computer's reaction time is random between .7-1.2 seconds.\n");
    printw("Beat the computer to survive!\n\n");
    printw("Press any key to continue...");
    getch();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Define possible keys for the challenge
    char valid_keys[] = {'a', 's', 'd', 'f', 'j', 'k', 'l', 'w','e', 'r', 'q', 'z', 'x', 'c'};
    int num_keys = 14;

    // Display "Get Ready" message
    clear();
    mvprintw(max_y/2, max_x/2 - 10, "Get Ready...");
    refresh();
    napms(1000);

    // Display 3-2-1 countdown
    for (int i = 3; i > 0; i--) {
        clear();
        attron(A_BOLD);
        mvprintw(max_y/2, max_x/2 - 1, "%d", i);
        attroff(A_BOLD);
        refresh();
        napms(1000);
    }

    // Random delay before showing target key (0.5-2 seconds)
    int wait_time = 500 + (rand() % 1500);
    napms(wait_time);

    // Choose random target key
    char target_key = valid_keys[rand() % num_keys];
    
    // Random computer reaction time (700-1200 milliseconds)
    int computer_delay = 700 + (rand() % 500);

    // Display challenge screen with target key
    clear();
    attron(A_BOLD | COLOR_PAIR(1));
    mvprintw(max_y/2 - 2, max_x/2 - 8, "QUICKDRAW!");
    attroff(A_BOLD | COLOR_PAIR(1));

    attron(A_BOLD);
    mvprintw(max_y/2, max_x/2 - 5, "PRESS: %c", target_key);
    attroff(A_BOLD);

    mvprintw(max_y/2 + 2, max_x/2 - 15, "Computer reacts in: %.2f seconds", computer_delay / 1000.0);
    refresh();

    // Start timing
    struct timespec start, current;
    clock_gettime(CLOCK_MONOTONIC, &start);

    nodelay(stdscr, TRUE);
    timeout(0);

    // Game state variables
    int player_time = -1;
    int winner = 0;  // 0=undecided, 1=player wins, 2=computer wins

    // Main game loop - check for player input vs computer time
    while (winner == 0) {
        int ch = getch();

        // Calculate elapsed time in milliseconds
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        int elapsed_ms = (int)((now.tv_sec - start.tv_sec) * 1000 +
                         (now.tv_nsec - start.tv_nsec) / 1000000);

        // Check if player pressed correct key
        if (ch == target_key && player_time == -1) {
            player_time = elapsed_ms;
            winner = 1;  // Player wins
        } 
        // Check if player pressed wrong key
        else if (ch != ERR && ch != target_key && player_time == -1) {
            player_time = 999999;  // Mark as wrong key
            winner = 2;            // Computer wins
            break;
        }

        // Check if computer time has elapsed (computer reacted first)
        if (elapsed_ms >= computer_delay && player_time == -1) {
            winner = 2;  // Computer wins
        }

        napms(1);
    }

    // Reset input settings
    nodelay(stdscr, FALSE);
    timeout(-1);

    clear();

    // Display results based on winner
    if (winner == 1 && player_time < 999999) {
        // Player won - display success screen
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(max_y/2 - 2, max_x/2 - 10, "YOU WIN!");
        attroff(COLOR_PAIR(2) | A_BOLD);
        mvprintw(max_y/2, max_x/2 - 10, "Your time: %dms", player_time);
        mvprintw(max_y/2 + 1, max_x/2 - 15, "Computer time: %dms", computer_delay);
        mvprintw(max_y/2 + 3, max_x/2 - 15, "You were %.2f seconds faster!",
                 (computer_delay - player_time) / 1000.0);
        refresh();
        napms(3000);
        clear();
    } else {
        // Player lost - display failure screen
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(max_y/2 - 2, max_x/2 - 15, "COMPUTER WINS!");
        attroff(COLOR_PAIR(1) | A_BOLD);
        if (player_time == 999999) {
            mvprintw(max_y/2, max_x/2 - 10, "Wrong key pressed!");
        } else {
            mvprintw(max_y/2, max_x/2 - 10, "Too slow!");
            mvprintw(max_y/2 + 1, max_x/2 - 15, "Computer time: %dms", computer_delay);
        }
        refresh();
        napms(2000);
        dead();
    }
}

// Display death screen when player fails a minigame
void dead(){
    clear();
    refresh();
    death++;  
    mvprintw(LINES/2, COLS/2 - 4, "YOU DIED!");
    refresh();
    napms(3000);
    clear();
}
// Display victory screen with total completion time
void win(int total_seconds){
    clear();
    mvprintw(LINES/2, COLS/2 - 12, "CONGRATULATIONS YOU BEAT THE SQUIB GAME!!!!!");
    mvprintw(LINES/2 + 1, COLS/2 - 10, "Your time: %d seconds!", total_seconds);
    refresh();
    napms(5000);
    endwin();
}