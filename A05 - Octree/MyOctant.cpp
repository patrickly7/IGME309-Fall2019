/*
	Name: MyOctant.cpp
	Author: Patrick Ly
	Date: 11/12/19
	Description: Implementation for an Octree.
*/
#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 0;
uint MyOctant::m_uIdealEntityCount = 0;

MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;
	m_uIdealEntityCount = a_nIdealEntityCount;

	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	// Get the Min and Max Vectors
	m_fSize = 68.0f;
	m_v3Min = m_v3Center - vector3(m_fSize / 2.0f);
	m_v3Max = m_v3Center + vector3(m_fSize / 2.0f);

	m_allOctants.push_back(this);

	// Create Children
	if (m_uMaxLevel > 0) { m_uChildren = 8; }
	Subdivide();

	AssignIDtoEntity();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;
	m_v3Min = m_v3Center - vector3(m_fSize / 2.0f);
	m_v3Max = m_v3Center + vector3(m_fSize / 2.0f);

	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();
}

MyOctant::MyOctant(MyOctant const& other)
{
	m_uOctantCount = other.m_uOctantCount;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}

	for (int i = 0; i < (int)other.m_EntityList.size(); i++)
	{
		m_EntityList[i] = other.m_EntityList[i];
	}

	for (int i = 0; i < (int)other.m_EntityList.size(); i++)
	{
		m_EntityList[i] = other.m_EntityList[i];
	}
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	m_uOctantCount = other.m_uOctantCount;
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;

	m_fSize = other.m_fSize;

	m_pMeshMngr = other.m_pMeshMngr;
	m_pEntityMngr = other.m_pEntityMngr;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}

	for (int i = 0; i < (int)other.m_EntityList.size(); i++)
	{
		m_EntityList[i] = other.m_EntityList[i];
	}

	for (int i = 0; i < (int)other.m_EntityList.size(); i++)
	{
		m_EntityList[i] = other.m_EntityList[i];
	}

	return *this;
}

MyOctant::~MyOctant(void)
{
	Release();
}

float MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex)
{
	MyEntity* currEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	if (!currEntity)
		return false;

	MyRigidBody* currRigidBody = currEntity->GetRigidBody();
	if (!currRigidBody)
		return false;

	auto rigidBodyMin = currRigidBody->GetMinGlobal();
	auto rigidBodyMax = currRigidBody->GetMaxGlobal();

	if ((rigidBodyMin.x < m_v3Min.x && rigidBodyMax.x < m_v3Min.x) || rigidBodyMin.x > m_v3Max.x)
		return false;

	if ((rigidBodyMin.y < m_v3Min.y && rigidBodyMax.y < m_v3Min.y) || rigidBodyMin.y > m_v3Max.y)
		return false;

	if ((rigidBodyMin.z < m_v3Min.z && rigidBodyMax.z < m_v3Min.z) || rigidBodyMin.z > m_v3Max.z)
		return false;

	return true;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// Draw All the Octants if Index is Out of Range
	if (a_nIndex < 0 || a_nIndex >= m_allOctants.size())
	{
		Display(C_YELLOW);
	}
	else
	{
		// Draw Only the Specified Octant
		MyOctant* octantToDraw = m_allOctants[a_nIndex];
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(octantToDraw->GetCenterGlobal()) *
			glm::scale(vector3(octantToDraw->GetSize())), C_YELLOW);
	}
}

void MyOctant::Display(vector3 a_v3Color)
{
	// Draw the Current Octant
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(vector3(m_fSize)), C_YELLOW);

	// Draw All of Its Children
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display();
	}
}

void Simplex::MyOctant::Subdivide(void)
{
	if (m_uChildren == 0)
		return;

	// Get the Possible Values for the Centers
	auto backValues = m_v3Center - vector3(m_fSize / 4);
	auto frontValues = m_v3Center + vector3(m_fSize / 4);

	// Set the Children's Centers
	m_pChild[0] = new MyOctant(backValues, m_fSize / 2);
	m_pChild[1] = new MyOctant(vector3(frontValues.x, backValues.y, backValues.z), m_fSize / 2);
	m_pChild[2] = new MyOctant(vector3(backValues.x, frontValues.y, backValues.z), m_fSize / 2);
	m_pChild[3] = new MyOctant(vector3(frontValues.x, frontValues.y, backValues.z), m_fSize / 2);
	m_pChild[4] = new MyOctant(vector3(backValues.x, backValues.y, frontValues.z), m_fSize / 2);
	m_pChild[5] = new MyOctant(vector3(frontValues.x, backValues.y, frontValues.z), m_fSize / 2);
	m_pChild[6] = new MyOctant(vector3(backValues.x, frontValues.y, frontValues.z), m_fSize / 2);
	m_pChild[7] = new MyOctant(frontValues, m_fSize / 2);
	
	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_uID = m_uOctantCount;
		if (m_uLevel + 1 < m_uMaxLevel && m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->m_uChildren = 8;
			m_pChild[i]->Subdivide();
		}

		m_uOctantCount++;

		m_allOctants.push_back(m_pChild[i]);
	}
}

MyOctant* MyOctant::GetChild(uint a_nChild)
{
	return m_pChild[a_nChild];
}

MyOctant* MyOctant::GetParent(void)
{
	return m_pParent;
}

bool MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	int entitiesInOctant = 0;

	int numEntities = m_pEntityMngr->GetEntityCount();
	for (int rbIndex = 0; rbIndex < numEntities; rbIndex++)
	{
		if (entitiesInOctant > a_nEntities)
			return true;

		if (IsColliding(rbIndex))
			entitiesInOctant++;
	}

	return false;
}

void Simplex::MyOctant::KillBranches(void)
{
	for (int i = 0; i < m_uChildren; i++)
	{
		if (m_pChild[i] != nullptr)
		{
			// Kill Off the Children First
			m_pChild[i]->KillBranches();

			delete m_pChild[i];
			m_pChild[i] = nullptr;
		}
	}
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	if (IsLeaf())
	{
		int numEntities = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < numEntities; i++)
		{
			if (IsColliding(i))
			{
				m_pEntityMngr->AddDimension(i, m_uID);
				m_EntityList.push_back(i);
			}
		}
	}
	else {
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->AssignIDtoEntity();
		}
	}
}

uint MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::MyOctant::Release(void)
{
	KillBranches();
}