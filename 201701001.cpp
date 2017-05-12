#include "stdafx.h"

/*
��Ʒ��� �������� �����ϼ���.
���и�: �����������б�
��  ��: �츮��
��  ��: ȫ�浿(��ǻ�Ͱ��а� 4�г�),...
������: 2017.5.10

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

int getScore(int value, int color)
{
	int weight = 1;
	if (color == EC)
		weight = -WEIGHT;
	switch(value) {
	case 1:
		return 10 * weight;
	case 2:
		return 40 * weight;
	case 3:
		return 160 * weight;
	case 4:
		return 640 * weight;
	case 5:
		return 10000 * weight;
	default:
		return value * weight;
	}
}

int getBoundedScore(int value, int color)
{
	int weight = 1;
	if (color == EC)
		weight = -WEIGHT;
	switch(value) {
	case 1:
		return 0 * weight;
	case 2:
		return 1 * weight;
	case 3:
		return 2 * weight;
	case 4:
		return 3 * weight;
	case 5:
		return 100000 * weight;
	default:
		return value * weight;
	}
}

int getBlockScore(int count, int block, int color)
{
	int blockWeight = 2 - block; // block�� 0~2�� ���̰� ũ�� ������ġ
	int colorWeight = 1; // ������ġ �⺻ = 1
	int returnValue = 0;
	if(color == EC)
		colorWeight = -2;
	
	if(count == 1)
		returnValue = 1 * blockWeight * colorWeight;
	else if(count == 2)
		returnValue = 2 * blockWeight * colorWeight;
	else if(count == 3)
		returnValue = 8 * blockWeight * colorWeight;
	else if(count == 4)
		returnValue = 64 * blockWeight * colorWeight;
	else if(count == 5)
		returnValue = 1024 * blockWeight * colorWeight;
	else {
		printf("���� ���� ����\n");
		returnValue = count * blockWeight * colorWeight;
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
	int boardValue(int x, int y)
	{
		// ��ǥ�� �ִ� ���� ��ȯ(BLACK, WHITE, EMPTY)
		if((0 <= x && x < MAXXY) && (0 <= y && y < MAXXY)) // 0 ~ 15
			return board[y][x];
		else
			return OUTBD;
	}
	void setEval(int _eval)
	{
		eval = _eval;
	}
	int getEval()
	{
		return eval;
	}
	void addXY(int x, int y, int color)
	{
		// �� ���� ���� newX, newY�� ������Ʈ�Ѵ�.
		board[y][x] = color;
		new_x = x;
		new_y = y;
	}
	void setXY(int x, int y)
	{
		new_x = x;
		new_y = y;
	}
	void getXY(int &x, int &y)
	{
		x = new_x;
		y = new_y;
	}
	void evaluation() // evaluation function
	{
		int x, y, i;
		int tempEval = 0;
		// �ٵ��ǿ� ��ġ�� ��� ���� ���� �˻��Ѵ�.
		for (y = 0; y < MAXXY; y++) {
			for (x = 0; x < MAXXY; x++) {
				int currentColor = board[y][x]; // ���� ��ġ�� ����
				if (currentColor == WHITE && currentColor == BLACK) { // �� ������ �ƴ� ���
					int enemyColor = (currentColor == WHITE) ? BLACK : WHITE;

					int tempValue = 1;
					bool isBlocked = false;
					for (i = 1; i <= 4; i++) { // up
						if (y - i < 0) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y - i][x] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y - i][x] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);


					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // up-right
						if (y - i < 0 || x + i >= MAXXY) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y - i][x + i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y - i][x + i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // right
						if (x + i >= MAXXY) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y][x + i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y][x + i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // right-down
						if (x + i >= MAXXY || y + i >= MAXXY) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y + i][x + i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y + i][x + i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // down
						if (y + i >= MAXXY) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y + i][x] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y + i][x] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // down-left
						if (y + i >= MAXXY || x - i < 0) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y + i][x - i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y + i][x - i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // left
						if (x - i < 0) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y][x - i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y][x - i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);

					tempValue = 1;
					isBlocked = false;
					for (i = 1; i <= 4; i++) { // left - up
						if (x - i < 0 || y - i < 0) { // �ٵ��� �ٱ����� �Ѿ��
							isBlocked = true;
							break;
						}
						if (board[y - i][x - i] != currentColor) {
							isBlocked = true;
							break;
						}
						if (board[y - i][x - i] == currentColor) // �� ���̸� �ӽ����� + 1
							tempValue++;
					}
					if (isBlocked)
						tempEval += getBoundedScore(tempValue, currentColor);
					else
						tempEval += getScore(tempValue, currentColor);
				}
			}
		}
		eval = tempEval;
	}

	void evaluation2()
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
		eval = tempEval;
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
		node.evaluation2();
		int temp = node.getEval();
		return node;
	}

	int i, j;

	// ��ȯ�� ���� �޾ƾ� �� - �������� ��ȯ�� ���ؼ�
	// ��ȯ���� ��带 ���� ��忡 ���� ������ �ʿ�� ���� - �ʿ��Ѱ� eval��
	// xy���� ��ȯ���� ����� ���� ����ϸ� �� �ǰ� �� �Լ��� ij�� ����ؾ���

	if (maximizingPlayer) { // (�ִ밪�� ����) �� ���� ���
		int bestValue = -99999;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// �ش� ĭ�� ��ĭ�� ��쿡�� Ʈ���� ����
				if (node.boardValue(i, j) == EMPTY) {
					Board nextNode = node; // minmax �Լ��� �� �ӽ� ���
					nextNode.addXY(i, j, MC); // xy ����� �� ����

					// ��ȯ���� ����� eval���� ����ϰ� �������� ����(������ x, y���� ù �Լ��� ���� ��ȯ�ؾ��ϱ⶧����)
					int v = minimax(nextNode, depth - 1, alpha, beta, false).getEval();

					bestValue = maxNode(bestValue, v);
					if (bestValue == v) {
						bestX = i;
						bestY = j;
					}
					/*
					// alpha-beta pruning
					alpha = maxNode(alpha, bestValue);
					if (beta <= alpha) {
						node.setEval(bestValue);
						node.setXY(bestX, bestY);
						return node;
					}
					*/
				}
			}
		}
		// node ������ eval, new_x, new_y ���� ����
		node.setEval(bestValue);
		node.setXY(bestX, bestY);
		return node;
	}
	else { // minimizingPlayer
		int bestValue = 99999;
		int bestX = 0, bestY = 0;
		for (i = 0; i < MAXXY; i++) {
			for (j = 0; j < MAXXY; j++) {
				// �ش� ĭ�� ��ĭ�� ��쿡�� Ʈ���� ����
				if (node.boardValue(i, j) == EMPTY) {
					Board nextNode = node; // minmax �Լ��� �� �ӽ� ���
					nextNode.addXY(i, j, EC); // �ӽ� ����� xy ��ǥ�� ���� ����

					int v = minimax(nextNode, depth - 1, alpha, beta, true).getEval();

					bestValue = minNode(bestValue, v);
					if (bestValue == v) {
						bestX = i;
						bestY = j;
					}
					/*
					// alpha-beta pruning
					beta = minNode(beta, bestValue);
					if (beta <= alpha) {
						node.setEval(bestValue);
						node.setXY(bestX, bestY);
						return node;
					}
					*/
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

	Board current_board;
	current_board.initBoard(); // ���� ������ �ʱ�ȭ

	int alpha = -99999, beta = 99999;
	Board new_board = minimax(current_board, 1, alpha, beta, true);

	new_board.getXY(*NewX, *NewY);

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
