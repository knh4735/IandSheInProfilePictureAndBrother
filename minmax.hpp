/*
 * minmax.hpp의 헤더파일
 *
 * Created by 조현규(2014210074)
 * Copyright © 2019 Team 프사 속 그녀와 나와 형. All rights reserved.
 */
#ifndef minmax_hpp
#define minmax_hpp

#include <stdint.h>
#include "main.hpp"

// 현재 플레이어 기준 게임 보드의 상태를 저장하는 구조체 State 선언
struct State{
  int total_drops = 0;
  uint64_t bitmap_player = 0;
  uint64_t bitmap_total = 0;
};

int run_ai(State state);

State init_state(State state, int col);
State* follow_link(State* state, int col);

#endif
