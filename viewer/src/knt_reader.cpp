#include "knt_reader.h"
#include "lodepng.h"

KntReader::~KntReader()
{
	if (m_file_buffer)
		fclose(m_file_buffer);
}

void KntReader::open(const std::string &_knt_file)
{
	m_file_buffer = fopen(_knt_file.c_str(), "rb");
	if (!m_file_buffer)
		throw std::runtime_error("fatal error: cannot open knt file !");
	
	// Read knt header
	int c_width = 0;
	int c_height = 0;
	int d_width = 0;
	int d_height = 0;
	int c_num = 0;
	int d_num = 0;
	int c_start_idx = 0;
	int d_start_idx = 0;

	fread(&c_width, sizeof(int), 1, m_file_buffer);
	fread(&c_height, sizeof(int), 1, m_file_buffer);
	fread(&d_width, sizeof(int), 1, m_file_buffer);
	fread(&d_height, sizeof(int), 1, m_file_buffer);
	fread(&c_num, sizeof(int), 1, m_file_buffer);
	fread(&d_num, sizeof(int), 1, m_file_buffer);
	fread(&c_start_idx, sizeof(int), 1, m_file_buffer);
	fread(&d_start_idx, sizeof(int), 1, m_file_buffer);
	fread(m_c_proj_mat.data(), sizeof(Eigen::Matrix3f), 1, m_file_buffer);
	fread(m_d_proj_mat.data(), sizeof(Eigen::Matrix3f), 1, m_file_buffer);
	fread(m_d2c_mat.data(), sizeof(Eigen::Matrix4f), 1, m_file_buffer);

	m_c_width = c_width;
	m_c_height = c_height;
	m_d_width = d_width;
	m_d_height = d_height;
	m_c_num = c_num;
	m_d_num = d_num;
	m_c_start_idx = c_start_idx;
	m_d_start_idx = d_start_idx;
	m_c_end_idx = c_start_idx+c_num;
	m_d_end_idx = d_start_idx+d_num;

	m_header_offset = 8*sizeof(int) + 2*sizeof(Eigen::Matrix3f) + sizeof(Eigen::Matrix4f);
}

cv::Mat KntReader::get_color_frame(int _frame_idx) const
{
	if (!m_file_buffer)
		return cv::Mat();

	if (_frame_idx < m_c_start_idx || _frame_idx >= m_c_end_idx)
		return cv::Mat();

	// Calculate head address
	fseek(m_file_buffer, m_header_offset, SEEK_SET);
	for (int idx = 0; idx < _frame_idx-m_c_start_idx; ++idx) {
		int data_size = 0;
		fread(&data_size, sizeof(int), 1, m_file_buffer);
		fseek(m_file_buffer, data_size, SEEK_CUR);
	}

	// Read compressed data of this frame
	int data_size = 0;
	fread(&data_size, sizeof(int), 1, m_file_buffer);
	unsigned char *compressed_data = new unsigned char[data_size];
	fread(compressed_data, sizeof(unsigned char), data_size, m_file_buffer);

	// Decompress image data
	unsigned char *raw_data = nullptr;
	unsigned int width = 0;
	unsigned int height = 0;
	lodepng_decode_memory(&raw_data, &width, &height, compressed_data, data_size, LCT_RGBA, 8);

	if (width != m_c_width || height != m_c_height)
		throw std::runtime_error("fatal error: color image size does not match that in the header !");

	cv::Mat color_map(height, width, CV_8UC4);
	memcpy(color_map.data, raw_data, 4*width*height);

	delete[] compressed_data;
	free(raw_data);

	return color_map;
}

cv::Mat KntReader::get_depth_frame(int _frame_idx) const
{
	if (!m_file_buffer)
		return cv::Mat();

	if (_frame_idx < m_d_start_idx || _frame_idx >= m_d_end_idx)
		return cv::Mat();

	// Skip knt header
	fseek(m_file_buffer, m_header_offset, SEEK_SET);

	// Skip color data
	for (int idx = 0; idx < m_c_num; ++idx) {
		int data_size = 0;
		fread(&data_size, sizeof(int), 1, m_file_buffer);
		fseek(m_file_buffer, data_size, SEEK_CUR);
	}

	// Calculate head address
	for (int idx = 0; idx < _frame_idx - m_d_start_idx; ++idx) {
		int data_size = 0;
		fread(&data_size, sizeof(int), 1, m_file_buffer);
		fseek(m_file_buffer, data_size, SEEK_CUR);
	}

	// Read compressed data of this frame
	int data_size = 0;
	fread(&data_size, sizeof(int), 1, m_file_buffer);
	unsigned char *compressed_data = new unsigned char[data_size];
	fread(compressed_data, sizeof(unsigned char), data_size, m_file_buffer);

	// Decompress image data
	unsigned char *raw_data = nullptr;
	unsigned int width = 0;
	unsigned int height = 0;
	lodepng_decode_memory(&raw_data, &width, &height, compressed_data, data_size, LCT_GREY, 16);

	if (width != m_d_width || height != m_d_height)
		throw std::runtime_error("fatal error: depth image size does not match that in the header !");

	cv::Mat depth_map(height, width, CV_16UC1);
	memcpy(depth_map.data, raw_data, sizeof(unsigned short)*width*height);

	delete[] compressed_data;
	free(raw_data);

	return depth_map;
}