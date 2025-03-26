//===================================================================
//Program : make essential assignemnt
//Designer : T.Aono
//Date : 2025/03/23
//===================================================================


//===================================================================
//include
//===================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pbo.h"
#include "../netlist/Netlist.h"
#include "../lib/lib.h"
#include "../TP/tp.h"

//定数定義
#define FLAG_UP 1 //含意操作用. 一意活性化処理の時は、信号線がキューに入っていることを表す
#define FLAG_DOWN 0
#define FLAG_MIDDLE 2 //一活性化のとき、非制御値を割当てない信号線.
					  //一意活性化探索の際、一度通った信号線

//グローバル変数宣言
QUE						   que2; //必須割当て解析でのみ使用するキュー

//プロトタイプ宣言
void unique_sentization(
	NLIST* t_net
);

void assign_unique_value(
	NLIST* t_net,
	int idx
);

void back_implication(
	NLIST* t_net
);

void forward_implication(
	NLIST* t_net
);

void InitQUE2(
	void
);

void que2ENQ(
	NLIST* netptr,			  /** pointer to netlist */
	int	 mode				  /** mode */
);

NLIST* que2DEQ(
	void
);

void uni_Qsort(
	int					left_most,				/* left-most value */
	int					right_most				/* right-most value */
);

//===============================================
//name : Assignment_Extraction
//function : 値割当て解析
//return : void
//===============================================
void Assignment_Extraction(
	Fault* t_fault //対象故障
)
{
	//キューを初期化
	que.maxnum = n_net;
	que.front = 0;
	que.num = 0;
	que.rear = 0;
	InitQUE2();

	//対象信号線をエンキュー
	que2ENQ(t_fault->net, QUE_MODE_ONE);
	t_fault->net->unique_flag = FLAG_UP;

	//一意活性化
	unique_sentization(t_fault);

	//対象信号線に値割当て
	if (0 == t_fault->ftype)
	{
		t_fault->net->logic_value = 1;
	}
	else
	{
		t_fault->net->logic_value = 0;
	}

	//対象信号線をエンキュー
	queENQ(t_fault->net, QUE_MODE_ONE);

	while (0 != que.num)
	{
		NLIST* t_net = queDEQ();

		//後方含意
		back_implication(t_net);

		//前方含意
		forward_implication(t_net);
	}

	return;
}

//===============================================
//name : unique_sentization
//function : 一意活性化(KIT)
//return : void
//===============================================
void unique_sentization(
	NLIST* t_net
)
{
	//故障設置信号線には別処理
	NLIST* tmp_net = que2DEQ();
	tmp_net->unique_flag = FLAG_MIDDLE;
	for (int i = 0; i < tmp_net->n_out; i++)
	{
		if (DFF != tmp_net->out[i]->type && FLAG_DOWN == tmp_net->out[i]->unique_flag)
		{
			que2ENQ(tmp_net->out[i], QUE_MODE_ONE);
			tmp_net->out[i]->unique_flag = FLAG_UP;
		}
	}
	//レベルソート
	if (1 < que2.num)
	{
		uni_Qsort(que2.front, que2.rear - 1);
	}

	while (0 != que2.num)
	{
		tmp_net = que2DEQ();
		tmp_net->unique_flag = FLAG_MIDDLE;
		//デキューした信号線は一意活性化対象信号線
		if (0 == que2.num)
		{
			//非制御値を割当てない信号線を探索
			for (int idx = 0; idx < tmp_net->n_in; idx++)
			{
				if (FLAG_MIDDLE == tmp_net->in[idx]->unique_flag)
				{
					//値割当て&キュー１にエンキュー
					assign_unique_value(tmp_net, idx);
					break;
				}
			}
		}
		for (int i = 0; i < tmp_net->n_out; i++)
		{
			if (DFF != tmp_net->out[i]->type && FLAG_DOWN == tmp_net->out[i]->unique_flag)
			{
				que2ENQ(tmp_net->out[i], QUE_MODE_ONE);
				tmp_net->out[i]->unique_flag = FLAG_UP;
			}
		}
		//レベルソート
		if (1 < que2.num)
		{
			uni_Qsort(que2.front, que2.rear - 1);
		}
	}

	return;
}

//===============================================
//name : assign_unique_value
//function : 一意活性化の非制御値割当て&キュー１へのエンキュー
//return : void
//===============================================
void assign_unique_value(
	NLIST* t_net,
	int idx
)
{
	//対象信号線が外部出力の時、一意活性化する必要がない
	if (0 == t_net->n_out)
	{
		return;
	}

	switch (t_net->type)
	{
	case AND:
	case NAND:

		for (int i = 0; i < t_net->n_in; i++)
		{
			//非制御値を割当てる信号線かつ、Dフロンティアが到達する可能性がないかつ、まだ値が割り当てられていない
			if (idx != i && FLAG_DOWN == t_net->in[i]->unique_flag && FLAG_DOWN == t_net->in[i]->flag)
			{
				t_net->in[i]->flag = FLAG_UP;
				t_net->in[i]->logic_value = 1;
				if (NULL == t_net->in[i]->consna)
				{
					t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));

					//対象信号線に1を割当てる
					sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
						t_net->in[i]->consna,
						t_net->in[i]->varnc[1]);

					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
		}
		break;
	case OR:
	case NOR:
		for (int i = 0; i < t_net->n_in; i++)
		{
			//非制御値を割当てる信号線かつ、Dフロンティアが到達する可能性がないかつ、まだ値が割り当てられていない
			if (idx != i && FLAG_DOWN == t_net->in[i]->unique_flag && FLAG_DOWN == t_net->in[i]->flag)
			{
				t_net->in[i]->flag = FLAG_UP;
				t_net->in[i]->logic_value = 0;
				if (NULL == t_net->in[i]->consna)
				{
					t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));

					//対象信号線に0を割当てる
					sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
						t_net->in[i]->consna,
						t_net->in[i]->varnc[1]);
					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
		}
		break;
	default:
		break;
	}


	return;
}

//===============================================
//name : back_implication
//function : 後方含意
//return : void
//===============================================
void back_implication(
	NLIST* t_net
)
{
	switch (t_net->type)
	{
	case AND:
		//信号線が1の時
		if (1 == t_net->logic_value)
		{
			for (int i = 0; i < t_net->n_in; i++)
			{
				if (FLAG_DOWN == t_net->in[i]->flag)
				{
					t_net->in[i]->flag = FLAG_UP;
					t_net->in[i]->logic_value = 1;
					if (NULL == t_net->in[i]->consna)
					{
						t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));

						//対象信号線に1を割当てる
						sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							t_net->in[i]->consna,
							t_net->in[i]->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					if (DFF != t_net->in[i]->type)
					{
						queENQ(t_net->in[i], QUE_MODE_ONE);
					}
				}

			}
		}
		break;

	case OR:
		//信号線が0の時
		if (0 == t_net->logic_value)
		{
			for (int i = 0; i < t_net->n_in; i++)
			{
				if (FLAG_DOWN == t_net->in[i]->flag)
				{
					t_net->in[i]->flag = FLAG_UP;
					t_net->in[i]->logic_value = 0;
					if (NULL == t_net->in[i]->consna)
					{
						t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							t_net->in[i]->consna,
							t_net->in[i]->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					if (DFF != t_net->in[i]->type)
					{
						queENQ(t_net->in[i], QUE_MODE_ONE);
					}
				}
			}
		}
		break;

	case NAND:
		//信号線が0の時
		if (0 == t_net->logic_value)
		{
			for (int i = 0; i < t_net->n_in; i++)
			{
				if (FLAG_DOWN == t_net->in[i]->flag)
				{
					t_net->in[i]->flag = FLAG_UP;
					t_net->in[i]->logic_value = 1;
					if (NULL == t_net->in[i]->consna)
					{
						t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に1を割当てる
						sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							t_net->in[i]->consna,
							t_net->in[i]->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					
					if (DFF != t_net->in[i]->type)
					{
						queENQ(t_net->in[i], QUE_MODE_ONE);
					}
				}
			}
		}
		break;

	case NOR:
		//信号線が1の時
		if (1 == t_net->logic_value)
		{
			for (int i = 0; i < t_net->n_in; i++)
			{
				if (FLAG_DOWN == t_net->in[i]->flag)
				{
					t_net->in[i]->flag = FLAG_UP;
					t_net->in[i]->logic_value = 0;
					if (NULL == t_net->in[i]->consna)
					{
						t_net->in[i]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(t_net->in[i]->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							t_net->in[i]->consna,
							t_net->in[i]->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					if (DFF != t_net->in[i]->type)
					{
						queENQ(t_net->in[i], QUE_MODE_ONE);
					}
				}
			}
		}
		break;

	case INV:
		if (FLAG_DOWN == t_net->in[0]->flag)
		{
			t_net->in[0]->flag = FLAG_UP;
			if (1 == t_net->logic_value)
			{
				t_net->in[0]->logic_value = 0;
				if (NULL == t_net->in[0]->consna)
				{
					t_net->in[0]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
					//対象信号線に0を割当てる
					sprintf_s(t_net->in[0]->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
						t_net->in[0]->consna,
						t_net->in[0]->varnc[1]);
					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
			//0の時
			else
			{
				t_net->in[0]->logic_value = 1;
				if (NULL == t_net->in[0]->consna)
				{
					t_net->in[0]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
					//対象信号線に1を割当てる
					sprintf_s(t_net->in[0]->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
						t_net->in[0]->consna,
						t_net->in[0]->varnc[1]);
					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
		}

		break;
	case BUF:
	case FOUT:
		if (FLAG_DOWN == t_net->in[0]->flag)
		{
			t_net->in[0]->flag = FLAG_UP;
			if (1 == t_net->logic_value)
			{
				t_net->in[0]->logic_value = 1;
				if (NULL == t_net->in[0]->consna)
				{
					t_net->in[0]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
					//対象信号線に1を割当てる
					sprintf_s(t_net->in[0]->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
						t_net->in[0]->consna,
						t_net->in[0]->varnc[1]);
					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
			//0の時
			else
			{
				t_net->in[0]->logic_value = 0;
				if (NULL == t_net->in[0]->consna)
				{
					t_net->in[0]->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
					//対象信号線に0を割当てる
					sprintf_s(t_net->in[0]->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
						t_net->in[0]->consna,
						t_net->in[0]->varnc[1]);
					//制約数更新
					OPBcalcSize(&opb.total, 0, 1, 0, 0);
				}
				if (DFF != t_net->in[i]->type)
				{
					queENQ(t_net->in[i], QUE_MODE_ONE);
				}
			}
		}

		break;

	default:
		break;
	}

	return;
}

//===============================================
//name : forward_implication
//function :	前方含意
//return : void
//===============================================
void forward_implication(
	NLIST* t_net
)
{
	int count;
	for (int i = 0; i < t_net->n_out; i++)
	{
		NLIST* tmp_net = t_net->out[i];
		switch (tmp_net->type)
		{
		case AND:
			count = 0;

			for (int j = 0; j < tmp_net->n_in; j++)
			{
				//入力論理値が０かつ、値が割り当てられている
				if (0 == tmp_net->in[j]->logic_value && FLAG_DOWN != tmp_net->in[j]->flag)
				{
					if (FLAG_DOWN == tmp_net->flag)
					{
						tmp_net->flag = FLAG_UP;
						tmp_net->logic_value = 0;
						if (NULL == tmp_net->consna)
						{
							tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
							//対象信号線に0を割当てる
							sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
								tmp_net->consna,
								tmp_net->varnc[1]);
							//制約数更新
							OPBcalcSize(&opb.total, 0, 1, 0, 0);
						}

						queENQ(tmp_net, QUE_MODE_ONE);
					}
					break;

				}
				else if (1 == tmp_net->in[j]->logic_value)
				{
					count++;
				}
			}
			//全信号線が非制御値だった場合
			if (count == tmp_net->n_in)
			{
				if (FLAG_DOWN == tmp_net->flag)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 1;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に1を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}

					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		case OR:
			count = 0;

			for (int j = 0; j < tmp_net->n_in; j++)
			{
				//入力論理値が1
				if (1 == tmp_net->in[j]->logic_value)
				{
					if (FLAG_DOWN == tmp_net->flag)
					{
						tmp_net->flag = FLAG_UP;
						tmp_net->logic_value = 1;
						if (NULL == tmp_net->consna)
						{
							tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
							//対象信号線に1を割当てる
							sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
								tmp_net->consna,
								tmp_net->varnc[1]);
							//制約数更新
							OPBcalcSize(&opb.total, 0, 1, 0, 0);
						}
						
						queENQ(tmp_net, QUE_MODE_ONE);
					}
					break;

				}
				//入力論理値が0かつ、値が割り当てられている
				else if (0 == tmp_net->in[j]->logic_value && FLAG_DOWN != tmp_net->in[j]->flag)
				{
					count++;
				}
			}
			//全信号線が非制御値だった場合
			if (count == tmp_net->n_in)
			{
				if (FLAG_DOWN == tmp_net->flag)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 0;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					
					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		case NAND:
			count = 0;

			for (int j = 0; j < tmp_net->n_in; j++)
			{
				//入力論理値が０かつ、値が割り当てられている
				if (0 == tmp_net->in[j]->logic_value && FLAG_DOWN != tmp_net->in[j]->flag)
				{
					if (FLAG_DOWN == tmp_net->flag)
					{
						tmp_net->flag = FLAG_UP;
						tmp_net->logic_value = 1;
						if (NULL == tmp_net->consna)
						{
							tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
							//対象信号線に1を割当てる
							sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
								tmp_net->consna,
								tmp_net->varnc[1]);
							//制約数更新
							OPBcalcSize(&opb.total, 0, 1, 0, 0);
						}
						
						queENQ(tmp_net, QUE_MODE_ONE);
					}
					break;

				}
				else if (1 == tmp_net->in[j]->logic_value)
				{
					count++;
				}
			}
			//全信号線が非制御値だった場合
			if (count == tmp_net->n_in)
			{
				if (FLAG_DOWN == tmp_net->flag)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 0;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					
					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		case NOR:
			count = 0;

			for (int j = 0; j < tmp_net->n_in; j++)
			{
				//入力論理値が1
				if (1 == tmp_net->in[j]->logic_value)
				{
					if (FLAG_DOWN == tmp_net->flag)
					{
						tmp_net->flag = FLAG_UP;
						tmp_net->logic_value = 0;
						if (NULL == tmp_net->consna)
						{
							tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
							//対象信号線に0を割当てる
							sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
								tmp_net->consna,
								tmp_net->varnc[1]);
							//制約数更新
							OPBcalcSize(&opb.total, 0, 1, 0, 0);
						}

						queENQ(tmp_net, QUE_MODE_ONE);
					}
					break;

				}
				//入力論理値が0かつ、値が割り当てられている
				else if (0 == tmp_net->in[j]->logic_value && FLAG_DOWN != tmp_net->in[j]->flag)
				{
					count++;
				}
			}
			//全信号線が非制御値だった場合
			if (count == tmp_net->n_in)
			{
				if (FLAG_DOWN == tmp_net->flag)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 1;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に1を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					
					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		case INV:
			if (FLAG_DOWN == tmp_net->flag)
			{
				if (0 == tmp_net->in[0]->logic_value)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 1;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に1を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					

					queENQ(tmp_net, QUE_MODE_ONE);
				}
				//1の時
				else
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 0;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					

					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		case BUF:
		case FOUT:
			if (FLAG_DOWN == tmp_net->flag)
			{
				if (0 == tmp_net->in[0]->logic_value)
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 0;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));
						//対象信号線に0を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 ~x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}

					queENQ(tmp_net, QUE_MODE_ONE);
				}
				//1の時
				else
				{
					tmp_net->flag = FLAG_UP;
					tmp_net->logic_value = 1;
					if (NULL == tmp_net->consna)
					{
						tmp_net->consna = (char*)allocMemory(ONE_CONS_SIZE, sizeof(char));

						//対象信号線に1を割当てる
						sprintf_s(tmp_net->consna, ONE_CONS_SIZE, "%s1 x%d =1;\n",
							tmp_net->consna,
							tmp_net->varnc[1]);
						//制約数更新
						OPBcalcSize(&opb.total, 0, 1, 0, 0);
					}
					
					queENQ(tmp_net, QUE_MODE_ONE);
				}
			}
			break;
		default:
			break;
		}
	}

	return;
}

//===============================================
//name : InitQUE2
//function : 必須割当てで使うキューの初期化
//return : void
//===============================================
void InitQUE2(
	void
)
{
	que2.maxnum = n_net;
	que2.front = 0;
	que2.num = 0;
	que2.rear = 0;
	que2.que = (NLIST**)allocMemory(que.maxnum, sizeof(NLIST*));

	return;
}

//*************************************************************************************************************
//	@name		：　que2ENQ
//	@function	：	必須割当てで使うキュー用のエンキュー関数
//	@return		：	(void)
//*************************************************************************************************************
void que2ENQ(
	NLIST* netptr,			  /** pointer to netlist */
	int	 mode				  /** mode */
)
{
	switch (mode)
	{
	case QUE_MODE_ONE:
		que2.que[que2.rear++] = netptr;
		que2.num++;

		if (que2.maxnum == que2.rear) que2.rear = 0;

		assert(que2.num < que2.maxnum);
		if (que2.maxnum < que2.num)
		{
			printf("SYSTEM ERROR: queue over flow.\n");

			exit(EXIT_FAILURE);
		}

		break;

	case QUE_MODE_ALLOUT:
		for (int i = 0; i < netptr->n_out; i++)
		{
			que2.que[que2.rear++] = netptr->out[i];
			que2.num++;

			if (que2.maxnum == que2.rear) que2.rear = 0;

			assert(que2.num < que2.maxnum);
			if (que2.maxnum < que2.num)
			{
				printf("SYSTEM ERROR: queue over flow.\n");

				exit(EXIT_FAILURE);
			}
		}

		break;

	case QUE_MODE_ALLIN:
		for (int i = 0; i < netptr->n_in; i++)
		{
			que2.que[que2.rear++] = netptr->in[i];
			que2.num++;

			if (que2.maxnum == que2.rear) que2.rear = 0;

			assert(que2.num < que2.maxnum);
			if (que2.maxnum < que2.num)
			{
				printf("SYSTEM ERROR: queue over flow.\n");

				exit(EXIT_FAILURE);
			}
		}

		break;

	default:
		printf("SYSTEM ERROR: queue mode error.\n");

		exit(EXIT_FAILURE);

		break;
	}

	return;
}

//*************************************************************************************************************
//	@name		：　que2DEQ
//	@function	：	必須割当てで使うキュー用のデキュー関数
//	@return		：	(NLIST*) pointer to nelist
//*************************************************************************************************************
NLIST* que2DEQ(
	void
)
{
	NLIST* netptr = (NLIST*)NULL;

	netptr = que2.que[que2.front++];
	que2.num--;

	if (que2.maxnum == que2.front)
	{
		que2.front = 0;
	}

	assert(que2.num >= 0);
	if (que2.num < 0)
	{
		printf("SYSTEM ERROR: queue under flow.\n");

		exit(EXIT_FAILURE);
	}

	return	netptr;
}

//*************************************************************************************************************
//	@name		：　uni_Qsort
//	@function	：	quick sort netlist levels in ascending order(For unique sentization)
//	@return		：	(void)
//*************************************************************************************************************
void uni_Qsort(
	int					left_most,				/* left-most value */
	int					right_most				/* right-most value */
)
{
	int left_i, right_i;
	int pivot_level;
	NLIST* tmp_nlist;

	/* initialize values */
	left_i = left_most;
	right_i = right_most;
	pivot_level = (que2.que[(left_most + right_most) / 2])->level;
	tmp_nlist = (NLIST*)NULL;

	/* quick sort */
	while (1)
	{
		while ((que2.que[left_i])->level < pivot_level) left_i++;

		while (pivot_level < (que2.que[right_i])->level) right_i--;

		if (left_i >= right_i) break;

		tmp_nlist = que2.que[left_i];
		que2.que[left_i] = que2.que[right_i];
		que2.que[right_i] = tmp_nlist;

		left_i++;
		right_i--;
	}

	if (left_most < left_i) Qsort(left_most, left_i - 1);

	if (right_i + 1 < right_most) Qsort(right_i + 1, right_most);

	return;
}
;