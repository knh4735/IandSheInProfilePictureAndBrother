/*
 * minmax 알고리즘 기반 AI를 이용하여 다음에 수를 둘 최적의 위치를 계산하여 리턴한다
 * 탐색은 시간 제한 내에 MIN_DEPTH 부터 MAX_DEPTH 까지 진행하며 마지막으로 완료된 DEPTH의 결과를 이용한다
 *
 * Created by 조현규(2014210074)
 * Copyright © 2019 Team 프사 속 그녀와 나와 형. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <string>
#include <time.h>
#include "minmax.hpp"
#include "rulebase.hpp"

// 보드의 가로, 세로, 총 셀 개수
#define TOTAL_CELLS 42

// Integer max 값
#define INF 2147483647
// connect 4일 경우의 weight
#define WEIGHT4 10000

// 최소 검색 깊이
int MIN_DEPTH = 13;
// 최대 검색 깊이
int MAX_DEPTH = 17;
// 검색 깊이
int SEARCH_DEPTH = MIN_DEPTH;

// connect 4 내 플레이어의 디스크 개수 당 가중치
int WEIGHT[4] = {0, 1, 5, 40};
// 검색 칼럼 순서
int col_order[WIDTH] = {4, 3, 5, 2, 6, 1, 7};
// 숫자에 대응되는 비트 개수
int bits_cnt[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
// 수직 형태의 connect 4 모델
int VERTICAL_HEX = 1 | 1 << 7 | 1 << 14 | 1 << 21;
// 슬래쉬 형태의 connect 4 모델
int SLASH_HEX = 1 << 3 | 1 << 9 | 1 << 15 | 1 << 21;
// 백슬래쉬 형태의 connect 4 모델
int BACKSLASH_HEX = 1 | 1 << 8 | 1 << 16 | 1 << 24;
// 알고리즘 타이머
clock_t TIMER;
// 알고리즘 타임 아웃 여부
bool TIMEOUT = false;
// 알고리즘 타임 아웃 리밋
int TIME_LIMIT = 110;
// 서치 결과 텍스트
std::string RESULT_TEXT[5];

/*
 * <bit board>
 *
 * #6 41 40 39 38 37 36 35
 * #5 34 33 32 31 30 29 28
 * #4 27 26 25 24 23 22 21
 * #3 20 19 18 17 16 15 14
 * #2 13 12 11 10  9  8  7
 * #1  6  5  4  3  2  1  0
 *    #1 #2 #3 #4 #5 #6 #7
 */

/*
 * @param [Integer] col 열
 * @param [Integer] row 행
 * @return [Integer] (열, 행) 위치의 비트 인덱스
 */
int bit_position(int col, int row) {
  return row * WIDTH - col;
}

/*
 * @param [Bitmap] bitmap_total 전체 디스크 비트맵
 * @param [Integer] col 열
 * @return [Integer] total bitmap의 col 내에서 비어있는 최상단 위치의 row 값 (1..6)
 */
int top_bit_row(uint64_t bitmap_total, int col) {
  for(int row = 1; row <= HEIGHT; row++)
    if((bitmap_total & 1ULL << bit_position(col, row)) == 0)
      return row;
  
  return 0;
}

/*
 * @param [State] state 현재 보드 상태
 * @param [Integer] col 열
 * @return [State] col 열에 새로운 디스크를 넣은 이후의 보드 상태
 */
State init_state(State state, int col) {
  State new_state;
  
  new_state.total_drops = state.total_drops + 1;
  new_state.bitmap_player = state.bitmap_player ^ state.bitmap_total;
  
  new_state.bitmap_total |= state.bitmap_total | 1UL << bit_position(col, top_bit_row(state.bitmap_total, col));
  
  return new_state;
}

/*
 * @param [State] state 현재 보드 상태
 * @param [Integer] col 수를 놓을 칼럼
 * @return [Boolean] 칼럼 위치에 디스크가 꽉 찾는지 여부
 */
bool is_not_full(State state, int col) {
  return (state.bitmap_total & (1ULL << bit_position(col, HEIGHT))) == 0;
}

/*
 * @param [State] state 현재 보드 상태
 * @param [Integer] col 수를 놓을 칼럼
 * @return [Boolean] 칼럼 위치에 수를 놓으면 connect 4가 완성되는지 여부
 */
bool is_connect4(State state, int col) {
  int top_row = top_bit_row(state.bitmap_total, col);
  uint64_t bitmap_player = state.bitmap_player | 1ULL << bit_position(col, top_row);
  
  // Horizontal
  for(int i = col-3; i <= col; i++) {
    if((i >= 1) && (i <= WIDTH - 3)) {
      int shift = bit_position(i, top_row) - 3;
      int hex = (bitmap_player >> shift) & 15;
      
      if(hex == 15) return true;
    }
  }
  
  // Vertical
  for(int i = top_row-3; i <= top_row; i++) {
    if((i >= 1) && (i <= HEIGHT - 3)) {
      int shift = bit_position(col, i);
      int hex = (bitmap_player >> shift) & VERTICAL_HEX;
      
      if(hex == VERTICAL_HEX) return true;
    }
  }
  
  // Diagonal
  for(int i = 0; i <= 3; i++) {
    int row = top_row - i;
    
    //SLASH
    int col_slash = col - i;
    if((row >= 1) && (row <= HEIGHT - 3) && (col_slash >= 1) && (col_slash <= WIDTH - 3)) {
      int shift = bit_position(col_slash, row) - 3;
      int hex = (bitmap_player >> shift) & SLASH_HEX;
      
      if(hex == SLASH_HEX) return true;
    }
    
    //BACKSLASH
    int col_backslash = col + i;
    if((row >= 1) && (row <= HEIGHT - 3) && (col_backslash >= 4) && (col_backslash <= WIDTH)){
      int shift = bit_position(col_backslash, row);
      int hex = (bitmap_player >> shift) & BACKSLASH_HEX;
      
      if(hex == BACKSLASH_HEX) return true;
    }
  }
  
  return false;
}

/*
 * 현재 플레이어 기준 보드 상태(State)의 평가함수 값을 계산하여 리턴한다
 *
 * @param [Bitmap] bitmap_player 평가함수 값을 계산할 플레이어의 디스크 비트맵
 * @param [Bitmap] bitmap_total 전체 디스크 비트맵
 * @return [Integer] 특정 플레이어 기준 평가함수 값
 */
int heuristic_score(uint64_t bitmap_player, uint64_t bitmap_total){
  int score = 0;
  
  // Horizontal
  for(int row = 1; row <= HEIGHT; row++) {
    for(int col = 1; col <= WIDTH-3; col++) {
      int shift = bit_position(col, row) - 3;
      int hex = (bitmap_player >> shift) & 15;
      
      score += WEIGHT[bits_cnt[hex]];
    }
  }
  
  // Vertical
  for(int col = 1; col <= WIDTH; col++) {
    for(int row = 1; row <= HEIGHT-3; row++) {
      int shift = bit_position(col, row);
      int hex = (bitmap_player >> shift) & VERTICAL_HEX;
      
      int horizontal_hex = (hex & 1) | (hex >> 7 & 1) << 1 | (hex >> 14 & 1) << 2 | (hex >> 21 & 1) << 3;
      score += WEIGHT[bits_cnt[horizontal_hex]];
    }
  }
  
  // Diagonal
  // Slash
  for(int row = 1; row <= HEIGHT-3; row++) {
    for(int col = 1; col <= WIDTH-3; col++) {
      int shift = bit_position(col, row) - 3;
      int hex = (bitmap_player >> shift) & SLASH_HEX;
      
      int laid_hex = (hex >> 3 & 1) | (hex >> 9 & 1) << 1 | (hex >> 15 & 1) << 2 | (hex >> 21 & 1) << 3;
      score += WEIGHT[bits_cnt[laid_hex]];
    }
  }
  
  // Backslash
  for(int row = 1; row <= HEIGHT-3; row++) {
    for(int col = 4; col <= WIDTH; col++) {
      int shift = bit_position(col, row);
      int hex = (bitmap_player >> shift) & BACKSLASH_HEX;
      
      int laid_hex = (hex & 1) | (hex >> 8 & 1) << 1 | (hex >> 16 & 1) << 2 | (hex >> 24 & 1) << 3;
      score += WEIGHT[bits_cnt[laid_hex]];
    }
  }
  
  return score;
}

/*
 * 현재 플레이어 기준 평가함수 값과 상대 플레이어 기준 평가함수 값의 차를 구하여 현재 전체 보드 상태의 평가함수 값을 도출한다.
 *
 * @param [State] state 현재 보드 상태
 * @return [Integer] 현재 보드 상태의 평가함수 값. (= 현재 플레이어 - 상대 플레이어)
 */
int evaluate(State state) {
  int eval_player = heuristic_score(state.bitmap_player, state.bitmap_total);
  int eval_opponent = heuristic_score(state.bitmap_player ^ state.bitmap_total, state.bitmap_total);
  
  return eval_player - eval_opponent;
}

// @return [Double] AI 로직 시작 이후 현재까지 경과된 시간(초)
double total_time() {
  return (double)(clock() - TIMER) / CLOCKS_PER_SEC;
}

/*
 * minmax 알고리즘을 이용하여 평가함수 값 중 최대값과 그 값을 갖는 칼럼(최적의 수)을 추산한다.
 *  alpha-beta pruning 및 평가함수 평균이 높은 중앙 칼럼부터 순차적으로 탐색하여 최적화하였음.
 *
 * @param [State] state 현재 보드 상태
 * @param [Integer] depth 현재 탐색 노드의 깊이
 * @param [Integer] alpha 프루닝 알파 값
 * @param [Integer] beta 프루닝 베타 값
 * @return [Tuple<Integer, Integer>] {최대 평가함수 값, 최대 평가함수를 갖는 칼럼}
 */
std::tuple<int, int> minmax(State state, int depth, int alpha, int beta) {
  // 알고리즘 러닝 타임이 리밋을 넘은 경우 에러 값을 리턴한다
  if(TIMEOUT) return {0, 0};
  if(depth == 5) {
    if(total_time() >= TIME_LIMIT) {
      TIMEOUT = true;
      return {evaluate(state), 0};
    }
  }
  
  // 리프 노드이거나 게임의 마지막 수인 경우 평가함수 값을 리턴한다
  if(depth == SEARCH_DEPTH || state.total_drops == TOTAL_CELLS)
    return {evaluate(state), 0};
  
  // 현재 플레이어가 다음 수로 게임을 끝낼 수 있는 경우 해당 수를 리턴한다
  for(int i = 1; i <= WIDTH; i++)
    if(is_not_full(state, i) && is_connect4(state, i))
      return {WEIGHT4, i};
  
  int alpha_col = 0;
  
  for(int i = 0; i < WIDTH; i++) {
    int col = col_order[i];
    
    if(is_not_full(state, col)) {
      // 룰 베이스 기반 AI가 두면 안되는 칼럼이면 첫 depth 조회 대상에서 제외
      if(depth == 0) {
        extern bool droppable_list[WIDTH + 1];
        if(!droppable_list[col]) {
          RESULT_TEXT[(SEARCH_DEPTH - MIN_DEPTH)/2] += "column #" + std::to_string(col) + "is passed by rule\n";
          continue;
        }
      }
      
      int score;
      
      // 특정 칼럼에 착수된 상태의 다음 보드 상태(State) 선언
      State next_state = init_state(state, col);
      
      // 다음 보드 상태를 기준으로 minmax 함수를 재귀호출
      std::tuple<int, int> result = minmax(next_state, depth+1, -beta, -alpha);
      score = -(std::get<0>(result));
      
      // alpha-beta pruning
      if(score >= beta) return {score, col};
      if(score > alpha) {
        alpha = score;
        alpha_col = col;
      }
      
      // 첫 탐색 노드에서 칼럼의 최종 score 값을 출력용 메시지 변수에 저장
      if(depth == 0) {
        RESULT_TEXT[(SEARCH_DEPTH - MIN_DEPTH)/2] += "column #" + std::to_string(col) + "'s score: " + std::to_string(score) + "\n";
      }
    }
  }
  
  return {alpha, alpha_col};
}

/*
 * @param [State] state 현재 보드 상태
 * @return [Integer] AI가 다음에 디스크를 둘 칼럼
 */
int run_ai(State state) {
  TIMER = clock();
  
  // 룰 베이스 기반 서치 및 무조건 두어야 하는 수가 있는 경우 해당 칼럼을 리턴함
  int rule_based_col = rule_based_eval();
  if(rule_based_col) return rule_based_col;
  
  std::tuple<int, int> result = {-INF, 0};
  
  // 착수된 전체 디스크 수가 15, 20개 이상인 경우 최대 탐색 깊이를 19, 21로 상향 조정
  if(state.total_drops >= 15) MAX_DEPTH = 19;
  if(state.total_drops >= 20) MAX_DEPTH = 21;
  
  // 착수된 전체 디스크 수가 20개 이하인 경우 탐색 깊이를 MIN_DEPTH 부터 MAX_DEPTH 까지 상향 조절하며 탐색한다
  for(int i = MIN_DEPTH; i <= MAX_DEPTH; i += 2) {
    SEARCH_DEPTH = i;
    
    printf("Search Started: %d Depth", SEARCH_DEPTH);
    std::tuple<int, int> temp_result = minmax(state, 0, -INF, INF);
    
    if(TIMEOUT) {
      if(SEARCH_DEPTH == MIN_DEPTH) {
        result = temp_result;
        SEARCH_DEPTH = i;
      }
      else SEARCH_DEPTH = i - 2;
      
      TIMEOUT = false;
      printf(" => Timeout: %.2f(s)\n", total_time());
      break;
    }
    else {
      result = temp_result;
      printf(" => Completed: %.2f(s)\n", total_time());
    }
  }
  
  // 각 칼럼 별 평가함수 결과 값 출력 및 출력용 메시지 변수 플러시
  printf("%s", RESULT_TEXT[(SEARCH_DEPTH - MIN_DEPTH)/2].c_str());
  for(int i = 0; i < 5; i++)
    RESULT_TEXT[i] = "";
  
  int score = std::get<0>(result);
  int col = std::get<1>(result);
  printf("=> Column: %d, Score: %d, Total time: %.2f(s)\n", col, score, total_time());
  
  return col;
}
