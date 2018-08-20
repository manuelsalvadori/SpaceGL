#ifndef SRC_SHADER_H_
#define SRC_SHADER_H_
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

    GLuint getID();

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

private:
    // the program ID
    GLuint ID;
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif /* SRC_SHADER_H_ */
