#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentMap.h"
#include "ComponentLight.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleFrameBuffer.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "MathGeoLib/include/Math/MathConstants.h"
#include "SceneImporter.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "debugdraw.h"
#include "Transform.h"
#include "GameObject.h"
#include "Brofiler/ProfilerCore/Brofiler.h"
#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838) 
#ifndef PAR_SHAPES_IMPLEMENTATION
#define PAR_SHAPES_IMPLEMENTATION
#include "Util/par_shapes.h"
#pragma warning(pop)
#endif


std::map<std::string, ComponentMesh*>ComponentMesh::meshesLoaded;

ComponentMesh::ComponentMesh(Primitives primitive) : Component(ComponentTypes::MESH_COMPONENT)
{	
	FromPrimitive(primitive);
}

void ComponentMesh::FromPrimitive(Primitives primitive)
{
	par_shapes_mesh_s* mesh;
	primitiveType = primitive;
	switch (primitive)
	{
		case Primitives::CUBE:
			mesh = par_shapes_create_cube_with_normals();
			break;
		case Primitives::SPHERE:
			mesh = par_shapes_create_parametric_sphere(20, 20);
			break;
		case Primitives::TORUS:
			mesh = par_shapes_create_torus(20, 20, 0.5f);
			break;
		case Primitives::CYLINDER:
			mesh = par_shapes_create_cylinder_complete(12, 12);
			break;
		case Primitives::PLANE:
			mesh = par_shapes_create_plane(8, 8);
			break;
	}
	
	nVertices = mesh->npoints;
	nCoords = mesh-> npoints * 2;
	nIndices = mesh->ntriangles * 3;	
	nNormals = mesh->npoints;
	meshVertices.resize(nVertices);
	for (unsigned i = 0u; i < nVertices; ++i)
	{
		meshVertices[i].x = mesh->points[i * 3];
		meshVertices[i].y = mesh->points[(i * 3) + 1];
		meshVertices[i].z = mesh->points[(i * 3) + 2];
	}

	if (mesh->normals != nullptr)
	{
		meshNormals.resize(nVertices);
		for (unsigned i = 0u; i < nVertices; ++i)
		{
			meshNormals[i].x = mesh->normals[i * 3];
			meshNormals[i].y = mesh->normals[(i * 3) + 1];
			meshNormals[i].z = mesh->normals[(i * 3) + 2];
		}
	}

	if (mesh->tcoords != nullptr)
	{
		meshTexCoords.resize(nCoords);
		memcpy(&meshTexCoords[0], mesh->tcoords, sizeof(float) * nCoords);
	}
	else
		nCoords = 0;

	meshIndices.resize(nIndices);
	for (unsigned i = 0u; i < nIndices; ++i)
	{
		meshIndices[i] = mesh->triangles[i];
	}

	material = new ComponentMaterial(1.f, 1.f, 1.f, 1.f); // create material	
	delete mesh;
}


ComponentMesh::~ComponentMesh()
{
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);
	if (material != nullptr && material->Release())
		RELEASE(material);
}

void ComponentMesh::EditorDraw()
{	
	ComponentMaterial* item_current = nullptr;	
	ImGui::PushID(this);
	if (ImGui::BeginCombo("Change Material", ""))
	{
		ComponentMaterial** mats = new ComponentMaterial*[ComponentMaterial::materialsLoaded.size()];
		unsigned i = 0u;
		for (std::map<std::string, ComponentMaterial*>::iterator it = ComponentMaterial::materialsLoaded.begin(); it != ComponentMaterial::materialsLoaded.end(); ++it)
		{
			mats[i] = (*it).second;
			++i;
		}
		for (unsigned n = 0u; n < ComponentMaterial::materialsLoaded.size(); ++n)
		{
			bool is_selected = (item_current == mats[n]);
			if (ImGui::Selectable(mats[n]->owner != nullptr ? mats[n]->owner->name : "", is_selected))
			{
				if (material->Release())
					RELEASE(material);
				
				item_current = mats[n];
				material = ComponentMaterial::GetMaterial(item_current->materialPath);				
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
		delete[] mats;
	}	
	ImGui::PopID();
	//show info from his material
	material->EditorDraw();
}

void ComponentMesh::Render(const ComponentCamera * camera, const Transform* transform, const std::vector<ComponentLight*> &directionals, const std::vector<ComponentLight*> &points, const std::vector<ComponentLight*> &spots, ModuleRender::RenderMode renderMode) const
{
	BROFILER_CATEGORY("Mesh Render", Profiler::Color::Aqua);

	unsigned program;
	switch (renderMode)
	{
	case ModuleRender::RenderMode::FORWARD:
	{
		program = *App->program->forwardRenderingProgram;
		glUseProgram(program);
		unsigned lightNum = 0u;
		for (ComponentLight* cL : directionals)
		{
			std::string posStr = "lightDirectionals[" + std::to_string(lightNum) + "]";
			glUniform3fv(glGetUniformLocation(program,
				(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
			glUniform3fv(glGetUniformLocation(program,
				(posStr + ".color").c_str()), 1, cL->color.ptr());
			++lightNum;
			if (lightNum == 2u) //max 2 directional lights Direct / Indirect
				break;
		}
		glUniform1i(glGetUniformLocation(program, "nDirectionals"), lightNum);

		lightNum = 0u;
		for (ComponentLight* cL : points)
		{
			if (cL->pointSphere.Intersects(*transform->owner->aaBBGlobal) || cL->pointSphere.Contains(*transform->owner->aaBBGlobal))
			{
				std::string posStr = "lightPoints[" + std::to_string(lightNum) + "]";
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".color").c_str()), 1, cL->color.ptr());
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".attenuation").c_str()), 1, cL->attenuation.ptr());
				++lightNum;
			}
		}
		glUniform1i(glGetUniformLocation(program, "nPoints"), lightNum);

		lightNum = 0u;
		for (ComponentLight* cL : spots)
		{
			if (cL->ConeContainsAABB(*transform->owner->aaBBGlobal))
			{
				std::string posStr = "lightSpots[" + std::to_string(lightNum) + "]";
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".color").c_str()), 1, cL->color.ptr());
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".attenuation").c_str()), 1, cL->attenuation.ptr());
				glUniform3fv(glGetUniformLocation(program,
					(posStr + ".direction").c_str()), 1, cL->owner->transform->front.ptr());
				glUniform1f(glGetUniformLocation(program,
					(posStr + ".inner").c_str()), cos(cL->innerAngle));
				glUniform1f(glGetUniformLocation(program,
					(posStr + ".outter").c_str()), cos(cL->outterAngle));
				++lightNum;
			}
		}
		glUniform1i(glGetUniformLocation(program, "nSpots"), lightNum);
		if (App->renderer->fog)
		{
			glUniform1f(glGetUniformLocation(program, "fogParameters.fogFalloff"), 1.f / App->renderer->fogFalloff);
			glUniform1f(glGetUniformLocation(program, "fogParameters.fogQuadratic"), 1.f / App->renderer->fogQuadratic);
			glUniform3fv(glGetUniformLocation(program, "fogParameters.fogColor"), 1, &App->renderer->fogColor[0]);
		}
		else
		{
			glUniform1f(glGetUniformLocation(program, "fogParameters.fogFalloff"), .0f);
			glUniform1f(glGetUniformLocation(program, "fogParameters.fogQuadratic"), .0f);
		}

		break;
	}
	case ModuleRender::RenderMode::DEFERRED:
		program = *App->program->deferredStage1Program;
		glUseProgram(program);
		break;
	}
	
	glUniform1f(glGetUniformLocation(program, "appScale"), App->appScale);
	

	glUniformMatrix4fv(glGetUniformLocation(program,
		"model"), 1, GL_TRUE, transform->modelMatrixGlobal.ptr());

	float4x4 view = camera->frustum.ViewMatrix(); //transform from 3x4 to 4x4
	glUniformMatrix4fv(glGetUniformLocation(program,
		"view"), 1, GL_TRUE, view.ptr());
	glUniformMatrix4fv(glGetUniformLocation(program,
		"proj"), 1, GL_TRUE, camera->frustum.ProjectionMatrix().ptr());
	if (material->texture != nullptr && material->texture->mapId > 0)
	{				
		glUniform1i(glGetUniformLocation(program, "mat.diffuseMap"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->texture->mapId);
	}
	
	if (material->emissive != nullptr && material->emissive->mapId > 0)
	{
		glUniform1i(glGetUniformLocation(program, "mat.emissiveMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->emissive->mapId);
	}

	if (material->occlusion != nullptr && material->occlusion->mapId > 0)
	{
		glUniform1i(glGetUniformLocation(program, "mat.occlusionMap"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material->occlusion->mapId);
	}

	if (material->specular != nullptr && material->specular->mapId > 0)
	{
		glUniform1i(glGetUniformLocation(program, "mat.specularMap"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material->specular->mapId);
	}


	glUniform4f(glGetUniformLocation(program, "mat.diffuseColor"), material->diffuseColor.x, material->diffuseColor.y, material->diffuseColor.z, material->diffuseColor.w);
	glUniform4f(glGetUniformLocation(program, "mat.emissiveColor"), material->emissiveColor.x, material->emissiveColor.y, material->emissiveColor.z, 1.0f);
	glUniform4f(glGetUniformLocation(program, "mat.specularColor"), material->specularColor.x, material->specularColor.y, material->specularColor.z, 1.0f);
	
	glUniform1f(glGetUniformLocation(program, "mat.shininess"), material->shininess);
	glUniform1f(glGetUniformLocation(program, "mat.k_ambient"), material->kAmbient);
	glUniform1f(glGetUniformLocation(program, "mat.k_diffuse"), material->kDiffuse);
	glUniform1f(glGetUniformLocation(program, "mat.k_specular"), material->kSpecular);

	if (App->renderer->fog)
	{
		glUniform1f(glGetUniformLocation(program, "fogParameters.fogFalloff"), 1.f / App->renderer->fogFalloff);
		glUniform1f(glGetUniformLocation(program, "fogParameters.fogQuadratic"), 1.f / App->renderer->fogQuadratic);
		glUniform3fv(glGetUniformLocation(program, "fogParameters.fogColor"), 1, &App->renderer->fogColor[0]);
	}
	else
	{
		glUniform1f(glGetUniformLocation(program, "fogParameters.fogFalloff"), .0f);
		glUniform1f(glGetUniformLocation(program, "fogParameters.fogQuadratic"), .0f);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIndex);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}

void ComponentMesh::RenderDeferred(const ModuleFrameBuffer* frameBuffer, const ComponentCamera* camera, const std::vector<ComponentLight*> &directionals, const std::vector<ComponentLight*> &points, const std::vector<ComponentLight*> &spots) const
{
	BROFILER_CATEGORY("Mesh Render", Profiler::Color::Aqua);
	unsigned program = *App->program->deferredStage2Program;
	glUseProgram(program);
	
	float4x4 model = float4x4::identity;
	model = model.FromTRS(float3(-1.f, -1.f, .0f), float4x4::identity, float3(2.f, 2.f, .1f));
	
	glUniformMatrix4fv(glGetUniformLocation(program,
		"model"), 1, GL_TRUE, model.ptr());

	glUniform1i(glGetUniformLocation(program, "gDiffuse"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBuffer->texColorBuffer);
	
	glUniform1i(glGetUniformLocation(program, "gPosition"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, frameBuffer->positionBuffer);
	
	glUniform1i(glGetUniformLocation(program, "gNormal"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, frameBuffer->normalsBuffer);

	glUniform1i(glGetUniformLocation(program, "gSpecular"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, frameBuffer->specularBuffer);

	glUniform1f(glGetUniformLocation(program, "appScale"), App->appScale);

	unsigned lightNum = 0u;
	for (ComponentLight* cL : directionals)
	{
		std::string posStr = "lightDirectionals[" + std::to_string(lightNum) + "]";
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".color").c_str()), 1, cL->color.ptr());
		++lightNum;
		if (lightNum == 2u) //max 2 directional lights Direct / Indirect
			break;
	}
	glUniform1i(glGetUniformLocation(program, "nDirectionals"), lightNum);

	lightNum = 0u;
	for (ComponentLight* cL : points)
	{
		std::string posStr = "lightPoints[" + std::to_string(lightNum) + "]";
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".color").c_str()), 1, cL->color.ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".attenuation").c_str()), 1, cL->attenuation.ptr());
		++lightNum;
}
	glUniform1i(glGetUniformLocation(program, "nPoints"), lightNum);

	lightNum = 0u;
	for (ComponentLight* cL : spots)
	{
		std::string posStr = "lightSpots[" + std::to_string(lightNum) + "]";
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".position").c_str()), 1, cL->owner->transform->getGlobalPosition().ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".color").c_str()), 1, cL->color.ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".attenuation").c_str()), 1, cL->attenuation.ptr());
		glUniform3fv(glGetUniformLocation(program,
			(posStr + ".direction").c_str()), 1, cL->owner->transform->front.ptr());
		glUniform1f(glGetUniformLocation(program,
			(posStr + ".inner").c_str()), cos(cL->innerAngle));
		glUniform1f(glGetUniformLocation(program,
			(posStr + ".outter").c_str()), cos(cL->outterAngle));
		++lightNum;
	}
	glUniform1i(glGetUniformLocation(program, "nSpots"), lightNum);

	float4x4 view = camera->frustum.ViewMatrix(); //transform from 3x4 to 4x4
	glUniformMatrix4fv(glGetUniformLocation(program,
		"view"), 1, GL_TRUE, view.ptr());

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIndex);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
}


ComponentMesh * ComponentMesh::Clone()
{
	ComponentMesh* newMesh = new ComponentMesh();
	newMesh->nVertices = nVertices;
	newMesh->nCoords = nCoords;
	newMesh->nIndices = nIndices;
	newMesh->nNormals = nNormals;
	newMesh->meshVertices = meshVertices;
	newMesh->meshNormals = meshNormals;
	newMesh->meshTexCoords = meshTexCoords;
	newMesh->meshIndices = meshIndices;
	newMesh->primitiveType = primitiveType;
	newMesh->material = material->Clone();

	if (VAO != 0u)
		newMesh->SendToGPU();

	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();
	std::string meshSavePath = "Library/Meshes/" + uuid + ".msh";
	sprintf_s(newMesh->meshPath, meshSavePath.c_str());
	SceneImporter si;
	si.SaveMesh(newMesh, meshSavePath.c_str()); //save a mesh copy
	return newMesh;
}

void ComponentMesh::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("meshPath"); writer.String(meshPath);
	writer.String("primitive"); writer.Int((int)primitiveType);
	writer.String("material");
	material->Serialize(writer);
	writer.EndObject();
}

void ComponentMesh::UnSerialize(rapidjson::Value & value)
{
	sprintf_s(meshPath, value["meshPath"].GetString());
	primitiveType = (Primitives)value["primitive"].GetInt();
	if (primitiveType != Primitives::VOID_PRIMITIVE)
	{
		FromPrimitive(primitiveType);
		RELEASE(material);
		material = ComponentMaterial::GetMaterial(value["material"]["materialPath"].GetString());
	}	
	
}

bool ComponentMesh::Release()
{
	if (primitiveType != Primitives::VOID_PRIMITIVE)
		return true;

	if (clients > 1) //still have clients -> no destroy
	{
		--clients;
		return false;
	}
	else
	{
		meshesLoaded.erase(meshPath); //unload completely
		return true;
	}
}

void ComponentMesh::SendToGPU()
{
	if (VAO > 0) //only once at gpu 
		return;
	GLuint vbo, vao, vio;
	unsigned totalSize = (sizeof(float3) * 2 * nVertices) + (sizeof(float) * 2 * nVertices);
	unsigned offsetTexCoords = nVertices * sizeof(float3);
	unsigned offsetNormals = offsetTexCoords + (nVertices * sizeof(float) * 2);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vio);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	
	glBufferData(GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, offsetTexCoords, &meshVertices[0]);
	if (meshTexCoords.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetTexCoords, sizeof(float) * meshTexCoords.size(), &meshTexCoords[0]);
	
	if (meshNormals.size() > 0)
		glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(float3) * meshNormals.size(), &meshNormals[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * nIndices, &meshIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);		
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE,	0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetTexCoords);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,	0, (void*)offsetNormals);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	VAO = vao;
	VIndex = vio;
	if (VAO == 0 || VIndex == 0)
	{
		LOG("Error sending mesh to GPU");
	}
}

void ComponentMesh::ReleaseFromGPU()
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

ComponentMesh* ComponentMesh::GetMesh(std::string path)
{
	std::map<std::string, ComponentMesh*>::iterator it = meshesLoaded.find(path);
	if (it == meshesLoaded.end())
	{
		SceneImporter si;
		ComponentMesh* mesh = si.LoadMesh(path.c_str());
		if (mesh != nullptr)
		{
			meshesLoaded[path] = mesh;
			++mesh->clients;
			return mesh;
		}
		else
			return nullptr;
	}		
	++(*it).second->clients;
	return (*it).second;
}
