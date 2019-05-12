/*
 * Team 프사속 그녀와 나와 형.
 * [전체]
 *  - AI 기본 전략 수립
 *  - 기본적인 평가함수 전략 수립
 * [김낙현(2014210036)]:
 *  - Connect 4 게임 기능 구현
 *  - 평가함수 계산 로직 구현 및 가중치 튜닝
 *  - 메모리 사용량 튜닝
 * [조현규(2014210074)]
 *  - MinMax 서치 로직 구현
 *  - Alpha-Beta Pruning 구현
 *  - 처리 퍼포먼스 튜닝
 * [김은주(2015410051)]
 *  - Rule based 로직 설계 및 구체화
 *  - Rule 기반 착수 로직 구현
 *  - Report 작성
 */


/*
 * 사람 vs AI 기반의 connect 4 게임 기능 및 main 함수 관리
 *
 * Created by 김낙현(2014210036)
 * Copyright © 2019 Team 프사 속 그녀와 나와 형. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.hpp"

const char SYMBOL[2] = {'O', 'X'};
char board[7][8];
int col_h[8]; //Height of each column
int total_drops;    //Total number of stone in a game
int ai_turn;  //Whether AI's turn
State state;

//Clear Console
void clear(){system("@cls||clear");}

//Initialize board and game variables
void init(){
  //Init Board
  memset(board, ' ', sizeof(board));
  board[0][0] = ' ';
  for(int i = 1; i < 7; i++) board[i][0] = i + '0';
  for(int i = 1; i < 8; i++) board[0][i] = i + '0';
  
  //Init column height index
  for(int i = 0; i < 8; i++) col_h[i] = 1;
  
  total_drops = 0;
  
  state.total_drops = state.bitmap_player = state.bitmap_total = 0;
  
  //Set First Order
  char t = -1;
  do{
    while(t != -1 && getchar() != '\n');
    printf("Player First? (Y/N) : ");
    scanf("%c", &t);
    if(t > 'Z') t -= 'a' - 'A';
  }while(t != 'Y' && t != 'N');
  
  ai_turn = (t == 'Y' ? 0 : 1);
}

//Test if game ends
//Param : pos (int) - position of new stone
int check_end(int pos){
  int c = pos,
  r = col_h[pos] - 1;
  
  //Row
  for(int i = c-3; i <= c; i++)
    if(i > 0 && i+3 <= WIDTH &&
       board[r][i] == board[r][i+1] && board[r][i+1] == board[r][i+2] && board[r][i+2] == board[r][i+3])
      
      return 1;
  
  //Colomn
  for(int i = r-3; i <= r; i++)
    if(i > 0 && i+3 <= HEIGHT &&
       board[i][c] == board[i+1][c] && board[i+1][c] == board[i+2][c] && board[i+2][c] == board[i+3][c])
      return 1;
  
  //Diagonal
  //ri : row index, ci : column index (increase for /), cj : column index (decrease for \)
  int ri = r - 3, ci = c - 3, cj = c + 3;
  for(int i = 0; i <= 3; i++){
    if(ri > 0 && ci > 0 && ri+3 <= HEIGHT && ci+3 <= WIDTH &&
       board[ri][ci] == board[ri+1][ci+1] && board[ri+1][ci+1] == board[ri+2][ci+2] && board[ri+2][ci+2] == board[ri+3][ci+3])
      return 1;
    
    if(ri > 0 && cj-3 > 0 && ri+3 <= HEIGHT && cj <= WIDTH &&
       board[ri][cj] == board[ri+1][cj-1] && board[ri+1][cj-1] == board[ri+2][cj-2] && board[ri+2][cj-2] == board[ri+3][cj-3])
      return 1;
    
    ri++; ci++; cj--;
  }
  
  return 0;
}

//Get Player's input
//Param : prev_pos (int) - Previous Position
int get_input(int prev_pos){
  int pos;
  do{
    while(getchar() != '\n');
    if(total_drops > 0) printf("%c : %d, %d\n", SYMBOL[ai_turn^1], prev_pos, col_h[prev_pos]-1);
    printf("Select Position [1, 7] : ");
  }while(scanf("%d", &pos) < 1 || pos < 1 || pos > WIDTH || col_h[pos] > HEIGHT || (total_drops == 0 && pos == 4));
  
  return pos;
}

int get_ai_pos(){
  int pos = rule_based_eval();
  //if pos = 0, not dropping immeidately
  if (!pos) {
    //minmax
    pos = run_ai(state);
  }
  return pos;
}

void drop(int pos){
  board[col_h[pos]++][pos] = SYMBOL[ai_turn];
  
  state = init_state(state, pos);
}

//ADD stone on board and DRAW Whole board
//Param : pos (int) - position of new stone
void draw_board(int pos){
  //clear();
  
  for(int i = HEIGHT; i >= 0; i--){
    for(int j = 0; j <= WIDTH; j++) printf("%c ", board[i][j]);
    printf("\n");
  }
}


void connect4(){
  int pos = 0;  //position of new stone
  
  while(total_drops < WIDTH * HEIGHT){
    if(ai_turn) pos = get_ai_pos();
    else pos = get_input(pos);
    
    drop(pos);
    draw_board(pos);
    
    if(check_end(pos)) break;
    
    ai_turn ^= 1;
    total_drops++;
  }
  
  if(total_drops == WIDTH * HEIGHT) printf("DRAW!!\n");
  else printf("%s win!!\n", ai_turn ? "AI" : "Player");
}

int main(){
  init();
  connect4();
  
  return 0;
}
