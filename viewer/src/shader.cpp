#include <fstream>
#include "shader.h"

extern const char color_vertex_shader[];
extern const char color_fragment_shader[];
extern const char depth_vertex_shader[];
extern const char depth_fragment_shader[];

Shader::Shader() : m_vertex_shader_id(0), m_fragment_shader_id(0), m_program_id(0),
m_vertex_source(nullptr), m_fragment_source(nullptr)
{

}

Shader::~Shader()
{
	glDeleteShader(m_vertex_shader_id);
	glDeleteShader(m_fragment_shader_id);
	glDeleteProgram(m_program_id);
}

void Shader::compile(bool is_color)
{
	// Bind to vertex and fragment shaders
	if (is_color) {
		m_vertex_source = color_vertex_shader;
		m_fragment_source = color_fragment_shader;
	}
	else {
		m_vertex_source = depth_vertex_shader;
		m_fragment_source = depth_fragment_shader;
	}

	// Create vertex shader
	m_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertex_shader_id, 1, &m_vertex_source, nullptr);

	// Create fragment shader
	m_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragment_shader_id, 1, &m_fragment_source, nullptr);

	// Compile vertex shader
	glCompileShader(m_vertex_shader_id);

	GLint vertex_shader_status = GL_FALSE;
	glGetShaderiv(m_vertex_shader_id, GL_COMPILE_STATUS, &vertex_shader_status);
	if (vertex_shader_status != GL_TRUE)
	{
		int info_log_length = 0;
		char info_log_buffer[1024] = { 0 };
		glGetShaderInfoLog(m_vertex_shader_id, 1024, &info_log_length, info_log_buffer);
		std::string exception_str;
		exception_str += std::string("fatal error: failed to compile vertex shader !\n");
		exception_str += std::string("error info:\n");
		exception_str += std::string(info_log_buffer);
		throw std::runtime_error(exception_str.c_str());
	}

	// Compile fragment shader
	glCompileShader(m_fragment_shader_id);
	
	GLint fragment_shader_status = GL_FALSE;
	glGetShaderiv(m_fragment_shader_id, GL_COMPILE_STATUS, &fragment_shader_status);
	if (fragment_shader_status != GL_TRUE)
	{
		int info_log_length = 0;
		char info_log_buffer[1024] = { 0 };
		glGetShaderInfoLog(m_fragment_shader_id, 1024, &info_log_length, info_log_buffer);
		std::string exception_str;
		exception_str += std::string("fatal error: failed to compile fragment shader !\n");
		exception_str += std::string("error info:\n");
		exception_str += std::string(info_log_buffer);
		throw std::runtime_error(exception_str.c_str());
	}

	// Create a shader program and attach shaders to it
	m_program_id = glCreateProgram();
	glAttachShader(m_program_id, m_vertex_shader_id);
	glAttachShader(m_program_id, m_fragment_shader_id);

	// Linking the program
	glLinkProgram(m_program_id);

	GLint program_status = GL_FALSE;
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &program_status);
	if (program_status != GL_TRUE)
	{
		int info_log_length = 0;
		char info_log_buffer[1024] = { 0 };
		glGetProgramInfoLog(m_program_id, 1024, &info_log_length, info_log_buffer);
		std::string exception_str;
		exception_str += std::string("fatal error: failed to link shader program !\n");
		exception_str += std::string("error info:\n");
		exception_str += std::string(info_log_buffer);
		throw std::runtime_error(exception_str.c_str());
	}
}

#include "shader_file.cpp"