#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"

int hit, miss, eviction;

typedef struct
{
    unsigned int usetime;
    unsigned int tag;
}block;

block *cache;
int s, E, b, t;

void cal(unsigned int addr, unsigned int time)
{
    unsigned int tag = addr >> b >> s;         //获得标记
    int setindex = (addr >> b) ^ (tag << s);   //获得块内偏移
    block *cacherow = cache + E * setindex;
    block *evictionrow = cacherow;
    int i;
    for (i = 0; i < E; i++)
    {
        if (cacherow[i].usetime && cacherow[i].tag == tag)
        {
            cacherow[i].usetime = time;
            hit++;
            return ;
        }
        else if (!cacherow[i].usetime)
        {
            miss++;
            cacherow[i].usetime = time;
            cacherow[i].tag = tag;
            return ;
        }
        else if (cacherow[i].usetime < evictionrow->usetime)
        {
            evictionrow = cacherow + i;
        }
    }
    miss++;
    eviction++;
    evictionrow->usetime = time;
    evictionrow->tag = tag;
}

int main(int argc, char *argv[])
{
    unsigned int time_tag = 0;
    FILE *f;
    //s个bit位表示组数
    //每组E行
    //b个bit位表示每行的block数
    //t文件
    int opt;
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1)
    {
        switch (opt)
        {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                f = fopen(optarg, "r");
                break;
        }
    }
    cache = (block*)malloc(sizeof(block) * E << s);
    memset(cache, 0, sizeof(block) * E << s);
    char op;
    unsigned int addr;
    while (fscanf(f, "%s%x, %*d", &op, &addr) > 0)
    {
        switch (op)
        {
            case 'M':
                hit++;
            case 'L':
            case 'S':
                cal(addr, ++time_tag);
        }
    }
    printSummary(hit, miss, eviction);
    return 0;
}
