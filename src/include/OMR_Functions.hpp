#pragma once 
#include <opencv2/opencv.hpp>
#include <iostream> 
#include <array>
#include <vector>
#include <string>
#include <cmath>

#include "Point_Operations.hpp"


double get_average_value(cv::Scalar scalar)
{
	double average_value = 0.f;
	for(int i = 0; i < scalar.channels; i++)
		average_value += scalar[i];

	return average_value/scalar.channels;
}


void parse_string(std::string& p_output_string, std::string& p_ip_address, std::string& p_port)
{
	p_output_string = std::string("");
	p_output_string = ("http://") + p_ip_address + (":") + p_port + ("/mjpegfeed?960x720");
}

bool is_greater(cv::Vec3b pixel_1, cv::Vec3b pixel_2)
{
	return (get_average_value(pixel_1) > get_average_value(pixel_2));
}

cv::Vec3b get_brightest_pixel(const cv::Mat& image)
{
	cv::Vec3b biggest_color = cv::Vec3b(0, 0, 0);
	for(int i = 0; i < image.rows; i++)
		for(int j = 0; j < image.cols; j++)
		{
			if(get_average_value(image.at<cv::Vec3b>(i, j)) > get_average_value(biggest_color))
			{
				biggest_color = image.at<cv::Vec3b>(i, j);
			}
		}
	biggest_color[0] -= biggest_color[0] * 0.3; 
	biggest_color[1] -= biggest_color[1] * 0.3; 
	biggest_color[2] -= biggest_color[2] * 0.3; 
	
	return biggest_color;
}



double get_average_value(cv::Vec3b scalar)
{
	double average_value = 0.f;
	for(int i = 0; i < scalar.channels; i++)
		average_value += scalar[i];

	return average_value/scalar.channels;
}


void edge_detection(cv::Mat& image, bool disable_dilation = false)
{
	cv::Scalar mean_value = cv::Scalar();
	cv::Mat kernel = cv::Mat();


	double mean_value_average = 0.0;
	mean_value = cv::mean(image);
	mean_value_average = (get_average_value(mean_value));
	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
	cv::Canny(image, image, mean_value_average, (mean_value_average) * 3, 3);
    if(disable_dilation == false)
        cv::dilate(image, image, kernel);
}


static std::vector<std::vector<cv::Point>> contour_list;

void find_points(cv::Mat& image, std::vector<cv::Point>& p_contour_poly)
{
	cv::findContours(image, contour_list, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<cv::Rect> bound_rect(contour_list.size());
	int maxArea = 0;

	for(int i = 0; i < contour_list.size(); i++)
	{
		int area = cv::contourArea(contour_list[i]);
		std::vector<cv::Point> current_contour_poly;
		if(area > 1000)
		{
			float arc_length = cv::arcLength(contour_list[i], true);
			cv::approxPolyDP(contour_list[i], current_contour_poly, 0.02 * arc_length, true);
			if(area > maxArea && current_contour_poly.size() == 4)
			{
				p_contour_poly = {current_contour_poly[0], 
								current_contour_poly[1],
								current_contour_poly[2],
								current_contour_poly[3]};
			}
		}
	}
}


void draw_points(cv::Mat& image, const std::vector<cv::Point>& point_list)
{
	for(int i = 0; i < point_list.size(); i++)
	{
		cv::circle(image, point_list[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
		cv::putText(image, std::to_string(i), cv::Point(point_list[i].x + 10, point_list[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
	}
	cv::line(image, point_list[0], point_list[1], cv::Scalar(0, 255, 0));
	cv::line(image, point_list[0], point_list[2], cv::Scalar(0, 255, 0));
	cv::line(image, point_list[1], point_list[3], cv::Scalar(0, 255, 0));
	cv::line(image, point_list[2], point_list[3], cv::Scalar(0, 255, 0));

}

std::vector<cv::Point> reorder(const std::vector<cv::Point>& point_list)
{
	std::vector<cv::Point> ordered_list;
	std::vector<Point_Operations> point_operations_list(point_list.size());

	for(int i = 0; i < point_list.size(); i++)
	{
		point_operations_list[i].index = i;
		point_operations_list[i].sum_value = point_list[i].x + point_list[i].y;
		point_operations_list[i].sub_value = point_list[i].x - point_list[i].y;
	}
	ordered_list.reserve(4);
	ordered_list.emplace_back(point_list[get_point_min_sum(point_operations_list)]); 
	ordered_list.emplace_back(point_list[get_point_max_sub(point_operations_list)]); 
	ordered_list.emplace_back(point_list[get_point_min_sub(point_operations_list)]); 
	ordered_list.emplace_back(point_list[get_point_max_sum(point_operations_list)]); 

	return ordered_list;
}


void wrap_image(cv::Mat& input_image, cv::Mat& output_image, std::vector<cv::Point> control_point_list, float wrapped_width, float wrapped_height)
{
	cv::Point2f control_point_source[4];
	cv::Point2f control_point_dst[4] = { {0.f, 0.f} , {wrapped_width, 0.f} , {0.f, wrapped_height} , {wrapped_width, wrapped_height} };
	
	for(int i = 0; i < control_point_list.size(); i++)
		control_point_source[i] = control_point_list[i];
	
	cv::Mat transformation_matrix = cv::getPerspectiveTransform(control_point_source, control_point_dst);
	cv::warpPerspective(input_image, output_image, transformation_matrix, cv::Size(wrapped_width, wrapped_height));
}

void sharpen_image(cv::Mat& image)
{
	cv::Mat kernel( 3 ,3 , CV_32F , cv::Scalar(0));

	kernel.at<float>(1,1)= 5.2;
	kernel.at<float>(0,1)= -1.0;
	kernel.at<float>(2,1)= -1.0;
	kernel.at<float>(1,0)= -1.0;
	kernel.at<float>(1,2)= -1.0;

	cv::filter2D(image, image, image.depth(), kernel);
}



// point 75, 400 -+ 5px
// height :- 350 px
// width:- 595 px
void crop_answer_block(cv::Mat& input_image, cv::Mat& output_image)
{
    const int x_pos = 55;
    const int y_pos = 395;
    const int height = 390;
    const int width = 640;

    cv::Rect answer_block_rect(x_pos, y_pos, width, height);
    output_image = input_image(answer_block_rect).clone();

    // cv::rectangle(input_image, answer_block_rect, cv::Scalar(0, 0, 255), 1);
}


//
float get_average_area(std::vector<cv::Rect>& bound_rect)
{
	float area = 0.f;
	for(auto& rect : bound_rect)
		area += rect.width * rect.height;
	
	area /= bound_rect.size();
	return area;
}

float distance_from_origin(cv::Point point_1)
{
	return sqrt( (point_1.x * point_1.x) + (point_1.y * point_1.y ) );
}

static cv::Mat horizontal_lines = cv::Mat();
static cv::Mat vertical_lines = cv::Mat();
static cv::Mat image_grid_removed = cv::Mat();


static std::vector<cv::Vec4i> vertical_line_list;
static std::vector<cv::Vec4i> horizontal_line_list;
static std::vector<cv::Vec4i> line_list;
static std::vector<std::vector<cv::Point>> image_contour_list;

std::vector<cv::Rect> get_answer_bound(cv::Mat& image)
{
	cv::Mat image_binary = image.clone();
	cv::Mat image_grid_removed = image.clone();
	

	cv::cvtColor(image_binary, image_binary, cv::COLOR_BGR2GRAY);
	cv::adaptiveThreshold(image_binary, image_binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);


	// extraction of the horizontal and vertical lines in the given image
	//
	horizontal_lines = image_binary.clone();
	vertical_lines = image_binary.clone();
	

	int horizontal_line_length = horizontal_lines.cols / 30;
	cv::Mat horizontal_morph_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_line_length, 1));
	cv::erode(horizontal_lines, horizontal_lines, horizontal_morph_kernel);
	cv::dilate(horizontal_lines, horizontal_lines, horizontal_morph_kernel);


	int vertical_line_length = vertical_lines.cols / 30;
	cv::Mat vertical_morph_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, vertical_line_length));
	cv::erode(vertical_lines, vertical_lines, vertical_morph_kernel);
	cv::dilate(vertical_lines, vertical_lines, vertical_morph_kernel);

	cv::Mat dilation_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(horizontal_lines, horizontal_lines, dilation_kernel);
	cv::dilate(vertical_lines, vertical_lines, dilation_kernel);
	//

	
	// getting the lines through probablistic hough line transform
	//
	cv::HoughLinesP(horizontal_lines, horizontal_line_list, 1, CV_PI/180, 50, 10, 50);
	cv::HoughLinesP(vertical_lines, vertical_line_list, 1, CV_PI/180, 50, 10, 50);
	
	// adding two of the lists in a bigger one list.
	line_list.resize(horizontal_line_list.size());
	line_list.insert(line_list.begin(), horizontal_line_list.begin(), horizontal_line_list.end()); 
    line_list.insert(line_list.end(), vertical_line_list.begin(), vertical_line_list.end());
	//

	image_grid_removed = image.clone();
	for(auto& line : line_list)
	{
		cv::Point point_1 = cv::Point(line[0], line[1]);
		cv::Point point_2 = cv::Point(line[2], line[3]);
		

		// increase the overall lines size
		//
		if(point_1.x == point_2.x)
		{
			point_2.y -= 60;
			point_1.y += 60;
		}

		if(point_1.y == point_2.y && (point_1.x < point_2.x))
		{
			point_1.x -= 60;
			point_2.x += 60;
		}

		if(point_1.y == point_2.y && (point_1.x > point_2.x))
		{
			point_1.x += 60;
			point_2.x -= 60;
		}
		//
		cv::line(image_grid_removed, point_1, point_2, cv::Scalar(255, 255, 255), 4);
	}
	

	// adding an adaptive threshold and then removing small noise
	// 
	cv::cvtColor(image_grid_removed, image_grid_removed, cv::COLOR_BGR2GRAY);
	cv::adaptiveThreshold(image_grid_removed, image_grid_removed, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);
	
	cv::Mat opening_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(image_grid_removed, image_grid_removed, cv::MORPH_CLOSE, opening_kernel);
	//

	// countour detection for the answer blocks, and countor to rectangle approximation to get the answer bounds needed.
	const constexpr int remove_area_limit = 20;
	
	cv::findContours(image_grid_removed, image_contour_list, cv::RETR_LIST , cv::CHAIN_APPROX_TC89_L1);

	std::vector<std::vector<cv::Point>> image_contour_poly_list(image_contour_list.size());
	std::vector<cv::Rect> boundRect(image_contour_list.size());

	for(int i = 0; i < image_contour_list.size(); i++)
	{
		cv::approxPolyDP(image_contour_list[i], image_contour_poly_list[i], 4, true);
        boundRect[i] = cv::boundingRect(image_contour_poly_list[i]);		
	}

	std::vector<cv::Rect>::iterator end_iter = std::remove_if(boundRect.begin(), boundRect.end(), [](cv::Rect rect_1) 
	{
        return (rect_1.width * rect_1.height) <= remove_area_limit;    // remove small rectangles numbers
    });

	boundRect.erase(end_iter, boundRect.end());
	return boundRect;
}


float get_distance(cv::Point point_1, cv::Point point_2)
{
	return sqrt( pow(point_2.x - point_1.x, 2 ) + pow(point_2.y - point_1.y, 2) );
}



void reposition_bound(std::vector<cv::Rect>& answer_bound_list)
{
	int answer_bound_size = answer_bound_list.size();
	int answer_bound_y_count = 20;
	int answer_bound_x_count = answer_bound_size / answer_bound_y_count;
	
	std::vector<int> deletion_list;
	for(int i = 0; i < answer_bound_x_count; i++)
	{
		if(i % 6 == 0)
		{	
			deletion_list.emplace_back(i);
			for(int y = 0; y < answer_bound_y_count; y++)
			{
				int y_index = (answer_bound_x_count +  y * answer_bound_x_count) + i;
				if(y_index < answer_bound_size)
					deletion_list.emplace_back(y_index);
			}
		}
	}

	std::sort(deletion_list.begin(), deletion_list.end());

	// delete the vectors from the list
	for(auto i = deletion_list.rbegin(); i != deletion_list.rend(); i++)
		answer_bound_list.erase(answer_bound_list.begin() + *i);

}

std::vector<cv::Rect> reorder_bounds(std::vector<cv::Rect>& answer_bound_list)
{
	std::vector<std::vector<cv::Rect>> row_list;
	std::vector<cv::Rect> current_row;
	std::vector<cv::Rect> sor_answer_list = answer_bound_list;
	
	int tolerance = 8;
	
	std::sort(sor_answer_list.begin(), sor_answer_list.end(), [](const cv::Rect rect_1, const cv::Rect rect_2)
	{
		return rect_1.y < rect_2.y;
	});


	current_row.emplace_back(sor_answer_list[0]);
	for(int i = 1; i < sor_answer_list.size(); i++)
	{
		if( abs(sor_answer_list[i].y - sor_answer_list[i - 1].y) <= tolerance )
			current_row.emplace_back(sor_answer_list[i]);

		if( abs(sor_answer_list[i].y - sor_answer_list[i - 1].y > tolerance) )
		{
			row_list.emplace_back(current_row);
			current_row.clear();
			current_row.emplace_back(sor_answer_list[i]);	
		}
	}

	if(current_row.empty() == false)
		row_list.emplace_back(current_row);


	std::vector<cv::Rect> sorted;	
	for(auto& row : row_list)
	{
		std::sort(row.begin(), row.end(), [](const cv::Rect rect_1, const cv::Rect rect_2)
		{	
			return rect_1.x < rect_2.x;
		});
    	sorted.insert(sorted.end(), row.begin(), row.end());
	}

	reposition_bound(sorted);
	return sorted;
}	

float get_value(cv::Mat answer_block, cv::Rect answer_bound, int total_count)
{
	float answer_value = 0.f;
	cv::Point answer_tl = cv::Point(answer_bound.x, answer_bound.y);
	cv::Point answer_br = cv::Point(answer_bound.x + answer_bound.width, answer_bound.y + answer_bound.height);

	cv::cvtColor(answer_block, answer_block, cv::COLOR_BGR2GRAY);
	
	for(int y_counter = answer_tl.y; y_counter < answer_br.y; y_counter++)
		for(int x_counter = answer_tl.x; x_counter < answer_br.x; x_counter++)
		{
			answer_value += static_cast<int>(answer_block.at<uchar>(y_counter, x_counter));
		}	
	answer_value /= total_count;
	return answer_value;
}

std::vector<std::vector<cv::Rect>> get_question_list(std::vector<cv::Rect>& answer_bounds)
{
	int total_count = answer_bounds.size();
	int y_count = 20;
	int x_count = total_count / y_count;

	std::vector<std::vector<cv::Rect>> question_list;

	for(int i = 0; i < 6; i++)
	{
		std::vector<cv::Rect> current_question;
		for(int j = 0; j < y_count; j++)
		{
			int question_index = j * x_count + (i * 5);
			current_question.emplace_back(answer_bounds[question_index]);

			if(i != 5)	
				for(int k = question_index + 1; k < (question_index + 4) + 1; k++)
					current_question.emplace_back(answer_bounds[k]);
			
			if(i == 5)
				for(int k = question_index + 1; k < (question_index + 4); k++)
					current_question.emplace_back(answer_bounds[k]);

			question_list.emplace_back(current_question);
			current_question.clear();
		}
	}
	return question_list;
}

std::vector<unsigned char> generate_answer()
{
	std::vector<unsigned char> answer;
	for(int i = 0; i < 120; i++)
	{
		if(i < 100)
			answer.emplace_back(static_cast<unsigned char>(65 + rand() % 4));
		if(i >= 100)
			answer.emplace_back(static_cast<unsigned char>(65 + rand() % 3));
	}
	return answer;
}

float evaluate_answer(cv::Mat image, std::vector<std::vector<cv::Rect>> question_list, std::vector<unsigned char> answer_list)
{
	float evaluation = 0.f;

	struct Choice
	{
		float average_value = 0.f;
		unsigned char choice_value;
	};


	for(int i = 0; i < question_list.size(); i++)
	{
		std::vector<Choice> choice_list;
		choice_list.reserve(question_list[i].size());

		for(int j = 0; j < question_list[i].size(); j++)
		{
			float average_area = get_average_area(question_list[i]);
			Choice choice = {get_value(image, question_list[i][j], average_area), static_cast<unsigned char>(65 + j)};
			choice_list.emplace_back(choice);
		}

		std::sort(choice_list.begin(), choice_list.end(), [](Choice choice_1, Choice choice_2)
		{	
			return choice_1.average_value < choice_2.average_value; 
		});

		if(choice_list[0].choice_value == answer_list[i])
		{
			int question_list_index = std::abs(65 - choice_list[0].choice_value);
			cv::rectangle(image, question_list[i][question_list_index].tl(),  question_list[i][question_list_index].br(), cv::Scalar(40, 230, 0));
			evaluation += 1;			
		}

		if(choice_list[0].choice_value != answer_list[i])
		{
			int question_list_index = std::abs(65 - answer_list[i]);
			cv::rectangle(image, question_list[i][question_list_index].tl(),  question_list[i][question_list_index].br(), cv::Scalar(0, 0, 255));
		}

		// for(cv::Rect& question_bound : question)
		// {
		// 	average_value.emplace_back(get_value(image, question_bound));	
		// }

	}

	return (evaluation / question_list.size()) * 100;
}