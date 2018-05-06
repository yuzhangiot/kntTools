#include "knt_viewer.h"
#include <stdexcept>
#include <thread>

Viewer::Viewer(const std::string &_knt_file, const ViewerType &_type)
{
	init(_knt_file, _type);
}

void Viewer::init(const std::string &_knt_file, const ViewerType &_type)
{
	m_type = _type;

	// Open knt file
	m_reader.open(_knt_file);
	int width = 0;
	int height = 0;
	if (m_type == ViewerType::COLOR) {
		width = m_reader.get_color_width();
		height = m_reader.get_color_height();
	}
	else if (m_type == ViewerType::DEPTH) {
		width = m_reader.get_depth_width();
		height = m_reader.get_depth_height();
	}

	// Get primary monitor
	m_monitor = glfwGetPrimaryMonitor();

	// Setup context info
	char title[128] = { 0 };
	if (m_type == ViewerType::COLOR)
		strcpy(title, "Color Viewer");
	else if (m_type == ViewerType::DEPTH)
		strcpy(title, "Depth Viewer");

	strcpy(m_info.win_title, title);
	m_info.win_height = height;
	m_info.win_width = width;
	m_info.major_ver = 4;
	m_info.minor_ver = 5;
	m_info.samples = 1;
	m_info.flags.all = 0;
	m_info.flags.cursor = 1;

	// Setup opengl context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_info.major_ver);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_info.minor_ver);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, m_info.samples);
	glfwWindowHint(GLFW_STEREO, m_info.flags.stereo ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// Create opengl window
	if (m_info.flags.fullscreen)
	{
		if (m_info.win_height == 0 || m_info.win_width == 0)
		{
			const GLFWvidmode *vid_mode = glfwGetVideoMode(m_monitor);
			m_info.win_height = vid_mode->height;
			m_info.win_width = vid_mode->width;
		}
		m_window = glfwCreateWindow(m_info.win_width, m_info.win_height, m_info.win_title, m_monitor, 0);
		glfwSwapInterval(int(m_info.flags.vsync));
	}
	else
	{
		if (!(m_window = glfwCreateWindow(m_info.win_width, m_info.win_height, m_info.win_title, 0, 0)))
		{
			throw std::runtime_error("fatal error: failed to create window !");
		}
	}

	// Make newly created context current
	glfwMakeContextCurrent(m_window);

	// Initialize glfw
	if (!gladLoadGL()) {
		throw std::runtime_error("fatal error: failed to initialize glad !");
	}

	// Disable depth test
	glDisable(GL_DEPTH_TEST);

	// Compile shaders
	m_shader.compile(m_type==ViewerType::COLOR);
	m_shader.use();

	// Generate vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a texture
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

	// Set viewport
	glViewport(0, 0, m_info.win_width, m_info.win_height);

	// Bind all callbacks
	glfwSetWindowSizeCallback(m_window, window_resize_callback);
	glfwSetCursorPosCallback(m_window, cursor_position_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);
}

void Viewer::bind(cv::Mat _texture_image)
{
	int width = _texture_image.cols;
	int height = _texture_image.rows;

	if (m_type == ViewerType::COLOR) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _texture_image.data);
	}
	else if (m_type == ViewerType::DEPTH) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16, width, height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_SHORT, _texture_image.data);
	}
}

void Viewer::commit()
{
	GLfloat color_clear[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat depth_clear = 1.0f;

	glClearBufferfv(GL_COLOR, 0, color_clear);
	glClearBufferfv(GL_DEPTH, 0, &depth_clear);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glfwSwapBuffers(m_window);
}

void Viewer::window_resize_callback(GLFWwindow *_win, int _width, int _height)
{
	glViewport(0, 0, _width, _height);
}

void Viewer::cursor_position_callback(GLFWwindow *_win, double xpos, double ypos)
{

}

void Viewer::mouse_button_callback(GLFWwindow *_win, int button, int action, int mods)
{

}

void Viewer::run()
{
	int frame_idx = 0;
	int nframes = 0;
	if (m_type == ViewerType::COLOR)
		nframes = m_reader.get_color_frames();
	else if (m_type == ViewerType::DEPTH)
		nframes = m_reader.get_depth_frames();

	if (nframes == 0)
		return;

	while (!glfwWindowShouldClose(m_window)) {
		cv::Mat new_frame;
		if (m_type == ViewerType::COLOR)
			new_frame = m_reader.get_color_frame(frame_idx);
		else if (m_type == ViewerType::DEPTH)
			new_frame = m_reader.get_depth_frame(frame_idx);

		bind(new_frame);
		commit();

		++frame_idx;
		if (frame_idx == nframes)
			frame_idx = 0;

		glfwPollEvents();
	}
}

KntViewer::KntViewer()
{
	if (!glfwInit()) {
		throw std::runtime_error("fatal error: cannot initialize glfw !");
	}
}

KntViewer::~KntViewer()
{
	glfwTerminate();
}

void KntViewer::init(const std::string &_knt_file)
{
	//m_reader.open(_knt_file);
	m_file = _knt_file;
}

void KntViewer::run()
{
	std::vector<std::thread> threads;
	threads.emplace_back(&KntViewer::thread_func, m_file, false);
	threads.emplace_back(&KntViewer::thread_func, m_file, true);
	threads[0].join();
	threads[1].join();
}

void KntViewer::thread_func(std::string _knt_file, bool _is_color)
{
	if (_is_color) {
		Viewer color_viewer(_knt_file, Viewer::ViewerType::COLOR);
		color_viewer.run();
	}
	else {
		Viewer depth_viewer(_knt_file, Viewer::ViewerType::DEPTH);
		depth_viewer.run();
	}
}