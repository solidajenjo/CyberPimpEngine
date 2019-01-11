#include "SubModuleEditorInspector.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleFrameBuffer.h"
#include "ModuleSpacePartitioning.h";
#include "AABBTree.h"
#include "debugdraw.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "Transform.h"
#include <list>


void SubModuleEditorInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		if (App->scene->selected != nullptr)
		{
			if (App->scene->IsRoot(App->scene->selected)) //root node unnaccesible to edit
			{
				ImGui::End();
				return;
			}
			if (App->scene->selected->transform != nullptr && !App->scene->selected->isContainer) //special gameobjects like directories or materials
			{
				App->scene->selected->transform->EditorDraw();
				ImGui::Separator();
				ImGui::Separator();
				ImGui::Separator();
				ImGui::Checkbox("Active", &App->scene->selected->active);
				ImGui::SameLine();
				if (ImGui::Checkbox("Static", &App->scene->selected->isStatic))
				{
					App->scene->selected->PropagateStaticCheck();
				}
				if (strlen(App->scene->selected->instanceOf) > 0)
				{
					ImGui::Text("Instance of %s", App->scene->FindInstanceOrigin(App->scene->selected->instanceOf)->name);
					if (ImGui::Button("Break instance"))
					{
						GameObject* clone = App->scene->selected->Clone(true);
						GameObject* original = App->scene->selected;
						App->scene->selected->parent->InsertChild(clone);	
						clone->parent->transform->PropagateTransform();
						if (App->scene->selected->treeNode != nullptr)
						{
							App->spacePartitioning->aabbTree.ReleaseNode(App->scene->selected->treeNode);
							App->spacePartitioning->aabbTree.InsertGO(clone);
						}
						App->scene->selected = clone;
						App->scene->DeleteGameObject(original);
					}
				}
			}
			bool firstMesh = false; //only one mesh drawing per gameobject
			ImGui::InputText("Name", &App->scene->selected->name[0], 40);
			if (!App->scene->selected->isContainer)
			{
				if (ImGui::Combo("Add component", &selectedNewComponent, "None\0Camera\0Light\0")) {
					if (selectedNewComponent - 1 == (int)Component::ComponentTypes::CAMERA_COMPONENT)
					{
						ComponentCamera* newCam = new ComponentCamera(false);
						newCam->RecalculateFrustum(App->scene->selected->transform->front, App->scene->selected->transform->up);
						App->scene->selected->InsertComponent(newCam);
					}
					if (!App->scene->selected->hasLights && selectedNewComponent - 1 == (int)Component::ComponentTypes::LIGHT_COMPONENT)
					{
						ComponentLight* newLight = new ComponentLight();
						App->scene->selected->InsertComponent(newLight);
						newLight->CalculateGuizmos();
						GameObject* fgo = new GameObject("");
						fgo->components.push_back(newLight);
						fgo->layer = GameObject::GameObjectLayers::LIGHTING;
						fgo->isFake = true;
						App->scene->selected->fakeGameObjectReference = fgo;
						App->scene->InsertFakeGameObject(fgo);
						App->scene->selected->hasLights = true;
					}
					else if (selectedNewComponent - 1 == (int)Component::ComponentTypes::LIGHT_COMPONENT)
					{
						LOG("Only one light component allowed");
					}
				}
			}
			bool deleteComponent = false;
			Component* deleteableComponent = nullptr;
			unsigned compId = 0u;
			for (std::list<Component*>::const_iterator it = App->scene->selected->components.begin(); it != App->scene->selected->components.end(); ++it)
			{							
				ImGui::PushID(++compId);
				if ((*it)->type != Component::ComponentTypes::MESH_COMPONENT)
				{
					if (!App->scene->selected->isContainer)
					{
						if (ImGui::Button("Remove"))
						{
							deleteComponent = true;
						}
					}
					(*it)->EditorDraw();
					switch((*it)->type)
					{
						case Component::ComponentTypes::LIGHT_COMPONENT:
						{
							ComponentLight* cL = (ComponentLight*)(*it);
							if (!deleteComponent)
							{
								App->frameBuffer->Bind();
								switch (cL->lightType)
								{
								case ComponentLight::LightTypes::DIRECTIONAL:
									dd::arrow(cL->owner->transform->getGlobalPosition(), cL->owner->transform->getGlobalPosition() + cL->owner->transform->getGlobalPosition().Normalized(), dd::colors::Gold, App->appScale);
									break;
								case ComponentLight::LightTypes::POINT:
									dd::sphere(cL->pointSphere.pos, dd::colors::Gold, cL->pointSphere.r);
									break;
								case ComponentLight::LightTypes::SPOT:
									dd::sphere(cL->pointSphere.pos, dd::colors::DarkGray, cL->pointSphere.r);
									dd::cone(App->scene->selected->transform->getGlobalPosition(), App->scene->selected->transform->front * cL->spotDistance, dd::colors::Gold, cL->spotEndRadius, .01f);
									break;
								}
								App->frameBuffer->UnBind();
							}
							else
							{
								App->scene->RemoveFakeGameObject(cL->owner->fakeGameObjectReference);
								cL->owner->hasLights = false;
								cL->owner->fakeGameObjectReference = nullptr;
								deleteableComponent = cL;
							}
							break;
						}
						default:
							if (deleteComponent)
							{
								deleteableComponent = (*it);
							}
							break;
					}
				}
				else if (!firstMesh) //if it's a mesh a different behaviour needed when multi-mesh gameobjects
				{

					firstMesh = true;
					ImGui::PushID(this);
					if (ImGui::CollapsingHeader("Mesh"))
					{
						for (std::list<Component*>::const_iterator it2 = App->scene->selected->components.begin(); it2 != App->scene->selected->components.end(); ++it2)
						{
							if ((*it2)->type == Component::ComponentTypes::MESH_COMPONENT)
								(*it2)->EditorDraw();
						}
					}
					ImGui::PopID();
				}
				ImGui::PopID();
			}
			if (deleteComponent)
			{
				deleteableComponent->owner->components.erase(std::find(deleteableComponent->owner->components.begin(), deleteableComponent->owner->components.end(), deleteableComponent));
				RELEASE(deleteableComponent);
			}
		}
		ImGui::End();
	}
}


