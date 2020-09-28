#include "kfc_graphics_pch.h"
#include "camera.h"

#include <math.h>
#include "graphics_device_globals.h"
#include "graphics_state_manager.h"
#include "graphics_consts.h"

// -------
// Camera
// -------
TCamera::TCamera(   bool bUpdateViewMatrix ,
                    bool bUpdateProjectionMatrix)
{
    SetViewDefaults         (bUpdateViewMatrix);
    SetProjectionDefaults   (bUpdateProjectionMatrix);
}

void TCamera::SetViewDefaults(bool bUpdateViewMatrix)
{
    m_Coords.       SetZero ();
    m_ViewDirection.SetZAxis();
    m_TopDirection. SetYAxis();

    if(bUpdateViewMatrix)
        UpdateViewMatrix();
}

void TCamera::SetProjectionDefaults(bool bUpdateProjectionMatrix)
{
    m_ClipPlanes    = g_GraphicsConsts.m_DefaultCameraZClipPlanes;
    m_fFOV          = g_GraphicsConsts.m_fDefaultCameraFOV;

    if(bUpdateProjectionMatrix)
        UpdateProjectionMatrix();
}

void TCamera::UpdateViewMatrix()
{
    m_ViewMatrix.SetBassis( m_TopDirection * m_ViewDirection,
                            m_TopDirection,
                            m_ViewDirection,
                            &(-m_Coords));
}

void TCamera::UpdateProjectionMatrix()
{
    float w = 1.0f / Tan(m_fFOV * 0.5f);
    float h = w * g_GraphicsDeviceGlobals.m_fXYAspectRatio;
    float q = m_ClipPlanes.m_Last / m_ClipPlanes.GetLength();

    m_ProjectionMatrix.SetZero();
    m_ProjectionMatrix._11 = w;
    m_ProjectionMatrix._22 = h;
    m_ProjectionMatrix._33 = q;
    m_ProjectionMatrix._34 = 1.0f;
    m_ProjectionMatrix._43 = -q * m_ClipPlanes.m_First;
}

void TCamera::Install() const
{
    g_GraphicsStateManager.SetTransform(D3DTS_VIEW,         m_ViewMatrix);
    g_GraphicsStateManager.SetTransform(D3DTS_PROJECTION,   m_ProjectionMatrix);
}

void TCamera::GetFromDevice()
{
    g_GraphicsStateManager.GetTransform(D3DTS_VIEW,         m_ViewMatrix);
    g_GraphicsStateManager.GetTransform(D3DTS_PROJECTION,   m_ProjectionMatrix);
}
