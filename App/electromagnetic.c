#include "electromagnetic.h"

/*----------------------------------------------变量----------------------------------------------*/
/*------------------------domestic various-----------------------*/
//树控制参数
struct MedianTree gLeftTree, gRightTree, gMiddleTree;

// 舵机控制参数
int AD_val[3];       //left 0, right 1, middle 2 //原始值
int dis_AD_val[3];   //归一化值
int disgy_AD_val[3]; //最近64个数中位数

int AD_val_min[3];

int AD_val_max[3];
/*---------------------------------------------manipulte EM function----------------------------------------------*/
void InitEM()
{
    AD_val_min[2] = AD_val_min[1] = AD_val_min[0] = 50;
    AD_val_max[2] = AD_val_max[1] = AD_val_max[0] = 1074;
    InitTree(&gLeftTree, ADC_LEFT);
    InitTree(&gRightTree, ADC_RIGHT);
    InitTree(&gMiddleTree, ADC_MIDDLE);
}

int ReadValue(ADCn_Ch_e adcn_ch)
{
    int j, sum;
    for (j = 0, sum = 0; j < 8; j++)
    {
        sum += adc_once(adcn_ch, ADC_10bit);
    }

    struct MedianTree* tree;
    uint8              dis;
    if (adcn_ch == ADC_LEFT)
    {
        tree = &gLeftTree;
        dis  = 0;
    }
    else if (adcn_ch == ADC_RIGHT)
    {
        tree = &gRightTree;
        dis  = 1;
    }
    else
    {
        tree = &gMiddleTree;
        dis  = 2;
    }

    tree->hvHead = (tree->hvHead + 1) & 63;
    TreeDelete(tree->tree, 1, 0, 127, tree->history[tree->hvHead]);
    AD_val[dis]                 = sum >> 3;
    dis_AD_val[dis]             = Normalized(dis);
    tree->history[tree->hvHead] = dis_AD_val[dis];
    TreeInsert(tree->tree, 1, 0, 127, dis_AD_val[dis]);
    disgy_AD_val[dis] = GetMedian(tree->tree, 1, 0, 127, 32);
    return disgy_AD_val[dis];
}

/*---------------------------------------------manipulte tree function----------------------------------------------*/
void InitTree(struct MedianTree* tree, ADCn_Ch_e adcn_ch)
{
    int    i, j;
    uint32 sum = 0;
    for (i = 0; i < 64; i++)
    {
        for (j = 0, sum = 0; j < 16; j++)
        {
            sum += adc_once(adcn_ch, ADC_10bit);
        }
        AD_val[0]        = sum >> 4;
        tree->history[i] = Normalized(0);
        TreeInsert(tree->tree, 1, 0, 127, tree->history[i]);
    }
    tree->hvHead = 63;
}
/*---------------------------------------------address EM function----------------------------------------------*/
uint8 Normalized(int index)
{
    if (AD_val[index] < AD_val_min[index])
        AD_val[index] = AD_val_min[index];
    else if (AD_val[index] > AD_val_max[index])
        AD_val[index] = AD_val_max[index];
    int result = ((AD_val[index] - AD_val_min[index]) >> 3);
    return result > 127 ? 127 : (uint8)result;
}
/*---------------------------------------------address tree function----------------------------------------------*/
void TreeInsert(uint8* tree, int id, int l, int r, int pos)
{
    tree[id]++;
    if (l >= r)
        return;
    int lc = id << 1, rc = (id << 1) | 1, mid = (l + r) >> 1;
    if (pos <= mid)
        TreeInsert(tree, lc, l, mid, pos);
    else
        TreeInsert(tree, rc, mid + 1, r, pos);
}

void TreeDelete(uint8* tree, int id, int l, int r, int pos)
{
    if (tree > 0)
        tree[id]--;
    if (l >= r)
        return;
    int lc = id << 1, rc = (id << 1) | 1, mid = (l + r) >> 1;
    if (pos <= mid)
        TreeDelete(tree, lc, l, mid, pos);
    else
        TreeDelete(tree, rc, mid + 1, r, pos);
}

int GetMedian(uint8* tree, int id, int l, int r, int c)
{
    if (l >= r)
        return l;
    int lc = id << 1, rc = (id << 1) | 1, mid = (l + r) >> 1;
    if (tree[lc] >= c)
        return GetMedian(tree, lc, l, mid, c);
    else
        return GetMedian(tree, rc, mid + 1, r, c - tree[lc]);
}

/* */
