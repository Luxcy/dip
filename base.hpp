//
// Created by lux813 on 19-3-28.
//

#ifndef DIP_BASE_HPP
#define DIP_BASE_HPP
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <algorithm>
#include <vector>
#include "global.h"
#include <iostream>
void itoa(int n, char* &ans,int radix)
{

	int k=0;
	do{
		int t=n%radix;
		if(t>=0&&t<=9)	ans[k]=t+'0';
		else ans[k]=t-10+'a';
		k++;
		n/=radix;
	}while(n!=0);

}
bool comp(const minker &a,const minker &b)
{
    return a.diff < b.diff;
}

double Sigma(std::vector<int> c, double u)
{
    double sum = 0;
    for(int i=0; i<c.size(); i++)
    {
        sum += pow(c[i]-u, 2);
    }
    return sum/c.size();
}
//扫描过的像素均为0
//扫描过的像素标签号均为Lab1
int Labeltwo(int *label)
{
    int sumtwo = 0;
    int sumone = 0;
    for(int i=0; i<3; i++)
    {
        if(label[i] <= 0){
            sumtwo ++;
        }else {
            sumone += label[i];
        }
    }
    if(sumone == 0){
        return -1;
    }
    if(label[0] == label[1] && label[1] == label[2]){
        return 1;
    }
    if(sumtwo == 2){
        return std::max(label[0],std::max(label[1],label[2]));
    }
    return 0;
}
//label1,label2
int *Labelmaxmin(int *label)
{
    static int m[2];
    std::sort(label,label+3);
    int i=0;
    while(label[i]<0) i++;
    m[0] = label[i];
    m[1] = label[2];
    return m;
}
#endif //DIP_BASE_HPP
