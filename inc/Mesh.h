#include "Dialogs.h"

//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve };
typedef GLshort Edge[2];

typedef enum { pointLight, sunLight };

class Children {
	public:
		TInt Id;
		GLfixed OriginalScaleX;
		GLfixed OriginalScaleY; 
		GLfixed OriginalScaleZ;		
};

class Object {
	public:
		TInt type;
		TBool visible;
		TBool seleccionado;
		GLfloat posX; GLfloat posY; GLfloat posZ;
		GLfloat rotX; GLfloat rotY; GLfloat rotZ;
		GLfixed scaleX; GLfixed scaleY; GLfixed scaleZ;
		TInt Id;
		TInt Parent;
		RArray<Children> Childrens;
		HBufC* name;
};

class Light { 
	public:
		TInt type;
		TInt lightId;
		GLfloat color[4];
};

class Material { 
	public:
		TBool textura;
		TBool transparent;
		TBool vertexColor;
		TBool lighting;
		TBool repeat;
		TBool uv8bit;
		TBool culling;
		TInt interpolacion;
		GLuint textureID;
		GLfloat diffuse[4];		
		GLfloat specular[4];	
		GLfloat emission[4];
		HBufC* name;
};

class VertexGroup { 
	public:
        RArray<TInt> indices;
};

class MaterialGroup { 
	public:
        TInt start; //donde esta el primer triangulo real
        TInt count; //cuantos triangulos son reales

        TInt startDrawn; //indice del primer triangulo para dibujar
		TInt indicesDrawnCount; //cuantos vertices son
		TInt material; //de que material
};

class Mesh { 
	public:
		TInt vertexSize;
		GLshort* vertex;
        RArray<VertexGroup> vertexGroups;
		GLubyte* vertexColor;
		GLbyte* normals;
		GLfloat* uv;
		//GLbyte* uv8;

		//caras
   		TInt facesSize;
   		GLushort* faces;
        RArray<MaterialGroup> materialsGroup;
};

class FaceCorners {
	public:
		TInt vertex;
		TInt uv;
		TInt normal;
		//seria genial agregar color despues
};

class Face { 
	public:
		RArray<FaceCorners> corner;
};

class Wavefront {
	public:
		RArray<GLshort> vertex;
		RArray<GLubyte> vertexColor;
		RArray<GLbyte> normals;
		RArray<GLfloat> uv;
		RArray<Face> faces;
		TInt facesSize;
		TInt facesCount;
        RArray<MaterialGroup> materialsGroup;

        void Wavefront::Reset(){
         	vertex.Close();
         	vertexColor.Close();
         	normals.Close();
         	uv.Close();
         	faces.Close();
         	materialsGroup.Close();
			facesSize  = 0;
			facesCount = 0;
		};	

		void Wavefront::ConvertToES1(Mesh& TempMesh){
			TempMesh.vertexSize = vertex.Count();
			TempMesh.vertex = new GLshort[vertex.Count()];
			TempMesh.vertexColor = new GLubyte[vertex.Count()];
			TempMesh.normals = new GLbyte[vertex.Count()];
			TempMesh.uv = new GLfloat[(vertex.Count()/3)*2];
			//valores por defecto
			for (TInt i = 0; i < vertex.Count()/3; i++) {
				TempMesh.vertex[i*3] = TempMesh.vertex[i*3+1] = TempMesh.vertex[i*3+2] = 0;
				TempMesh.vertexColor[i*3] = TempMesh.vertexColor[i*3+1] = TempMesh.vertexColor[i*3+2] = 255;
				TempMesh.normals[i*3] = TempMesh.normals[i*3+1] = TempMesh.normals[i*3+2] = 0;
				TempMesh.uv[i*2] = TempMesh.uv[i*2+1] = 0;
			}
			
			//agrega las caras
			TempMesh.faces = new GLushort[facesSize];
			TempMesh.facesSize = facesSize;
			//HBufC* noteBuf3 = HBufC::NewLC(180);
			for (TInt i = 0; i < faces.Count(); i++) {
				for (TInt f = 0; f < faces[i].corner.Count(); f++) {
					TInt vertexIndice = faces[i].corner[f].vertex;
					TInt normalIndice = faces[i].corner[f].normal;
					TInt uvIndice = faces[i].corner[f].uv;
					TempMesh.faces[i*3+f] = vertexIndice;	
					for(TInt v=0; v < 3; v++){
						TempMesh.vertex[vertexIndice*3+v] = vertex[vertexIndice*3+v];
						TempMesh.normals[vertexIndice*3+v] = normals[normalIndice*3+v];
					}
					for(TInt v=0; v < 2; v++){
						TempMesh.uv[vertexIndice*2+v] = uv[uvIndice*2+v];
					}
					
					/*_LIT(KFormatString3, "F: %d c: %d i: %d\nv1: %d\nv2: %d\nv3: %d");
					noteBuf3->Des().Format(KFormatString3, i+1, f+1, normalIndice+1, normals[normalIndice*3], normals[normalIndice*3+1], normals[normalIndice*3+2]);
					CDialogs::Alert(noteBuf3);*/

					/*_LIT(KFormatString4, "F: %d c: %d i: %d\nuv1: %f\nuv2: %f");
					noteBuf3->Des().Format(KFormatString4, i+1, f+1, uvIndice+1, uv[uvIndice*2], uv[uvIndice*2+1]);
					CDialogs::Alert(noteBuf3);*/
				}
			
				/*TInt indiceCara = a*9+f*3;

				TInt indiceVertice = (ListCaras[indiceCara]-*acumuladoVertices)*3;
				TInt indiceNormales = (ListCaras[indiceCara]-*acumuladoNormales)*3;
				TInt indiceUV = (ListCaras[indiceCara]-*acumuladoUVs)*2;
				TInt indiceColor = (ListCaras[indiceCara]-*acumuladoVertices)*4;

				tempMesh.faces[(a-ultimoIndiceinicio)*3+f] = ListCaras[indiceCara]-*acumuladoVertices;			
				for(TInt v=0; v < 3; v++){
					//a*9 es que ListCaras tiene 9 valores por cara, 3 vertices, 3 normales y 3 UV
					//f*3 es para ir por las distintas "/" 1/1/1
					tempMesh.vertex[indiceVertice+v]  = ListVertices[(ListCaras[indiceCara]-*acumuladoVertices)*3+v];	
					tempMesh.normals[indiceNormales+v] = ListNormals[(ListCaras[indiceCara+2]-*acumuladoNormales)*3+v];
					tempMesh.vertexColor[indiceColor+v] = ListColors[(ListCaras[indiceCara]-*acumuladoVertices)*3+v];
				}
				for(TInt uv=0; uv < 2; uv++){
					tempMesh.uv[indiceUV+uv] = ListUVs[(ListCaras[indiceCara+1]-*acumuladoUVs)*2+uv];
				}*/
			}	

			//si tiene materiales
			if (materialsGroup.Count() > 0){			
				for (TInt i = 0; i < materialsGroup.Count(); i++) {
					TempMesh.materialsGroup.Append(materialsGroup[i]);				
				}
			}
			else {
				MaterialGroup tempFaceGroup;
				tempFaceGroup.start = 0; //donde esta el primer triangulo real
				tempFaceGroup.count = facesSize/3; //cuantos triangulos son reales

				tempFaceGroup.startDrawn = 0; //indice del primer triangulo para dibujar
				tempFaceGroup.indicesDrawnCount = facesSize; //cuantos vertices son
				tempFaceGroup.material = 0; //de que material

				TempMesh.materialsGroup.Append(tempFaceGroup);
			}
			Reset();
		};	
};
