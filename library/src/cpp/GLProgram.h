#ifndef OF_3D_ENGINE_GLPROGRAM_H
#define OF_3D_ENGINE_GLPROGRAM_H

#include "GPCP-Common.h"

NS_GPCP_BEGIN

    class GLProgram {
    public:
        GLProgram();

        virtual ~GLProgram();

        static GLProgram *createWithShaderString(const string &vertexShaderString, const string &fragmentShaderString);

        void addAttribute(string attributeName);

        GLint attributeIndex(string attributeName);

        GLint uniformIndex(string uniformName);

        bool link();

        void use();

        void validate();

    private:
        GLuint _program;
        GLuint _vertShader;
        GLuint _fragShader;

        vector<string> _attributes;
        vector<string> _uniforms;

        void initWithShaderString(const string &vertexShaderString, const string &fragmentShaderString);

        bool compileShader(GLuint *shader, GLenum type, string shaderString);

        void releaseShader(GLuint program, GLuint shader);
    };

NS_GPCP_END

#endif // OF_3D_ENGINE_GLPROGRAM_H