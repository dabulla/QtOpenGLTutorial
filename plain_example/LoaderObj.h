#pragma once

#include <qfile.h>
#include <qlist.h>
#include <qgl.h>

class LoaderObj
{
private:
	GLuint m_vertexCount;
	GLuint m_indexCount;
	GLuint m_texCount;
	GLfloat *m_vertices;
	GLfloat *m_normals;
	GLfloat *m_texCoords;
	GLfloat *m_tangents;
	GLfloat *m_bitangents;
	GLuint *m_indices;
public:
	LoaderObj(const char* fileName);
	virtual ~LoaderObj(void);
	
	GLuint getVertexCount() const;
	GLuint getIndexCount() const;
	GLfloat* getVB() const;
	GLuint* getIB() const;
	GLfloat* getCalculatedNormals() const;
	GLfloat* getCalculatedTexCoords() const;
	GLfloat* getCalculatedTangents() const;
	GLfloat* getCalculatedBitangents() const;
};

