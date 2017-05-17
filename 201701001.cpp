#include "stdafx.h"

/*
■아래에 팀정보를 기재하세요.
대학명: 경북대학교
팀  명: 내일부터패치
팀  원: 김우찬(컴퓨터학부 3학년)
제출일: 2017.5.10
제출코드: 22702680

■작성 시 주의사항
- C++기본 문법에 의하여 작성
- 외부 라이브러리 사용 금지 (dll, STL등)
- for문 인덱스 변수는 외부에 미리 선언할 것 (아래와 같이 하지 말 것)
for (int i = 0; i < 10...
- 파일 내에 global변수, 함수, class등 선언 가능, 단 다른 팀과 중복되지 않도록 명명
- 제출시 본 파일의 함수 f201701001()를 해당 팀번호로 수정하고 "팀번호.cpp"로 저장하여 제출
*/

// 상수 지정
#define MAXXY 16
#define BLACK 1
#define WHITE 0
#define EMPTY -1
#define OUTBD -2

#define WEIGHT 2

// B함수는 오목판의 해당 위치의 상황을 얻고자 할 때, 사용하는 함수로 리턴은 BLACK, WHITE, EMPTY, OUTBD중 한 값을 리턴함
// B(-3,6)는 오목판 바깥을 지시하므로 OUTBD(-2)를 리턴값으로 얻음
// B(-3,6,BLACK)는 오목판 밖을 지시하나, 리턴값은 def값으로 넘겨준 BLACK를 리턴값으로 얻음
extern int B(int x, int y, int def = OUTBD);

// AI 작성시, B(int x,int y,int)함수를 통해 판세를 분석하고 알을 둘 위치 값을 인자 *NewX, *NewY를 통해 호출 함수로 되돌린다.
// 다음은 적의 알이 가로로 연속 2개 있고 그 오른쪽이 비어있는 곳을 찾아 알을 두는 AI의 예이다.
//
// NewX,NewY : AI에 의해 알을 두고자 하는 값에 대한 리턴 포인터 변수
// mc : AI가 둘 알의 색 (BLACK 또는 WHITE)
// CurTurn : 현재 진행된 수
// 4개 방향으로의 전술 적용을 위해, COmDevDlg::IsGameOver() 함수의 구현 참조

static int MC;
static int EC;

int getBlockScore(int count, int block, int color)
{
	int colorWeight = 1; // 색가중치 기본 = 1
	int returnValue = 0;
	if(color == EC)
		colorWeight = -10;

	if(count == 1) {
		if(block == 0)
			returnValue = 3 * colorWeight;
		if(block == 1)
			returnValue = 1 * colorWeight;
	}
	else if(count == 2) {
		if(block == 0)
			returnValue = 12 * colorWeight;
		if(block == 1)
			returnValue = 4 * colorWeight;
	}
	else if(count == 3) {
		if(block == 0)
			returnValue = 48 * colorWeight; // 3개에서 1쪽 막힌거와 안 막힌거의 차이는 크다
		if(block == 1)
			returnValue = 16 * colorWeight;
	}
	else if(count == 4) {
		if(block == 0)
			returnValue = 192 * colorWeight; // 사실상 게임 끝
		if(block == 1)
			returnValue = 128 * colorWeight;
	}
	else { // count == 5
		returnValue = 1920 * colorWeight; // 그냥 게임 끝
	}
	return returnValue;
}
class Board {
private:
	int board[MAXXY][MAXXY];
	int new_x;
	int new_y;
	int eval;

public:
	Board()
	{
		eval = 0;
	}
	void initBoard()
	{
		int i, j;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				board[j][i] = B(i, j);
			}
		}
	}
	int boardValue(int x, int y) // 좌표에 있는 값을 반환(BLACK, WHITE, EMPTY)
	{
		if((0 <= x && x < MAXXY) && (0 <= y && y < MAXXY)) // 0 ~ 15
			return board[y][x];
		else
			return OUTBD;
	}
	bool isNear(const int x, const int y) // 해당 위치 주변에 돌이 있는지를 판단
	{
		int _y, _x, dist = 2;
		for(_y = y - dist; _y <= y + dist; _y++) {
			for(_x = x - dist; _x <= x + dist; _x++) {
				if(boardValue(_x, _y) == WHITE || boardValue(_x, _y) == BLACK)
					return true;
			}
		}
		return false;
	}
	void setEval(int _eval)
	{
		eval = _eval;
	}
	int getEval()
	{
		return eval;
	}
	bool addXY(int _x, int _y, int color) // 새 말을 놓고 newX, newY를 업데이트한다, 5개짜리 돌이 나오면 true 리턴
	{
		board[_y][_x] = color;
		new_x = _x;
		new_y = _y;
		// 바둑알을 놓으면서 5개가 되는지 확인
		int x, y, i;
		for (y = -1; y <= MAXXY; y++) {
			for (x = -1; x <= MAXXY; x++) {
				int current = boardValue(x, y); // 현재 위치의 상태
				int myCount, enemy;
				int right = boardValue(x + 1, y);
				int rightDown = boardValue(x + 1, y + 1);
				int down = boardValue(x, y + 1);
				int downLeft = boardValue(x - 1, y + 1);

				if ((right == color) && current != right) { // right가 바둑알이면서 현재 위치의 상태와 다를 때
					enemy = (right == WHITE) ? BLACK : WHITE;
					myCount = 1;
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y);
						if(tempVal == OUTBD || tempVal == enemy) {
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					if(myCount == 5)
						return true;
				}
				if ((rightDown == color) && current != rightDown) { // right-down가 바둑알이면서 현재 위치의 상태와 다를 때
					enemy = (rightDown == WHITE) ? BLACK : WHITE;
					myCount = 1;
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					if(myCount == 5)
						return true;
				}
				if ((down == color) && current != down) { // down가 바둑알이면서 현재 위치와는 다를 때
					enemy = (down == WHITE) ? BLACK : WHITE;
					myCount = 1;
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					if(myCount == 5)
						return true;
				}
				if ((downLeft == color) && current != downLeft) { // down-left가 바둑알이면서 현재 위치와는 다를 때
					enemy = (downLeft == WHITE) ? BLACK : WHITE;
					myCount = 1;
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x - i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					if(myCount == 5)
						return true;
				}
			}
		}
		return false;
	}
	void setXY(int x, int y)
	{
		new_x = x;
		new_y = y;
	}
	void getXY(int *x, int *y)
	{
		*x = new_x;
		*y = new_y;
	}
	void evaluation(int depth)
	{
		int x, y, i;
		int tempEval = 0;
		// 바둑판에 배치된 모든 말에 대해 검사한다.
		for (y = -1; y <= MAXXY; y++) {
			for (x = -1; x <= MAXXY; x++) {
				int current = boardValue(x, y); // 현재 위치의 상태
				int myCount, blockCount, enemy;
				int right = boardValue(x + 1, y);
				int rightDown = boardValue(x + 1, y + 1);
				int down = boardValue(x, y + 1);
				int downLeft = boardValue(x - 1, y + 1);

				if ((right == WHITE || right == BLACK) && current != right) { // right가 바둑알이면서 현재 위치의 상태와 다를 때
					enemy = (right == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // 좌우에 막힌 개수(최대 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // 시작점이 바둑판 외부이거나 적이라면 blockCount가 1로 시작(이미 한 면이 막혔으므로)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // 찾은 장소가 바둑판 밖이거나 적이면 blockCount를 +1 하고 break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, right);
				}
				if ((rightDown == WHITE || rightDown == BLACK) && current != rightDown) { // right-down가 바둑알이면서 현재 위치의 상태와 다를 때
					enemy = (rightDown == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // 좌우에 막힌 개수(최대 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // 시작점이 바둑판 외부이거나 적이라면 blockCount가 1로 시작(이미 한 면이 막혔으므로)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // 찾은 장소가 바둑판 밖이거나 적이면 blockCount를 +1 하고 break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, rightDown);
				}
				if ((down == WHITE || down == BLACK) && current != down) { // down가 바둑알이면서 현재 위치와는 다를 때
					enemy = (down == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // 좌우에 막힌 개수(최대 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // 시작점이 바둑판 외부이거나 적이라면 blockCount가 1로 시작(이미 한 면이 막혔으므로)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // 찾은 장소가 바둑판 밖이거나 적이면 blockCount를 +1 하고 break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, down);
				}
				if ((downLeft == WHITE || downLeft == BLACK) && current != downLeft) { // down-left가 바둑알이면서 현재 위치와는 다를 때
					enemy = (downLeft == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // 좌우에 막힌 개수(최대 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // 시작점이 바둑판 외부이거나 적이라면 blockCount가 1로 시작(이미 한 면이 막혔으므로)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x - i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // 찾은 장소가 바둑판 밖이거나 적이면 blockCount를 +1 하고 break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, downLeft);
				}
			}
		}
		eval = tempEval * depth * 2;
	}
};

int maxNode(int a, int b)
{
	if (a < b)
		return b;
	else
		return a;
}

int minNode(int a, int b)
{
	if (a > b)
		return b;
	else
		return a;
}

Board minimax(Board node, int depth, int alpha, int beta, bool maximizingPlayer)
{
	if (depth == 0) {
		node.evaluation(1);
		return node;
	}

	int i, j;

	// 반환은 노드로 받아야 함 - 최종적인 반환을 위해서
	// 반환받은 노드를 현재 노드에 굳이 대입할 필요는 없음 - 필요한건 eval값
	// xy값은 반환받은 노드의 것을 사용하면 안 되고 현 함수의 ij를 사용해야함

	if (maximizingPlayer) { // (최대값을 고르는) 내 턴일 경우
		int bestValue = -99999999, v;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// 해당 칸이 빈칸인 경우에만 트리를 생성
				if (node.boardValue(i, j) == EMPTY && node.isNear(i, j)) {
					Board nextNode = node; // minmax 함수에 들어감 임시 노드

					if(nextNode.addXY(i, j, MC)) { // xy 경우의 수 대입(5개짜리 돌이 있으면 true)
						nextNode.evaluation(depth);
						v = nextNode.getEval();
						printf("five in a raw\n");
					}
					else {
						// 반환받은 노드의 eval값만 사용하고 나머지는 버림(어차피 x, y값은 첫 함수의 값을 반환해야하기때문에)
						v = minimax(nextNode, depth - 1, alpha, beta, false).getEval();
					}
					bestValue = maxNode(bestValue, v);
					if (bestValue == v) {
						bestX = i;
						bestY = j;
					}

					// alpha-beta pruning
					alpha = maxNode(alpha, bestValue);
					if (beta <= alpha) {
						node.setEval(bestValue);
						node.setXY(bestX, bestY);
						return node;
					}

				}
			}
		}
		// node 변수의 eval, new_x, new_y 값을 설정
		node.setEval(bestValue);
		node.setXY(bestX, bestY);
		return node;
	}
	else { // minimizingPlayer
		int bestValue = 99999999, v;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// 해당 칸이 빈칸인 경우에만 트리를 생성
				if (node.boardValue(i, j) == EMPTY && node.isNear(i, j)) {
					Board nextNode = node; // minmax 함수에 들어감 임시 노드

					if(nextNode.addXY(i, j, EC)) { // xy 경우의 수 대입(5개짜리 돌이 있으면 true)
						nextNode.evaluation(depth);
						v = nextNode.getEval();
						printf("five in a raw\n");
					}
					else {
						// 반환받은 노드의 eval값만 사용하고 나머지는 버림(어차피 x, y값은 첫 함수의 값을 반환해야하기때문에)
						v = minimax(nextNode, depth - 1, alpha, beta, true).getEval();
					}
					bestValue = minNode(bestValue, v);
					if (bestValue == v) {
						bestX = i;
						bestY = j;
					}

					// alpha-beta pruning
					beta = minNode(beta, bestValue);
					if (beta <= alpha) {
						node.setEval(bestValue);
						node.setXY(bestX, bestY);
						return node;
					}
				}
			}
		}
		node.setEval(bestValue);
		node.setXY(bestX, bestY);
		return node;
	}
}

void f201701001(int *NewX, int *NewY, int mc, int CurTurn)
{
	int ec = (mc == WHITE) ? BLACK : WHITE;	//	적의 색 확인
	MC = mc;
	EC = ec;

	srand(time(NULL));

	Board current_board;
	current_board.initBoard(); // 현재 판으로 초기화

	int alpha = -99999999, beta = 99999999;
	Board new_board = minimax(current_board, 2, alpha, beta, true);

	new_board.getXY(NewX, NewY);

	if (CurTurn == 0) {
		*NewX = rand() % MAXXY;
		*NewY = rand() % MAXXY;
	}
	while(1) {
		if (B(*NewX, *NewY) == EMPTY) {
			break;
		}
		*NewX = rand() % MAXXY;
		*NewY = rand() % MAXXY;
	}

}
