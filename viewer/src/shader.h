#ifndef _SHADER_H_
#define _SHADER_H_

#include <iostream>
#include <string>
#include <glad/glad.h>

class Shader
{
public:

	Shader();
	~Shader();

	void		compile(bool is_color = true);
	void		use() const { glUseProgram(m_program_id); }
	GLuint		get_program_id() const { return m_program_id; }

private:
	// Shader ids and program id
	GLuint		m_vertex_shader_id;
	GLuint		m_fragment_shader_id;
	GLuint		m_program_id;

	// Shader source
	const char *m_vertex_source;
	const char *m_fragment_source;
};

#endif