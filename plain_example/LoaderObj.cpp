#include "LoaderObj.h"
#include <qtextstream.h>
#include <qregularexpression.h>
#include <qdebug.h>

using namespace std;
//Two helper structs, because we can't use float arrays in templates
struct VertexPos
{
	float x;
	float y;
	float z;
};

struct VertexTex
{
	float u;
	float v;
};

LoaderObj::LoaderObj(const char* fileName):
	m_vertexCount(0),
	m_indexCount(0),
	m_texCount(0)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical(file.errorString().toLatin1().data());
	}
	QTextStream stream(&file);
	QList<VertexPos> vertices;
	QList<VertexTex> texcoords;
	int texCount;
	QList<int> indices;
	QString allLines = stream.readAll();
	QStringList all = allLines.split('\n', QString::SkipEmptyParts);
	QString line;
	//while(!stream.atEnd())
	int curLine = 0;
	int numLines = all.size();
	while(curLine < numLines)
    {
		if(curLine%10000==0)
		{
			qDebug() << "line: " << curLine << " of " << numLines << " (" << ((float)curLine/(float)numLines)*100.f << "%)";
		}
		VertexPos vertex;
		VertexTex texcoord;
		QStringList vals;
		QByteArray ba = all[curLine].toLatin1();
		line = all[curLine];
		//const char* pszBuff = line.toStdString().c_str();
		const char* pszBuff = ba.constData();
  		switch(pszBuff[0])
		{
		case 'v':
			switch(pszBuff[1]){
			case ' ':
				//Create new vertex
				sscanf_s(pszBuff, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				vertices.push_back(vertex);
				++m_vertexCount;
				break;
			case 'n':
				//Create new vertex normal
				//sscanf(pszBuff, "vn %f %f %f", &x, &y, &z);
				//normals.push_back(vertex);
				//++m_normalsCount;
				break;
			case 't':
				//Create new UV
				sscanf(pszBuff, "vt %f %f", &texcoord.u, &texcoord.v);
				texcoords.push_back(texcoord);
				++m_texCount;
				break;
			}
			break;

		case 'f':
			vals = line.split(' ', QString::SkipEmptyParts);
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
			}
			break;
		default:
			;//just go on
		}
		curLine++;
    }
	//m_vertices contains only positions. I could also be calls "m_positions". Normals can also be counted to a vertex.
	//Historically positions and normals where in the same buffer. The interpretation of the floats in the buffer could be configured.
	// In new versions of OpenGL, position and normals can be defined in different buffers.
	m_vertices = new GLfloat[3*m_vertexCount];
	m_normals = new GLfloat[m_vertexCount*3];
	m_texCoords = new GLfloat[m_vertexCount*2];
	m_tangents = new GLfloat[m_vertexCount*3];
	m_bitangents = new GLfloat[m_vertexCount*3];
	m_indices = new GLuint[indices.size()];
	
	for(int i=m_vertexCount-1 ; i>=0 ; i--)
	{
		m_vertices[i*3+0] = vertices[i].x;
		m_vertices[i*3+1] = vertices[i].y;
		m_vertices[i*3+2] = vertices[i].z;
	}

	for(int i=0 ; i<indices.size()-1 ; i+=3)
	{
		m_indices[i] = indices[i];
		m_indices[i+1] = indices[i+1];
		m_indices[i+2] = indices[i+2];
	}
	qDebug() << "Vertices:" << m_vertexCount << "\nIndices:" << m_indexCount << "\n";

	///// Texture Coordinates /////
	if(m_texCount != 0)
	{
		for(int i=m_texCount-1 ; i>=0 ; i--)
		{
			m_texCoords[i*2+0] = texcoords[i].u;
			m_texCoords[i*2+1] = texcoords[i].v;
		}
	}
	else
	{
		/// Texture Coordinates (Spherical)
		// Convert vertex positions to spherical coordinates.
		// see wikipedia for more info http://de.wikipedia.org/wiki/Kugelkoordinaten
		for(int i=m_vertexCount-1 ; i>=0 ; i--)
		{
			float x = m_vertices[i*3+0];
			float y = m_vertices[i*3+1];
			float z = m_vertices[i*3+2];

			float r = sqrtf(powf(x,2.f)+powf(y,2.f)+powf(z,2.f));
			float phi = atan2f(x, y);
			float theta = acosf(z/r);
			m_texCoords[i*2+0] = phi;
			m_texCoords[i*2+1] = theta;
		}
	}

	///// Calculate Normals /////
	// Algorithm:
	// 1. for each triangle, calculate the normal: use two sides of the tri (a,b) and calculate the cross product
	// 2. normalize the normal (length = 1)
	// 3. Add it to the vertex' normal and count it's accumulated normals
	// 4. divide each vertex' normal by the number of normals that where added in order to normalize it again.

	memset(m_normals, 0, m_vertexCount*3*sizeof(GLfloat));
	memset(m_tangents, 0, m_vertexCount*3*sizeof(GLfloat));
	memset(m_bitangents, 0, m_vertexCount*3*sizeof(GLfloat));
	unsigned short *adjacentFaces = new unsigned short[m_vertexCount];
	memset(adjacentFaces, 0, m_vertexCount*sizeof(unsigned short));

	GLfloat a[3], b[3], crossprod[3], tangent[3], bitangent[3], deltaUV1[2], deltaUV2[2];
	GLfloat invlength;
	for(int i=0 ; i<indices.size()-1 ; i+=3)
	{
		GLint i1 = m_indices[i];
		GLint i2 = m_indices[i+1];
		GLint i3 = m_indices[i+2];
		//construct pointer to the three indexed vertices
		GLfloat* pV1 = m_vertices+i1*3;
		GLfloat* pV2 = m_vertices+i2*3;
		GLfloat* pV3 = m_vertices+i3*3;
		//get the difference in position (two edges of the triangle)
		//vec3 a = p2.xyz - p1.xyz;
		//vec3 b = p2.xyz - p3.xyz;
		a[0] = pV1[0]-pV2[0];
		a[1] = pV1[1]-pV2[1];
		a[2] = pV1[2]-pV2[2];
		b[0] = pV2[0]-pV3[0];
		b[1] = pV2[1]-pV3[1];
		b[2] = pV2[2]-pV3[2];
		//cross
		crossprod[0] = a[1]*b[2]-a[2]*b[1];
		crossprod[1] = a[2]*b[0]-a[0]*b[2];
		crossprod[2] = a[0]*b[1]-a[1]*b[0];
		//normalize, switch side (ccw triangles)
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

		/// calculate tanget / bitangent ///
		// use uv texture coordinates of the indexed vertices (construct pointers)
		GLfloat* pUV1 = m_texCoords+i1*2;
		GLfloat* pUV2 = m_texCoords+i2*2;
		GLfloat* pUV3 = m_texCoords+i3*2;
		deltaUV1[0] = pUV2[0]-pUV1[0];
		deltaUV1[1] = pUV2[1]-pUV1[1];
		deltaUV2[0] = pUV2[0]-pUV3[0];
		deltaUV2[1] = pUV2[1]-pUV3[1];

		float r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);
		tangent[0] = (a[0] * deltaUV2[1]   - b[0] * deltaUV1[1])*r;
		tangent[1] = (a[1] * deltaUV2[1]   - b[1] * deltaUV1[1])*r;
		tangent[2] = (a[2] * deltaUV2[1]   - b[2] * deltaUV1[1])*r;
		bitangent[0] = (b[0] * deltaUV1[0]   - a[0] * deltaUV2[0])*r;
		bitangent[1] = (b[1] * deltaUV1[0]   - a[1] * deltaUV2[0])*r;
		bitangent[2] = (b[2] * deltaUV1[0]   - a[2] * deltaUV2[0])*r;
		
		m_tangents[i1*3] += tangent[0];
		m_tangents[i1*3+1] += tangent[1];
		m_tangents[i1*3+2] += tangent[2];
		m_tangents[i2*3] += tangent[0];
		m_tangents[i2*3+1] += tangent[1];
		m_tangents[i2*3+2] += tangent[2];
		m_tangents[i3*3] += tangent[0];
		m_tangents[i3*3+1] += tangent[1];
		m_tangents[i3*3+2] += tangent[2];
		
		m_bitangents[i1*3] += tangent[0];
		m_bitangents[i1*3+1] += tangent[1];
		m_bitangents[i1*3+2] += tangent[2];
		m_bitangents[i2*3] += tangent[0];
		m_bitangents[i2*3+1] += tangent[1];
		m_bitangents[i2*3+2] += tangent[2];
		m_bitangents[i3*3] += tangent[0];
		m_bitangents[i3*3+1] += tangent[1];
		m_bitangents[i3*3+2] += tangent[2];

		adjacentFaces[i1]++;
		adjacentFaces[i2]++;
		adjacentFaces[i3]++;
	}
	//normalize
	for(unsigned int i=0 ; i<m_vertexCount ; i++)
	{
		GLfloat invlength = 1.f/adjacentFaces[i];
		m_normals[i*3] *= invlength;
		m_normals[i*3+1] *= invlength;
		m_normals[i*3+2] *= invlength;

		m_tangents[i*3] *= invlength;
		m_tangents[i*3+1] *= invlength;
		m_tangents[i*3+2] *= invlength;

		m_bitangents[i*3] *= invlength;
		m_bitangents[i*3+1] *= invlength;
		m_bitangents[i*3+2] *= invlength;

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
	delete [] m_texCoords;
	delete [] m_tangents;
	delete [] m_bitangents;
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

GLfloat* LoaderObj::getVB() const
{
	return m_vertices;
}

GLuint* LoaderObj::getIB() const
{
	return m_indices;
}

GLfloat* LoaderObj::getCalculatedTexCoords() const
{
	return m_texCoords;
}

GLfloat* LoaderObj::getCalculatedTangents() const
{
	return m_tangents;
}

GLfloat* LoaderObj::getCalculatedBitangents() const
{
	return m_bitangents;
}