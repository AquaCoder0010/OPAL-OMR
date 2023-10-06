#pragma once 
#include <vector>
#include <algorithm>

struct PointOperations
{
	int sum_value = 0;
	int sub_value = 0;
	int index = 0;
};

static int getMaxSub(std::vector<PointOperations>& operations_list)
{
	std::vector<PointOperations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](PointOperations v_1, PointOperations v_2)
	{
		return v_1.sub_value < v_2.sub_value;
	}
	);
	return operations_list_copy[operations_list_copy.size() - 1].index;
}

static int getMinSub(std::vector<PointOperations>& operations_list)
{
	std::vector<PointOperations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](PointOperations v_1, PointOperations v_2)
	{
		return v_1.sub_value < v_2.sub_value;
	}
	);
	return operations_list_copy[0].index;
}

static int getMaxSum(std::vector<PointOperations>& operations_list)
{
	std::vector<PointOperations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](PointOperations v_1, PointOperations v_2)
	{
		return v_1.sum_value < v_2.sum_value;
	}
	);
	return operations_list_copy[operations_list_copy.size() - 1].index;
}

static int getMinSum(std::vector<PointOperations>& operations_list)
{
	std::vector<PointOperations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](PointOperations v_1, PointOperations v_2)
	{
		return v_1.sum_value < v_2.sum_value;
	}
	);
	return operations_list_copy[0].index;
}
