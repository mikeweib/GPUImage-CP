
#include "GLProgram.h"

#include "GPCP-Common.h"

USING_NS_GPCP;

NS_GPCP_BEGIN

    GLProgram *GLProgram::createWithShaderString(string vertexShaderString, string fragmentShaderString) {
        GLProgram *program = new GLProgram();

        program->initWithShaderString(vertexShaderString, fragmentShaderString);

        return program;
    }

    GLProgram::GLProgram() : _program(0),
                             _vertShader(0),
                             _fragShader(0) {

    }

    GLProgram::~GLProgram() {
        releaseShader(_program, _vertShader);
        releaseShader(_program, _fragShader);

        _vertShader = 0;
        _fragShader = 0;

        if (_program)
            glDeleteProgram(_program);
    }

    void GLProgram::addAttribute(string attributeName) {
        if (find(_attributes.begin(), _attributes.end(), attributeName) == _attributes.end()) {
            glBindAttribLocation(_program,
                                 _attributes.size(),
                                 attributeName.c_str());
            _attributes.emplace_back(attributeName);
        }
    }

    GLint GLProgram::attributeIndex(string attributeName) {
        return distance(_attributes.begin(), find(_attributes.begin(), _attributes.end(), attributeName));
    }

    GLint GLProgram::uniformIndex(string uniformName) {
        return glGetUniformLocation(_program, uniformName.c_str());
    }

    bool GLProgram::link() {
        GLint status;

        glLinkProgram(_program);

        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        return (status == GL_TRUE);
    }

    void GLProgram::use() {
        glUseProgram(_program);
    }

    void GLProgram::validate() {
        GLint logLength;

        glValidateProgram(_program);
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *logContent = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(_program, logLength, &logLength, logContent);
            LOGD("Validate program log: [%s]", logContent);
            free(logContent);
        }
    }


    // private
    void GLProgram::initWithShaderString(string vertexShaderString, string fragmentShaderString) {
        _program = glCreateProgram();

        if(!compileShader(&_vertShader, GL_VERTEX_SHADER, vertexShaderString)) {
            LOGD("Failed to compile vertex shader");
        }

        if(!compileShader(&_fragShader, GL_FRAGMENT_SHADER, fragmentShaderString)) {
            LOGD("Failed to compile vertex shader");
        }

        glAttachShader(_program, _vertShader);
        glAttachShader(_program, _fragShader);
    }

    bool GLProgram::compileShader(GLuint *shader, GLenum type, string shaderString) {
        if (shaderString.empty()) {
            LOGD("Failed to load shader. shader content is empty.");
            return false;
        }

        GLint status;
        const GLchar *source = (GLchar *) shaderString.c_str();

        *shader = glCreateShader(type);
        glShaderSource(*shader, 1, &source, nullptr);
        glCompileShader(*shader);

        glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

        if (status != GL_TRUE) {
            GLint logLength;
            glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0) {
                GLchar *logContent = (GLchar *)calloc(logLength + 1, sizeof(GLchar));
                glGetShaderInfoLog(*shader, logLength, &logLength, logContent);

                LOGD("Compile shader error: [%s]", logContent);

                free(logContent);
            }

        }

        return (status == GL_TRUE);
    }

    void GLProgram::releaseShader(GLuint program, GLuint shader) {
        if (shader) {
            LOGD("Release shader id is %d", shader);
            glDetachShader(program, shader);
            glDeleteShader(shader);
        }
    }

NS_GPCP_END