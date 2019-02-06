#ifndef TRIANGLEMESH_HPP_INCLUDED
#define TRIANGLEMESH_HPP_INCLUDED

#include <vector>
#include <emlFormat.h>
#define MOLLER_TRUMBORE
#define CULLING

constexpr float kEpsilon = 1e-8;
class TriangleMesh {
	public:
		TriangleMesh() = default;
		~TriangleMesh() {}

		void LoadFromFile(std::string path) {


			std::ifstream inFILE(path.c_str(), std::ios::binary);
			if(!inFILE.is_open()) {
				std::cout << "Fail to open EML file" << std::endl;
				return;
			}

			int numvertices = 0;
			int numIndices = 0;

			t_eml_header* header = new t_eml_header();
			t_Lumps* lumps = new t_Lumps[maxLumps];

			inFILE.seekg(0, inFILE.beg);
			inFILE.read((char*)&header[0], sizeof(t_eml_header));

			if((int)header->format != EMLHEADER) {
				std::cout << "This file is not a valid SEML file. " << std::endl;
				return;
			}

			if((float)header->version != emlVersion) {
				std::cout << "This file version doesn't match the one required."<< std::endl <<
				          "This file: " << (float)header->version << " Required: " << emlVersion << std::endl;
				return;
			}

			//Read the lump of this file
			inFILE.seekg(sizeof(t_eml_header), inFILE.beg);
			for(int i = 0; i < maxLumps; ++i) {
				inFILE.read((char*)&lumps[i], sizeof(t_Lumps));
			}

			//Get the array of vertices in the file
			numvertices = lumps[lVertices].size / sizeof(t_Vertex);
			t_Vertex* l_vertices = new t_Vertex[numvertices];
			inFILE.seekg(lumps[lVertices].offset, inFILE.beg);
			for(int i = 0; i < numvertices; ++i) {
				inFILE.read((char*)&l_vertices[i], sizeof(t_Vertex));
			}

			//Get the array of indices in the file
			numIndices = lumps[lIndices].size / sizeof(t_Index);
			t_Index* l_indices = new t_Index[numIndices];
			inFILE.seekg(lumps[lIndices].offset, inFILE.beg);
			for(int i = 0; i < numIndices; ++i) {
				inFILE.read((char*)&l_indices[i], sizeof(t_Index));
			}

            std::cout << "Num vertices " << numvertices << std::endl;
			for( auto i = 0; i < numvertices; ++i ) {
				m_Vertices.push_back(glm::vec3(l_vertices[i].position.x, l_vertices[i].position.y, l_vertices[i].position.z));
				m_Normals.push_back(glm::vec3(l_vertices[i].normal.x, l_vertices[i].normal.y, l_vertices[i].normal.z));
				m_TexCoords.push_back(glm::vec2(l_vertices[i].texcoord.s, l_vertices[i].texcoord.t));
			}

			for(auto i = 0; i < numIndices; ++i) {
				m_Indices.push_back(l_indices[i].index);
			}



			delete header;
			delete[] lumps;
			delete[] l_vertices;

			inFILE.close();
		}

		bool rayTriangleIntersect(
		    const glm::vec3 &orig, const glm::vec3 &dir,
		    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
		    float &t, float &u, float &v) {
#ifdef MOLLER_TRUMBORE
			glm::vec3 v0v1 = v1 - v0;
			glm::vec3 v0v2 = v2 - v0;
			glm::vec3 pvec = glm::cross(dir,v0v2);
			float det = glm::dot(v0v1,pvec);
#ifdef CULLING
			// if the determinant is negative the triangle is backfacing
			// if the determinant is close to 0, the ray misses the triangle
			if (det < kEpsilon) return false;
#else
			// ray and triangle are parallel if det is close to 0
			if (fabs(det) < kEpsilon) return false;
#endif
			float invDet = 1 / det;

			glm::vec3 tvec = orig - v0;
			u = glm::dot(tvec,pvec) * invDet;
			if (u < 0 || u > 1) return false;

			glm::vec3 qvec = glm::cross(tvec,v0v1);
			v = glm::dot(dir,qvec) * invDet;
			if (v < 0 || u + v > 1) return false;

			t = glm::dot(v0v2,qvec) * invDet;

			return true;
#else
			// compute plane's normal
			glm::vec3 v0v1 = v1 - v0;
			glm::vec3 v0v2 = v2 - v0;
			// no need to normalize
			glm::vec3 N = glm::cross(v0v1,v0v2); // N
			float denom = glm::dot(N, N);

			// Step 1: finding P

			// check if ray and plane are parallel ?
			float NdotRayDirection = glm::dot(N, dir);
			if (fabs(NdotRayDirection) < kEpsilon) // almost 0
				return false; // they are parallel so they don't intersect !

			// compute d parameter using equation 2
			float d = glm::dot(N, v0);

			// compute t (equation 3)
			t = (glm::dot(N, orig) + d) / NdotRayDirection;
			// check if the triangle is in behind the ray
			if (t < 0) return false; // the triangle is behind

			// compute the intersection point using equation 1
			glm::vec3 P = orig + t * dir;

			// Step 2: inside-outside test
			glm::vec3 C; // vector perpendicular to triangle's plane

			// edge 0
			glm::vec3 edge0 = v1 - v0;
			glm::vec3 vp0 = P - v0;
			C = glm::cross(edge0, vp0);
			if (glm::dot(N, C) < 0) return false; // P is on the right side

			// edge 1
			glm::vec3 edge1 = v2 - v1;
			glm::vec3 vp1 = P - v1;
			C = glm::cross(edge1, vp1);
			if ((u = glm::dot(N, C)) < 0)  return false; // P is on the right side

			// edge 2
			glm::vec3 edge2 = v0 - v2;
			glm::vec3 vp2 = P - v2;
			C = glm::cross(edge2, vp2);
			if ((v = glm::dot(N, C)) < 0) return false; // P is on the right side;

			u /= denom;
			v /= denom;

			return true; // this ray hits the triangle
#endif
		}
const float kInfinity = std::numeric_limits<float>::max();
		bool Intersect(const glm::vec3 &orig, const glm::vec3 &dir, float &tNear, uint32_t &triIndex, glm::mat4 model, glm::mat4 proj, glm::mat4 view) {
		    try{
			uint32_t j = 0;
			bool isect = false;
			for (uint32_t i = 0; i < m_Vertices.size(); i++) {
				glm::vec3 v0 = m_Vertices[m_Indices[j]];
				glm::vec3 v1 = m_Vertices[m_Indices[j + 1]];
				glm::vec3 v2 = m_Vertices[m_Indices[j + 2]];
				glm::vec4 v04 = proj * model * glm::vec4(v0, 1.0);
				glm::vec4 v14 = proj * model * glm::vec4(v1, 1.0);
				glm::vec4 v24 = proj * model * glm::vec4(v2, 1.0);
				v0 = glm::vec3(v04.x, v04.y, v04.z);
				v1 = glm::vec3(v14.x, v14.y, v14.z);
				v2 = glm::vec3(v24.x, v24.y, v24.z);
				//std::cout << "intersect" << std::endl;
                float t = kInfinity, u, v;
				if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) && t < tNear) {

                    tNear = t;
					triIndex = i;
					isect |= true;
				}
				j += 3;
			}

			return isect;
		    } catch(...)
		    {
		        std::cout << "Exception in intersect" << std::endl;
		    }
		}

		glm::vec3 getNormal(unsigned triIndex)
		{
            const glm::vec3 &v0 = m_Vertices[m_Indices[triIndex * 3]];
            const glm::vec3 &v1 = m_Vertices[m_Indices[triIndex * 3 + 1]];
            const glm::vec3 &v2 = m_Vertices[m_Indices[triIndex * 3 + 2]];
            return glm::normalize(glm::cross(v1 - v0, v2 - v0));
		}

		glm::vec2 getTexCoords(unsigned triIndex)
		{
            const glm::vec2 &v0 = m_TexCoords[m_Indices[triIndex * 3]];
            const glm::vec2 &v1 = m_TexCoords[m_Indices[triIndex * 3 + 1]];
            const glm::vec2 &v2 = m_TexCoords[m_Indices[triIndex * 3 + 2]];

		    glm::vec2 avg = (v0 + v1 + v2) / 3.0f;

		    return avg;

		}

		void Destroy() {}
	private:

		std::vector<glm::vec3> m_Vertices;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec2> m_TexCoords;
		std::vector<unsigned short> m_Indices;

	protected:

};

#endif // TRIANGLEMESH_HPP_INCLUDED
