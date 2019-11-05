#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

bool MyRigidBody::Separation(MyRigidBody* const a_pOther, vector3 axis)
{
	// Invalid Axis
	if (axis == ZERO_V3 || axis.length() == 0)
 		return false;

	// Find the 8 Corners of the Oriented Bounding Box (A)
	std::vector<vector3> corners;
	corners.push_back(vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z)); // Front Top Left
	corners.push_back(m_v3MaxL);									// Front Top Right
	corners.push_back(vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z)); // Front Bottom Left
	corners.push_back(vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z)); // Front Bottom Right

	corners.push_back(vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z)); // Back Top Left
	corners.push_back(vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z)); // Back Top Right
	corners.push_back(m_v3MinL);									// Back Bottom Left
	corners.push_back(vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z)); // Back Bottom Right

	// Get Initial Min/Max Values for A
	corners[0] = vector3(m_m4ToWorld * vector4(corners[0], 1.0f));
	float aMin = glm::dot(corners[0], axis) / glm::length(axis);;
	float aMax = glm::dot(corners[0], axis) / glm::length(axis);;
	
	for (int i = 1; i < (int)corners.size(); i++)
	{
		// Convert from Local Space to Global Space
		corners[i] = vector3(m_m4ToWorld * vector4(corners[i], 1.0f));

		// Update the Min/Max Projections for A
		float projection = glm::dot(corners[i], axis) / glm::length(axis);

		if (projection < aMin)
			aMin = projection;
		else if (projection > aMax)
			aMax = projection;
	}

	// Find the 8 Corners of the Other Oriented Bounding Box (B)
	std::vector<vector3> otherCorners;
	auto otherMinL = a_pOther->GetMinLocal();
	auto otherMaxL = a_pOther->GetMaxLocal();
	otherCorners.push_back(vector3(otherMinL.x, otherMaxL.y, otherMaxL.z)); // Front Top Left
	otherCorners.push_back(otherMaxL);									    // Front Top Right
	otherCorners.push_back(vector3(otherMinL.x, otherMinL.y, otherMaxL.z)); // Front Bottom Left
	otherCorners.push_back(vector3(otherMaxL.x, otherMinL.y, otherMaxL.z)); // Front Bottom Right

	otherCorners.push_back(vector3(otherMinL.x, otherMaxL.y, otherMinL.z)); // Back Top Left
	otherCorners.push_back(vector3(otherMaxL.x, otherMaxL.y, otherMinL.z)); // Back Top Right
	otherCorners.push_back(otherMinL);									    // Back Bottom Left
	otherCorners.push_back(vector3(otherMaxL.x, otherMinL.y, otherMinL.z)); // Back Bottom Right

	// Get Initial Min/Max Values for B
	otherCorners[0] = vector3(a_pOther->GetModelMatrix() * vector4(otherCorners[0], 1.0f));
	float bMin = glm::dot(otherCorners[0], axis) / glm::length(axis);;
	float bMax = glm::dot(otherCorners[0], axis) / glm::length(axis);;

	for (int i = 1; i < (int)otherCorners.size(); i++)
	{
		// Convert from Local Space to Global Space
		otherCorners[i] = vector3(a_pOther->GetModelMatrix() * vector4(otherCorners[i], 1.0f));

		// Update the Min/Max Projections for B
		float projection = glm::dot(otherCorners[i], axis) / glm::length(axis);

		if (projection < bMin)
			bMin = projection;
		else if (projection > bMax)
			bMax = projection;
	}

	// Check for Overlapping Between the Projections
	if ((aMin < bMin && aMax > bMin) || (aMin < bMax && aMax > bMax) ||
		(aMin > bMin && aMin < bMax) || (aMax > bMin && aMax < bMax))
		return false;

	return true;
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	// Get the X, Y, Z axes for Each OBB
	std::vector<vector3> axes;

	glm::mat3 A = glm::mat3(m_m4ToWorld);
	vector3 AX = A[0];
	axes.push_back(AX);
	vector3 AY = A[1];
	axes.push_back(AY);
	vector3 AZ = A[2];
	axes.push_back(AZ);

	glm::mat3 B = glm::mat3(a_pOther->GetModelMatrix());
	vector3 BX = B[0];
	axes.push_back(BX);
	vector3 BY = B[1];
	axes.push_back(BY);
	vector3 BZ = B[2];
	axes.push_back(BZ);

	// Get the other 9 Axes through Cross Products
	axes.push_back( glm::cross(AX, BX) ); // AX x BX
	axes.push_back( glm::cross(AX, BY) ); // AX x BY
	axes.push_back( glm::cross(AX, BZ) ); // AX x BZ

	axes.push_back( glm::cross(AY, BX) ); // AY x BX
	axes.push_back( glm::cross(AY, BY) ); // AY x BY
	axes.push_back( glm::cross(AY, BZ) ); // AY x BZ

	axes.push_back( glm::cross(AZ, BX) ); // AZ x BX
	axes.push_back( glm::cross(AZ, BY) ); // AZ x BY
	axes.push_back( glm::cross(AZ, BZ) ); // AZ x BZ

	// Check if any axis test separates the two objects
	for (int idx = 0; idx < (int)axes.size(); idx++)
	{
		if (Separation(a_pOther, axes[idx]))
			return static_cast<eSATResults>(idx + 1);
	}

	// There is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}