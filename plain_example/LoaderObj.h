#pragma once

#include <qfile.h>
#include <qlist.h>
#include <qgl.h>

class LoaderObj
{
private:
	GLuint m_floatsPerVert;
	GLuint m_vertexCount;
	GLuint m_indexCount;
	GLfloat* m_vertices;
	GLuint* m_indices;
	GLfloat* m_normals;
public:
	LoaderObj(QFile& file);
	virtual ~LoaderObj(void);
	
	GLuint getVertexCount() const;
	GLuint getIndexCount() const;
//	GLuint getFloatsPerVert() const;
	GLfloat* getVB() const;
	GLuint* getIB() const;
	GLfloat* getCalculatedNormals() const;
};

