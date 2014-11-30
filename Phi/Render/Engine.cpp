#include <PreCompile.h>
#include <Render/Engine.h>

namespace Engine
{
	////////////////////////////////////////////////////////////////
	// Buffer
	////////////////////////////////////////////////////////////////
	void    Buffer::create(int type, int flags, int size, const void* data)
	{
		glGenBuffers(1, &mId);
		glBindBuffer(type, mId);
		glBufferData(type, size, data, flags);
	}
	
	void    Buffer::unbind(int type)
	{
		glBindBuffer(type, 0);
	}
	
	void    Buffer::bind(int type)
	{
		glBindBuffer(type, mId);
	}
	
	void    Buffer::update(int type, const void* data, int offset, int size)
	{
		glBufferSubData(type, offset, size, data);
	}

	void*   Buffer::map(int type, int access)
	{
		return glMapBuffer(type, access);
	}

	void    Buffer::unmap(int type)
	{
		glUnmapBuffer(type);
	}
	
	void    Buffer::destroy()
	{
		if (mId != -1)
			glDeleteBuffers(1, &mId);
		mId = -1;
	}
	
	Buffer::~Buffer()
	{
		destroy();
	}
	
	///////////////////////////////////////////////////////////////////
	// Shader
	///////////////////////////////////////////////////////////////////
	GLuint Shader::compile(int type, const char *src)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			printLog(shader, "SHADER", true);
			return -1;
		}
		return shader;
	}
	
	Shader::~Shader()
	{
		destroy();
	}
	
	void Shader::destroy()
	{
		for (int i = 0; i < mShaderCount; ++i)
		{
			int id = mShaderId[i];
			if (id != -1)
			{
				glDetachShader(mId, mShaderId[i]);
				glDeleteShader(mShaderId[i]);
			}
		}
		if (mId != -1)
			glDeleteProgram(mId);
		mShaderCount = 0;
		mId = -1;
	}
	
	inline bool readAllFile(const char* fileName, std::string& out)
	{
		std::ifstream stream;
		stream.open(fileName);
		if (stream.is_open())
		{
			stream.seekg(0, std::ios::end);
			out.reserve(size_t(stream.tellg()));
			stream.seekg(0, std::ios::beg);
		
			out.assign((std::istreambuf_iterator<char>(stream)),
				   std::istreambuf_iterator<char>());
			return true;
		}
		return false;
	}
	
	void Shader::create()
	{
		mId = glCreateProgram();
	}
	
	void Shader::addFromFile(const char *shaderFile, int type)
	{
		std::string code;
		readAllFile(shaderFile, code);
		add(code.c_str(), type);
	}
	
	void Shader::add(const char *shader, int type)
	{
		GLuint id = compile(type, shader);
		glAttachShader(mId, id);
		mShaderId[mShaderCount++] = id;
	}
	
	void Shader::bindAttribute(const char* name, int location)
	{
		glBindAttribLocation(mId, location, name);
	}

	void Shader::printLog(int id, const char* desc, bool isShader)
	{
		int logLength;
		if (isShader)
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		else
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			std::string log;
			log.resize(logLength);
			if (isShader)
				glGetShaderInfoLog(id, logLength, &logLength, &log[0]);
			else
				glGetProgramInfoLog(id, logLength, &logLength, &log[0]);
			printf("%s: %s",desc, log.c_str());
		}
	}
	
	void Shader::link()
	{
		glLinkProgram(mId);
		printLog(mId, "LINK", false);
	}

	void Shader::validate()
	{
		glValidateProgram(mId);
		printLog(mId, "VALIDATE", false);
	}

	int  Shader::getAttribute(const char* name)
	{
		return glGetAttribLocation(mId, name);
	}
	
	int  Shader::getUniform(const char* name)
	{
		return glGetUniformLocation(mId, name);
	}
	
	void Shader::bind()
	{
		glUseProgram(mId);
	}
	
	void Shader::setUniform(int id, glm::mat4& v)
	{
		glUniformMatrix4fv(id, 1, GL_FALSE, &v[0][0]);
	}

	void Shader::setUniform(int id, glm::mat3& v)
	{
		glUniformMatrix3fv(id, 1, GL_FALSE, &v[0][0]);
	}

	void Shader::setUniform(int id, glm::vec4& v)
	{
		glUniform4fv(id, 1, &v[0]);
	}
	
	void Shader::setUniform(int id, glm::vec3& v)
	{
		glUniform3fv(id, 1, &v[0]);
	}

	void Shader::setUniform(int id, glm::vec2& v)
	{
		glUniform2fv(id, 1, &v[0]);
	}

	void Shader::setUniform(int id, float v)
	{
		glUniform1fv(id, 1, &v);
	}

	void Shader::setUniform(int id, int v)
	{
		glUniform1iv(id, 1, &v);
	}

	void Shader::unbind()
	{
		glUseProgram(0);
	}
	
	void VertexArray::destroy()
	{
		if (mId != -1)
			glDeleteVertexArrays(1, &mId);
		mId = -1;
	}
	
	void VertexArray::create()
	{
		glGenVertexArrays(1, &mId);
	}
	
	void VertexArray::bind()
	{
		glBindVertexArray(mId);
	}
	
	void VertexArray::unbind()
	{
		glBindVertexArray(0);
	}
	
	void VertexArray::attrib(GLuint index, int size, int type, int normalized, int stride, int offset)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalized, stride, (void*) (size_t)offset);
	}
	
	void Texture::createFromBuffer(GLubyte* buffer, int width, int height, int type, int channels)
	{
		glGenTextures(1, &mId);
		GLint internals = 0;
		
		switch (channels)
		{
		case 1: internals = GL_RED; break;
		case 2: internals = GL_RG; break;
		case 3: internals = GL_RGB; break;
		case 4: internals = GL_RGBA; break;
		}
		mType = type;
		bind(0);
		glTexImage2D(mType, 0, internals, width, height, 0, internals, GL_UNSIGNED_BYTE, buffer);
		unbind();
	}

	void Texture::createFromFile(const std::string &filePath, int type)
	{
		mType = type;
	}
	
	void Texture::destroy()
	{
		glDeleteTextures(1, &mId);
	}
	
	void Texture::bind(int unit)
	{
		mUnit = unit;
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(mType, mId);
	}
	
	void Texture::unbind()
	{
		glActiveTexture(GL_TEXTURE0 + mUnit);
		glBindTexture(mType, 0);
	}
	
};
