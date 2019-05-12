/*
 * minmax 기반 서치 알고리즘으로 대응하기 힘든 수나 커넥트 4 게임에서 권장되는 강력한 정석 수를
 * 룰 베이스 기반으로 서치하여 리턴함
 *
 * Created by 김은주(2015410051)
 * Copyright © 2019 Team 프사 속 그녀와 나와 형. All rights reserved.
 */
#include "main.hpp"
#include "rulebase.hpp"

const char SYMBOL[2] = { 'O', 'X' };
bool droppable_list[WIDTH + 1] = { false, };
const char rule6a[6] = { ' ', ' ', 'X', 'X', ' ', ' ' };
const char rule6b[5] = { ' ', 'X', ' ', 'X', ' ' };
const char rule7a[6] = { ' ', ' ', 'O', 'O', ' ', ' ' };
const char rule7b[5] = { ' ', 'O', ' ', 'O', ' ' };

const char ptrn33[6][4][4] = {
  //PTRN 1
  {'X', 'A', 'A', 'A',
    'A', ' ', 'A', 'A',
    'D', ' ', 'D', 'X',
    ' ', ' ', ' ', 'D'},
  // ptrn 1 좌우 반전
  {'A', 'A', 'A', 'X',
    'A', 'A', ' ', 'A',
    'X', 'D', ' ', 'D',
    'D', ' ', ' ', ' '},
  // PTRN 2
  {'X', 'A', 'A', 'A',
    'D', 'D', ' ', 'X',
    ' ', ' ', ' ', 'A',
    ' ', ' ', ' ', 'D'},
  //PTRN 2 좌우 반전
  {'A', 'A', 'A', 'X',
    'A', 'A', ' ', 'A',
    'X', 'D', ' ', 'D',
    'D', ' ', ' ', ' '},
  //PTRN 3
  {'X', 'A', 'A', 'A',
    'A', 'X', 'A', 'A',
    'A', 'A', ' ', 'A',
    'D', 'D', ' ', 'D'},
  //PTRN 3 좌우 반전
  {'A', 'A', 'A', 'X',
    'A', 'A', 'X', 'A',
    'A', ' ', 'A', 'A',
    'D', ' ', 'D', 'D'},
};


/*
 * rule-base에 근거하여 AI가 다음에 필수적으로 둬야하는 수를 리턴하거나, 다음 수에서 두는 것을 권장하지 않는 칼럼 리스트를 지정한다
 * @return [Integer] AI가 다음 디스크를 둬야하는 칼럼 (0: 없음, 그 외: 해당 칼럼에 두어야 함)
 */
int rule_based_eval() {
  extern int total_drops;
  extern char board[7][8];
  extern int col_h[8];
  extern int ai_turn;
  
  for (int i = 1; i <= WIDTH; i++)
    droppable_list[i] = true;
  
  // 1. if started first and is first turn
  if (total_drops == 0) {
    srand((unsigned int)time(0));
    int random = rand() % 2;
    if (random) random = 3;
    else random = 5;
    printf("rule#1: First turn ruled\n");
    return random;
  }
  
  // 2. if started late and is firt turn
  if (total_drops == 1) {
    printf("rule#2: Second turn ruled\n");
    return 4;
  }
  
  // 3.Win immediately
  // check all i if ai can win if droppable
  for (int i = 1; i <= WIDTH; i++) {
    if (col_h[i] <= HEIGHT) { // do if it is droppable
      board[col_h[i]][i] = SYMBOL[ai_turn];
      col_h[i]++;
      
      if (check_end(i)) {
        col_h[i]--;
        board[col_h[i]][i] = ' ';
        printf("rule#3: We Win:) \n");
        return i;
      }
      col_h[i]--;
      board[col_h[i]][i] = ' ';
    }
  }
  
  // 4  & 5 check immediately losing after my turn
  for (int i = 1; i <= WIDTH; i++) {
    // 4. lose after my turn -> defense
    if (col_h[i] <= HEIGHT) {
      board[col_h[i]][i] = SYMBOL[!ai_turn];
      col_h[i]++;
      
      if (check_end(i)) {
        col_h[i]--;
        board[col_h[i]][i] = ' ';
        printf("rule#4: DEFENSE!!! \n");
        return i;
      }
      col_h[i]--;
      board[col_h[i]][i] = ' ';
    }
    
    // 5. lose with enemy choosing same column -> do not choose that column
    if (col_h[i] <= HEIGHT - 1) {
      board[col_h[i]][i] = SYMBOL[ai_turn];
      col_h[i]++;
      board[col_h[i]][i] = SYMBOL[!ai_turn];
      col_h[i]++;
      
      if (check_end(i)) droppable_list[i] = false;
      
      col_h[i]--;
      board[col_h[i]][i] = ' ';
      col_h[i]--;
      board[col_h[i]][i] = ' ';
    }
  }
  // 6. win next turn
  // 6-A. _ _ O O _ _
  for (int i = 1; i <= 6; i++) {
    for (int j = 1; j <= 2; j++) {
      bool match_rule = true;
      for (int k = 0; k <= 5; k++)
        if (board[i - 1][j + k] == ' ' || board[i][j + k] != rule6a[k])
          match_rule = false;
      
      if (match_rule) {
        printf("rule#6-A: _ _ O O _ _ we win\n");
        srand((unsigned int)time(0));
        int random = rand() % 2;
        if (random) random = 1;
        else random = 4;
        return random + j; // pick either 2 or 5
      }
    }
  }
  
  // 6-B. _ O _ O _
  for (int i = 1; i <= 6; i++) {
    for (int j = 1; j <= 3; j++) {
      bool match_rule = true;
      
      for (int k = 0; k <= 4; k++)
        if (board[i - 1][j + k] == ' ' || board[i][j + k] != rule6b[k])
          match_rule = false;
      
      if (match_rule) {
        printf("rule#6-B: _ O _ O _ we win\n");
        return j + 2; // pick center
      }
    }
  }
  
  // 6-C. pattern matching
  for (int x = 1; x <= 3; x++) {
    for (int y = 1; y <= 4; y++) {
      for (int n = 0; n < 6; n++) {
        
        int check = 0;
        int droppable_place = 3;
        int choose_col = 0;
        
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            // X : AI TURN ' ' : BLANK  A : ANY DISK  D: AI'S DROPPABLE PLACE
            
            if (ptrn33[n][i][j] == SYMBOL[ai_turn] && ptrn33[n][i][j] == board[x + i][y + j]) {
              check++;
            }
            if (ptrn33[n][i][j] == ' ' && ptrn33[n][i][j] == board[x + i][y + j]) {
              check++;
            }
            if (ptrn33[n][i][j] == 'A' && board[x + i][y + j] != ' ') {
              check++;
            }
            if (ptrn33[n][i][j] == 'D' && board[x + i][y + j] == SYMBOL[ai_turn]) {
              droppable_place--;
              check++;
            }
            if (ptrn33[n][i][j] == 'D' && board[x + i][y + j] == ' ') {
              choose_col = y + j;
              check++;
            }
          }
        }
        if (check == 16 && droppable_place == 1) {
          printf("rule #6-C-%d: pattern matching \n", n + 1);
          return choose_col;
        }
      }
    }
  }
  
  //7. lose next turn
  
  // 7-A. _ _ O O _ _
  for (int i = 1; i <= 6; i++) {
    for (int j = 1; j <= 2; j++) {
      bool match_rule = true;
      for (int k = 0; k <= 5; k++)
        if (board[i - 1][j + k] == ' ' || board[i][j + k] != rule7a[k])
          match_rule = false;
      
      if (match_rule) {
        printf("rule#7-A: DEFENSE _ _ O O _ _ \n");
        return j + 1; // pick either 2 or 5
      }
    }
  }
  
  // 7-B. _ O _ O _
  for (int i = 1; i <= 6; i++) {
    for (int j = 1; j <= 3; j++) {
      bool match_rule = true;
      
      for (int k = 0; k <= 4; k++)
        if (board[i - 1][j + k] == ' ' || board[i][j + k] != rule7b[k])
          match_rule = false;
      
      if (match_rule) {
        printf("rule#7-B:DEFENSE _ O _ O _ \n");
        return j + 2; // pick center
      }
    }
  }
  
  // 7-C. pattern matching
  for (int x = 1; x <= 3; x++) {
    for (int y = 1; y <= 4; y++) {
      for (int n = 0; n < 6; n++) {
        
        int check = 0;
        int droppable_place = 3;
        int choose_col = 0;
        
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            // X : AI TURN ' ' : BLANK  A : ANY DISK  D: AI'S DROPPABLE PLACE
            
            if (ptrn33[n][i][j] == SYMBOL[ai_turn] && SYMBOL[!ai_turn] == board[x + i][y + j]) {
              check++;
            }
            if (ptrn33[n][i][j] == ' ' && ptrn33[n][i][j] == board[x + i][y + j]) {
              check++;
            }
            if (ptrn33[n][i][j] == 'A' && board[x + i][y + j] != ' ') {
              check++;
            }
            if (ptrn33[n][i][j] == 'D' && board[x + i][y + j] == SYMBOL[!ai_turn]) {
              droppable_place--;
              check++;
            }
            if (ptrn33[n][i][j] == 'D' && board[x + i][y + j] == ' ') {
              choose_col = y + j;
              check++;
            }
          }
        }
        if (check == 16 && droppable_place == 1) {
          printf("rule #7-C-%d: DEFENSE pattern matching \n", n + 1);
          return choose_col;
        }
      }
    }
  }
  
  
  
  
  /* 8. 초기 TRAP막기
   * 8-A.
   *   O
   *  OX
   *  XXO
   * 23456
   * -> 2
   */
  if (total_drops == 6) {
    // 위의 형태
    if (board[1][3] == SYMBOL[ai_turn] && board[1][4] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] &&
        board[2][3] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[1][5] == SYMBOL[!ai_turn]) {
      printf("rule#8-A DEFENSE Trap\n");
      return 2;
    }
    
    // 대칭
    else if (board[1][5] == SYMBOL[ai_turn] && board[1][4] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] &&
             board[2][5] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[1][3] == SYMBOL[!ai_turn]) {
      printf("rule#8-A DEFENSE Trap\n");
      return 6;
    }
    /* 8-B
     *  O
     *  X
     *  O
     * OXX
     * 234
     * -> 2
     */
    // 위의 형태
    if (board[1][3] == SYMBOL[ai_turn] && board[1][4] == SYMBOL[ai_turn] && board[3][3] == SYMBOL[ai_turn] &&
        board[1][2] == SYMBOL[!ai_turn] && board[2][3] == SYMBOL[!ai_turn] && board[4][3] == SYMBOL[!ai_turn]) {
      printf("rule#8-B DEFENSE Trap\n");
      return 2;
    }
    // 대칭
    else if (board[1][5] == SYMBOL[ai_turn] && board[1][4] == SYMBOL[ai_turn] && board[3][5] == SYMBOL[ai_turn] &&
             board[1][6] == SYMBOL[!ai_turn] && board[2][5] == SYMBOL[!ai_turn] && board[4][5] == SYMBOL[!ai_turn]) {
      printf("rule#8-B DEFENSE Trap\n");
      return 6;
    }
    /*
     *  O
     *  X
     *  O
     *  X
     * XO
     * 3456
     * -> 6
     */
    // 위의 형태
    if (board[1][3] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] && board[4][4] == SYMBOL[ai_turn] &&
        board[1][4] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[5][4] == SYMBOL[!ai_turn]) {
      printf("rule#8-C DEFENSE Trap\n");
      return 6;
    }
    // 대칭
    else if (board[1][5] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] && board[4][4] == SYMBOL[ai_turn] &&
             board[1][4] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[5][4] == SYMBOL[!ai_turn]) {
      printf("rule#8-C DEFENSE Trap\n");
      return 2;
    }
  }
  
  /*
   *   O
   *   X
   *   O
   *   XO
   * X OX
   * 23456
   * -> 6
   */
  if (total_drops == 8) {
    // 위의 형태
    if (board[1][2] == SYMBOL[ai_turn] && board[1][5] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] && board[4][4] == SYMBOL[ai_turn] &&
        board[1][4] == SYMBOL[!ai_turn] && board[2][5] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[5][4] == SYMBOL[!ai_turn]) {
      printf("rule#8-D DEFENSE Trap\n");
      return 6;
    }
    // 대칭
    else if (board[1][6] == SYMBOL[ai_turn] && board[1][3] == SYMBOL[ai_turn] && board[2][4] == SYMBOL[ai_turn] && board[4][4] == SYMBOL[ai_turn] &&
             board[1][4] == SYMBOL[!ai_turn] && board[2][3] == SYMBOL[!ai_turn] && board[3][4] == SYMBOL[!ai_turn] && board[5][4] == SYMBOL[!ai_turn]) {
      printf("rule#8-D DEFENSE Trap\n");
      return 2;
    }
  }
  
  
  
  bool lose_flag = true;
  for (int i = 1; i <= WIDTH; i++)
    if (droppable_list[i] == true && col_h[i] <= HEIGHT)
      lose_flag = false;
  
  if (lose_flag)
    for (int i = 1; i <= WIDTH; i++)
      droppable_list[i] = true;
  
  return 0;
}
