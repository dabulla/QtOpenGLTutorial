#include "LoaderObj.h"
#include <qtextstream.h>
#include <qregularexpression.h>
#include <qdebug.h>

using namespace std;

LoaderObj::LoaderObj(QFile& file):
	m_floatsPerVert(0),
	m_vertexCount(0),
	m_indexCount(0)
{
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical(file.errorString().toLatin1().data());
	}
	QTextStream stream(&file);
	QList<QList<float>> vertices;
	QList<QList<float>> texcoords;
	int floatsPerVert=0, floatsPerTexcoord=0;
	QList<int> indices;
	QString line;
	int vertexTexCount = 0;
	while(!stream.atEnd())
    {
		QList<float> vertex;
		QList<float> texcoord;
		QStringList vals;
		line = stream.readLine().trimmed();
		switch(line[0].cell())
		{
		case 'v':
			if(line[1].cell() == 't')
			{
				vals = line.split(QRegularExpression("[ \\t]+"));
				vals.pop_front(); // get rid of 'vt'
				if(floatsPerTexcoord == 0)
				{
					floatsPerTexcoord = vals.size();
				}
				else if(floatsPerTexcoord != vals.size())
				{
					qCritical("invariant number of vertex attributes (texcoords) in the file.");
				}
				while(!vals.empty())
				{
					texcoord.push_back(vals.front().toFloat());
					vals.pop_front();
				}
				texcoords.push_back(texcoord);
				++vertexTexCount;
			}
			else
			{
				vals = line.split(QRegularExpression("[ \\t]+"));
				vals.pop_front(); // get rid of 'v'
				if(floatsPerVert == 0)
				{
					floatsPerVert = vals.size();
				}
				else if(floatsPerVert != vals.size())
				{
					qCritical("invariant number of vertex attributes in the file.");
				}
				while(!vals.empty())
				{
					vertex.push_back(vals.front().toFloat());
					vals.pop_front();
				}
				vertices.push_back(vertex);
				++m_vertexCount;
			}
			break;
		case 'f':
			vals = line.split(QRegularExpression("[ \\t]+"));
			vals.pop_front(); // get rid of 'f'
			if(vals.size() == 3)
			{
				if(vals[0].contains("/")) // we have texture coordinate indices.
				{
					vals[0] = vals[0].split("/")[0];
					vals[1] = vals[1].split("/")[0];
					vals[2] = vals[2].split("/")[0];
				}
				int a = vals[0].toInt()-1;
				int b = vals[1].toInt()-1;
				int c = vals[2].toInt()-1;
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				m_indexCount += 3;
			}
			else if(vals.size() == 4)
			{
				if(vals[0].contains("/")) // we have texture coordinate indices.
				{
					vals[0] = vals[0].split("/")[0];
					vals[1] = vals[1].split("/")[0];
					vals[2] = vals[2].split("/")[0];
					vals[3] = vals[3].split("/")[0];
				}
				int a = vals[0].toInt()-1;
				int b = vals[1].toInt()-1;
				int c = vals[2].toInt()-1;
				int d = vals[3].toInt()-1;
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(a);
				indices.push_back(c);
				indices.push_back(d);
				m_indexCount += 6;
			}
			else
			{
				qCritical("3DSModel has faces with more than 4 or less than 3 vertices!");
				// TODO if there are more than three verts, make tris out of them
				if(vals.size() < 3)
				{
					break;
				}
			}
			break;
		default:
			;//just go on
		}
    }

	m_floatsPerVert = floatsPerVert+floatsPerTexcoord;
	m_vertices = new GLfloat[m_floatsPerVert*m_vertexCount];
	m_indices = new GLuint[indices.size()];
	
	for(int i=m_vertexCount-1 ; i>=0 ; i--)
	{
		if(vertices[i].size() != floatsPerVert || (!texcoords.empty() && texcoords[i].size() != floatsPerTexcoord))
		{
			qCritical("Vertexpositions or texture coordinates were not correctly parsed.");
		}
		for(int j=vertices[i].size()-1 ; j>=0 ; j--)
		{
			m_vertices[i*m_floatsPerVert+j] = vertices[i][j];
		}
		if((!texcoords.empty()))
		{
			for(int j=texcoords[i].size()-1 ; j>=0 ; j--)
			{
				m_vertices[i*m_floatsPerVert+j+floatsPerVert] = texcoords[i][j];
			}
		}
	}
	for(int i=(indices.size()-1) ; i>=0 ; i-=3)
	{
		m_indices[i] = indices[i];
		m_indices[i-1] = indices[i-1];
		m_indices[i-2] = indices[i-2];
	}
	char buff[64];
	qDebug() << "Vertices:" << m_vertexCount << "\nIndices:" << m_indexCount << "\nFloatsPerVert:" << m_floatsPerVert << "\n";
}

LoaderObj::~LoaderObj(void)
{
	delete [] m_vertices;
	delete [] m_indices;
}

GLuint LoaderObj::getVertexCount()
{
	return m_vertexCount;
}
GLuint LoaderObj::getIndexCount()
{
	return m_indexCount;
}

GLuint LoaderObj::getFloatsPerVert()
{
	return m_floatsPerVert;
}

GLfloat* LoaderObj::getVB()
{
	return m_vertices;
}

GLuint* LoaderObj::getIB()
{
	return m_indices;
}
