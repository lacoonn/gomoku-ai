#include "stdafx.h"

/*
��Ʒ��� �������� �����ϼ���.
���и�: ��ϴ��б�
��  ��: ���Ϻ�����ġ
��  ��: �����(��ǻ���к� 3�г�)
������: 2017.5.10
�����ڵ�: 22702680

���ۼ� �� ���ǻ���
- C++�⺻ ������ ���Ͽ� �ۼ�
- �ܺ� ���̺귯�� ��� ���� (dll, STL��)
- for�� �ε��� ������ �ܺο� �̸� ������ �� (�Ʒ��� ���� ���� �� ��)
for (int i = 0; i < 10...
- ���� ���� global����, �Լ�, class�� ���� ����, �� �ٸ� ���� �ߺ����� �ʵ��� ���
- ����� �� ������ �Լ� f201701001()�� �ش� ����ȣ�� �����ϰ� "����ȣ.cpp"�� �����Ͽ� ����
*/

// ��� ����
#define MAXXY 16
#define BLACK 1
#define WHITE 0
#define EMPTY -1
#define OUTBD -2

#define WEIGHT 2

// B�Լ��� �������� �ش� ��ġ�� ��Ȳ�� ����� �� ��, ����ϴ� �Լ��� ������ BLACK, WHITE, EMPTY, OUTBD�� �� ���� ������
// B(-3,6)�� ������ �ٱ��� �����ϹǷ� OUTBD(-2)�� ���ϰ����� ����
// B(-3,6,BLACK)�� ������ ���� �����ϳ�, ���ϰ��� def������ �Ѱ��� BLACK�� ���ϰ����� ����
extern int B(int x, int y, int def = OUTBD);

// AI �ۼ���, B(int x,int y,int)�Լ��� ���� �Ǽ��� �м��ϰ� ���� �� ��ġ ���� ���� *NewX, *NewY�� ���� ȣ�� �Լ��� �ǵ�����.
// ������ ���� ���� ���η� ���� 2�� �ְ� �� �������� ����ִ� ���� ã�� ���� �δ� AI�� ���̴�.
//
// NewX,NewY : AI�� ���� ���� �ΰ��� �ϴ� ���� ���� ���� ������ ����
// mc : AI�� �� ���� �� (BLACK �Ǵ� WHITE)
// CurTurn : ���� ����� ��
// 4�� ���������� ���� ������ ����, COmDevDlg::IsGameOver() �Լ��� ���� ����

static int MC;
static int EC;

int getBlockScore(int count, int block, int color)
{
	int colorWeight = 1; // ������ġ �⺻ = 1
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
			returnValue = 48 * colorWeight; // 3������ 1�� �����ſ� �� �������� ���̴� ũ��
		if(block == 1)
			returnValue = 16 * colorWeight;
	}
	else if(count == 4) {
		if(block == 0)
			returnValue = 192 * colorWeight; // ��ǻ� ���� ��
		if(block == 1)
			returnValue = 128 * colorWeight;
	}
	else { // count == 5
		returnValue = 1920 * colorWeight; // �׳� ���� ��
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
	int boardValue(int x, int y) // ��ǥ�� �ִ� ���� ��ȯ(BLACK, WHITE, EMPTY)
	{
		if((0 <= x && x < MAXXY) && (0 <= y && y < MAXXY)) // 0 ~ 15
			return board[y][x];
		else
			return OUTBD;
	}
	bool isNear(const int x, const int y) // �ش� ��ġ �ֺ��� ���� �ִ����� �Ǵ�
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
	bool addXY(int _x, int _y, int color) // �� ���� ���� newX, newY�� ������Ʈ�Ѵ�, 5��¥�� ���� ������ true ����
	{
		board[_y][_x] = color;
		new_x = _x;
		new_y = _y;
		// �ٵϾ��� �����鼭 5���� �Ǵ��� Ȯ��
		int x, y, i;
		for (y = -1; y <= MAXXY; y++) {
			for (x = -1; x <= MAXXY; x++) {
				int current = boardValue(x, y); // ���� ��ġ�� ����
				int myCount, enemy;
				int right = boardValue(x + 1, y);
				int rightDown = boardValue(x + 1, y + 1);
				int down = boardValue(x, y + 1);
				int downLeft = boardValue(x - 1, y + 1);

				if ((right == color) && current != right) { // right�� �ٵϾ��̸鼭 ���� ��ġ�� ���¿� �ٸ� ��
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
				if ((rightDown == color) && current != rightDown) { // right-down�� �ٵϾ��̸鼭 ���� ��ġ�� ���¿� �ٸ� ��
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
				if ((down == color) && current != down) { // down�� �ٵϾ��̸鼭 ���� ��ġ�ʹ� �ٸ� ��
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
				if ((downLeft == color) && current != downLeft) { // down-left�� �ٵϾ��̸鼭 ���� ��ġ�ʹ� �ٸ� ��
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
		// �ٵ��ǿ� ��ġ�� ��� ���� ���� �˻��Ѵ�.
		for (y = -1; y <= MAXXY; y++) {
			for (x = -1; x <= MAXXY; x++) {
				int current = boardValue(x, y); // ���� ��ġ�� ����
				int myCount, blockCount, enemy;
				int right = boardValue(x + 1, y);
				int rightDown = boardValue(x + 1, y + 1);
				int down = boardValue(x, y + 1);
				int downLeft = boardValue(x - 1, y + 1);

				if ((right == WHITE || right == BLACK) && current != right) { // right�� �ٵϾ��̸鼭 ���� ��ġ�� ���¿� �ٸ� ��
					enemy = (right == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // �¿쿡 ���� ����(�ִ� 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // �������� �ٵ��� �ܺ��̰ų� ���̶�� blockCount�� 1�� ����(�̹� �� ���� �������Ƿ�)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // ã�� ��Ұ� �ٵ��� ���̰ų� ���̸� blockCount�� +1 �ϰ� break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, right);
				}
				if ((rightDown == WHITE || rightDown == BLACK) && current != rightDown) { // right-down�� �ٵϾ��̸鼭 ���� ��ġ�� ���¿� �ٸ� ��
					enemy = (rightDown == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // �¿쿡 ���� ����(�ִ� 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // �������� �ٵ��� �ܺ��̰ų� ���̶�� blockCount�� 1�� ����(�̹� �� ���� �������Ƿ�)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x + i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // ã�� ��Ұ� �ٵ��� ���̰ų� ���̸� blockCount�� +1 �ϰ� break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, rightDown);
				}
				if ((down == WHITE || down == BLACK) && current != down) { // down�� �ٵϾ��̸鼭 ���� ��ġ�ʹ� �ٸ� ��
					enemy = (down == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // �¿쿡 ���� ����(�ִ� 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // �������� �ٵ��� �ܺ��̰ų� ���̶�� blockCount�� 1�� ����(�̹� �� ���� �������Ƿ�)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // ã�� ��Ұ� �ٵ��� ���̰ų� ���̸� blockCount�� +1 �ϰ� break
							break;
						}
						if(tempVal == EMPTY)
							break;
						myCount++;
					}
					tempEval += getBlockScore(myCount, blockCount, down);
				}
				if ((downLeft == WHITE || downLeft == BLACK) && current != downLeft) { // down-left�� �ٵϾ��̸鼭 ���� ��ġ�ʹ� �ٸ� ��
					enemy = (downLeft == WHITE) ? BLACK : WHITE;
					myCount = 1;
					blockCount = 0; // �¿쿡 ���� ����(�ִ� 2)
					if(current == OUTBD || current == enemy)
						blockCount = 1; // �������� �ٵ��� �ܺ��̰ų� ���̶�� blockCount�� 1�� ����(�̹� �� ���� �������Ƿ�)
					for(i = 2; i <= 5; i++) {
						int tempVal = boardValue(x - i, y + i);
						if(tempVal == OUTBD || tempVal == enemy) {
							blockCount++; // ã�� ��Ұ� �ٵ��� ���̰ų� ���̸� blockCount�� +1 �ϰ� break
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

	// ��ȯ�� ���� �޾ƾ� �� - �������� ��ȯ�� ���ؼ�
	// ��ȯ���� ��带 ���� ��忡 ���� ������ �ʿ�� ���� - �ʿ��Ѱ� eval��
	// xy���� ��ȯ���� ����� ���� ����ϸ� �� �ǰ� �� �Լ��� ij�� ����ؾ���

	if (maximizingPlayer) { // (�ִ밪�� ����) �� ���� ���
		int bestValue = -99999999, v;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// �ش� ĭ�� ��ĭ�� ��쿡�� Ʈ���� ����
				if (node.boardValue(i, j) == EMPTY && node.isNear(i, j)) {
					Board nextNode = node; // minmax �Լ��� �� �ӽ� ���

					if(nextNode.addXY(i, j, MC)) { // xy ����� �� ����(5��¥�� ���� ������ true)
						nextNode.evaluation(depth);
						v = nextNode.getEval();
						printf("five in a raw\n");
					}
					else {
						// ��ȯ���� ����� eval���� ����ϰ� �������� ����(������ x, y���� ù �Լ��� ���� ��ȯ�ؾ��ϱ⶧����)
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
		// node ������ eval, new_x, new_y ���� ����
		node.setEval(bestValue);
		node.setXY(bestX, bestY);
		return node;
	}
	else { // minimizingPlayer
		int bestValue = 99999999, v;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// �ش� ĭ�� ��ĭ�� ��쿡�� Ʈ���� ����
				if (node.boardValue(i, j) == EMPTY && node.isNear(i, j)) {
					Board nextNode = node; // minmax �Լ��� �� �ӽ� ���

					if(nextNode.addXY(i, j, EC)) { // xy ����� �� ����(5��¥�� ���� ������ true)
						nextNode.evaluation(depth);
						v = nextNode.getEval();
						printf("five in a raw\n");
					}
					else {
						// ��ȯ���� ����� eval���� ����ϰ� �������� ����(������ x, y���� ù �Լ��� ���� ��ȯ�ؾ��ϱ⶧����)
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
	int ec = (mc == WHITE) ? BLACK : WHITE;	//	���� �� Ȯ��
	MC = mc;
	EC = ec;

	srand(time(NULL));

	Board current_board;
	current_board.initBoard(); // ���� ������ �ʱ�ȭ

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
