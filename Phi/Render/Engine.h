#pragma once

namespace Engine
{
	enum PassFrame
	{
		Pre, 
		Normal,
		Post
	};
	
	enum PassElement
	{		
		Solid,
		Transparent
	};
	
	
	template <typename type>
	struct BinderTemplate {
	public:
		BinderTemplate(type& object) : mObject(object) { mObject.bind(); }
		~BinderTemplate() { type::unbind(); }
	private:
		type& mObject;
	};
		
	struct Buffer
	{
	protected:
		Buffer() { mId = -1; }
		~Buffer();
	public:
		GLuint id() { return mId;}
		void    destroy();

	protected:
		void    create(int type, int flags, int size, const void* data = NULL);
		void    bind(int type);
		void*   map(int type, int access);
		void    unmap(int type);
		void    update(int type, const void*data, int offset, int size);
	protected:
		static void unbind(int type);
		
	protected:
		GLuint mId;
	};
	
#define BUFFER_TYPE(name, gltype) \
	struct name : public Buffer \
	{\
	public:\
		typedef BinderTemplate<name> Binder; \
	public:\
		inline void     create(int flags, int size, const void* data = NULL) { Buffer::create(gltype, flags, size, data); } \
		inline void     bind() { Buffer::bind(gltype); } \
		inline void 	update(const void* data, int offset, int size) { Buffer::update(gltype, data, offset, size); } \
		inline void* 	map(int access) { return Buffer::map(gltype, access); } \
		inline void     unmap() { Buffer::unmap(gltype); } \
	public: \
		inline void operator () () { bind(); } \
	public: \
		static  void unbind() { Buffer::unbind(gltype); }\
	};
	
	BUFFER_TYPE(VertexBuffer, GL_ARRAY_BUFFER);
	BUFFER_TYPE(IndexBuffer, GL_ELEMENT_ARRAY_BUFFER);
	
	struct Shader
	{
	public:
		Shader() { mId = -1; mShaderCount = 0; }
		~Shader();
	public:
		typedef BinderTemplate<Shader> Binder;

	public:
		void create();
		void add(const char* shader, int type);
		void addFromFile(const char* shaderFile, int type);
		void bindAttribute(const char* name, int location);
		void link();
		
		void destroy();
		void bind();
		void validate();
		int  getAttribute(const char* name);
		int  getUniform(const char* name);
		void setUniform(int id, glm::mat4& v);
		void setUniform(int id, glm::mat3& v);
		void setUniform(int id, glm::vec4& v);
		void setUniform(int id, glm::vec3& v);
		void setUniform(int id, glm::vec2& v);
		void setUniform(int id, float v);
		void setUniform(int id, int v);

		
	public:
		inline void operator () () { bind(); }
	
	public:
		static void unbind();

	protected:
		void   printLog(int id, const char* desc, bool isShader);
		GLuint compile(int type, const char* src);
		
	protected:
		GLuint mId;
		GLuint mShaderId[3];
		int    mShaderCount;
	};
	
	struct VertexArray
	{
	public:
		VertexArray() { mId = -1; }
		~VertexArray() { destroy(); }
	public:
		typedef BinderTemplate<VertexArray> Binder;

	public:
		void create();
		void destroy();
		void bind();
		
	public:
		inline void operator () () { bind(); }
	
	public:
		static void attrib(GLuint index, int size, int type, int normalized, int stride, int offset);
		static void unbind();
		
	private:
		GLuint mId;
	};
	
	struct Texture
	{
	public:
		Texture() { mId = -1; }
		~Texture() { destroy(); }
	public:
		
	public:
		void createFromFile(const std::string& filePath, int type);
		void createFromBuffer(GLubyte* buffer, int width, int height, int type, int channels);
		void destroy();
		void bind(int unit);
		
	public:
		inline void operator () (int unit) { bind(unit); }
		
	public:
		void unbind();
		
	private:
		GLuint  mId;
		int     mUnit;
		int     mType;
		
	};

}
