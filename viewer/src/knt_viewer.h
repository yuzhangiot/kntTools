#ifndef _KNT_VIEWER_H_
#define _KNT_VIEWER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "shader.h"
#include "knt_reader.h"

struct ViewerProperty
{
	ViewerProperty()
	{
		memset(win_title, 0, 128);
		win_width = 0;
		win_height = 0;
		major_ver = 4;
		minor_ver = 5;
		samples = 1;
		flags.all = 0;
	}

	char win_title[128];
	int win_width;
	int win_height;
	int major_ver;
	int minor_ver;
	int samples;
	union
	{
		struct
		{
			unsigned int fullscreen : 1;
			unsigned int vsync : 1;
			unsigned int cursor : 1;
			unsigned int stereo : 1;
			unsigned int debug : 1;
		};
		unsigned int all;
	} flags;
};

class Viewer {
public:
	enum class ViewerType { DEPTH, COLOR, IR, UNKNOWN };
	Viewer() {}
	Viewer(const std::string &_knt_file, const ViewerType &_type);

	void init(const std::string &_knt_file, const ViewerType &_type);
	void run();

private:
	void bind(cv::Mat _texture_image);
	void commit();

	ViewerType m_type;
	KntReader m_reader;
	ViewerProperty m_info;
	GLFWmonitor *m_monitor = nullptr;
	GLFWwindow *m_window = nullptr;
	Shader m_shader;
	GLuint m_texture;
	GLuint m_vao;

	// callbacks
	static void window_resize_callback(GLFWwindow *_win, int _width, int _height);
	static void cursor_position_callback(GLFWwindow *_win, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow *_win, int button, int action, int mods);
};

class KntViewer {
public:
	KntViewer();
	~KntViewer();
	KntViewer(const KntViewer&) = delete;
	KntViewer& operator=(const KntViewer&) = delete;

	void init(const std::string &_knt_file);
	void run();
	static void thread_func(std::string _knt_file, bool _is_color);

private:
	std::string m_file;
	Viewer m_color_viewer;
	Viewer m_depth_viewer;
	KntReader m_reader;
};

#endif