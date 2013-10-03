#pragma once

#include <qfile.h>
#include <qlist.h>
#include <qgl.h>

struct Float3;

class LoaderObj
{
private:
	GLfloat* m_vertices;
	GLuint* m_indices;
	GLfloat* m_normals;
	void prepareBuffers();
public:
	GLuint m_vertexCount;
	GLuint m_indexCount;
	QList<Float3> m_listVertices;
	QList<GLuint> m_listIndices;
	QList<Float3> m_listNormals;
	LoaderObj(const char* fileName);
	virtual ~LoaderObj(void);
	
	GLuint getVertexCount() const;
	GLuint getIndexCount() const;
//	GLuint getFloatsPerVert() const;
	GLfloat* getVB() const;
	GLuint* getIB() const;
	GLfloat* getCalculatedNormals() const;
};

