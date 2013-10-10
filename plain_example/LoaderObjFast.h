#pragma once

#include <qfile.h>
#include <qlist.h>
#include <qgl.h>
#include <vector>

struct Float3;

class LoaderObjFast
{
private:
	GLfloat* m_vertices;
	GLuint* m_indices;
	GLfloat* m_normals;
	void prepareBuffers();
public:
	GLuint m_vertexCount;
	GLuint m_indexCount;
	//FIX: Evil bug when using QList: https://bugreports.qt-project.org/browse/QTBUG-7884
	//QList<Float3> m_listVertices;
	//QList<GLuint> m_listIndices;
	//QList<Float3> m_listNormals;
	std::vector<Float3> m_listVertices;
	std::vector<GLuint> m_listIndices;
	std::vector<Float3> m_listNormals;
	LoaderObjFast(const char* fileName);
	virtual ~LoaderObjFast(void);
	
	GLuint getVertexCount() const;
	GLuint getIndexCount() const;
//	GLuint getFloatsPerVert() const;
	GLfloat* getVB() const;
	GLuint* getIB() const;
	GLfloat* getCalculatedNormals() const;
};

