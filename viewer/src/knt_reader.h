#ifndef _KNT_READER_H_
#define _KNT_READER_H_

#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>

class KntReader {
public:
	KntReader() {}
	~KntReader();
	KntReader(const std::string &_knt_file) { open(_knt_file); }

	void open(const std::string &_knt_file);

	int get_color_frames() const { return m_c_num; }
	int get_depth_frames() const { return m_d_num; }
	int get_color_width() const { return m_c_width; }
	int get_color_height() const { return m_c_height; }
	int get_depth_width() const { return m_d_width; }
	int get_depth_height() const { return m_d_height; }
	int get_color_start_idx() const { return m_c_start_idx; }
	int get_depth_start_idx() const { return m_d_start_idx; }
	int get_color_end_idx() const { return m_c_end_idx; }
	int get_depth_end_idx() const { return m_d_end_idx; }

	cv::Mat get_color_frame(int _frame_idx) const;
	cv::Mat get_depth_frame(int _frame_idx) const;

	Eigen::Matrix3f get_color_proj_mat() const { return m_c_proj_mat; }
	Eigen::Matrix3f get_depth_proj_mat() const { return m_d_proj_mat; }
	Eigen::Matrix4f get_d2c_mat() const { return m_d2c_mat; }

private:
	FILE *m_file_buffer = nullptr;
	
	int m_header_offset = 0;

	int m_c_width = 0;
	int m_c_height = 0;
	int m_d_width = 0;
	int m_d_height = 0;
	int m_c_num = 0;
	int m_d_num = 0;
	int m_c_start_idx = 0;
	int m_c_end_idx = 0;
	int m_d_start_idx = 0;
	int m_d_end_idx = 0;

	Eigen::Matrix3f m_c_proj_mat;
	Eigen::Matrix3f m_d_proj_mat;
	Eigen::Matrix4f m_d2c_mat;
};

#endif