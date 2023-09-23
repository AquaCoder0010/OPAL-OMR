#pragma once 
#include <vector>

struct Point_Operations
{
	int sum_value = 0;
	int sub_value = 0;
	int index = 0;
};

int get_point_max_sub(std::vector<Point_Operations>& operations_list)
{
	std::vector<Point_Operations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](Point_Operations v_1, Point_Operations v_2)
	{
		return v_1.sub_value < v_2.sub_value;
	}
	);
	return operations_list_copy[operations_list_copy.size() - 1].index;
}

int get_point_min_sub(std::vector<Point_Operations>& operations_list)
{
	std::vector<Point_Operations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](Point_Operations v_1, Point_Operations v_2)
	{
		return v_1.sub_value < v_2.sub_value;
	}
	);
	return operations_list_copy[0].index;
}

int get_point_max_sum(std::vector<Point_Operations>& operations_list)
{
	std::vector<Point_Operations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](Point_Operations v_1, Point_Operations v_2)
	{
		return v_1.sum_value < v_2.sum_value;
	}
	);
	return operations_list_copy[operations_list_copy.size() - 1].index;
}

int get_point_min_sum(std::vector<Point_Operations>& operations_list)
{
	std::vector<Point_Operations> operations_list_copy = operations_list;
	
	std::sort(operations_list_copy.begin(), operations_list_copy.end(), 
	[](Point_Operations v_1, Point_Operations v_2)
	{
		return v_1.sum_value < v_2.sum_value;
	}
	);
	return operations_list_copy[0].index;
}
