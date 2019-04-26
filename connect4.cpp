#include <stdio.h>
#include <string.h>

#define W 7
#define H 6

const char SYMBOL[2] = {'O', 'X'};
char board[7][8];
int col_h[8];	//Height of each column
int total;		//Total number of stone in a game
int ai_turn;	//Whether AI's turn

//Clear Console
void clear(){printf("\e[1;1H\e[2J");}


//Initialize board and game variables
void init(){
	//Init Board
	memset(board, ' ', sizeof(board));
	for(int i = 0; i < 7; i++) board[i][0] = i + '0';
	for(int i = 1; i < 8; i++) board[0][i] = i + '0';

	//Init column height index
	for(int i = 0; i < 8; i++) col_h[i] = 1;

	total = 0;

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


//Get Player's input
//Param : prev_pos (int) - Previous Position
int get_input(int prev_pos){
	int pos;
	do{
		while(getchar() != '\n');
		if(total > 0) printf("%c : %d, %d\n", SYMBOL[ai_turn^1], prev_pos, col_h[prev_pos]-1);
		printf("Select Position [1, 7] : ");
	}while(scanf("%d", &pos) < 1 || pos < 1 || pos > W || col_h[pos] > H);
	return pos;
}

int get_ai_pos(int pos){
	return get_input(pos);
}


//ADD stone on board and DRAW Whole board
//Param : pos (int) - position of new stone
void draw_board(int pos){
	clear();
	board[col_h[pos]++][pos] = SYMBOL[ai_turn];

	for(int i = H; i >= 0; i--){
		for(int j = 0; j <= W; j++) printf("%c ", board[i][j]);
		printf("\n");
	}
}


//Test if game is end
//Param : pos (int) - position of new stone
int check_end(int pos){
	int c = pos,
		r = col_h[pos] - 1;

	//Row 
	for(int i = c-3; i <= c; i++)
		if(i > 0 && i+3 <= W && 
			board[r][i] == board[r][i+1] && board[r][i+1] == board[r][i+2] && board[r][i+2] == board[r][i+3])
			
			return 1;

	//Colomn
	for(int i = r-3; i <= r; i++)
		if(i > 0 && i+3 <= H &&
			board[i][c] == board[i+1][c] && board[i+1][c] == board[i+2][c] && board[i+2][c] == board[i+3][c])
			return 1;

	//Diagonal
	//ri : row index, ci : column index (increase for /), cj : column index (decrease for \)
	int ri = r - 3, ci = c - 3, cj = c + 3;
	for(int i = 0; i <= 3; i++){
		if(ri > 0 && ci > 0 && ri+3 <= H && ci+3 <= W && 
			board[ri][ci] == board[ri+1][ci+1] && board[ri+1][ci+1] == board[ri+2][ci+2] && board[ri+2][ci+2] == board[ri+3][ci+3])
			return 1;

		if(ri > 0 && cj-3 > 0 && ri+3 <= H && cj <= W && 
			board[ri][cj] == board[ri+1][cj-1] && board[ri+1][cj-1] == board[ri+2][cj-2] && board[ri+2][cj-2] == board[ri+3][cj-3])
			return 1;

		ri++; ci++; cj--;
	}

	return 0;
}



void connect4(){
	int pos;	//position of new stone
	
	while(total < W * H){
		if(ai_turn) pos = get_ai_pos(pos);
		else pos = get_input(pos);

		draw_board(pos);

		if(check_end(pos)) break;

		ai_turn ^= 1;
		total++;
	}

	if(total == W * H) printf("DRAW!!\n");
	else printf("%s win!!\n", ai_turn ? "AI" : "Human");
}


int main(){
	init();
	connect4();

	return 0;
}