#include "LoaderObjFast.h"
#include <qtextstream.h>
#include <qregularexpression.h>
#include <qdebug.h>
#include "obj-0.1\obj.hpp"
#include "obj-0.1\obj\obj_parser.hpp"

using namespace std;

//Two helper structs, because we can't use float arrays in templates
struct Float3
{
	float x;
	float y;
	float z;
	Float3(float x, float y, float z):x(x),y(y),z(z) {}
};

struct Float2
{
	float u;
	float v;
};

LoaderObj* tempThis = (LoaderObj*)0;

void geometric_vertex_callback(obj::float_type x, obj::float_type y, obj::float_type z)
{
				tempThis->m_listVertices.push_back(Float3(x,y,z));
				++tempThis->m_vertexCount;
}

void triangular_face_geometric_vertices_callback(obj::index_type a, obj::index_type b, obj::index_type c)
{
				tempThis->m_listIndices.push_back(a-1);
				tempThis->m_listIndices.push_back(b-1);
				tempThis->m_listIndices.push_back(c-1);
				tempThis->m_indexCount += 3;
}

void triangular_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& a, const obj::index_2_tuple_type& b, const obj::index_2_tuple_type& c)
{
}
void triangular_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type&, const obj::index_2_tuple_type&, const obj::index_2_tuple_type&){}
void triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type&, const obj::index_3_tuple_type&, const obj::index_3_tuple_type&){}
void quadrilateral_face_geometric_vertices_callback(obj::index_type a, obj::index_type b, obj::index_type c, obj::index_type d)
{
	tempThis->m_listIndices.push_back(a-1);
	tempThis->m_listIndices.push_back(b-1);
	tempThis->m_listIndices.push_back(c-1);
	tempThis->m_listIndices.push_back(a-1);
	tempThis->m_listIndices.push_back(c-1);
	tempThis->m_listIndices.push_back(d-1);
	tempThis->m_indexCount += 6;
}
void quadrilateral_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& a, const obj::index_2_tuple_type& b, const obj::index_2_tuple_type& c, const obj::index_2_tuple_type& d)
{
	quadrilateral_face_geometric_vertices_callback(std::get<0>(a), std::get<0>(b), std::get<0>(c), std::get<0>(d));
}
void quadrilateral_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& a, const obj::index_2_tuple_type& b, const obj::index_2_tuple_type& c, const obj::index_2_tuple_type& d)
{
	quadrilateral_face_geometric_vertices_callback(std::get<0>(a), std::get<0>(b), std::get<0>(c), std::get<0>(d));
}
void quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& a, const obj::index_3_tuple_type& b, const obj::index_3_tuple_type& c, const obj::index_3_tuple_type& d)
{
	quadrilateral_face_geometric_vertices_callback(std::get<0>(a), std::get<0>(b), std::get<0>(c), std::get<0>(d));
}
void polygonal_face_geometric_vertices_begin_callback(obj::index_type, obj::index_type, obj::index_type){}
void polygonal_face_geometric_vertices_vertex_callback(obj::index_type){}
void polygonal_face_geometric_vertices_end_callback(){}
void polygonal_face_geometric_vertices_texture_vertices_begin_callback(const obj::index_2_tuple_type&, const obj::index_2_tuple_type&, const obj::index_2_tuple_type&){}
void polygonal_face_geometric_vertices_texture_vertices_vertex_callback(const obj::index_2_tuple_type&){}
void polygonal_face_geometric_vertices_texture_vertices_end_callback(){}
void polygonal_face_geometric_vertices_vertex_normals_begin_callback(const obj::index_2_tuple_type&, const obj::index_2_tuple_type&, const obj::index_2_tuple_type&){}
void polygonal_face_geometric_vertices_vertex_normals_vertex_callback(const obj::index_2_tuple_type&){}
void polygonal_face_geometric_vertices_vertex_normals_end_callback(){}
void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback(const obj::index_3_tuple_type&, const obj::index_3_tuple_type&, const obj::index_3_tuple_type&){}
void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback(const obj::index_3_tuple_type&){}
void polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback(){}

void vertex_normal_callback(obj::float_type, obj::float_type, obj::float_type)
{
}

void error_callback(std::size_t size, const std::string& msg)
{
	qDebug() << msg.c_str();
}

LoaderObj::LoaderObj(const char* fileName):
	m_vertexCount(0),
	m_indexCount(0)
{
	obj::obj_parser obj_parser;
	tempThis = this;
	obj_parser.geometric_vertex_callback(geometric_vertex_callback);
	obj_parser.error_callback(error_callback);
	//obj_parser.
	//obj_parser.texture_vertex_callback()
	//obj_parser.vertex_normal_callback(vertex_normal_callback);
	obj_parser.face_callbacks(
		triangular_face_geometric_vertices_callback,
		triangular_face_geometric_vertices_texture_vertices_callback,
		triangular_face_geometric_vertices_vertex_normals_callback,
		triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback,
		quadrilateral_face_geometric_vertices_callback,
		quadrilateral_face_geometric_vertices_texture_vertices_callback,
		quadrilateral_face_geometric_vertices_vertex_normals_callback,
		quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback,
		polygonal_face_geometric_vertices_begin_callback,
		polygonal_face_geometric_vertices_vertex_callback,
		polygonal_face_geometric_vertices_end_callback,
		polygonal_face_geometric_vertices_texture_vertices_begin_callback,
		polygonal_face_geometric_vertices_texture_vertices_vertex_callback,
		polygonal_face_geometric_vertices_texture_vertices_end_callback,
		polygonal_face_geometric_vertices_vertex_normals_begin_callback,
		polygonal_face_geometric_vertices_vertex_normals_vertex_callback,
		polygonal_face_geometric_vertices_vertex_normals_end_callback,
		polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback,
		polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback,
		polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback);

	obj_parser.parse(fileName);
	tempThis = (LoaderObj*)0;
	prepareBuffers();
}

void LoaderObj::prepareBuffers()
{
	//m_vertices contains only positions. I could also be calls "m_positions". Normals can also be counted to a vertex.
	//Historically positions and normals where in the same buffer. The interpretation of the floats in the buffer could be configured.
	// In new versions of OpenGL, position and normals can be defined in different buffers.
	m_vertices = new GLfloat[m_vertexCount*3];
	m_normals = new GLfloat[m_vertexCount*3];
	m_indices = new GLuint[m_indexCount];
	
	for(unsigned int i=0 ; i<m_vertexCount ; i++)
	{
		m_vertices[i*3+0] = m_listVertices[i].x;
		m_vertices[i*3+1] = m_listVertices[i].y;
		m_vertices[i*3+2] = m_listVertices[i].z;
	}

	for(unsigned int i=0 ; i<m_indexCount-3 ; i+=3)
	{
		m_indices[i] = m_listIndices[i];
		m_indices[i+1] = m_listIndices[i+1];
		m_indices[i+2] = m_listIndices[i+2];
	}
	qDebug() << "Vertices:" << m_vertexCount << "\nIndices:" << m_indexCount << "\n";


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
	for(unsigned int i=0 ; i<m_indexCount-3 ; i+=3)
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
	for(unsigned int i=0 ; i<m_vertexCount ; i++)
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
