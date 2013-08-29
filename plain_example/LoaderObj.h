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
public:
	LoaderObj(QFile& file);
	virtual ~LoaderObj(void);
	
	GLuint getVertexCount();
	GLuint getIndexCount();
	GLuint getFloatsPerVert();
	GLfloat* getVB();
	GLuint* getIB();
};

