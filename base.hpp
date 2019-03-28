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
#endif //DIP_BASE_HPP
