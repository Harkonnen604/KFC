#ifndef camera_h
#define camera_h

#include <KFC_KTL\basic_bitypes.h>
#include "d3d_vector.h"
#include "d3d_matrix.h"

// -------
// Camera
// -------
struct TCamera
{
	// View parameters
	TD3DVector m_Coords;
	TD3DVector m_ViewDirection;
	TD3DVector m_TopDirection;
	
	// Projection parameters
	FSEGMENT	m_ClipPlanes;
	float		m_fFOV;

	// Matrices
	TD3DMatrix m_ViewMatrix;
	TD3DMatrix m_ProjectionMatrix;


	TCamera(bool bUpdateViewMatrix       = true,
			bool bUpdateProjectionMatrix = true);

	void SetViewDefaults		(bool bUpdateViewMatrix);
	void SetProjectionDefaults	(bool bUpdateProjectionMatrix);

	void UpdateViewMatrix		();
	void UpdateProjectionMatrix	();

	void Install() const;
	
	void GetFromDevice();
};

#endif // camera_h
