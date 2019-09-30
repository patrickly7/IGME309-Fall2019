#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Patrick Ly - pxl7219@rit.edu";

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}

	//if there are no segments create 7
	if (m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1 ? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		
		std::vector<vector3> currStopList;

		auto distBetweenPoints = 2 * PI / i;
		// Get the stop points for each torus
		for (uint side = 0; side < i; side++)
		{
			// Calculate the Next Stop Point of the Torus
			double currentAngle = side * distBetweenPoints;
			auto stopPoint = vector3(cos(currentAngle) * fSize, sin(currentAngle) * fSize, 0);

			currStopList.push_back(stopPoint);
		}

		m_stopsList.push_back(currStopList);

		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// Get a timer (Taken from AppClass.cpp in E05.5 - LERP)
	static float fTimer = 0; // Store the new timer
	static uint uClock = m_pSystem->GenClock(); // Generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); // Get the delta time for that timer

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); // View Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); // Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; // Offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	int currIdx = (int)floor(fTimer / m_timeToNextPoint);
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		auto currOrbitStops = m_stopsList[i].size(); // Number of Stops in this Orbit

		// Calculate the Start and End Points for Current Path
		vector3 startPoint = m_stopsList[i][currIdx % currOrbitStops];
		vector3 endPoint = m_stopsList[i][(currIdx + 1) % currOrbitStops];

		// Use LERP Interpolation to Move Between Stop Points
		float currentProgress = (float)fmod(fTimer, m_timeToNextPoint) / m_timeToNextPoint; // Between 0 and 1

		// Calculate the current position
		vector3 v3CurrentPos = glm::lerp(startPoint, endPoint, currentProgress);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		// Draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}