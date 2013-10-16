#ifndef MATERIAL_H
#define MATERIAL_H

#include <texture.h>

#include <QMap>
#include <QOpenGLShaderProgram>
#include <QPair>
#include <QSharedPointer>

typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class TextureUnitConfiguration : public QPair<TexturePtr, GLuint>
{
public:
    TextureUnitConfiguration()
        : QPair<TexturePtr, GLuint>( TexturePtr(), 0 )
    {
    }

    explicit TextureUnitConfiguration( const TexturePtr& texture, const GLuint& sampler )
        : QPair<TexturePtr, GLuint>( texture, sampler )
    {
    }

    void setTexture( const TexturePtr& texture ) { first = texture; }
    TexturePtr texture() const { return first; }

    void setSampler( const GLuint sampler ) { second = sampler; }
    GLuint sampler() const { return second; }
};

class QOpenGLFunctions_4_2_Core;

class Material
{
public:
    Material();
    ~Material();

    void bind();

    void setShaders( const QString& vertexShader,
                     const QString& fragmentShader );
    void setShaders( const QString& vertexShader,
                     const QString& geometryShader,
                     const QString& fragmentShader );
    void setShaders( const QString& vertexShader,
                     const QString& tessellationControlShader,
                     const QString& tessellationEvaluationShader,
                     const QString& geometryShader,
                     const QString& fragmentShader );
	void setShadersFromString( const QString& vertexShader,
                     const QString& fragmentShader );
    void setShader( const QOpenGLShaderProgramPtr& shader );

    QOpenGLShaderProgramPtr shader() const { return m_shader; }

    void setTextureUnitConfiguration( GLuint unit, TexturePtr texture, GLuint sampler );
    void setTextureUnitConfiguration( GLuint unit, TexturePtr texture, GLuint sampler, const QByteArray& uniformName );
    TextureUnitConfiguration textureUnitConfiguration( GLuint unit ) const;

private:
    // For now we assume that we own the shader
    /** \todo Allow this to use reference to non-owned shader */
    QOpenGLShaderProgramPtr m_shader;

    // This map contains the configuration for the texture units
    QMap<GLuint, TextureUnitConfiguration> m_unitConfigs;
    QMap<GLuint, QByteArray> m_samplerUniforms;

    QOpenGLFunctions_4_2_Core* m_funcs;
};

typedef QSharedPointer<Material> MaterialPtr;

#endif // MATERIAL_H
