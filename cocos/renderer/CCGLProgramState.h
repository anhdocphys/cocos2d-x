/****************************************************************************
Copyright 2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCGLPROGRAMSTATE_H__
#define __CCGLPROGRAMSTATE_H__

#include "base/ccTypes.h"
#include "base/CCVector.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include <unordered_map>

NS_CC_BEGIN

class GLProgram;
class Texture2D;
struct Uniform;
struct VertexAttrib;

//
//
// UniformValue
//
//
class UniformValue
{
    friend class GLProgram;

public:
    UniformValue();
    UniformValue(Uniform *uniform, GLProgram* glprogram);
    ~UniformValue();

    void setFloat(float value);
    void setInt(int value);
    void setVec2(const Vector2& value);
    void setVec3(const Vector3& value);
    void setVec4(const Vector4& value);
    void setMat4(const Matrix& value);
    void setCallback(const std::function<void(Uniform*)> &callback);
    void setTexture(GLuint textureId, GLuint activeTexture);

    void apply();

protected:
	Uniform* _uniform;  // weak ref
    GLProgram* _glprogram; // weak ref
    bool _useCallback;

    union U{
        float floatValue;
        int intValue;
        float v2Value[2];
        float v3Value[3];
        float v4Value[4];
        float matrixValue[16];
        struct {
            GLuint textureId;
            GLuint textureUnit;
        } tex;
        std::function<void(Uniform*)> *callback;

        U() { memset( this, 0, sizeof(*this) ); }
        ~U(){}
        U& operator=( const U& other ) {
            memcpy(this, &other, sizeof(*this));
            return *this;
        }
    } _value;
};

//
//
// VertexAttribValue
//
//
class VertexAttribValue
{
    friend class GLProgram;
    friend class GLProgramState;

public:
    VertexAttribValue(VertexAttrib *vertexAttrib);
    VertexAttribValue();
    ~VertexAttribValue();

	void setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer);
    void setCallback(const std::function<void(VertexAttrib*)> &callback);
    void apply();

protected:
	VertexAttrib* _vertexAttrib;  // weak ref
    bool _useCallback;
    bool _enabled;

    union U{
        struct {
            GLint size;
            GLenum type;
            GLboolean normalized;
            GLsizei stride;
            GLvoid *pointer;
        } pointer;
        std::function<void(VertexAttrib*)> *callback;

        U() { memset( this, 0, sizeof(*this) ); }
        ~U(){}
        U& operator=( const U& other ) {
            memcpy(this, &other, sizeof(*this));
            return *this;
        }
    } _value;
};


/**
 GLProgramState holds the 'state' (uniforms and attributes) of the GLProgram.
 A GLProgram can be used by thousands of Nodes, but if different uniform values 
 are going to be used, then each node will need its own GLProgramState
 */
class GLProgramState : public Ref
{
    friend class GLProgramStateCache;
public:

    /** returns a new instance of GLProgramState for a given GLProgram */
    static GLProgramState* create(GLProgram* glprogram);

    /** gets-or-creates an instance of GLProgramState for a given GLProgram */
    static GLProgramState* getOrCreateWithGLProgram(GLProgram* glprogram);

    /** gets-or-creates an instance of GLProgramState for a given GLProgramName */
    static GLProgramState* getOrCreateWithGLProgramName(const std::string &glProgramName );

    void apply(const Matrix& modelView);

    void setGLProgram(GLProgram* glprogram);
    GLProgram* getGLProgram() const { return _glprogram; }

    // vertex attribs
    uint32_t getVertexAttribsFlags() const { return _vertexAttribsFlags; }
    ssize_t getVertexAttribCount() const { return _attributes.size(); }
    void setVertexAttribCallback(const std::string &name, const std::function<void(VertexAttrib*)> &callback);
    void setVertexAttribPointer(const std::string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid *pointer);

    // user defined uniforms
    ssize_t getUniformCount() const { return _uniforms.size(); }
    void setUniformInt(const std::string &uniformName, int value);
    void setUniformFloat(const std::string &uniformName, float value);
    void setUniformVec2(const std::string &uniformName, const Vector2& value);
    void setUniformVec3(const std::string &uniformName, const Vector3& value);
    void setUniformVec4(const std::string &uniformName, const Vector4& value);
    void setUniformMat4(const std::string &uniformName, const Matrix& value);
    void setUniformCallback(const std::string &uniformName, const std::function<void(Uniform*)> &callback);
    void setUniformTexture(const std::string &uniformName, Texture2D *texture);
    void setUniformTexture(const std::string &uniformName, GLuint textureId);

protected:
    GLProgramState();
    ~GLProgramState();
    bool init(GLProgram* program);
    void resetGLProgram();
    VertexAttribValue* getVertexAttribValue(const std::string &attributeName);
    UniformValue* getUniformValue(const std::string &uniformName);

    std::unordered_map<std::string, UniformValue> _uniforms;
    std::unordered_map<std::string, VertexAttribValue> _attributes;

    int _textureUnitIndex;
    uint32_t _vertexAttribsFlags;
    GLProgram *_glprogram;
};

NS_CC_END

#endif /* __CCGLPROGRAMSTATE_H__ */
