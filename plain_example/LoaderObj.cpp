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
				//vals = line.split(QRegularExpression("[ \\t]+"));
				//vals.pop_front(); // get rid of 'vt'
				//if(floatsPerTexcoord == 0)
				//{
				//	floatsPerTexcoord = vals.size();
				//}
				//else if(floatsPerTexcoord != vals.size())
				//{
				//	qCritical("invariant number of vertex attributes (texcoords) in the file.");
				//}
				//while(!vals.empty())
				//{
				//	texcoord.push_back(vals.front().toFloat());
				//	vals.pop_front();
				//}
				//texcoords.push_back(texcoord);
				//++vertexTexCount;
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
				//1 based index in file. 0 based index in our datastructure
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
				qCritical("Model has faces with more than 4 or less than 3 vertices!");
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
	//m_vertices contains only positions. I could also be calls "m_positions". Normals can also be counted to a vertex.
	//Historically positions and normals where in the same buffer. The interpretation of the floats in the buffer could be configured.
	// In new versions of OpenGL, position and normals can be defined in different buffers.
	m_vertices = new GLfloat[m_floatsPerVert*m_vertexCount];
	m_normals = new GLfloat[m_vertexCount*3];
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

	for(int i=0 ; i<indices.size()-1 ; i+=3)
	{
		m_indices[i] = indices[i];
		m_indices[i+1] = indices[i+1];
		m_indices[i+2] = indices[i+2];
	}
	char buff[64];
	qDebug() << "Vertices:" << m_vertexCount << "\nIndices:" << m_indexCount << "\nFloatsPerVert:" << m_floatsPerVert << "\n";


	///// Calculate Normals /////
	// Algorithm:
	// 1. for each triangle, calculate the normal: use two sides of the tri (a,b) and calculate the cross product
	// 2. normalize the normal (length = 1)
	// 3. Add it to the vertex' normal and count it's accumulated normals
	// 4. divide each vertex' normal by the number of normals that where added in order to normalize it again.

	memset(m_normals, 0, m_vertexCount*3*sizeof(GLfloat));
	unsigned short *adjacentFaces = new unsigned short[m_vertexCount];
	memset(adjacentFaces, 0, m_vertexCount*sizeof(unsigned short));

	GLfloat a[3], b[3], crossprod[3];
	GLfloat invlength;
	for(int i=0 ; i<indices.size()-1 ; i+=3)
	{
		GLint i1 = m_indices[i];
		GLint i2 = m_indices[i+1];
		GLint i3 = m_indices[i+2];
		GLfloat* pV1 = m_vertices+i1*3;
		GLfloat* pV2 = m_vertices+i2*3;
		GLfloat* pV3 = m_vertices+i3*3;
		//vec3 a = p1.xyz - p2.xyz;
		//vec3 b = p3.xyz - p2.xyz;
		a[0] = pV1[0]-pV2[0];
		a[1] = pV1[1]-pV2[1];
		a[2] = pV1[2]-pV2[2];
		b[0] = pV3[0]-pV2[0];
		b[1] = pV3[1]-pV2[1];
		b[2] = pV3[2]-pV2[2];
		//cross
		crossprod[0] = a[1]*b[2]-a[2]*b[1];
		crossprod[1] = a[2]*b[0]-a[0]*b[2];
		crossprod[2] = a[0]*b[1]-a[1]*b[0];
		//normalize
		invlength = 1.f/sqrt(pow(crossprod[0],2.f)+pow(crossprod[1],2.f)+pow(crossprod[2],2.f));
		crossprod[0] *= invlength;
		crossprod[1] *= invlength;
		crossprod[2] *= invlength;

		m_normals[i1*3] += crossprod[0];
		m_normals[i1*3+1] += crossprod[1];
		m_normals[i1*3+2] += crossprod[2];
		m_normals[i2*3] += crossprod[0];
		m_normals[i2*3+1] += crossprod[1];
		m_normals[i2*3+2] += crossprod[2];
		m_normals[i3*3] += crossprod[0];
		m_normals[i3*3+1] += crossprod[1];
		m_normals[i3*3+2] += crossprod[2];
		adjacentFaces[i1]++;
		adjacentFaces[i2]++;
		adjacentFaces[i3]++;
	}
	//normalize
	for(int i=0 ; i<m_vertexCount ; i++)
	{
		GLfloat invlength = 1.f/adjacentFaces[i];
		m_normals[i*3] *= invlength;
		m_normals[i*3+1] *= invlength;
		m_normals[i*3+2] *= invlength;
	}

	delete [] adjacentFaces;
}

GLfloat* LoaderObj::getCalculatedNormals() const
{
	return m_normals;
}

LoaderObj::~LoaderObj(void)
{
	delete [] m_vertices;
	delete [] m_normals;
	delete [] m_indices;
}

GLuint LoaderObj::getVertexCount() const
{
	return m_vertexCount;
}
GLuint LoaderObj::getIndexCount() const
{
	return m_indexCount;
}

//GLuint LoaderObj::getFloatsPerVert() const
//{
//	return m_floatsPerVert;
//}

GLfloat* LoaderObj::getVB() const
{
	return m_vertices;
}

GLuint* LoaderObj::getIB() const
{
	return m_indices;
}
