#include "modelLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
// source from Cory Arnett-O'Brien (Game Engine Architecture)


	aiVector3D CalculateSurfaceNormal(aiVector3D a, aiVector3D b, aiVector3D c)
	{
		aiVector3D U = (b - a);
		aiVector3D V = (c - a);

		aiVector3D normal;
		normal.x = (U.y * V.z) - (U.z * V.y);
		normal.y = (U.z * V.x) - (U.x * V.z);

		normal.z = (U.x * V.y) - (U.y * V.x);
		return normal;

	}


	bool Model::loadFromFile(const char* filename, glhelper::Mesh* mesh)
	{
		//temporary vector	for storing model vertices loaded from file
		std::vector<Vertex> loadedVertices;

		//create importer object
		Assimp::Importer imp;

		//load model into scene object
		const aiScene* pScene = imp.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

		//check if file was opened ok
		if (!pScene)
		{
			return false;
		}
		const aiMesh* _mesh;
		//get the vertices loaded from the model object and put them in a temporary vector
		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++)
		{
			_mesh = pScene->mMeshes[MeshIdx];

			for (int faceIndex = 0; faceIndex < _mesh->mNumFaces; faceIndex++)
			{
				const aiFace& face = _mesh->mFaces[faceIndex];


				//extract a vertex from the meshes main vertex array 
				//for each point in the face. 3 as in there are three vertices 
				//in a face (triangulated)
				for (int vertInx = 0; vertInx < 3; vertInx++)
				{
					//extract pos and tex co-ord based on its index number
					//not directly from mesh arrays
					const aiVector3D* pos = &_mesh->mVertices[face.mIndices[vertInx]];

					//get uvs for vertex this code assumes there are uvs
					//defined in the model or it will crash
					const aiVector3D uv = _mesh->mTextureCoords[0][face.mIndices[vertInx]];

					if (_mesh->HasNormals())
					{
						const aiVector3D& normal = _mesh->mNormals[MeshIdx];

						//create a new object in the shape array based on the extracted vertex
						//this shape array will then be used to create the vertex buffer
						loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y, normal.x, normal.y, normal.z));
					}
					else
					{
						aiVector3D& A = _mesh->mVertices[face.mIndices[0]];
						aiVector3D& B = _mesh->mVertices[face.mIndices[1]];
						aiVector3D& C = _mesh->mVertices[face.mIndices[2]];
						aiVector3D normals = CalculateSurfaceNormal(A, B, C);
						//create a new object in the shape array based on the extracted vertex
						//this shape array will then be used to create the vertex buffer
						loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y, normals.x, normals.y, normals.z));
					}



				}
			}
			std::vector<Eigen::Vector3f> verts(_mesh->mNumVertices);
			std::vector<Eigen::Vector3f> norms(_mesh->mNumVertices);
			std::vector<Eigen::Vector2f> uvs(_mesh->mNumVertices);
			std::vector<GLuint> elems(_mesh->mNumFaces * 3);
			//Add bi tangents
			//TODO
			mesh->vert(verts);
			mesh->norm(norms);
			mesh->tex(uvs);
			mesh->elems(elems);

		}



		numVertices = loadedVertices.size();

		//copy vertices into memory buffer
		glGenBuffers(1, &vbo);

		//create the buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//copy vertex data from vector to buffer
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), loadedVertices.data(), GL_STATIC_DRAW);

		//remove the buffer from the pipeline 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;

		//source ends here
	}
