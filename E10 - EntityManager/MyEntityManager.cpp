#include "MyEntityManager.h"
using namespace Simplex;

//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;

void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList.clear();
}

void MyEntityManager::Release(void)
{
	// Safely Deallocate Each Entity
	for (int idx = 0; idx < (int)m_uEntityCount; idx++)
	{
		SafeDelete( m_entityList[idx] );
	}

	m_uEntityCount = 0;
	m_entityList.clear();
}

MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}

void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// Find the Index of the Entity with a Matching ID
	for (int idx = 0; idx < (int)m_uEntityCount; idx++)
	{
		if (m_entityList[idx]->GetUniqueID() == a_sUniqueID)
		{
			return idx;
		}
	}

	return -1;
}

//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetModel();
}

Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	MyEntity* currEntity = MyEntity::GetEntity(a_sUniqueID);
	if (!currEntity)
	{
		return nullptr;
	}
	
	return currEntity->GetModel();
}

RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetRigidBody();
}

RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	MyEntity* currEntity = MyEntity::GetEntity(a_sUniqueID);
	if (!currEntity)
	{
		return nullptr;
	}

	return currEntity->GetRigidBody();
}

matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return IDENTITY_M4;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetModelMatrix();
}

matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	MyEntity* currEntity = MyEntity::GetEntity(a_sUniqueID);
	if (!currEntity)
	{
		return IDENTITY_M4;
	}

	return currEntity->GetModelMatrix();
}

void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	MyEntity* currEntity = MyEntity::GetEntity(a_sUniqueID);
	if (!currEntity)
	{
		return;
	}

	currEntity->SetModelMatrix(a_m4ToWorld);
}

void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}

void SetAxisVisibility(bool a_bVisibility, uint a_uIndex = -1)
{

}

void SetAxisVisibility(bool a_bVisibility, String a_sUniqueID)
{

}

//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};

// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Check for Any Collisions Between Entities
	for (int i = 0; i < (int)m_uEntityCount - 1; i++)
	{
		for (int j = i + 1; j < (int)m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding( m_entityList[j] );
		}
	}
}

void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* newEntity = new MyEntity(a_sFileName, a_sUniqueID);
	m_entityList.push_back(newEntity);
	m_uEntityCount += 1;
}

void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	MyEntity* entityToRemove = m_entityList[a_uIndex];
	SafeDelete(entityToRemove);
	m_entityList.erase(m_entityList.begin() + a_uIndex);

	m_uEntityCount -= 1;
}

void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	RemoveEntity( GetEntityIndex(a_sUniqueID) );
}

String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return "";
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetUniqueID();
}

MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex];
}

void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	if (m_entityList.size() == 0)
	{
		return;
	}

	// Index Out of Bounds
	if (a_uIndex < 0 || a_uIndex >= m_uEntityCount)
	{
		// Add Every Entity to the Render List
		for (int idx = 0; idx < (int)m_uEntityCount; idx++)
		{
			m_entityList[idx]->AddToRenderList(a_bRigidBody);
		}
	}
	else
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}

void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	MyEntity* currEntity = MyEntity::GetEntity(a_sUniqueID);
	if (!currEntity)
	{
		return;
	}

	currEntity->AddToRenderList(a_bRigidBody);
}