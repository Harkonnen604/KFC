#ifndef physical_objcet_h
#define physical_objcet_h

#include <KFC_Math\mesh.h>

// ----------------
// Physical object
// ----------------
class TPhysicalObject
{
public:
	virtual ~TPhysicalObject() {}

	virtual void BeginFrame() = 0;

	virtual void EndFrame(double dStepTime) = 0;
};
/*
// ----------------------
// Rigid physical object
// ----------------------
class TRigidPhysicalObject : public TPhysicalObject
{
private:	
	double m_dMass;

	double m_dInvMass;


	TDVector3 m_Coords;

	TDVector3 m_LinearVelocity;

	TDVector3 m_LinearForce;

public:
	TObjectPointer<TRigidPhysicalModel> m_Model;


	TRigidPhysicalObject();

	double GetMass() const
		{ return m_dMass; }

	void SetMass(double dSMass);

	void BeginFrame();

	void EndFrame(double dStepTime);
};*/

// ---------------------
// Soft physical object
// ---------------------
class TSoftPhysicalObject : public TPhysicalObject
{
public:
	// Normals mode
	enum TNormalsMode
	{
		NM_ORIGINAL		= 0,
		NM_UPDATE		= 1,
		NM_NORMALIZE	= 2,
		NM_FORCE_UINT	= UINT_MAX
	};

private:
	// Edge
	struct TEdgeInfo
	{
		double m_dEdgeLength;
		double m_dCrossEdgeLength;
		double m_dVertexFaceLength12;
		double m_dVertexFaceLength21;
	};

	TMesh			m_Mesh;
	TNormalsMode	m_NormalsMode;
	double			m_dMass;
	double			m_dEdgeStiffness;
	double			m_dCrossEdgeStiffness;
	double			m_dVertexFaceStiffness;

	double m_dInvVertexMass;

	TArray<TEdgeInfo> m_EdgeInfos;

	TArray<TDVector3, true> m_VertexVelocities;
	TArray<TDVector3, true> m_VertexForces;


	void ApplyNormalsMode();

public:
	TSoftPhysicalObject();

	const TMesh& GetMesh() const
		{ return m_Mesh; }	

	void SetMesh(const TMesh& SMesh);

	TNormalsMode GetNormalsMode() const
		{ return m_NormalsMode; }

	void SetNormalsMode(TNormalsMode SNormalsMode);

	double GetMass() const
		{ return m_dMass; }

	void SetMass(double dSMass);

	double GetEdgeStiffness() const
		{ return m_dEdgeStiffness; }

	void SetEdgeStiffness(double dSEdgeStiffness);

	double GetCrossEdgeStiffness() const
		{ return m_dCrossEdgeStiffness; }

	void SetCrossEdgeStiffness(double dSCrossEdgeStiffness);

	double GetVertexFaceStiffness() const
		{ return m_dVertexFaceStiffness; }

	void SetVertexFaceStiffness(double dSVertexFaceStiffness);

	void BeginFrame();

	void EndFrame(double dStepTime);
};

#endif // physical_objcet_h