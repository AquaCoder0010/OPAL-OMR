#pragma once 
#include <vector>
#include <algorithm>

struct PointOperations
{
	int sum_value = 0;
	int sub_value = 0;
	int index = 0;
};

int getMaxSub(std::vector<PointOperations>& operations_list);
int getMinSub(std::vector<PointOperations>& operations_list);
int getMaxSum(std::vector<PointOperations>& operations_list);
int getMinSum(std::vector<PointOperations>& operations_list);