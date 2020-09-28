#include "kfc_physics_pch.h"
#include "physical_object.h"

/*
// ----------------------
// Rigid physical object
// ----------------------
TRigidPhysicalObject::TRigidPhysicalObject()
{
    m_LinearVelocity.SetZero();
}

void TRigidPhysicalObject::SetMesh(const TMesh& SMesh)
{
    DEBUG_VERIFY(SMesh.IsAllocated());

    m_Mesh = SMesh;
}

void TRigidPhysicalObject::SetMass(double dSMass)
{
    DEBUG_VERIFY(Sign(dSMass) > 0);

    m_dMass = dSMass;

    m_dInvMass = 1.0 / dSMass;
}

void TRigidPhysicalObject::BeginFrame()
{
    DEBUG_VERIFY(m_Model.IsAllocated());

    m_LinearForce.SetZero();
}

void TRigidPhysicalObject::EndFrame(double dStepTime)
{
    DEBUG_VERIFY(m_Model.IsAllocated());

    m_LinearVelocity += m_LinearForce * dStepTime;

    m_Coords += m_LinearVelocity * dStepTime;
}*/

// ---------------------
// Soft physical object
// ---------------------
TSoftPhysicalObject::TSoftPhysicalObject()
{
    m_dMass                 = 1.0;
    m_NormalsMode           = NM_UPDATE;
    m_dEdgeStiffness        = 0.0;
    m_dCrossEdgeStiffness   = 0.0;
    m_dVertexFaceStiffness  = 0.0;
}

void TSoftPhysicalObject::SetMesh(const TMesh& SMesh)
{
    DEBUG_VERIFY(SMesh.IsAllocated());

    DEBUG_VERIFY(SMesh.AreEdgesUpdated());

    m_Mesh = SMesh;

    m_dInvVertexMass = m_Mesh.GetNVertices() / m_dMass;

    size_t i;

    // Calculating edge infos
    {
        m_EdgeInfos.SetN(m_Mesh.GetNEdges());

        const TMesh::TEdge *pEdge;

        TEdgeInfo* pEdgeInfo;

        for(pEdge = m_Mesh.m_Edges.GetDataPtr(),
                pEdgeInfo = m_EdgeInfos.GetDataPtr(),
                i = m_Mesh.GetNEdges() ;
            i ;
            pEdge++, pEdgeInfo++, i--)
        {
            pEdgeInfo->m_dEdgeLength =
                (   m_Mesh.m_Vertices[pEdge->v2].m_Coords -
                    m_Mesh.m_Vertices[pEdge->v1].m_Coords).GetLength();

            pEdgeInfo->m_dCrossEdgeLength =
                m_Mesh.FindEdge(pEdge->ov1, pEdge->ov2) ?
                    -1.0 :
                    (   m_Mesh.m_Vertices[pEdge->ov2].m_Coords -
                        m_Mesh.m_Vertices[pEdge->ov1].m_Coords).GetLength();

            {
                const TDVector3& Origin = m_Mesh.m_Vertices[pEdge->ov2].m_Coords;

                pEdgeInfo->m_dVertexFaceLength12 =
                    (m_Mesh.m_Vertices[pEdge->ov1].m_Coords - Origin) &
                        (   (m_Mesh.m_Vertices[pEdge->v2].m_Coords - Origin) *
                            (m_Mesh.m_Vertices[pEdge->v1].m_Coords - Origin)).Normalized();
            }

            {
                const TDVector3& Origin = m_Mesh.m_Vertices[pEdge->ov1].m_Coords;

                pEdgeInfo->m_dVertexFaceLength21 =
                    (m_Mesh.m_Vertices[pEdge->ov2].m_Coords - Origin) &
                        (   (m_Mesh.m_Vertices[pEdge->v1].m_Coords - Origin) *
                            (m_Mesh.m_Vertices[pEdge->v2].m_Coords - Origin)).Normalized();
            }
        }
    }

    m_VertexVelocities.SetN(m_Mesh.GetNVertices());

    m_VertexForces.SetN(m_Mesh.GetNVertices());

    m_VertexVelocities.ZeroData();

    ApplyNormalsMode();
}

void TSoftPhysicalObject::SetNormalsMode(TNormalsMode SNormalsMode)
{
    m_NormalsMode = SNormalsMode;

    ApplyNormalsMode();
}

void TSoftPhysicalObject::SetMass(double dSMass)
{
    DEBUG_VERIFY(m_Mesh.IsAllocated());

    DEBUG_VERIFY(Sign(dSMass) > 0);

    m_dMass = dSMass;

    m_dInvVertexMass = m_Mesh.GetNVertices() / m_dMass;
}

void TSoftPhysicalObject::SetEdgeStiffness(double dSEdgeStiffness)
{
    DEBUG_VERIFY(Sign(dSEdgeStiffness) >= 0);

    m_dEdgeStiffness = dSEdgeStiffness;
}

void TSoftPhysicalObject::SetCrossEdgeStiffness(double dSCrossEdgeStiffness)
{
    DEBUG_VERIFY(Sign(dSCrossEdgeStiffness) >= 0);

    m_dCrossEdgeStiffness = dSCrossEdgeStiffness;
}

void TSoftPhysicalObject::SetVertexFaceStiffness(double dSVertexFaceStiffness)
{
    DEBUG_VERIFY(Sign(dSVertexFaceStiffness) >= 0);

    m_dVertexFaceStiffness = dSVertexFaceStiffness;
}

void TSoftPhysicalObject::ApplyNormalsMode()
{
    if(m_NormalsMode == NM_UPDATE || m_NormalsMode == NM_NORMALIZE)
        m_Mesh.UpdateVertexNormals(m_NormalsMode == NM_NORMALIZE);
}

void TSoftPhysicalObject::BeginFrame()
{
    DEBUG_VERIFY(m_Mesh.IsAllocated());

    m_VertexForces.ZeroData();
}

void TSoftPhysicalObject::EndFrame(double dStepTime)
{
    DEBUG_VERIFY(m_Mesh.IsAllocated());

    size_t i;

    const TMesh::TEdge* pEdge;

    const TEdgeInfo* pEdgeInfo;

    // Calculating edge forces
    if(Sign(m_dEdgeStiffness))
    {
        for(pEdge = m_Mesh.m_Edges.GetDataPtr(),
                pEdgeInfo = m_EdgeInfos.GetDataPtr(),
                i = m_Mesh.GetNEdges() ;
            i ;
            pEdge++, pEdgeInfo++, i--)
        {
            TDVector3 Force12 =
                m_Mesh.m_Vertices[pEdge->v2].m_Coords - m_Mesh.m_Vertices[pEdge->v1].m_Coords;

            const double dLength = Force12.GetLength();

            if(Sign(dLength))
                Force12 /= dLength;

            Force12 *= (dLength - pEdgeInfo->m_dEdgeLength) * m_dEdgeStiffness;

            m_VertexForces[pEdge->v1] += Force12;
            m_VertexForces[pEdge->v2] -= Force12;
        }
    }

    // Calculating cross-edge forces
    if(Sign(m_dCrossEdgeStiffness))
    {
        for(pEdge = m_Mesh.m_Edges.GetDataPtr(),
                pEdgeInfo = m_EdgeInfos.GetDataPtr(),
                i = m_Mesh.GetNEdges() ;
            i ;
            pEdge++, pEdgeInfo++, i--)
        {
            if(pEdgeInfo->m_dCrossEdgeLength < 0.0) // another edge
                continue;

            TDVector3 Force12 =
                m_Mesh.m_Vertices[pEdge->ov2].m_Coords - m_Mesh.m_Vertices[pEdge->ov1].m_Coords;

            const double dInterFaceLength = Force12.GetLength();

            if(Sign(dInterFaceLength))
                Force12 /= dInterFaceLength;

            Force12 *= (dInterFaceLength - pEdgeInfo->m_dCrossEdgeLength) * m_dCrossEdgeStiffness;

            m_VertexForces[pEdge->ov1] += Force12;
            m_VertexForces[pEdge->ov2] -= Force12;
        }
    }

    // Calculating vertex-face forces
    if(Sign(m_dVertexFaceStiffness))
    {
        for(pEdge = m_Mesh.m_Edges.GetDataPtr(),
                pEdgeInfo = m_EdgeInfos.GetDataPtr(),
                i = m_Mesh.GetNEdges() ;
            i ;
            pEdge++, pEdgeInfo++, i--)
        {
            // {{{
        }
    }

    // Apllying vertex forces
    {
        const double dVelocityCoef = m_dInvVertexMass * dStepTime;

        TMesh::TVertex* pVertex = m_Mesh.m_Vertices.GetDataPtr();

        TDVector3* pVertexVelocity = m_VertexVelocities.GetDataPtr();

        const TDVector3* pVertexForce = m_VertexForces.GetDataPtr();

        for(i = m_Mesh.GetNVertices() ; i ; i--, pVertex++, pVertexVelocity++, pVertexForce++)
        {
            *pVertexVelocity += *pVertexForce * dVelocityCoef;

            pVertex->m_Coords += *pVertexVelocity * dStepTime;
        }
    }

    // Applying normals mode
    ApplyNormalsMode();
}
