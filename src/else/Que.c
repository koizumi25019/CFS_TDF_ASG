//------------------------------------------------------------------------
//インクルード
//------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#include<string.h>

//------------------------------------------------------------------------
//自作インクルード
//------------------------------------------------------------------------
#include"../header/Define.h"

//=========================================================================
//  関数名 : enqueue
//  機  能 : キューにNLISTを入れる
//  戻り値 : なし
//  引  数 : NLIST，タイプ（0：指定された信号線のみ，1:出力線全部，2：入力線全部）
//=========================================================================
void enqueue(NLIST *data, int mode) {

	int i;

	switch (mode) {

	case 0:
		Que.que[Que.rear] = data;
		Que.num += 1;
		Que.rear += 1;
		if (Que.max == Que.rear) {
			Que.rear = 0;
		}
		if (Que.max < Que.num) {
			printf("Que：Over Flow\n");
			exit(EXIT_FAILURE);
		}
		break;

	case 1:
		for (i = 0; i < data->n_out; i++) {
			Que.que[Que.rear] = data->out[i];
			Que.num += 1;
			Que.rear += 1;
			if (Que.max == Que.rear) {
				Que.rear = 0;
			}
			if (Que.max < Que.num) {
				printf("Que：Over Flow\n");
				exit(EXIT_FAILURE);
			}
		}
		break;

	case 2:
		for (i = 0; i < data->n_in; i++) {
			Que.que[Que.rear] = data->in[i];
			Que.num += 1;
			Que.rear += 1;
			if (Que.max == Que.rear) {
				Que.rear = 0;
			}
			if (Que.max < Que.num) {
				printf("Que：Over Flow\n");
				exit(EXIT_FAILURE);
			}
		}
		break;

	}

}

//=========================================================================
//  関数名 : dequeue
//  機  能 : キューから1つNLISTを取り出す
//  戻り値 : NLIST
//  引  数 : なし
//=========================================================================
NLIST* dequeue() {

	NLIST *p;

	p = *(Que.que + Que.front);

	*(Que.que + Que.front) = NULL;

	Que.num -= 1;
	Que.front += 1;

	if (Que.max == Que.front) {
		Que.front = 0;
	}

	return p;

}


//=========================================================================
//  関数名 : queue_emp
//  機  能 : スタックが空か判定
//  戻り値 : 0（無），1（有）
//  引  数 : なし
//=========================================================================
int queue_emp() {

	if (Que.front == Que.rear) {
		return 0;
	}

	return 1;

}