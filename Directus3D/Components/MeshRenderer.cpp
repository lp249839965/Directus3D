/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ===================================
#include "MeshRenderer.h"
#include "Transform.h"
#include "../IO/Serializer.h"
#include "../Logging/Log.h"
#include "../Core/GameObject.h"
#include "../Graphics/Shaders/ShaderVariation.h"
#include "../Graphics/Mesh.h"
#include "../FileSystem/FileSystem.h"
#include "../Resource/ResourceCache.h"
//==============================================

//= NAMESPACES ====================
using namespace std;
using namespace Directus::Math;
using namespace Directus::Resource;
//=================================

MeshRenderer::MeshRenderer()
{
	m_castShadows = true;
	m_receiveShadows = true;
	m_materialType = Imported;
}

MeshRenderer::~MeshRenderer()
{

}

//= ICOMPONENT ===============================================================
void MeshRenderer::Awake()
{
	
}

void MeshRenderer::Start()
{

}

void MeshRenderer::Remove()
{

}

void MeshRenderer::Update()
{

}

void MeshRenderer::Serialize()
{
	Serializer::WriteInt((int)m_materialType);
	Serializer::WriteSTR(!m_material.expired() ? m_material.lock()->GetID() : (string)DATA_NOT_ASSIGNED);
	Serializer::WriteBool(m_castShadows);
	Serializer::WriteBool(m_receiveShadows);
}

void MeshRenderer::Deserialize()
{
	m_materialType = (MaterialType)Serializer::ReadInt();
	string materialID = Serializer::ReadSTR();
	m_castShadows = Serializer::ReadBool();
	m_receiveShadows = Serializer::ReadBool();

	m_materialType == Imported ? SetMaterial(materialID) : SetMaterial(m_materialType);
}
//==============================================================================

//= MISC =======================================================================
void MeshRenderer::Render(unsigned int indexCount)
{
	auto materialWeakPTr = GetMaterial();
	auto materialSharedPtr = materialWeakPTr.lock();

	if (!materialSharedPtr) // Check if a material exists
	{
		LOG_WARNING("GameObject \"" + g_gameObject->GetName() + "\" has no material. It can't be rendered.");
		return;
	}

	if (!materialSharedPtr->HasShader()) // Check if the material has a shader
	{
		LOG_WARNING("GameObject \"" + g_gameObject->GetName() + "\" has a material but not a shader associated with it. It can't be rendered.");
		return;
	}

	// Set the buffers and draw
	materialSharedPtr->GetShader().lock()->Render(indexCount);
}

//==============================================================================

//= MATERIAL ===================================================================
// All functions (set/load) resolve to this
void MeshRenderer::SetMaterial(weak_ptr<Material> material)
{
	m_material = g_context->GetSubsystem<ResourceCache>()->AddResource(material.lock());
}

void MeshRenderer::SetMaterial(MaterialType type)
{
	shared_ptr<Material> material;

	switch (type)
	{
	case Basic:
		material = make_shared<Material>(g_context);
		material->SetName("Basic");
		material->SetColorAlbedo(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		material->SetIsEditable(false);
		m_materialType = Basic;
		break;

	case Skybox:
		material = make_shared<Material>(g_context);
		material->SetName("Skybox");
		material->SetFaceCullMode(CullNone);
		material->SetColorAlbedo(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		material->SetIsEditable(false);
		m_materialType = Skybox;	
		break;

	default:
		break;
	}

	SetMaterial(material);
}

weak_ptr<Material> MeshRenderer::SetMaterial(const string& ID)
{
	SetMaterial(g_context->GetSubsystem<ResourceCache>()->GetResourceByID<Material>(ID));
	return GetMaterial();
}

weak_ptr<Material> MeshRenderer::LoadMaterial(const string& filePath)
{
	SetMaterial(g_context->GetSubsystem<ResourceCache>()->LoadResource<Material>(filePath));
	return GetMaterial();
}
//==============================================================================