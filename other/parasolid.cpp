#include "pch.h"
#include "parasolid.h"

// !!!
#include <KFC_Common/file.h>
#include <KFC_Image/image.h>

namespace parasolid
{

// -------
// Consts
// -------
const double g_dPI = acos(-1.0);

// --------------------------------
// Triangulation type declarations
// --------------------------------
struct TTriVertex;
struct TTriEdge;
struct TTriRegion;

// ------------------------
// Triangulation loop node
// ------------------------
struct TTriLoopNode
{
public:
    vector xyz;
    T_UV_Point uv;

public:
    TTriLoopNode() {}

    // All members constructor (used for border vertices)
    TTriLoopNode(const vector& sxyz, const T_UV_Point& suv) :
        xyz(sxyz), uv(suv) {}

    // XYZ setter, used for loop control points
    TTriLoopNode(const vector& sxyz, const ANY_SURF* pSurface) :
        xyz(sxyz), uv(pSurface->GetApproxUV(sxyz)) {}

    // UV setter, used for face inner points
    TTriLoopNode(const T_UV_Point& suv, const ANY_SURF* pSurface) :
        xyz(pSurface->GetApproxXYZ(suv)), uv(suv) {}
};

inline int Compare(const TTriLoopNode& n1, const TTriLoopNode& n2)
    { return Compare(n1.uv, n2.uv); }

inline bool operator < (const TTriLoopNode& n1, const TTriLoopNode& n2)
    { return Compare(n1, n2) < 0; }

// ------------------------------
// Triangulation helper routines
// ------------------------------
inline void AddFace(std::vector<vector>& RFaces, const vector& p1, const vector& p2, const vector& p3, char cSense)
{
    if(p1 == p2 || p2 == p3 || p3 == p1) // sphere and cone caps may produce such
        return;

    if(cSense == '-')
    {
        RFaces.push_back(p1);
        RFaces.push_back(p3);
        RFaces.push_back(p2);
    }
    else
    {
        RFaces.push_back(p1);
        RFaces.push_back(p2);
        RFaces.push_back(p3);
    }
}

static void TriangulateStrip(std::vector<vector>& RFaces, const std::vector<TTriLoopNode>& p1, const std::vector<TTriLoopNode>& p2, char cSense)
{
    if(p1.empty() || p2.empty())
        return;

    size_t i1 = 0, i2 = 0;

    for(;;)
    {
        if(i1 < p1.size() - 1 && i2 < p2.size() - 1) // adding shorter edge
        {
            if(Compare((p1[i1+1].uv - p2[i2].uv).GetLengthSquare(), (p2[i2+1].uv - p1[i1].uv).GetLengthSquare()) <= 0)
                AddFace(RFaces, p1[i1].xyz, p1[i1+1].xyz, p2[i2].xyz, cSense), i1++;
            else
                AddFace(RFaces, p1[i1].xyz, p2[i2+1].xyz, p2[i2].xyz, cSense), i2++;
        }
        else if(i1 < p1.size() - 1) // no choice
        {
            AddFace(RFaces, p1[i1].xyz, p1[i1+1].xyz, p2[i2].xyz, cSense), i1++;
        }
        else if(i2 < p2.size() - 1) // no choice
        {
            AddFace(RFaces, p1[i1].xyz, p2[i2+1].xyz, p2[i2].xyz, cSense), i2++;
        }
        else
        {
            break;
        }
    }
}

// ---------------------------
// Border processing routines
// ---------------------------
static int GetBorderSide(const TTriLoopNode bc[4], const T_UV_Point& uv)
{
    for(int s = 0 ; s < 4 ; s++)
    {
        int ns = (s+1)&3;

        if(bc[s].uv.u == bc[ns].uv.u && !Compare(uv.u, bc[s].uv.u))
            return Compare(uv.v, bc[ns].uv.v) ? s : ns;

        if(bc[s].uv.v == bc[ns].uv.v && !Compare(uv.v, bc[s].uv.v))
            return Compare(uv.u, bc[ns].uv.u) ? s : ns;
    }

    return -1;
}

inline double GetBorderParam(const TTriLoopNode bc[4], int s, const T_UV_Point& uv)
{
    const T_UV_Point& uv1 = bc[s].uv;
    const T_UV_Point& uv2 = bc[(s+1)&3].uv;

    return  uv1.u == uv2.u ?
                uv1.v < uv2.v ? +uv.v : -uv.v :
                uv1.u < uv2.u ? +uv.u : -uv.u;
}

inline bool GetBorderSideAndParam(const TTriLoopNode bc[4], const T_UV_Point& uv, int& s, double& t)
{
    if((s = GetBorderSide(bc, uv)) == -1)
        return false;

    t = GetBorderParam(bc, s, uv);

    return true;
}

inline double GetBorderMinParam(const TTriLoopNode bc[4], int s)
    { return GetBorderParam(bc, s, bc[s].uv); }

inline double GetBorderMaxParam(const TTriLoopNode bc[4], int s)
    { return GetBorderParam(bc, s, bc[(s+1)&3].uv); }

inline void NormalizeBorderParam(const TTriLoopNode bc[4], int& s, double& t)
{
    if(!Compare(t, GetBorderMaxParam(bc, s)))
        t = GetBorderMinParam(bc, ++s &= 3);
}

inline T_UV_Point GetBorderUV(const TTriLoopNode bc[4], int s, double t)
{
    const T_UV_Point& uv1 = bc[s].uv;
    const T_UV_Point& uv2 = bc[(s+1)&3].uv;

    return  uv1.u == uv2.u ?
                T_UV_Point(uv1.u, uv1.v < uv2.v ? +t : -t) :
                T_UV_Point(uv1.u < uv2.u ? +t : -t, uv1.v);
}

inline bool DoesBorderSideExist(const TTriLoopNode bc[4], int s)
    { return IsFinite(GetBorderMinParam(bc, (s+1)&3)); }

static void UpdateClosedUV(const TTriLoopNode bc[4], T_UV_Point& uv1, T_UV_Point& uv2, const ANY_SURF* pSurface)
{
    if(pSurface->GetUTopology() >= TPL_CLOSED)
    {
        if(Compare(uv2.u - uv1.u, pSurface->GetULength() * 0.5) > 0)
        {
            if(!Compare(uv1.u, pSurface->GetMinU()))
                uv1.u = pSurface->GetMaxU();
            else
                uv2.u -= pSurface->GetULength();
        }
        else if(Compare(uv1.u - uv2.u, pSurface->GetULength() * 0.5) > 0)
        {
            uv2.u += pSurface->GetULength();
        }
    }

    if(pSurface->GetVTopology() >= TPL_CLOSED)
    {
        if(Compare(uv2.v - uv1.v, pSurface->GetVLength() * 0.5) > 0)
        {
            if(!Compare(uv1.v, pSurface->GetMinV()))
                uv1.v = pSurface->GetMaxV();
            else
                uv2.v -= pSurface->GetVLength();
        }
        else if(Compare(uv1.v - uv2.v, pSurface->GetVLength() * 0.5) > 0)
        {
            uv2.v += pSurface->GetVLength();
        }
    }
}

static void UpdateBorderCorner(TTriLoopNode bc[4], int s, const vector& xyz, const ANY_SURF* pSurface)
{
    if(pSurface->GetUTopology() >= TPL_CLOSED && pSurface->GetVTopology() >= TPL_CLOSED) // all corners
    {
        for(s = 0 ; s < 4 ; s++)
            bc[s].xyz = xyz;
    }
    else if(pSurface->GetUTopology() >= TPL_CLOSED) // both U-symmetric corners
    {
        bc[s].xyz = bc[3-s].xyz = xyz;
    }
    else if(pSurface->GetVTopology() >= TPL_CLOSED) // both V-symmetric corners
    {
        bc[s^1].xyz = bc[s^1].xyz = xyz;
    }
    else // just this corner
    {
        bc[s].xyz = xyz;
    }
}

// -----------------------------------
// Triangulation border angular param
// -----------------------------------
struct TTriBorderAngularParam
{
public:
    double t, a;

public:
    TTriBorderAngularParam() {}

    TTriBorderAngularParam(double st, double sa) : t(st), a(sa) {}
};

inline int Compare(const TTriBorderAngularParam& p1, const TTriBorderAngularParam& p2)
{
    int d;

    if(d = Compare(p1.t, p2.t))
        return d;

    if(d = Compare(p1.a, p2.a))
        return d;

    return 0;
}

inline bool operator < (const TTriBorderAngularParam& p1, const TTriBorderAngularParam& p2)
    { return Compare(p1, p2) < 0; }

// ---------------------------------
// Extended triangulation loop node
// ---------------------------------
struct TExtTriLoopNode : TTriLoopNode
{
    // next point, -1 for segments entering border
    size_t next;

    // border parameters, valid only when next==-1
    int s;
    TTriBorderAngularParam p;

    // visited marker for traversal
    bool visited;

    TExtTriLoopNode(const TTriLoopNode& node) : TTriLoopNode(node)
    {
        next = -1;
        visited = false;
    }

    TExtTriLoopNode(const TTriLoopNode& node, int ss, const TTriBorderAngularParam& sp) :
        TTriLoopNode(node), s(ss), p(sp)
    {
        next = -1;
        visited = false;
    }
};

// ---------------------
// Triangulation vertex
// ---------------------
struct TTriVertex : public TTriLoopNode
{
    const TTriEdge* e1;
    const TTriEdge* e2;

    const TTriEdge* GetOtherEdge(const TTriEdge* e) const
        { return e == e1 ? e2 : e1; }
};

inline bool DoTriVerticesShareEdge(const TTriVertex& v1, const TTriVertex& v2)
    { return v1.e2 == v2.e1 || v2.e2 == v1.e1; }

// ---------------------
// Triangulation region
// ---------------------
struct TTriRegion
{
public:
    typedef std::map<double, const TTriVertex*, precision_less> TScanVertices;

private:
    // Sets 'dLocalV' to 'dRemoteV' and then fills 'LocalVertices' with values from 'RemoteVertices'
    // which fall between left and right edge along 'dLocalV' scan line
    void SetSide(   std::vector<const TTriVertex*>& LocalVertices, double& dLocalV,
                    const TScanVertices& RemoteVertices, double dRemoteV) const;

public:
    TTriEdge* le; // left  edge
    TTriEdge* re; // right edge
    mutable std::vector<const TTriVertex*> Vertices1;
    mutable std::vector<const TTriVertex*> Vertices2;
    mutable double v1;
    mutable double v2;

public:
    TTriRegion(TTriEdge* sle, TTriEdge* sre) : le(sle), re(sre) {}

    // Scan-line comparator
    struct less : std::binary_function<TTriRegion, TTriRegion, bool>
    {
    public:
        double& v; // current scan-line

    public:
        less(double& sv) : v(sv) {}

        inline bool operator () (const TTriRegion& r1, const TTriRegion& r2) const;
    };

    void SetSide1(const TScanVertices& Vertices, double v) const
        { SetSide(Vertices1, v1, Vertices, v); }

    void SetSide2(const TScanVertices& Vertices, double v) const
        { SetSide(Vertices2, v2, Vertices, v); }

    void Triangulate(std::vector<vector>& RFaces, const ANY_SURF* pSurface, char cSense) const;

    TTriEdge* GetOtherEdge(TTriEdge* e) const
        { return e == le ? re : le; }
};

typedef std::multiset<TTriRegion, TTriRegion::less> TTriRegions;

// --------------------------------
// Triangulation region references
// --------------------------------
struct TTriRegionRefsLess : std::binary_function<TTriRegions::iterator, TTriRegions::iterator, bool>
{
    inline bool operator () (const TTriRegions::iterator& i1, const TTriRegions::iterator& i2) const
        { return &*i1 < &*i2; }
};

typedef std::set<TTriRegions::iterator, TTriRegionRefsLess> TTriRegionRefs;

// -------------------
// Triangulation edge
// -------------------
struct TTriEdge
{
public:
    TTriVertex* v1;
    TTriVertex* v2;

    double ua, ub; // U = ub + ua * V

    vector xyza, xyzb; // XYZA = xyzb + xyza * V

    TTriRegions::iterator reg; // adjacent region

public:
    TTriVertex* GetOtherVertex(TTriVertex* v) const
        { return v == v1 ? v2 : v1; }

    double GetU(double v) const
    {
        if(!Compare(v, v2->uv.v)) // refers 'v2' or is horizontal case
            return v2->uv.u;

        if(!Compare(v, v1->uv.v)) // refers 'v1'
            return v1->uv.u;

        // General case
        return ub + ua * v;
    }

    TTriVertex GetTV(double v) const
    {
        if(!Compare(v, v2->uv.v)) // refers 'v2' or is horizontal case
            return *v2;

        if(!Compare(v, v1->uv.v)) // refers 'v1'
            return *v1;

        // General case
        TTriVertex tv;

        tv.uv = T_UV_Point(GetU(v), v);

        tv.xyz = xyzb + xyza * v;

        tv.e1 = tv.e2 = this;

        return tv;
    }
};

// ----------------------------
// Triangulation control point
// ----------------------------
struct TTriControlPoint
{
    TTriVertex* v; // control point coordinate
    TTriEdge* e1; // lower  (or lefter)  edge
    TTriEdge* e2; // higher (or righter) edge

    bool IsE1Start() const
        { return e1->v1 == v; }

    bool IsE2Start() const
        { return e2->v1 == v; }
};

inline int Compare(const TTriControlPoint& cp1, const TTriControlPoint& cp2)
{
    int d;

    if(d = Compare(*cp1.v, *cp2.v))
        return d;

    if(d = Compare((int)cp1.IsE1Start(), (int)cp2.IsE1Start()))
        return d;

    if(d = Compare((int)cp1.IsE2Start(), (int)cp2.IsE2Start()))
        return d;

    return Compare(cp1.v, cp2.v); // just for some predictable order
}

bool operator < (const TTriControlPoint& cp1, const TTriControlPoint& cp2)
{
    return Compare(cp1, cp2) < 0;
}

// ------------------------------------
// Triangulation region implementation
// ------------------------------------
inline bool TTriRegion::less::operator () (const TTriRegion& r1, const TTriRegion& r2) const
{
    return Compare(r1.le->GetU(v), r2.le->GetU(v)) > 0;
}

void TTriRegion::SetSide(   std::vector<const TTriVertex*>& LocalVertices, double& dLocalV,
                            const TScanVertices& RemoteVertices, double dRemoteV) const
{
    dLocalV = dRemoteV;

    double lu = le->GetU(dLocalV);
    double ru = re->GetU(dLocalV);

    LocalVertices.clear();

    for(TScanVertices::const_iterator i = RemoteVertices.upper_bound(lu) ;
        i != RemoteVertices.end() && Compare(i->first, ru) < 0 ;
        ++i)
    {
        LocalVertices.push_back(i->second);
    }
}

static void AddScanPoint(std::vector<TTriLoopNode>& p, TTriVertex& LastTV, const TTriVertex& CurTV, const ANY_SURF* pSurface)
{
    if(!p.empty())
    {
        std::vector<double> u;
        GenerateMidPoints(u, LastTV.uv.u, CurTV.uv.u, pSurface->GetUValues());

        if(DoTriVerticesShareEdge(LastTV, CurTV)) // U->XYZ interpolation
        {
            // xyz = xyzb + xyza * u
            vector xyza = (CurTV.xyz - LastTV.xyz) / (CurTV.uv.u - LastTV.uv.u);
            vector xyzb = LastTV.xyz - xyza * LastTV.uv.u;

            for(size_t i = 0 ; i < u.size() ; i++)
                p.push_back(TTriLoopNode(xyzb + xyza * u[i], T_UV_Point(u[i], CurTV.uv.v)));
        }
        else // UV->XYZ transformation
        {
            for(size_t i = 0 ; i < u.size() ; i++)
                p.push_back(TTriLoopNode(T_UV_Point(u[i], CurTV.uv.v), pSurface));
        }
    }

    p.push_back(LastTV = CurTV);
}

void TTriRegion::Triangulate(std::vector<vector>& RFaces, const ANY_SURF* pSurface, char cSense) const
{
    if(!Compare(v1, v2))
        return;

    assert(Compare(v1, v2) < 0);

    // Generating top vertices
    std::vector<TTriLoopNode> TopVertices;

    {
        TTriVertex ltv = le->GetTV(v1);
        TTriVertex rtv = re->GetTV(v1);

        TTriVertex LastTV;

        // Left point
        AddScanPoint(TopVertices, LastTV, ltv, pSurface);

        // Mid points
        for(size_t i = 0 ; i < Vertices1.size() ; i++)
            AddScanPoint(TopVertices, LastTV, *Vertices1[i], pSurface);

        // Right point
        if(Compare(ltv, rtv) < 0)
            AddScanPoint(TopVertices, LastTV, rtv, pSurface);
    }

    // Generating bottom vertices
    std::vector<TTriLoopNode> BottomVertices;

    {
        TTriVertex ltv = le->GetTV(v2);
        TTriVertex rtv = re->GetTV(v2);

        TTriVertex LastTV;

        // Left point
        AddScanPoint(BottomVertices, LastTV, ltv, pSurface);

        // Mid points
        for(size_t j = 0 ; j < Vertices2.size() ; j++)
            AddScanPoint(BottomVertices, LastTV, *Vertices2[j], pSurface);

        // Right point
        if(Compare(ltv, rtv) < 0)
            AddScanPoint(BottomVertices, LastTV, rtv, pSurface);
    }

    // Triangulating strip
    TriangulateStrip(RFaces, TopVertices, BottomVertices, cSense);
}

// ----------------
// UV triangulator
// ----------------
static int GetTriLoopOrientation(const std::vector<TTriLoopNode>& Loop)
{
    if(Loop.size() < 3)
        return 0;

    // Performing 0..1 UV scaling to avoid precision problems
    T_UV_Point MinUV(+DBL_MAX, +DBL_MAX);
    T_UV_Point MaxUV(-DBL_MAX, -DBL_MAX);

    for(size_t i = 0 ; i < Loop.size() ; i++)
    {
        MinUV.u = Min(MinUV.u, Loop[i].uv.u);
        MinUV.v = Min(MinUV.v, Loop[i].uv.v);

        MaxUV.u = Max(MaxUV.u, Loop[i].uv.u);
        MaxUV.v = Max(MaxUV.v, Loop[i].uv.v);
    }

    if(!Compare(MinUV.u, MaxUV.u) || !Compare(MinUV.v, MaxUV.v)) // degenerate, removing
        return 0;

    double uc = 1.0 / (MaxUV.u - MinUV.u);
    double vc = 1.0 / (MaxUV.v - MinUV.v);

    double dArea = 0.0; // oriented loop area

    for(size_t i = 0 ; i < Loop.size() ; i++)
    {
        size_t ni = GetNextCyclicIdx(i, Loop.size());

        double u1 = MinUV.u + (Loop[ i].uv.u - MinUV.u) * uc;
        double v1 = MinUV.v + (Loop[ i].uv.v - MinUV.v) * vc;

        double u2 = MinUV.u + (Loop[ni].uv.u - MinUV.u) * uc;
        double v2 = MinUV.v + (Loop[ni].uv.v - MinUV.v) * vc;

        dArea += u1 * v2 - u2 * v1;
    }

    return Sign(dArea);
}

static void RemoveDegenerateTriLoops(std::vector<std::vector<TTriLoopNode> >& Loops)
{
    for(size_t i = 0 ; i < Loops.size() ; i++)
    {
        size_t n2 = 0;

        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            if(!j || Compare(Loops[i][j-1], Loops[i][j]))
                Loops[i][n2++] = Loops[i][j];
        }

        if(n2 && !Compare(Loops[i][0], Loops[i][n2-1]))
            n2--;

        Loops[i].erase(Loops[i].begin() + n2, Loops[i].end());

        if(Loops[i].size() < 3)
            Loops.erase(Loops.begin() + i--);
    }
}

static void RefineTriLoopCaps(std::vector<std::vector<TTriLoopNode> >& Loops, const ANY_SURF* pSurface)
{
    if(!pSurface->IsPointMinV() && !pSurface->IsPointMaxV())
        return;

    // Collapsing multiple point-capped points to single point
    RemoveDegenerateTriLoops(Loops);

    // Splitting point-capped points into two
    for(size_t i = 0 ; i < Loops.size() ; i++)
    {
        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            if(pSurface->IsPointCapped(Loops[i][j].uv.v))
            {
                double pu = Loops[i][GetPrevCyclicIdx(j, Loops[i].size())].uv.u;
                double nu = Loops[i][GetNextCyclicIdx(j, Loops[i].size())].uv.u;

                Loops[i].insert(Loops[i].begin() + j + 1, Loops[i][j]);

                Loops[i][j  ].uv.u = pu;
                Loops[i][j+1].uv.u = nu;

                j++;
            }
        }
    }
}

static bool LimitTriangulationSpace(std::vector<std::vector<TTriLoopNode> >& Loops, const ANY_SURF* pSurface, char cSense)
{
    RemoveDegenerateTriLoops(Loops);

    // Precaching topology and dimensions
    bool bIsUClosed = pSurface->GetUTopology() >= TPL_CLOSED;
    bool bIsVClosed = pSurface->GetVTopology() >= TPL_CLOSED;

    double dMinU = pSurface->GetMinU();
    double dMaxU = pSurface->GetMaxU();

    double dMinV = pSurface->GetMinV();
    double dMaxV = pSurface->GetMaxV();

    double dULength = bIsUClosed ? pSurface->GetULength() : DBL_MAX;
    double dVLength = bIsVClosed ? pSurface->GetVLength() : DBL_MAX;
    double dHalfULength = bIsUClosed ? dULength * 0.5 : DBL_MAX;
    double dHalfVLength = bIsVClosed ? dVLength * 0.5 : DBL_MAX;

    // Refining point caps and point wraps
    RefineTriLoopCaps(Loops, pSurface);

    // Border corners, counter-clockwise for positive sense, starting with bottom-right
    // XYZ is set later because some UV coordinates may be infinite
    TTriLoopNode BorderCorners[4] =
    {
        TTriLoopNode(vector(0, 0, 0), T_UV_Point(dMaxU, dMinV)),
        TTriLoopNode(vector(0, 0, 0), T_UV_Point(dMaxU, dMaxV)),
        TTriLoopNode(vector(0, 0, 0), T_UV_Point(dMinU, dMaxV)),
        TTriLoopNode(vector(0, 0, 0), T_UV_Point(dMinU, dMinV))
    };

    for(int s = 0 ; s < 4 ; s++)
    {
        if(IsFinite(BorderCorners[s].uv))
            BorderCorners[s].xyz = pSurface->GetApproxXYZ(BorderCorners[s].uv);
    }

    // Negative sense should be walked clockwise
    if(cSense == '-')
    {
        std::swap(BorderCorners[0], BorderCorners[3]);
        std::swap(BorderCorners[1], BorderCorners[2]);
    }

    // opposite side:      s^2
    // opposite U  corner: 3-s
    // opposite V  corner: s^1
    // opposite UV corner: s^2

    // Border steppings (U/V type does not depend on sense)
    TParamValues BorderParamValues[4] =
        {pSurface->GetVValues(), pSurface->GetUValues(), pSurface->GetVValues(), pSurface->GetUValues()};

    // Updating border steppings to reflect border param direction
    for(unsigned s = 0 ; s < 4 ; s++)
    {
        if( BorderCorners[s].uv.u > BorderCorners[(s+1)&3].uv.u ||
            BorderCorners[s].uv.v > BorderCorners[(s+1)&3].uv.v)
        {
            TParamValues RevValues;

            for(TParamValues::const_iterator iter = BorderParamValues[s].begin() ;
                iter != BorderParamValues[s].end() ;
                ++iter)
            {
                RevValues.insert(-*iter);
            }

            BorderParamValues[s] = RevValues;
        }
    }

    // Number of positively-oriented loops (with respect to sense) which do not cross border
    size_t szNInsideLoops = 0;

    // Linked loop nodes (including new border points)
    std::vector<TExtTriLoopNode> ExtNodes;

    // Border outgoing points
    typedef std::multimap<TTriBorderAngularParam, size_t> TBorderOutPoints; // maps border parameter to index out leaving ext-node
    TBorderOutPoints BorderOutPoints[4];

    // Border loop points
    typedef std::map<double, vector, precision_less> TBorderProfilePoints; // maps border parameter to its XYZ value
    TBorderProfilePoints BorderProfilePoints[4];

    typedef std::multimap<double, int> TBorderProfileEdges; // paints border with +-1 along its parameter
    TBorderProfileEdges BorderProfileEdges[4];

    // Registering border points
    for(size_t i = 0 ; i < Loops.size() ; i++)
    {
        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            // Getting border point
            int    s;
            double t;

            if(!GetBorderSideAndParam(BorderCorners, Loops[i][j].uv, s, t))
                continue;

            // Registering border point
            if(!Compare(t, GetBorderMinParam(BorderCorners, s))) // corner
            {
                UpdateBorderCorner(BorderCorners, s, Loops[i][j].xyz, pSurface);
            }
            else // side middle
            {
                BorderProfilePoints[s].insert(TBorderProfilePoints::value_type(t, Loops[i][j].xyz));

                if((s & 1) ? bIsVClosed : bIsUClosed)
                    BorderProfilePoints[s^2].insert(TBorderProfilePoints::value_type(-t, Loops[i][j].xyz));
            }
        }
    }

    // Registering border in/out points, edges and creating ExtTriLoops for traversal
    for(size_t i = 0 ; i < Loops.size() ; i++)
    {
        size_t first = ExtNodes.size();

        bool bBorderCrossed = false;

        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            size_t nj = GetNextCyclicIdx(j, Loops[i].size());

            int    s1, s2;
            double t1, t2;

            GetBorderSideAndParam(BorderCorners, Loops[i][ j].uv, s1, t1);
            GetBorderSideAndParam(BorderCorners, Loops[i][nj].uv, s2, t2);

            if(s1 != -1 || s2 != -1)
                bBorderCrossed = true;

            // Checking if both endpoints belong to same border side
            if(s1 != -1 && s2 != -1)
            {
                if(((s1+1)&3) == s2 && !Compare(t2, GetBorderMinParam(BorderCorners, s2)))
                    t2 = GetBorderMaxParam(BorderCorners, --s2 &= 3);
                else if(((s2+1)&3) == s1 && !Compare(t1, GetBorderMinParam(BorderCorners, s1)))
                    t1 = GetBorderMaxParam(BorderCorners, --s1 &= 3);

                if(s1 == s2)
                {
                    vector xyz1 = Loops[i][ j].xyz;
                    vector xyz2 = Loops[i][nj].xyz;

                    if(t1 > t2)
                    {
                        std::swap(xyz1, xyz2);
                        std::swap(t1, t2);
                    }

                    if( ((s1 & 1) ? bIsUClosed : bIsVClosed) &&
                        Compare(t2 - t1, (s1 & 1) ? dHalfULength : dHalfVLength) > 0) // (min-t1) and (t2-max) segments
                    {
                        // Updating border corners
                        {
                            vector xyz =
                                BorderCorners[s1].uv.u < BorderCorners[(s1+1)&3].uv.u ||
                                BorderCorners[s1].uv.v < BorderCorners[(s1+1)&3].uv.v ?
                                    xyz1 + (xyz2 - xyz1) * (GetBorderMinParam(BorderCorners, s1) - t1) / (t2 - t1 - ((s1 & 1) ? dULength : dVLength)) :
                                    xyz2 + (xyz1 - xyz2) * (GetBorderMaxParam(BorderCorners, s1) - t2) / (t1 - t2 + ((s1 & 1) ? dULength : dVLength));

                            UpdateBorderCorner(BorderCorners, s1, xyz, pSurface);
                        }

                        // Registering border edges
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(GetBorderMinParam(BorderCorners, s1), +1));
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(+t1, -1));
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(+t2, +1));
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(GetBorderMaxParam(BorderCorners, s1), -1));

                        if((s1 & 1) ? bIsVClosed : bIsUClosed)
                        {
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-GetBorderMaxParam(BorderCorners, s1), +1));
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-t2, -1));
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-t1, +1));
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-GetBorderMinParam(BorderCorners, s1), -1));
                        }
                    }
                    else // (t1-t2) segment
                    {
                        // Registering border edges
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(+t1, +1));
                        BorderProfileEdges[s1].insert(TBorderProfileEdges::value_type(+t2, -1));

                        if((s1 & 1) ? bIsVClosed : bIsUClosed)
                        {
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-t2, +1));
                            BorderProfileEdges[s1^2].insert(TBorderProfileEdges::value_type(-t1, -1));
                        }
                    }

                    continue; // in-border edges produce no enter/exit/inner points
                }
            }

            // Getting UVs, adjusted for closed UV space
            T_UV_Point uv1 = Loops[i][ j].uv;
            T_UV_Point uv2 = Loops[i][nj].uv;

            UpdateClosedUV(BorderCorners, uv1, uv2, pSurface);

            // Registering loop node
            ExtNodes.push_back(TTriLoopNode(Loops[i][j].xyz, uv1));
            ExtNodes.rbegin()->next = ExtNodes.size();

            if(s1 != -1) // 1st is exit point
            {
                // Updating side parametrization with updated UV1
                GetBorderSideAndParam(BorderCorners, uv1, s1, t1);

                // Registering exit point
                double a1 = GetBorderParam(BorderCorners, s1, (uv2 - uv1).Normalized());

                BorderOutPoints[s1].insert(TBorderOutPoints::value_type(TTriBorderAngularParam(t1, a1), ExtNodes.size() - 1));
            }

            // Getting closed surface border intersections
            const vector& xyz1 = Loops[i][ j].xyz;
            const vector& xyz2 = Loops[i][nj].xyz;

            double     t  [2]; // intersection 't' of loop segment as 0..1
            int        is [2]; // border entry side
            int        os [2]; // border exit  side
            T_UV_Point dlt[2]; // UV(out-side) - UV(in-side)
            unsigned n = 0; // number of intersections

            // Getting border intersection points
            if(bIsUClosed) // is U-closed and neither point is on V-side
            {
                if(Compare(uv1.u, dMinU) > 0 && Compare(uv2.u, dMinU) < 0) // u=0/maxU crossing
                    t[n] = (dMinU - uv1.u) / (uv2.u - uv1.u), dlt[n] = T_UV_Point(+dULength, 0), is[n] = cSense == '-' ? 0 : 2, n++;
                else if(Compare(uv1.u, dMaxU) < 0 && Compare(uv2.u, dMaxU) > 0) // u=maxU/0 crossing
                    t[n] = (dMaxU - uv1.u) / (uv2.u - uv1.u), dlt[n] = T_UV_Point(-dULength, 0), is[n] = cSense == '-' ? 2 : 0, n++;
            }

            if(bIsVClosed) // is V-closed and neither point is on U-side
            {
                if(Compare(uv1.v, dMinV) > 0 && Compare(uv2.v, dMinV) < 0) // v=0/maxV crossing
                    t[n] = (dMinV - uv1.v) / (uv2.v - uv1.v), dlt[n] = T_UV_Point(0, +dVLength), is[n] = 3, n++;
                else if(Compare(uv1.v, dMaxV) < 0 && Compare(uv2.v, dMaxV) > 0) // v=maxV/0 crossing
                    t[n] = (dMaxV - uv1.v) / (uv2.v - uv1.v), dlt[n] = T_UV_Point(0, -dVLength), is[n] = 1, n++;
            }

            for(unsigned k = 0 ; k < n ; k++)
                os[k] = is[k] ^ 2;

            // Sorting border intersection points
            if(n == 2)
            {
                int d = Compare(t[0], t[1]);

                if(!d) // identical
                {
                    os[0] = os[1], dlt[0] += dlt[1], n--;
                }
                else if(d > 0) // different, but in wrong order
                {
                    std::swap(t  [0], t  [1]);
                    std::swap(is [0], is [1]);
                    std::swap(os [0], os [1]);
                    std::swap(dlt[0], dlt[1]);
                }
            }

            // Splitting loop along border intersection points (at most 2)
            for(unsigned k = 0 ; k < n ; k++)
            {
                // Intersection XYZ coordinate (linear interpolation of loop XYZ representation)
                vector ixyz = xyz1 + (xyz2 - xyz1) * t[k];

                // Intersection UV coordinate (linear interpolation of loop UV representation)
                T_UV_Point iuv = uv1 + (uv2 - uv1) * t[k];

                // Getting border entry side parameter
                double ist = GetBorderParam(BorderCorners, is[k], iuv);
                NormalizeBorderParam(BorderCorners, is[k], ist);
                double isa = GetBorderParam(BorderCorners, is[k], (uv1 - uv2).Normalized());

                // Checking for corner intersection
                if(!Compare(ist, GetBorderMinParam(BorderCorners, is[k])))
                    UpdateBorderCorner(BorderCorners, is[k], ixyz, pSurface);

                // Registering in-side entry
                ExtNodes.push_back(TExtTriLoopNode(TTriLoopNode(ixyz, iuv), is[k], TTriBorderAngularParam(ist, isa)));

                // Jumping to the opposite side
                uv1 += dlt[k];
                uv2 += dlt[k];
                iuv += dlt[k];

                // Getting border exit side and its parameter
                double ost = GetBorderParam(BorderCorners, os[k], iuv);
                NormalizeBorderParam(BorderCorners, os[k], ost);
                double osa = GetBorderParam(BorderCorners, os[k], (uv2 - uv1).Normalized());

                // Registering opposite border side exit
                ExtNodes.push_back(TTriLoopNode(ixyz, iuv));
                ExtNodes.rbegin()->next = ExtNodes.size();

                BorderOutPoints[os[k]].insert(TBorderOutPoints::value_type(TTriBorderAngularParam(ost, osa), ExtNodes.size() - 1));
            }

            if(s2 != -1) // 2nd is entry point
            {
                // Performing 2nd-point-aligned UV update
                T_UV_Point uv1 = Loops[i][ j].uv;
                T_UV_Point uv2 = Loops[i][nj].uv;

                UpdateClosedUV(BorderCorners, uv2, uv1, pSurface); // note uv2,uv1 order (uv2 is primary)

                GetBorderSideAndParam(BorderCorners, uv2, s2, t2);

                // Registering entry point
                double a2 = GetBorderParam(BorderCorners, s2, (uv1 - uv2).Normalized());

                ExtNodes.push_back(TExtTriLoopNode(TTriLoopNode(Loops[i][nj].xyz, uv2), s2, TTriBorderAngularParam(t2, a2)));
            }
        }

        if(first < ExtNodes.size() && ExtNodes.rbegin()->next != -1)
            ExtNodes.rbegin()->next = first;

        if(!bBorderCrossed && GetTriLoopOrientation(Loops[i]) * (cSense == '-' ? -1 : +1) >= 0)
            szNInsideLoops++;
    }

    // Interpolating border parts not covered by loop edges
    for(int s = 0 ; s < 4 ; s++)
    {
        if(!DoesBorderSideExist(BorderCorners, s))
            continue;

        int dep = 0;

        double pt = GetBorderMinParam(BorderCorners, s);
        double mt = GetBorderMaxParam(BorderCorners, s);

        TBorderProfileEdges::const_iterator ce = BorderProfileEdges[s].begin();

        for(TBorderProfilePoints::const_iterator cp = BorderProfilePoints[s].begin() ;
            cp != BorderProfilePoints[s].end() ;
            ++cp)
        {
            for( ; ce != BorderProfileEdges[s].end() && Compare(ce->first, cp->first) < 0 ; ++ce)
                dep += ce->second;

            if(pt != -DBL_MAX && !dep) // got finite border segment not covered by loop edges
            {
                std::vector<double> t;
                GenerateMidPoints(t, pt, cp->first, BorderParamValues[s]);

                for(size_t i = 0 ; i < t.size() ; i++)
                    BorderProfilePoints[s].insert(TBorderProfilePoints::value_type(t[i], pSurface->GetApproxXYZ(GetBorderUV(BorderCorners, s, t[i]))));
            }

            pt = cp->first;
        }

        // Processing final stride from last loop point to next corner
        if(pt != -DBL_MAX && mt != +DBL_MAX)
        {
            for( ; ce != BorderProfileEdges[s].end() && Compare(ce->first, mt) < 0 ; ++ce)
                dep += ce->second;

            if(!dep)
            {
                std::vector<double> t;
                GenerateMidPoints(t, pt, mt, BorderParamValues[s]);

                for(size_t i = 0 ; i < t.size() ; i++)
                    BorderProfilePoints[s].insert(TBorderProfilePoints::value_type(t[i], pSurface->GetApproxXYZ(GetBorderUV(BorderCorners, s, t[i]))));
            }
        }
    }

    // Refilling loops vector from ext-loops chains
    bool bBorderTraversed = false;

    Loops.clear();

    for(size_t i = 0 ; i < ExtNodes.size() ; i++)
    {
        if(ExtNodes[i].visited) // already visited
            continue;

        std::vector<TTriLoopNode> Loop;

        for(size_t j = i ; !ExtNodes[j].visited ; j = ExtNodes[j].next)
        {
            ExtNodes[j].visited = true;

            Loop.push_back(ExtNodes[j]);

            if(ExtNodes[j].next == -1) // border traversal required
            {
                bBorderTraversed = true;

                int cs = ExtNodes[j].s;
                TTriBorderAngularParam cp = ExtNodes[j].p;

                for(;;)
                {
                    assert( !BorderOutPoints[0].empty() ||
                            !BorderOutPoints[1].empty() ||
                            !BorderOutPoints[2].empty() ||
                            !BorderOutPoints[3].empty());

                    // Getting nearest exit along current side
                    TBorderOutPoints::iterator target_iter = BorderOutPoints[cs].lower_bound(cp);

                    double lt;

                    if(target_iter != BorderOutPoints[cs].end())
                        lt = target_iter->first.t;
                    else
                        lt = GetBorderMaxParam(BorderCorners, cs);

                    if(lt == DBL_MAX) // attempt to traverse infinite border side
                        return false;

                    for(TBorderProfilePoints::iterator pt_iter = BorderProfilePoints[cs].upper_bound(cp.t) ;
                        pt_iter != BorderProfilePoints[cs].end() && Compare(pt_iter->first, lt) < 0 ;
                        ++pt_iter)
                    {
                        Loop.push_back(TTriLoopNode(pt_iter->second, GetBorderUV(BorderCorners, cs, pt_iter->first)));
                    }

                    if(target_iter != BorderOutPoints[cs].end())
                    {
                        ExtNodes[j].next = target_iter->second;
                        BorderOutPoints[cs].erase(target_iter);
                        break;
                    }

                    cp.t = GetBorderMinParam(BorderCorners, ++cs &= 3), cp.a = -DBL_MAX;

                    Loop.push_back(BorderCorners[cs]);
                }
            }
        }

        Loops.push_back(Loop);
    }

    // Checking if entire border must be considered as a separate loop
    if(!bBorderTraversed && !szNInsideLoops)
    {
        if(pSurface->GetUTopology() < TPL_OPEN || pSurface->GetVTopology() < TPL_OPEN) // infinite face, invalid
            return false;

        std::vector<TTriLoopNode> BorderLoop;

        std::vector<double> u;
        u.push_back(dMinU);
        GenerateMidPoints(u, dMinU, dMaxU, pSurface->GetUValues());
        u.push_back(dMaxU);

        std::vector<double> v;
        v.push_back(dMinV);
        GenerateMidPoints(v, dMinV, dMaxV, pSurface->GetVValues());
        v.push_back(dMaxV);

        for(size_t i = 0 ; i < u.size() - 1 ; i++)
            BorderLoop.push_back(TTriLoopNode(T_UV_Point(u[i], dMinV), pSurface));

        for(size_t i = 0 ; i < v.size() - 1 ; i++)
            BorderLoop.push_back(TTriLoopNode(T_UV_Point(dMaxU, v[i]), pSurface));

        for(size_t i = u.size() - 1 ; i ; i--)
            BorderLoop.push_back(TTriLoopNode(T_UV_Point(u[i], dMaxV), pSurface));

        for(size_t i = v.size() - 1 ; i ; i--)
            BorderLoop.push_back(TTriLoopNode(T_UV_Point(dMinU, v[i]), pSurface));

        Loops.push_back(BorderLoop);
    }

    RemoveDegenerateTriLoops(Loops);

    return true;
}

static bool TriangulateUV_Surface(  std::vector<vector>& RFaces,
                                    const ANY_SURF* pSurface, char cSense,
                                    std::vector<std::vector<TTriLoopNode> >& Loops)
{
    // Converting closed U/V space to limited U/V space
    if(!LimitTriangulationSpace(Loops, pSurface, cSense))
        return false;

    // Calculating total number of loop vertices
    size_t szTotalVertices = 0;

    for(size_t i = 0 ; i < Loops.size() ; i++)
        szTotalVertices += Loops[i].size();

    // Preallocating vertices and edges arrays
    std::vector<TTriVertex> Vertices(szTotalVertices);
    std::vector<TTriEdge>   Edges   (szTotalVertices);

    // Generating triangulation vertices
    for(size_t i = 0, szBaseIndex = 0 ; i < Loops.size() ; szBaseIndex += Loops[i++].size())
    {
        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            size_t pj = GetPrevCyclicIdx(j, Loops[i].size());

            TTriVertex& Vertex = Vertices[szBaseIndex + j];

            Vertex.uv  = Loops[i][j].uv;
            Vertex.xyz = Loops[i][j].xyz;

            Vertex.e1 = &Edges[szBaseIndex + pj];
            Vertex.e2 = &Edges[szBaseIndex +  j];
        }
    }

    // Generating triangulation edges
    for(size_t i = 0, szBaseIndex = 0 ; i < Loops.size() ; szBaseIndex += Loops[i++].size())
    {
        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            size_t nj = GetNextCyclicIdx(j, Loops[i].size());

            TTriEdge& Edge = Edges[szBaseIndex + j];

            Edge.v1 = &Vertices[szBaseIndex +  j];
            Edge.v2 = &Vertices[szBaseIndex + nj];

            {
                int d;

                if(!(d = Compare(*Edge.v1, *Edge.v2)))
                    d = Compare(Edge.v1, Edge.v2);

                if(d > 0)
                    std::swap(Edge.v1, Edge.v2);
            }

            if(!Compare(Edge.v1->uv.v, Edge.v2->uv.v)) // horizontal edge
            {
                Edge.ua = DBL_MAX; // required for proper control-point edges ordering
            }
            else
            {
                Edge.ua = (Edge.v2->uv.u - Edge.v1->uv.u) / (Edge.v2->uv.v - Edge.v1->uv.v);
                Edge.ub = Edge.v1->uv.u - Edge.ua * Edge.v1->uv.v;

                Edge.xyza = (Edge.v2->xyz - Edge.v1->xyz) / (Edge.v2->uv.v - Edge.v1->uv.v);
                Edge.xyzb = Edge.v1->xyz - Edge.xyza * Edge.v1->uv.v;
            }
        }
    }

    // Generating triangulation control points
    std::vector<TTriControlPoint> ControlPoints;

    for(size_t i = 0, szBaseIndex = 0 ; i < Loops.size() ; szBaseIndex += Loops[i++].size())
    {
        for(size_t j = 0 ; j < Loops[i].size() ; j++)
        {
            size_t pj = GetPrevCyclicIdx(j, Loops[i].size());

            TTriControlPoint cp;

            cp.v = &Vertices[szBaseIndex + j];

            cp.e1 = &Edges[szBaseIndex + pj];
            cp.e2 = &Edges[szBaseIndex +  j];

            if(cp.IsE1Start() && cp.IsE2Start()) // both start
            {
                if(cp.e1->ua == DBL_MAX && cp.e2->ua == DBL_MAX) // both horizontal, shorter should be first
                {
                    if(Compare(*cp.e1->v2, *cp.e2->v2) > 0)
                        std::swap(cp.e1, cp.e2);
                }
                else if(cp.e1->ua == DBL_MAX) // only first is horizontal, should be last
                {
                    std::swap(cp.e1, cp.e2);
                }
                else if(cp.e2->ua != DBL_MAX) // both non-horizontal, lesser 'ua' should be first
                {
                    if(Compare(cp.e1->ua, cp.e2->ua) > 0)
                        std::swap(cp.e1, cp.e2);
                }
            }
            else if(!cp.IsE1Start() && !cp.IsE2Start()) // both stop
            {
                if(cp.e1->ua == DBL_MAX && cp.e2->ua == DBL_MAX) // both horizontal, shorter should be first
                {
                    if(Compare(*cp.e1->v1, *cp.e2->v1) < 0)
                        std::swap(cp.e1, cp.e2);
                }
                else if(cp.e2->ua == DBL_MAX) // only last is horizontal, should be first
                {
                    std::swap(cp.e1, cp.e2);
                }
                else if(cp.e1->ua != DBL_MAX) // both non-horizontal, bigger 'ua' should be first
                {
                    if(Compare(cp.e1->ua,  cp.e2->ua) < 0)
                        std::swap(cp.e1, cp.e2);
                }
            }
            else if(cp.IsE1Start() && !cp.IsE2Start()) // start-stop should be stop-start (updater node)
            {
                std::swap(cp.e1, cp.e2);
            }

            ControlPoints.push_back(cp);
        }
    }

    sort(ControlPoints.begin(), ControlPoints.end());

    size_t szOldN = RFaces.size();

    // Processing triangulation control points
    double dCurV;
    TTriRegion::less RegComp(dCurV);
    TTriRegions Regions(RegComp);

    for(size_t i = 0 ; i < ControlPoints.size() ; )
    {
        dCurV = ControlPoints[i].v->uv.v;

        TTriRegion::TScanVertices ScanVertices;

        for(size_t j = i ; j < ControlPoints.size() && !Compare(ControlPoints[j].v->uv.v, dCurV) ; j++)
            ScanVertices.insert(TTriRegion::TScanVertices::value_type(ControlPoints[j].v->uv.u, ControlPoints[j].v));

        TTriRegionRefs NewRegions;

        for( ; i < ControlPoints.size() && !Compare(ControlPoints[i].v->uv.v, dCurV) ; i++)
        {
            TTriControlPoint& cp = ControlPoints[i];

            if(cp.IsE1Start() && cp.IsE2Start()) // splitter or new region
            {
                TTriRegions::iterator RegIter = Regions.upper_bound(TTriRegion(cp.e1, cp.e2));

                if(RegIter != Regions.end() && Compare(cp.v->uv.u, RegIter->re->GetU(dCurV)) < 0) // splitter
                {
                    if(Compare(RegIter->le->GetU(dCurV), RegIter->re->GetU(dCurV)) > 0)
                    {
                        RFaces.resize(szOldN);
                        return false;
                    }

                    // Triangulating owner
                    if(NewRegions.find(RegIter) == NewRegions.end())
                    {
                        RegIter->SetSide2(ScanVertices, dCurV);
                        RegIter->Triangulate(RFaces, pSurface, cSense);
                    }

                    // Creating left offspring
                    {
                        TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(RegIter->le, cp.e1));
                        SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                        NewRegions.insert(SubRegIter);
                    }

                    // Creating right offspring
                    {
                        TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(cp.e2, RegIter->re));
                        SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                        NewRegions.insert(SubRegIter);
                    }

                    // Deleting owner
                    {
                        TTriRegionRefs::iterator RefIter = NewRegions.find(RegIter);

                        if(RefIter != NewRegions.end())
                            NewRegions.erase(RefIter);

                        Regions.erase(RegIter);
                    }
                }
                else // new region
                {
                    TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(cp.e1, cp.e2));
                    SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                    NewRegions.insert(SubRegIter);
                }
            }
            else if(!cp.IsE1Start() && !cp.IsE2Start()) // merger or collapser
            {
                if(cp.e1->reg == cp.e2->reg) // collapser
                {
                    TTriRegions::iterator RegIter = cp.e1->reg;

                    // Triangulating collapsed region
                    if(NewRegions.find(RegIter) == NewRegions.end())
                    {
                        RegIter->SetSide2(ScanVertices, dCurV);
                        RegIter->Triangulate(RFaces, pSurface, cSense);
                    }

                    // Deleting collapsed region
                    {
                        TTriRegionRefs::iterator RefIter = NewRegions.find(RegIter);

                        if(RefIter != NewRegions.end())
                            NewRegions.erase(RefIter);

                        Regions.erase(RegIter);
                    }
                }
                else // merger
                {
                    if( cp.e1 != cp.e1->reg->re || cp.e2 != cp.e2->reg->le ||
                        Compare(cp.e1->reg->le->GetU(dCurV), cp.e1->reg->re->GetU(dCurV)) > 0 ||
                        Compare(cp.e2->reg->le->GetU(dCurV), cp.e2->reg->re->GetU(dCurV)) > 0)
                    {
                        RFaces.resize(szOldN);
                        return false;
                    }

                    // Triangulating left side
                    if(NewRegions.find(cp.e1->reg) == NewRegions.end())
                    {
                        cp.e1->reg->SetSide2(ScanVertices, dCurV);
                        cp.e1->reg->Triangulate(RFaces, pSurface, cSense);
                    }

                    // Triangulating right side
                    if(NewRegions.find(cp.e2->reg) == NewRegions.end())
                    {
                        cp.e2->reg->SetSide2(ScanVertices, dCurV);
                        cp.e2->reg->Triangulate(RFaces, pSurface, cSense);
                    }

                    // Creating united region
                    {
                        TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(cp.e1->reg->le, cp.e2->reg->re));
                        SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                        NewRegions.insert(SubRegIter);
                    }

                    // Deleting left side region
                    {
                        TTriRegionRefs::iterator RefIter = NewRegions.find(cp.e1->reg);

                        if(RefIter != NewRegions.end())
                            NewRegions.erase(RefIter);

                        Regions.erase(cp.e1->reg);
                    }

                    // Deleting right side region
                    {
                        TTriRegionRefs::iterator RefIter = NewRegions.find(cp.e2->reg);

                        if(RefIter != NewRegions.end())
                            NewRegions.erase(RefIter);

                        Regions.erase(cp.e2->reg);
                    }
                }
            }
            else // updater
            {
                TTriRegions::iterator RegIter = cp.e1->reg;

                if(Compare(RegIter->le->GetU(dCurV), RegIter->re->GetU(dCurV)) > 0)
                {
                    RFaces.resize(szOldN);
                    return false;
                }

                // Triangulating updated region
                if(NewRegions.find(RegIter) == NewRegions.end())
                {
                    RegIter->SetSide2(ScanVertices, dCurV);
                    RegIter->Triangulate(RFaces, pSurface, cSense);
                }

                // Creating new updated region
                if(RegIter->le == cp.e1) // left updater
                {
                    TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(cp.e2, RegIter->re));
                    SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                    NewRegions.insert(SubRegIter);
                }
                else // right updater
                {
                    TTriRegions::iterator SubRegIter = Regions.insert(TTriRegion(RegIter->le, cp.e2));
                    SubRegIter->le->reg = SubRegIter->re->reg = SubRegIter;
                    NewRegions.insert(SubRegIter);
                }

                // Deleting updated region
                {
                    TTriRegionRefs::iterator RefIter = NewRegions.find(RegIter);

                    if(RefIter != NewRegions.end())
                        NewRegions.erase(RefIter);

                    Regions.erase(RegIter);
                }
            }
        }

        // Setting top of survived regions
        for(TTriRegionRefs::const_iterator RefIter = NewRegions.begin() ; RefIter != NewRegions.end() ; ++RefIter)
            (*RefIter)->SetSide1(ScanVertices, dCurV);
    }

    assert(Regions.empty());

    return true;
}

// -----
// NODE
// -----
std::string NODE::GetTypeString() const
{
    // Getting node type via RTTI. Under MSVC2008 these are of the form 'struct parasolid::ELLIPSE'
    std::string Type = typeid(*this).name();

    if(Type.length() >= 7 && Type.substr(0, 7) == "struct ")
        Type = Type.substr(7);

    if(Type.length() >= 11 && Type.substr(0, 11) == "parasolid::")
        Type = Type.substr(11);

    return Type;
}

std::string NODE::GetExpandedTypeString() const
{
    std::string Type = GetTypeString();

    if(const TRIMMED_CURVE* pTrimmedCurve = dynamic_cast<const TRIMMED_CURVE*>(this))
    {
        Type += " (";
        Type += pTrimmedCurve->basis_curve->GetExpandedTypeString();
        Type += ")";
    }
    else if(const SP_CURVE* pSP_Curve = dynamic_cast<const SP_CURVE*>(this))
    {
        Type += " (";
        Type += pSP_Curve->surface->GetExpandedTypeString();
        Type += ")";
    }
    else if(const INTERSECTION* pIntersection = dynamic_cast<const INTERSECTION*>(this))
    {
        Type += " (";
        Type += pIntersection->surface[0]->GetExpandedTypeString();
        Type += ", ";
        Type += pIntersection->surface[1]->GetExpandedTypeString();
        Type += ")";
    }
    else if(const OFFSET_SURF* pOffsetSurface = dynamic_cast<const OFFSET_SURF*>(this))
    {
        Type += " (";
        Type += pOffsetSurface->surface->GetExpandedTypeString();
        Type += ")";
    }
    else if(const SWEPT_SURF* pSweptSurface = dynamic_cast<const SWEPT_SURF*>(this))
    {
        Type += " (";
        Type += pSweptSurface->section->GetExpandedTypeString();
        Type += ")";
    }
    else if(const SPUN_SURF* pSpunSurface = dynamic_cast<const SPUN_SURF*>(this))
    {
        Type += " (";
        Type += pSpunSurface->profile->GetExpandedTypeString();
        Type += ")";
    }
    else if(const BLENDED_EDGE* pBlendedEdge = dynamic_cast<const BLENDED_EDGE*>(this))
    {
        Type += " (";
        Type += pBlendedEdge->surface[0]->GetExpandedTypeString();
        Type += ", ";
        Type += pBlendedEdge->surface[1]->GetExpandedTypeString();
        Type += ", ";
        Type += pBlendedEdge->spine->GetExpandedTypeString();
        Type += ")";
    }
    else if(const BLEND_BOUND* pBlendBound = dynamic_cast<const BLEND_BOUND*>(this))
    {
        Type += " (";
        Type += pBlendBound->blend->GetExpandedTypeString();
        Type += ")";
    }

    return Type;
}

NODE* NODE::CreateFromType(int iType)
{
    switch(iType)
    {
    case  10: return new ASSEMBLY;
    case  11: return new INSTANCE;
    case  12: return new BODY;
    case  13: return new SHELL;
    case  14: return new FACE;
    case  15: return new LOOP;
    case  16: return new EDGE;
    case  17: return new FIN;
    case  18: return new VERTEX;
    case  19: return new REGION;
    case  29: return new POINT;
    case  30: return new LINE;
    case  31: return new CIRCLE;
    case  32: return new ELLIPSE;
    case  38: return new INTERSECTION;
    case  40: return new CHART;
    case  41: return new LIMIT;
    case  45: return new BSPLINE_VERTICES;
    case  50: return new PLANE;
    case  51: return new CYLINDER;
    case  52: return new CONE;
    case  53: return new SPHERE;
    case  54: return new TORUS;
    case  56: return new BLENDED_EDGE;
    case  59: return new BLEND_BOUND;
    case  60: return new OFFSET_SURF;
    case  67: return new SWEPT_SURF;
    case  68: return new SPUN_SURF;
    case  70: return new LIST;
    case  74: return new POINTER_LIS_BLOCK;
    case  79: return new ATT_DEF_ID;
    case  80: return new ATTRIB_DEF;
    case  81: return new ATTRIBUTE;
    case  82: return new INT_VALUES;
    case  83: return new REAL_VALUES;
    case  84: return new CHAR_VALUES;
    case  85: return new POINT_VALUES;
    case  86: return new VECTOR_VALUES;
    case  87: return new AXIS_VALUES;
    case  88: return new TAG_VALUES;
    case  89: return new DIRECTION_VALUES;
    case  90: return new GROUP;
    case  91: return new MEMBER_OF_GROUP;
    case  98: return new UNICODE_VALUES;
    case  99: return new FIELD_NAMES;
    case 100: return new TRANSFORM;
    case 101: return new WORLD;
    case 102: return new KEY;
    case 120: return new PE_SURF;
    case 121: return new INT_PE_DATA;
    case 122: return new EXT_PE_DATA;
    case 124: return new B_SURFACE;
    case 125: return new SURFACE_DATA;
    case 126: return new NURBS_SURF;
    case 127: return new KNOT_MULT;
    case 128: return new KNOT_SET;
    case 130: return new PE_CURVE;
    case 133: return new TRIMMED_CURVE;
    case 134: return new B_CURVE;
    case 135: return new CURVE_DATA;
    case 136: return new NURBS_CURVE;
    case 137: return new SP_CURVE;
    case 141: return new GEOMETRIC_OWNER;
    case 184: return new HELIX_CU_FORM;
    }

    char Buf[32];
    sprintf(Buf, "%d", iType);

    throw TException((std::string)"Unsupported node type " + Buf);
}

NODE* NODE::Load(T_XT_Reader& Reader)
{
    // Loading node type
    int iType = Reader.ReadInt();

    if(iType == 1) // terminator
    {
        if(Reader.ReadInt()) // checking that next integer is 0
            throw TException("Invalid 1 0 terminator node");

        return NULL; // returning terminator node
    }

    // Creating node from type and putting it into ptr-holder to avoid memory leak
    // in case 'LoadData' throws an exception
    TPtrHolder<NODE> pNode(CreateFromType(iType));

    // Loading node fields (virtual call)
    pNode->LoadData(Reader);

    // Passing pointer ownership back to caller
    return pNode.Extract();
}

void NODE::LoadData(T_XT_Reader& Reader)
{
    // Loading node index
    index = Reader.ReadInt();

    // Asserting node index is non-zero
    if(!index)
        throw TException("Zero index detected");
}

// -------------
// NODE_WITH_ID
// -------------
void NODE_WITH_ID::LoadData(T_XT_Reader& Reader)
{
    // Loading base class fields
    NODE::LoadData(Reader);

    // Loading NODE ID
    Reader.ReadInt(m_iID);
}

// -----------
// Renderable
// -----------
void RENDERABLE::Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth) const
{
    if(szDepth > 100)
        throw TException("Renderable node depth is over 100.");
}

// ----------
// XT reader
// ----------
T_XT_Reader::T_XT_Reader()
{
    m_pFile = NULL;

    ResetLine();
}

void T_XT_Reader::ResetLine()
{
    m_Line[0] = 0;

    m_pCur = m_Line;
}

bool T_XT_Reader::ReadLine()
{
    assert(m_pFile);

    for(;;)
    {
        // Reseting current line
        m_Line[0] = 0;

        if(feof(m_pFile))
            return false; // end of file reached

        // Reading next line
        char* _none = fgets(m_Line, sizeof(m_Line), m_pFile); // avoiding 'warn_unused_result'

        // Removing trailing ' ', '\r', '\n'
        size_t l = strlen(m_Line);

        while(l && strchr(" \r\n", m_Line[l-1]))
            m_Line[--l] = 0;

        if(l) // non-empty line was read, stopping. otherwise will read next one
            break;
    }

    // Setting in-line pointer to its first character
    m_pCur = m_Line;

    return true;
}

void T_XT_Reader::ReadNumericToken(char* s, size_t n)
{
    char* p = s; // current character pointer

    for(;;)
    {
        char c = InternalReadChar(true); // reading character, allowing EOF

        if(c == ' ' || c == EOF)
            break;

        if(n <= 1) // too many characters before first space
            throw TException("Token is too long");

        // Writing read character into buffer (s;n)
        *p++ = c, n--;

        if(c == '?') // single question mark is special case for numeric tokens
            break;
    }

    *p = 0; // null-terminating read token

    if(p == s) // checking for empty token (e.g. two successive spaces or preliminary end of file)
        throw TException("Empty token detected");
}

void T_XT_Reader::ReadTextHeader()
{
    // Reading lines until line beginning with **END_OF_HEADER** is detected
    for(;;)
    {
        if(!ReadLine())
            throw TException("Unable to reach end of text header");

        if(!memcmp(m_Line, "**END_OF_HEADER**", 17))
            break;
    }

    // Resetting current line. File should not point to format header
    ResetLine();
}

void T_XT_Reader::ReadFormatHeader()
{
    // Ensuring first character is 'T' (text format_)
    if(ReadChar() != 'T')
        throw TException("Non-text file detected");

    ReadString(); // modeler version

    std::string ver = ReadString(); // schema version

    if(ver.length() < 6 || ver.substr(ver.length() - 6, 6) != "_12006")
        throw TException("Schema version must end with \"_12006\"");

    ReadInt(); // user field size
}

void T_XT_Reader::ReadByte(byte& v)
{
    int w = ReadInt();

    if(w < 0 || w > UCHAR_MAX)
        throw TException("Byte value out of range");

    v = (byte)w;
}

void T_XT_Reader::ReadShort(short& v)
{
    int w = ReadInt();

    if(w < SHRT_MIN || w > SHRT_MAX)
        throw TException("Short value out of range");

    v = (short)w;
}

void T_XT_Reader::ReadInt(int& v)
{
    char buf[64];

    ReadNumericToken(buf, sizeof(buf));

    if(!buf[0])
        throw TException("Unexpected space or end-of-file instead of integer value");

    v = !strcmp(buf, "?") ? PARASOLID_UNDEF_INT : atoi(buf);
}

void T_XT_Reader::ReadReal(double& v)
{
    char buf[64];

    ReadNumericToken(buf, sizeof(buf));

    if(!buf[0])
        throw TException("Unexpected space or end-of-file instead of real value");

    v = !strcmp(buf, "?") ? PARASOLID_UNDEF_REAL : atof(buf);
}

void T_XT_Reader::ReadLogical(logical& v)
{
    char c = ReadChar();

    switch(c)
    {
    case '?':
        v = log_undef;
        break;

    case 'F':
        v = log_false;
        break;

    case 'T':
        v = log_true;
        break;

    default:
        {
            char buf[2] = {c, 0};
            throw TException((std::string)"Unrecognized boolean value '" + buf + "'");
        }
    }
}

void T_XT_Reader::ReadString(std::string& s)
{
    // Reading length
    byte l = ReadByte();

    // Reading characters
    char buf[0x100];

    for(size_t i = 0 ; i < l ; i++)
        buf[i] = ReadChar();

    buf[l] = 0;

    s = buf;
}

void T_XT_Reader::ReadVector(vector& v)
{
    v.x = ReadReal();

    if(v.x == PARASOLID_UNDEF_REAL) // text-mode only special case
    {
        v.y = PARASOLID_UNDEF_REAL;
        v.z = PARASOLID_UNDEF_REAL;
    }
    else
    {
        v.y = ReadReal();
        v.z = ReadReal();
    }
}

void T_XT_Reader::ReadInterval(interval& i)
{
    i.low   = ReadReal();
    i.high  = ReadReal();
}

void T_XT_Reader::ReadBox(box& b)
{
    ReadInterval(b.x);
    ReadInterval(b.y);
    ReadInterval(b.z);
}

void T_XT_Reader::ReadFile(const char* pFileName, NODES& RNodes, TNodeIndexMap& RNodeIndexMap, double dMaxAngle)
{
    if(!(m_pFile = fopen(pFileName, "rt")))
        throw TException(std::string("Error opening '") + pFileName + "'");

    // Clearing nodes list and index map
    RNodes.clear();

    RNodeIndexMap.clear();

    try
    {
        // Reading text header
        ReadTextHeader();

        // Reading format header
        ReadFormatHeader();

        for(;;)
        {
            // Loading next node
            NODE* pNode = NODE::Load(*this);

            if(!pNode) // termiantor node
                break;

            // Adding empty TPtrHolder and assigning it a value of 'pNode'.
            // RNodes.push_back(pNode) is not an option because it will
            // invoke copy constructor on temporary TPtrHolder and eventually
            // 'CreateCopy' call on pNode.
            RNodes.push_back(NULL), *RNodes.rbegin() = pNode;

            // Checking for duplicate node index
            if(RNodeIndexMap.find(pNode->GetIndex()) != RNodeIndexMap.end())
                throw TException("Duplicate node index detected");

            // Registering node in index map
            RNodeIndexMap.insert(TNodeIndexMap::value_type(pNode->GetIndex(), pNode));
        }

        // Resolving pointers of all read nodes
        for(NODES::iterator i = RNodes.begin() ; i != RNodes.end() ; ++i)
            (*i)->ResolvePtrs(RNodeIndexMap);

        // Preparing all read nodes
        for(NODES::iterator i = RNodes.begin() ; i != RNodes.end() ; ++i)
            (*i)->Prepare(dMaxAngle);
    }

    catch(...)
    {
        fclose(m_pFile), m_pFile = NULL;
        RNodes.clear();
        RNodeIndexMap.clear();
        throw;
    }
}

// ----------
// ANY_CURVE
// ----------
void ANY_CURVE::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (owner);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (geometric_owner);
    Reader.ReadChar (sense);
}

void ANY_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, geometric_owner);
}

// ---------
// ANY_SURF
// ---------
void ANY_SURF::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (owner);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (geometric_owner);
    Reader.ReadChar (sense);
}

void ANY_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, geometric_owner);
}

// --------------------
// Curve approximation
// --------------------
void TCurveApproximation::Build(const ANY_CURVE* pCurve)
{
    Clear();

    for(TParamValues::const_iterator i = pCurve->GetTValues().begin() ; i != pCurve->GetTValues().end() ; ++i)
        AddPoint(*i, pCurve->GetRealXYZ(*i), pCurve->GetRealNoSenseTangent(*i, -1), pCurve->GetRealNoSenseTangent(*i, +1));
}

vector TCurveApproximation::GetXYZ(double t) const
{
    TApproxParamValues::const_iterator t_citer = m_t.lower_bound(t);
    TApproxParamValues::const_iterator t_piter;

    unsigned i = t_citer->second;

    if(i)
        --i, --(t_piter = t_citer);
    else
        t_piter = t_citer++;

    return m_p[i] + (m_p[i+1] - m_p[i]) * ((t - t_piter->first) / (t_citer->first - t_piter->first));
}

vector TCurveApproximation::GetTangent(double t, int t_bias) const
{
    TApproxParamValues::const_iterator t_citer = m_t.lower_bound(t);

    unsigned i = t_citer->second;

    vector tan;

    if(!Compare(t, t_citer->first))
    {
        return m_tan[t_bias >= 0][i];
    }
    else
    {
        TApproxParamValues::const_iterator t_piter;

        --i, --(t_piter = t_citer);

        tan = m_tan[1][i] + (m_tan[0][i+1] - m_tan[1][i]) * ((t - t_piter->first) / (t_citer->first - t_piter->first));
    }

    return tan.Normalized();
}

double TCurveApproximation::GetT(const vector& p) const
{
    if(m_p.empty())
        return 0.0;

    double md = DBL_MAX;
    double mt;

    size_t i = 0;

    TApproxParamValues::const_iterator t_citer, t_niter;

    for(StartTwoIters(m_t, t_citer, t_niter) ; t_citer != m_t.end() ; IncTwoIters(m_t, t_citer, t_niter), i++)
    {
        // Checking vertex
        {
            double d = (p - m_p[i]).GetLengthSquare();

            if(d < md)
                md = d, mt = t_citer->first;
        }

        // Checking edge
        if(t_niter != m_t.end())
        {
            double dt = t_niter->first - t_citer->first;
            vector dp = m_p[i+1] - m_p[i];

            double t = SafeDiv((p - m_p[i]) & dp, dp.GetLengthSquare());

            if(Compare(t, 0.0) >= 0 && Compare(t, 1.0) <= 0)
            {
                double d = (m_p[i] + dp * t - p).GetLengthSquare();

                if(d < md)
                    md = d, mt = t_citer->first + dt * t;
            }
        }
    }

    return mt;
}

// ----------------------
// Surface approximation
// ----------------------
void TSurfaceApproximation::Build(const ANY_SURF* pSurface)
{
    m_u.clear();
    m_v.clear();
    m_p.clear();
    m_norm[0][0].clear();
    m_norm[0][1].clear();
    m_norm[1][0].clear();
    m_norm[1][1].clear();

    for(TParamValues::const_iterator i = pSurface->GetUValues().begin() ; i != pSurface->GetUValues().end() ; ++i)
        m_u.insert(std::map<double, unsigned>::value_type(*i, m_u.size()));

    for(TParamValues::const_iterator j = pSurface->GetVValues().begin() ; j != pSurface->GetVValues().end() ; ++j)
        m_v.insert(std::map<double, unsigned>::value_type(*j, m_v.size()));

    // Point coords and normals
    {
        m_p.            resize(m_u.size() * m_v.size());
        m_norm[0][0].   resize(m_u.size() * m_v.size());
        m_norm[0][1].   resize(m_u.size() * m_v.size());
        m_norm[1][0].   resize(m_u.size() * m_v.size());
        m_norm[1][1].   resize(m_u.size() * m_v.size());

        unsigned k = 0;

        for(TApproxParamValues::const_iterator u_iter = m_u.begin() ; u_iter != m_u.end() ; ++u_iter)
        {
            for(TApproxParamValues::const_iterator v_iter = m_v.begin() ; v_iter != m_v.end() ; ++v_iter, ++k)
            {
                m_p[k] = pSurface->GetRealXYZ(T_UV_Point(u_iter->first, v_iter->first));

                m_norm[0][0][k] = pSurface->GetRealNoSenseNormal(T_UV_Point(u_iter->first, v_iter->first), -1, -1);
                m_norm[0][1][k] = pSurface->GetRealNoSenseNormal(T_UV_Point(u_iter->first, v_iter->first), -1, +1);
                m_norm[1][0][k] = pSurface->GetRealNoSenseNormal(T_UV_Point(u_iter->first, v_iter->first), +1, -1);
                m_norm[1][1][k] = pSurface->GetRealNoSenseNormal(T_UV_Point(u_iter->first, v_iter->first), +1, +1);
            }
        }
    }

    // Triangle backtransform matrices
    {
        m_trans1.resize(m_u.size() * m_v.size());
        m_trans2.resize(m_u.size() * m_v.size());

        unsigned k = 0;

        TApproxParamValues::const_iterator u_citer, u_niter;

        for(StartTwoIters(m_u, u_citer, u_niter) ; u_citer != m_u.end() ; IncTwoIters(m_u, u_citer, u_niter))
        {
            TApproxParamValues::const_iterator v_citer, v_niter;

            for(StartTwoIters(m_v, v_citer, v_niter) ; v_citer != m_v.end() ; IncTwoIters(m_v, v_citer, v_niter), k++)
            {
                if(u_niter == m_u.end() || v_niter == m_v.end())
                    continue;

                // Triangle 1
                {
                    vector u_axis = m_p[k + m_v.size()] - m_p[k];
                    vector v_axis = m_p[k + 1]          - m_p[k];
                    vector d_axis = (u_axis * v_axis).Normalized();

                    if(u_axis.IsZero() || v_axis.IsZero() || d_axis.IsZero())
                        m_trans1[k].SetZero();
                    else
                        m_trans1[k].SetBasis(u_axis, v_axis, d_axis).Inverse();
                }

                // Triangle 2
                {
                    vector u_axis = m_p[k + 1]          - m_p[k + m_v.size() + 1];
                    vector v_axis = m_p[k + m_v.size()] - m_p[k + m_v.size() + 1];
                    vector d_axis = (u_axis * v_axis).Normalized();

                    if(u_axis.IsZero() || v_axis.IsZero() || d_axis.IsZero())
                        m_trans2[k].SetZero();
                    else
                        m_trans2[k].SetBasis(u_axis, v_axis, d_axis).Inverse();
                }
            }
        }
    }
}

vector TSurfaceApproximation::GetXYZ(const T_UV_Point& uv) const
{
    if(m_p.empty())
        return vector(0, 0, 0);

    TApproxParamValues::const_iterator u_citer = m_u.lower_bound(uv.u);
    TApproxParamValues::const_iterator u_piter;

    unsigned i = u_citer->second;

    if(i)
        --i, --(u_piter = u_citer);
    else
        u_piter = u_citer++;

    TApproxParamValues::const_iterator v_citer = m_v.lower_bound(uv.v);
    TApproxParamValues::const_iterator v_piter;

    unsigned j = v_citer->second;

    if(j)
        --j, --(v_piter = v_citer);
    else
        v_piter = v_citer++;

    size_t k = i * m_v.size() + j;

    double ut = (uv.u - u_piter->first) / (u_citer->first - u_piter->first);
    double vt = (uv.v - v_piter->first) / (v_citer->first - v_piter->first);

    if(Compare(ut + vt, 1.0) <= 0)
        return m_p[k] * (1 - ut - vt) + m_p[k + m_v.size()] * ut + m_p[k + 1] * vt;
    else
        return m_p[k + m_v.size() + 1] * (ut + vt - 1) + m_p[k + 1] * (1 - ut) + m_p[k + m_v.size()] * (1 - vt);
}

vector TSurfaceApproximation::GetNormal(const T_UV_Point& uv, int u_bias, int v_bias) const
{
    if(m_p.empty())
        return vector(0, 0, 0);

    TApproxParamValues::const_iterator u_citer = m_u.lower_bound(uv.u);
    unsigned i = u_citer->second;

    TApproxParamValues::const_iterator v_citer = m_v.lower_bound(uv.v);
    unsigned j = v_citer->second;

    if(!Compare(uv.u, u_citer->first) && !Compare(uv.v, v_citer->first))
    {
        unsigned k = i * m_v.size() + j;

        return m_norm[u_bias >= 0][v_bias >= 0][k];
    }

    if(!Compare(uv.u, u_citer->first))
    {
        TApproxParamValues::const_iterator v_piter;

        --j, --(v_piter = v_citer);

        unsigned k = i * m_v.size() + j;

        return (m_norm[u_bias >= 0][1][k] +
                    (m_norm[u_bias >= 0][0][k + 1] - m_norm[u_bias >= 0][1][k]) *
                        ((uv.v - v_piter->first) / (v_citer->first - v_piter->first))).Normalized();
    }

    if(!Compare(uv.v, v_citer->first))
    {
        TApproxParamValues::const_iterator u_piter;

        --i, --(u_piter = u_citer);

        unsigned k = i * m_v.size() + j;

        return (m_norm[1][v_bias >= 0][k] +
                (m_norm[0][v_bias >= 0][k + m_v.size()] - m_norm[1][v_bias >= 0][k]) *
                    ((uv.u - u_piter->first) / (u_citer->first - u_piter->first))).Normalized();
    }

    {
        TApproxParamValues::const_iterator u_piter;
        TApproxParamValues::const_iterator v_piter;

        --i, --(u_piter = u_citer);
        --j, --(v_piter = v_citer);

        unsigned k = i * m_v.size() + j;

        double ut = (uv.u - u_piter->first) / (u_citer->first - u_piter->first);
        double vt = (uv.v - v_piter->first) / (v_citer->first - v_piter->first);

        if(Compare(ut + vt, 1.0) <= 0)
            return (m_norm[1][1][k] * (1 - ut - vt) + m_norm[0][1][k + m_v.size()] * ut + m_norm[1][0][k + 1] * vt).Normalized();
        else
            return (m_norm[0][0][k + m_v.size() + 1] * (ut + vt - 1) + m_norm[1][0][k + 1] * (1 - ut) + m_norm[0][1][k + m_v.size()] * (1 - vt)).Normalized();
    }
}

T_UV_Point TSurfaceApproximation::GetUV(const vector& p) const
{
    if(m_p.empty())
        return T_UV_Point(0, 0);

    double md = DBL_MAX;
    T_UV_Point muv;

    unsigned k = 0;

    TApproxParamValues::const_iterator u_citer, u_niter;

    for(StartTwoIters(m_u, u_citer, u_niter) ; u_citer != m_u.end() ; IncTwoIters(m_u, u_citer, u_niter))
    {
        TApproxParamValues::const_iterator v_citer, v_niter;

        for(StartTwoIters(m_v, v_citer, v_niter) ; v_citer != m_v.end() ; IncTwoIters(m_v, v_citer, v_niter), k++)
        {
            // Vertex
            {
                double d = (p - m_p[k]).GetLengthSquare();

                if(d < md)
                    md = d, muv = T_UV_Point(u_citer->first, v_citer->first);
            }

            // U edge
            if(u_niter != m_u.end())
            {
                double du = u_niter->first - u_citer->first;

                vector dp = m_p[k + m_v.size()] - m_p[k];

                double t = SafeDiv((p - m_p[k]) & dp, dp.GetLengthSquare());

                if(Compare(t, 0.0) >= 0 && Compare(t, 1.0) <= 0)
                {
                    double d = (m_p[k] + dp * t - p).GetLengthSquare();

                    if(d < md)
                        md = d, muv = T_UV_Point(u_citer->first + du * t, v_citer->first);
                }
            }

            // V edge
            if(v_niter != m_v.end())
            {
                double dv = v_niter->first - v_citer->first;

                vector dp = m_p[k + 1] - m_p[k];

                double t = SafeDiv((p - m_p[k]) & dp, dp.GetLengthSquare());

                if(Compare(t, 0.0) >= 0 && Compare(t, 1.0) <= 0)
                {
                    double d = (m_p[k] + dp * t - p).GetLengthSquare();

                    if(d < md)
                        md = d, muv = T_UV_Point(u_citer->first, v_citer->first + dv * t);
                }
            }

            // UV edge
            if(u_niter != m_u.end() && v_niter != m_v.end())
            {
                double du = u_citer->first - u_niter->first;
                double dv = v_niter->first - v_citer->first;

                vector dp = m_p[k + 1] - m_p[k + m_v.size()];

                double t = SafeDiv((p - m_p[k + m_v.size()]) & dp, dp.GetLengthSquare());

                if(Compare(t, 0.0) >= 0 && Compare(t, 1.0) <= 0)
                {
                    double d = (m_p[k + m_v.size()] + dp * t - p).GetLengthSquare();

                    if(d < md)
                        md = d, muv = T_UV_Point(u_niter->first + du * t, v_citer->first + dv * t);
                }
            }

            // UV quadrant
            if(u_niter != m_u.end() && v_niter != m_v.end())
            {
                // Triangle 1
                if(!m_trans1[k].IsZero())
                {
                    vector tp = m_trans1[k] * (p - m_p[k]);

                    if(Compare(tp.x, 0.0) >= 0 && Compare(tp.y, 0.0) >= 0 && Compare(tp.x + tp.y, 1.0) <= 0 && tp.z * tp.z < md)
                    {
                        md = tp.z * tp.z;

                        muv = T_UV_Point(   u_citer->first + (u_niter->first - u_citer->first) * tp.x,
                                            v_citer->first + (v_niter->first - v_citer->first) * tp.y);
                    }
                }

                // Triangle 2
                if(!m_trans2[k].IsZero())
                {
                    vector tp = m_trans2[k] * (p - m_p[k + m_v.size() + 1]);

                    if(Compare(tp.x, 0.0) >= 0 && Compare(tp.y, 0.0) >= 0 && Compare(tp.x + tp.y, 1.0) <= 0 && tp.z * tp.z < md)
                    {
                        md = tp.z * tp.z;

                        muv = T_UV_Point(   u_niter->first + (u_citer->first - u_niter->first) * tp.x,
                                            v_niter->first + (v_citer->first - v_niter->first) * tp.y);
                    }
                }
            }
        }
    }

    return muv;
}

// -----
// LINE
// -----
void LINE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadVector(pvec);
    Reader.ReadVector(direction);
}

// -------
// CIRCLE
// -------
void CIRCLE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadVector   (centre);
    Reader.ReadVector   (normal);
    Reader.ReadVector   (x_axis);
    Reader.ReadReal     (radius);
}

bool CIRCLE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    y_axis = normal * x_axis;

    inv_transform.SetBasis(x_axis, y_axis, normal).Inverse();

    for(double t = 0 ; Compare(t, g_dPI * 2) <= 0 ; t += dMaxAngle)
        t_values.insert(t);

    return true;
}

// --------
// ELLIPSE
// --------
void ELLIPSE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadVector   (centre);
    Reader.ReadVector   (normal);
    Reader.ReadVector   (x_axis);
    Reader.ReadReal     (major_radius);
    Reader.ReadReal     (minor_radius);
}

bool ELLIPSE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    y_axis = normal * x_axis;

    inv_transform.SetBasis(x_axis * minor_radius, y_axis * major_radius, normal).Inverse();

    std::vector<double> v;

    v.push_back(0);

    for(double t = dMaxAngle ; Compare(t, g_dPI * 0.5) < 0 ; t += dMaxAngle)
        v.push_back(atan(minor_radius / (major_radius * tan(t))));

    v.push_back(g_dPI * 0.5);

    for(size_t i = 0 ; i < v.size() ; i++)
    {
        t_values.insert(v[i]);
        t_values.insert(g_dPI - v[i]);
        t_values.insert(g_dPI + v[i]);
        t_values.insert(g_dPI * 2.0 - v[i]);
    }

    return true;
}

// ------------
// NURBS_CURVE
// ------------
void NURBS_CURVE::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadShort    (degree);
    Reader.ReadInt      (n_vertices);
    Reader.ReadShort    (vertex_dim);
    Reader.ReadInt      (n_knots);
    Reader.ReadEnum     (knot_type);
    Reader.ReadLogical  (periodic);
    Reader.ReadLogical  (closed);
    Reader.ReadLogical  (rational);
    Reader.ReadEnum     (curve_form);
    Reader.ReadPtr      (bspline_vertices);
    Reader.ReadPtr      (knot_mult);
    Reader.ReadPtr      (knots);
}

void NURBS_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, bspline_vertices);
    ResolvePtr(Map, knot_mult);
    ResolvePtr(Map, knots);
}

bool NURBS_CURVE::InternalPrepare(double dMaxAngle)
{
    if(!NODE::InternalPrepare(dMaxAngle))
        return false;

    if( !knots              || !knots->             Prepare(dMaxAngle) ||
        !knot_mult          || !knot_mult->         Prepare(dMaxAngle) ||
        !bspline_vertices   || !bspline_vertices->  Prepare(dMaxAngle))
    {
        return false;
    }

    if(!closed && periodic)
        return false;

    if(degree < 1 || degree > 7)
        return false;

    return true;
}

// -----------------
// BSPLINE_VERTICES
// -----------------
void BSPLINE_VERTICES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    vertices.reserve(n);

    for( ; n ; n--)
        vertices.push_back(Reader.ReadReal());
}

// ---------
// KNOT_SET
// ---------
void KNOT_SET::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    knots.reserve(n);

    for( ; n ; n--)
        knots.push_back(Reader.ReadReal());
}

// ----------
// KNOT_MULT
// ----------
void KNOT_MULT::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    mult.reserve(n);

    for( ; n ; n--)
        mult.push_back(Reader.ReadShort());
}

// -----------
// CURVE_DATA
// -----------
void CURVE_DATA::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadEnum (self_int);
    Reader.ReadPtr  (analytic_form);
}

void CURVE_DATA::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, analytic_form);
}

// --------------
// HELIX_CU_FORM
// --------------
void HELIX_CU_FORM::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadVector   (axis_pt);
    Reader.ReadVector   (axis_dir);
    Reader.ReadVector   (point);
    Reader.ReadChar     (hand);
    Reader.ReadInterval (turns);
    Reader.ReadReal     (pitch);
    Reader.ReadReal     (tol);
}

// --------
// B_CURVE
// --------
void B_CURVE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadPtr(nurbs);
    Reader.ReadPtr(data);
}

void B_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_CURVE::ResolvePtrs(Map);

    ResolvePtr(Map, nurbs);
    ResolvePtr(Map, data);
}

void B_CURVE::ParseVertex(unsigned i, vector& v, double& w) const
{
    const double* c = &nurbs->bspline_vertices->vertices[nurbs->vertex_dim * i];

    v.Set(0, 0, 0);

    if(nurbs->rational)
    {
        if(nurbs->vertex_dim >= 2)
        {
            v.x = c[0];

            if(nurbs->vertex_dim >= 3)
            {
                v.y = c[1];

                if(nurbs->vertex_dim >= 4)
                    v.z = c[2];
            }
        }

        w = c[nurbs->vertex_dim - 1];
    }
    else
    {
        if(nurbs->vertex_dim >= 1)
        {
            v.x = c[0];

            if(nurbs->vertex_dim >= 2)
            {
                v.y = c[1];

                if(nurbs->vertex_dim >= 3)
                    v.z = c[2];
            }
        }

        w = 1.0;
    }
}

bool B_CURVE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    if(!nurbs || !nurbs->Prepare(dMaxAngle))
        return false;

    // Removing zero-mult knots
    if(nurbs->knots->knots.size() != nurbs->knot_mult->mult.size())
        return false;

    for(size_t i = 0 ; i < nurbs->knots->knots.size() ; i++)
    {
        if(!nurbs->knot_mult->mult[i])
        {
            nurbs->knots->    knots.erase(nurbs->knots->    knots.begin() + i);
            nurbs->knot_mult->mult. erase(nurbs->knot_mult->mult. begin() + i);

            i--;
        }
    }

    // Asserting data integrity
    if( nurbs->bspline_vertices->vertices.size() != nurbs->n_vertices * nurbs->vertex_dim ||
        nurbs->knots->    knots.size() != nurbs->n_knots ||
        nurbs->knot_mult->mult. size() != nurbs->n_knots)
    {
        return false;
    }

    // Expanding knots
    for(int i = 0 ; i < nurbs->n_knots ; i++)
        exp_t_knots.insert(exp_t_knots.end(), nurbs->knot_mult->mult[i], nurbs->knots->knots[i]);

    if(nurbs->n_vertices != exp_t_knots.size() - nurbs->degree - 1)
        return false;

    unsigned uSubdivPrecision = (unsigned)Ceil(g_dPI / dMaxAngle);

    double dMaxAngCos = cos(dMaxAngle);

    // Filling valid T knots and segmentation
    for(int i = nurbs->degree ; i < nurbs->n_vertices ; i++)
    {
        if(exp_t_knots[i] == exp_t_knots[i+1])
            continue;

        valid_t_segs.insert(std::map<double, unsigned>::value_type(exp_t_knots[i+1], i - nurbs->degree));

        if(GetDimensions() == 2)
        {
            t_values.insert(exp_t_knots[i]);
        }
        else
        {
            vector PrevTangent, CurTangent;

            double dTStep = (exp_t_knots[i+1] - exp_t_knots[i]) / uSubdivPrecision;

            for(unsigned ii = 0 ; ii < uSubdivPrecision ; ii++)
            {
                CurTangent = GetXYZdt(exp_t_knots[i] + ii * dTStep, i - nurbs->degree).Normalized();

                if(!ii || PrevTangent.IsZero() || CurTangent.IsZero() || Compare(PrevTangent & CurTangent, dMaxAngCos) <= 0)
                    t_values.insert(exp_t_knots[i] + ii * dTStep), PrevTangent = CurTangent;
            }
        }
    }

    t_values.insert(exp_t_knots[nurbs->n_vertices]);

    return true;
}

void B_CURVE::InternalPostPrepare()
{
    ANY_CURVE::InternalPostPrepare();

    approx.Build(this);
}

vector B_CURVE::GetRealXYZ(double t) const
{
    if(valid_t_segs.empty()) // safety
        return vector(0, 0, 0);

    t = NormalizeT(t);

    TApproxParamValues::const_iterator iter = valid_t_segs.lower_bound(t);

    size_t bi = iter->second;

    vector cv[8];
    double cw[8];

    for(int i = 0 ; i <= nurbs->degree ; i++)
        ParseVertex(bi + i, cv[i], cw[i]);

    // Transforming (0) vector to (deg) vector
    for(int d = 1 ; d <= nurbs->degree ; d++)
    {
        for(int i = nurbs->degree ; i >= d ; i--)
        {
            double c = SafeDiv( t - exp_t_knots[bi + i],
                                exp_t_knots[bi + i + nurbs->degree + 1 - d] - exp_t_knots[bi + i]);

            cv[i] = cv[i-1] + (cv[i] - cv[i-1]) * c;
            cw[i] = cw[i-1] + (cw[i] - cw[i-1]) * c;
        }
    }

    vector v = cv[nurbs->degree];
    double w = cw[nurbs->degree];

    return nurbs->rational && w ? v / w : v;
}

vector B_CURVE::GetXYZdt(double t, size_t bi) const
{
    vector cv[8];
    double cw[8];

    vector cvdt[8];
    double cwdt[8];

    for(int i = 0 ; i <= nurbs->degree ; i++)
    {
        ParseVertex(bi + i, cv[i], cw[i]);

        cvdt[i] = vector(0, 0, 0);
        cwdt[i] = 0;
    }

    for(int d = 1 ; d <= nurbs->degree ; d++)
    {
        for(int i = nurbs->degree ; i >= d ; i--)
        {
            double cd = SafeDiv(1, exp_t_knots[bi + i + nurbs->degree + 1 - d] - exp_t_knots[bi + i]);

            double c = (t - exp_t_knots[bi + i]) * cd;

            cvdt[i] = cvdt[i-1] + (cvdt[i] - cvdt[i-1]) * c + (cv[i] - cv[i-1]) * cd;
            cwdt[i] = cwdt[i-1] + (cwdt[i] - cwdt[i-1]) * c + (cw[i] - cw[i-1]) * cd;

            cv[i] = cv[i-1] + (cv[i] - cv[i-1]) * c;
            cw[i] = cw[i-1] + (cw[i] - cw[i-1]) * c;
        }
    }

    vector v = cv[nurbs->degree];
    double w = cw[nurbs->degree];

    vector vdt = cvdt[nurbs->degree];
    double wdt = cwdt[nurbs->degree];

    return nurbs->rational ? vdt * w - v * wdt : vdt;
}

vector B_CURVE::GetRealNoSenseTangent(double t, int t_bias) const
{
    if(valid_t_segs.empty()) // safety
        return vector(0, 0, 0);

    t = NormalizeT(t);

    TApproxParamValues::const_iterator t_iter;

    if(!Compare(t, GetMinT()) && t_bias < 0 && GetTopology() >= TPL_PERIODIC)
    {
        --(t_iter = valid_t_segs.end()), t = GetMaxT();
    }
    else
    {
        t_iter = valid_t_segs.lower_bound(t);

        if(!Compare(t_iter->first, t) && t_bias >= 0)
        {
            if(++t_iter == valid_t_segs.end())
            {
                if(GetTopology() == TPL_PERIODIC)
                    t_iter = valid_t_segs.begin(), t = GetMinT();
                else
                    t_iter--;
            }
        }
    }

    return GetXYZdt(t, t_iter->second).Normalized();
}

// ------
// CHART
// ------
void CHART::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    Reader.ReadReal (base_parameter);
    Reader.ReadReal (base_scale);
    Reader.ReadInt  (chart_count);
    Reader.ReadReal (chordal_error);
    Reader.ReadReal (angular_error);
    Reader.ReadReal (parameter_error[0]);
    Reader.ReadReal (parameter_error[1]);

    vec.reserve(n);

    for( ; n ; n--)
        vec.push_back(Reader.ReadVector());
}

bool CHART::InternalPrepare(double dMaxAngle)
{
    if(!NODE::InternalPrepare(dMaxAngle))
        return false;

    if(vec.size() != chart_count)
        return false;

    if(chart_count < 2)
        return false;

    return true;
}

// ------
// LIMIT
// ------
void LIMIT::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    Reader.ReadChar(type);

    Hvec.reserve(n);

    for( ; n ; n--)
        Hvec.push_back(Reader.ReadVector());
}

// -------------
// INTERSECTION
// -------------
void INTERSECTION::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadPtr(surface[0]);
    Reader.ReadPtr(surface[1]);
    Reader.ReadPtr(chart);
    Reader.ReadPtr(start);
    Reader.ReadPtr(end);
}

void INTERSECTION::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_CURVE::ResolvePtrs(Map);

    ResolvePtr(Map, surface[0]);
    ResolvePtr(Map, surface[1]);
    ResolvePtr(Map, chart);
    ResolvePtr(Map, start);
    ResolvePtr(Map, end);
}

bool INTERSECTION::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    if( !surface[0] || !surface[0]->Prepare(dMaxAngle) ||
        !surface[1] || !surface[1]->Prepare(dMaxAngle) ||
        !chart      || !chart->     Prepare(dMaxAngle) ||
        !start      || !start->     Prepare(dMaxAngle) ||
        !end        || !end->       Prepare(dMaxAngle))
    {
        return false;
    }

    approx.Clear();

    double t  = chart->base_parameter;
    double f  = chart->base_scale;
    vector c  = chart->vec[1] - chart->vec[0];
    double cl = c.GetLength();

    vector tg;

    tg = (  surface[0]->GetApproxNormal(surface[0]->GetApproxUV(chart->vec[0])) *
            surface[1]->GetApproxNormal(surface[1]->GetApproxUV(chart->vec[0]))).Normalized();

    approx.AddPoint(t, chart->vec[0], tg, tg);

    for(int i = 1 ; ; i++)
    {
        double dt = cl * f;

        t += dt;

        tg = (  surface[0]->GetApproxNormal(surface[0]->GetApproxUV(chart->vec[i])) *
                surface[1]->GetApproxNormal(surface[1]->GetApproxUV(chart->vec[i]))).Normalized();

        if(Sign(dt))
            approx.AddPoint(t, chart->vec[i], tg, tg);

        if(i == chart->chart_count - 1)
            break;

        double cosb = fabs(SafeDiv(tg & c, cl));

        c  = chart->vec[i+1] - chart->vec[i];
        cl = c.GetLength();

        double cosa = fabs(SafeDiv(tg & c, cl));

        f *= SafeDiv(cosa, cosb);
    }

    for(TApproxParamValues::const_iterator iter = approx.GetTValues().begin() ; iter != approx.GetTValues().end() ; ++iter)
        t_values.insert(iter->first);

    return true;
}

// --------------
// TRIMMED_CURVE
// --------------
void TRIMMED_CURVE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadPtr      (basis_curve);
    Reader.ReadVector   (point_1);
    Reader.ReadVector   (point_2);
    Reader.ReadReal     (parm_1);
    Reader.ReadReal     (parm_2);
}

void TRIMMED_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_CURVE::ResolvePtrs(Map);

    ResolvePtr(Map, basis_curve);
}

bool TRIMMED_CURVE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    if(!basis_curve || !basis_curve->Prepare(dMaxAngle))
        return false;

    if(sense == '-')
        return false;

    sense = basis_curve->sense;

    if(Sign(parm_2 - parm_1) != (sense == '-' ? -1 : +1))
        return false;

    if(sense == '-')
    {
        std::swap(parm_1,  parm_2);
        std::swap(point_1, point_2);
    }

    double nt1 = basis_curve->NormalizeT(parm_1);
    double nt2 = basis_curve->NormalizeT(parm_2);

    t_values.insert(parm_1);

    if(basis_curve->GetTopology() >= TPL_PERIODIC)
    {
        double off = parm_1 - nt1;

        if(Compare(nt1, nt2) < 0)
        {
            for(TParamValues::const_iterator iter =
                    basis_curve->GetTValues().upper_bound(nt1) ;
                iter != basis_curve->GetTValues().end() && Compare(*iter, nt2) < 0 ;
                ++iter)
            {
                t_values.insert(off + *iter);
            }
        }
        else
        {
            for(TParamValues::const_iterator iter =
                    basis_curve->GetTValues().upper_bound(nt1) ;
                iter != basis_curve->GetTValues().end() && Compare(*iter, basis_curve->GetMaxT()) < 0 ;
                ++iter)
            {
                t_values.insert(off + *iter);
            }

            off += basis_curve->GetTLength();

            for(TParamValues::const_iterator iter =
                    basis_curve->GetTValues().begin() ;
                iter != basis_curve->GetTValues().end() && Compare(*iter, nt2) < 0 ;
                ++iter)
            {
                t_values.insert(off + *iter);
            }
        }
    }
    else
    {
        for(TParamValues::const_iterator iter =
                basis_curve->GetTValues().upper_bound(nt1) ;
            iter != basis_curve->GetTValues().end() && Compare(*iter, nt2) < 0 ;
            ++iter)
        {
            t_values.insert(*iter);
        }
    }

    t_values.insert(parm_2);

    return true;
}

double TRIMMED_CURVE::GetApproxT(const vector& xyz) const
{
    double t = basis_curve->GetApproxT(xyz);

    if(basis_curve->GetTopology() >= TPL_PERIODIC)
    {
        if((t = parm_1 + fmod(t - parm_1, basis_curve->GetTLength())) < parm_1)
            t += basis_curve->GetTLength();

        if(Compare(t, parm_2) > 0)
            t = parm_2;
        else if(!Compare(t, parm_2) && !Compare(parm_2 - parm_1, basis_curve->GetTLength()))
            t = parm_1;
    }
    else
    {
        t = NormalizeT(t);
    }

    return t;
}

// ---------
// PE_CURVE
// ---------
void PE_CURVE::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    ANY_CURVE::LoadData(Reader);

    Reader.ReadChar (type);
    Reader.ReadPtr  (data);
    Reader.ReadPtr  (tf);

    internal_geom.reserve(n);

    for( ; n ; n--)
        internal_geom.push_back(Reader.ReadPtr<PE_INT_GEOM>());
}

void PE_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_CURVE::ResolvePtrs(Map);

    ResolvePtr(Map, data);
    ResolvePtr(Map, tf);

    for(std::vector<PE_INT_GEOM*>::iterator i = internal_geom.begin() ; i != internal_geom.end() ; ++i)
        ResolvePtr(Map, *i);
}

// ------------
// EXT_PE_DATA
// ------------
void EXT_PE_DATA::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadPtr(key);
    Reader.ReadPtr(real_array);
    Reader.ReadPtr(int_array);
}

void EXT_PE_DATA::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, key);
    ResolvePtr(Map, real_array);
    ResolvePtr(Map, int_array);
}

// ------------
// INT_PE_DATA
// ------------
void INT_PE_DATA::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadInt(geom_type);
    Reader.ReadPtr(real_array);
    Reader.ReadPtr(int_array);
}

void INT_PE_DATA::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, real_array);
    ResolvePtr(Map, int_array);
}

// ---------
// SP_CURVE
// ---------
void SP_CURVE::LoadData(T_XT_Reader& Reader)
{
    ANY_CURVE::LoadData(Reader);

    Reader.ReadPtr  (surface);
    Reader.ReadPtr  (b_curve);
    Reader.ReadPtr  (original);
    Reader.ReadReal (tolerance_to_original);
}

void SP_CURVE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_CURVE::ResolvePtrs(Map);

    ResolvePtr(Map, surface);
    ResolvePtr(Map, b_curve);
    ResolvePtr(Map, original);
}

bool SP_CURVE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_CURVE::InternalPrepare(dMaxAngle))
        return false;

    if( !b_curve || !b_curve->Prepare(dMaxAngle) ||
        !surface || !surface->Prepare(dMaxAngle))
    {
        return false;
    }

    if(b_curve->GetDimensions() != 2)
        return false;

    if(b_curve->GetTValues().size() < 2)
        return false;

    topology =
        b_curve->GetTopology() >= TPL_CLOSED || GetRealXYZ(b_curve->GetMinT()) == GetRealXYZ(b_curve->GetMaxT()) ?
            TPL_PERIODIC : TPL_OPEN;

    unsigned uSubdivPrecision = (unsigned)Ceil(g_dPI * 4 / dMaxAngle);

    double dMaxAngCos = cos(dMaxAngle);

    TParamValues::const_iterator niter = b_curve->GetTValues().begin();
    TParamValues::const_iterator iter = niter++;

    for( ; niter != b_curve->GetTValues().end() ; iter = niter++)
    {
        vector PrevTangent, CurTangent;

        double dTStep = (*niter - *iter) / uSubdivPrecision;

        for(unsigned ii = 0 ; ii < uSubdivPrecision ; ii++)
        {
            CurTangent = (GetRealXYZ(*iter + dTStep * (ii + 1)) - GetRealXYZ(*iter + dTStep * ii)).Normalized();

            if(!ii || PrevTangent.IsZero() || CurTangent.IsZero() || Compare(PrevTangent & CurTangent, dMaxAngCos) <= 0)
                t_values.insert(*iter + ii * dTStep), PrevTangent = CurTangent;
        }
    }

    t_values.insert(*iter);

    return true;
}

void SP_CURVE::InternalPostPrepare()
{
    ANY_CURVE::InternalPostPrepare();

    approx.Build(this);
}

vector SP_CURVE::GetRealNoSenseTangent(double t, int t_bias) const
{
    t = NormalizeT(t);

    TParamValues::const_iterator iter = t_values.lower_bound(t);

    if(!Compare(t, *iter)) // hits knot
    {
        if(t_bias < 0) // (iter-1 ; iter)
        {
            if(iter == t_values.begin()) // t=mint, should either loop back or shift ahead
            {
                if(GetTopology() == TPL_PERIODIC) // looping back and adjusting 't'
                    t = *--(iter = t_values.end());
                else // shifting ahead
                    ++iter;
            }
        }
        else // (iter ; iter+1)
        {
            if(++iter == t_values.end()) // t=maxt, should either loop back or shift behind
            {
                if(GetTopology() == TPL_PERIODIC) // looping back and adjusting 't'
                    t = *(iter = t_values.begin())++;
                else // shifting behind
                    --iter;
            }
        }
    }

    TParamValues::const_iterator piter;
    --(piter = iter);

    double dt = (*iter - *piter) * 0.1;

    if(t_bias < 0)
        return (GetRealXYZ(t) - GetRealXYZ(Max(t - dt, *piter))).Normalized();
    else
        return (GetRealXYZ(Min(t + dt, *iter)) - GetRealXYZ(t)).Normalized();
}

// ------
// PLANE
// ------
void PLANE::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadVector(pvec);
    Reader.ReadVector(normal);
    Reader.ReadVector(x_axis);
}

bool PLANE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    y_axis = normal * x_axis;

    inv_transform.SetBasis(x_axis, y_axis, normal).Inverse();

    return true;
}

// ---------
// CYLINDER
// ---------
void CYLINDER::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadVector   (pvec);
    Reader.ReadVector   (axis);
    Reader.ReadReal     (radius);
    Reader.ReadVector   (x_axis);
}

bool CYLINDER::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    y_axis = axis * x_axis;

    inv_transform.SetBasis(x_axis, y_axis, axis).Inverse();

    for(double u = 0 ; Compare(u, g_dPI * 2) <= 0 ; u += dMaxAngle)
        u_values.insert(u);

    return true;
}

// -----
// CONE
// -----
void CONE::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadVector   (pvec);
    Reader.ReadVector   (axis);
    Reader.ReadReal     (radius);
    Reader.ReadReal     (sin_half_angle);
    Reader.ReadReal     (cos_half_angle);
    Reader.ReadVector   (x_axis);
}

bool CONE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    // Transmitted cones do not match specification, accounting for that
    axis  = -axis;
    sense = MulSense(sense, '-');

    y_axis = axis * x_axis;
    inv_transform.SetBasis(x_axis, y_axis, axis).Inverse();

    tan_half_angle = sin_half_angle / cos_half_angle;

    for(double u = 0 ; Compare(u, g_dPI * 2) <= 0 ; u += dMaxAngle)
        u_values.insert(u);

    v_values.insert(-radius / tan_half_angle);

    return true;
}

// -------
// SPHERE
// -------
void SPHERE::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadVector   (centre);
    Reader.ReadReal     (radius);
    Reader.ReadVector   (axis);
    Reader.ReadVector   (x_axis);
}

bool SPHERE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    y_axis = axis * x_axis;

    inv_transform.SetBasis(x_axis, y_axis, axis).Inverse();

    for(double u = 0 ; Compare(u, g_dPI * 2) <= 0 ; u += dMaxAngle)
        u_values.insert(u);

    for(double v = -g_dPI * 0.5 ; Compare(v, g_dPI * 0.5) <= 0 ; v += dMaxAngle)
        v_values.insert(v);

    return true;
}

// ------
// TORUS
// ------
void TORUS::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadVector   (centre);
    Reader.ReadVector   (axis);
    Reader.ReadReal     (major_radius);
    Reader.ReadReal     (minor_radius);
    Reader.ReadVector   (x_axis);
}

bool TORUS::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    y_axis = axis * x_axis;

    inv_transform.SetBasis(x_axis, y_axis, axis).Inverse();

    for(double u = 0 ; Compare(u, g_dPI * 2) <= 0 ; u += dMaxAngle)
        u_values.insert(u);

    for(double v = 0 ; Compare(v, g_dPI * 2) <= 0 ; v += dMaxAngle)
        v_values.insert(v);

    return true;
}

// -------------
// BLENDED_EDGE
// -------------
void BLENDED_EDGE::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadChar (blend_type);
    Reader.ReadPtr  (surface[0]);
    Reader.ReadPtr  (surface[1]);
    Reader.ReadPtr  (spine);
    Reader.ReadReal (range[0]);
    Reader.ReadReal (range[1]);
    Reader.ReadReal (thumb_weight[0]);
    Reader.ReadReal (thumb_weight[1]);
    Reader.ReadPtr  (boundary[0]);
    Reader.ReadPtr  (boundary[1]);
    Reader.ReadPtr  (start);
    Reader.ReadPtr  (end);
}

void BLENDED_EDGE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, surface[0]);
    ResolvePtr(Map, surface[1]);
    ResolvePtr(Map, spine);
    ResolvePtr(Map, boundary[0]);
    ResolvePtr(Map, boundary[1]);
    ResolvePtr(Map, start);
    ResolvePtr(Map, end);
}

bool BLENDED_EDGE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    if( !surface[0] || !surface[0]->Prepare(dMaxAngle) ||
        !surface[1] || !surface[1]->Prepare(dMaxAngle) ||
        !spine      || !spine->     Prepare(dMaxAngle))
    {
        return false;
    }

    // !!!
    sense = MulSense(sense, '-');

    if(fabs(range[0]) != fabs(range[1]))
        return false;

    u_values = spine->GetTValues();

    for(double v = 0 ; Compare(v, 1.0) <= 0 ; v += 0.1)
        v_values.insert(v);

    return true;
}

void BLENDED_EDGE::GetNodeInfo(double u, TNodeInfo& RInfo) const
{
    TNodes::const_iterator iter = nodes.find(u);

    if(iter != nodes.end())
    {
        RInfo = iter->second;
        return;
    }

    RInfo.spine_xyz = spine->GetRealXYZ     (u);
    RInfo.spine_tg  = spine->GetRealTangent (u);

    RInfo.x_axis = OrthogonalVector(RInfo.spine_tg);
    RInfo.y_axis = RInfo.spine_tg * RInfo.x_axis;

    RInfo.inv_transform.SetBasis(RInfo.x_axis, RInfo.y_axis, RInfo.spine_tg).Inverse();

    for(unsigned i = 0 ; i < 2 ; i++)
    {
        RInfo.surf_uv[i] = surface[i]->GetApproxUV(RInfo.spine_xyz);

        RInfo.surf_xyz      [i] = surface[i]->GetApproxXYZ      (RInfo.surf_uv[i]);
        RInfo.surf_normal   [i] = surface[i]->GetApproxNormal   (RInfo.surf_uv[i]);

        RInfo.surf_ang[i] = Get2D_Angle(RInfo.inv_transform * (RInfo.surf_xyz[i] - RInfo.spine_xyz));
    }

    if(Compare(RInfo.surf_ang[1], RInfo.surf_ang[0]) < 0)
        RInfo.surf_ang[1] += g_dPI * 2;

    nodes.insert(TNodes::value_type(u, RInfo));
}

// ------------
// BLEND_BOUND
// ------------
void BLEND_BOUND::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadShort(boundary);
    Reader.ReadPtr  (blend);
}

void BLEND_BOUND::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, blend);
}

bool BLEND_BOUND::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    if(!blend || !blend->Prepare(dMaxAngle))
        return false;

    if(boundary < 0 || boundary >= 2)
        return false;

    u_values = blend->spine->GetTValues();

    return true;
}

// ------------
// OFFSET_SURF
// ------------
void OFFSET_SURF::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadChar     (check);
    Reader.ReadLogical  (true_offset);
    Reader.ReadPtr      (surface);
    Reader.ReadReal     (offset);
    Reader.ReadReal     (scale);
}

void OFFSET_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, surface);
}

bool OFFSET_SURF::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    if(!surface || !surface->Prepare(dMaxAngle))
        return false;

    if(sense != surface->sense)
        return false;

    u_values = surface->GetUValues();
    v_values = surface->GetVValues();

    return true;
}

// -----------
// NURBS_SURF
// -----------
void NURBS_SURF::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadLogical  (u_periodic);
    Reader.ReadLogical  (v_periodic);
    Reader.ReadShort    (u_degree);
    Reader.ReadShort    (v_degree);
    Reader.ReadInt      (n_u_vertices);
    Reader.ReadInt      (n_v_vertices);
    Reader.ReadEnum     (u_knot_type);
    Reader.ReadEnum     (v_knot_type);
    Reader.ReadInt      (n_u_knots);
    Reader.ReadInt      (n_v_knots);
    Reader.ReadLogical  (rational);
    Reader.ReadLogical  (u_closed);
    Reader.ReadLogical  (v_closed);
    Reader.ReadEnum     (surface_form);
    Reader.ReadShort    (vertex_dim);
    Reader.ReadPtr      (bspline_vertices);
    Reader.ReadPtr      (u_knot_mult);
    Reader.ReadPtr      (v_knot_mult);
    Reader.ReadPtr      (u_knots);
    Reader.ReadPtr      (v_knots);
}

void NURBS_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, bspline_vertices);
    ResolvePtr(Map, u_knot_mult);
    ResolvePtr(Map, v_knot_mult);
    ResolvePtr(Map, u_knots);
    ResolvePtr(Map, v_knots);
}

bool NURBS_SURF::InternalPrepare(double dMaxAngle)
{
    if(!NODE::InternalPrepare(dMaxAngle))
        return false;

    if( !u_knots            || !u_knots->           Prepare(dMaxAngle) ||
        !v_knots            || !v_knots->           Prepare(dMaxAngle) ||
        !u_knot_mult        || !u_knot_mult->       Prepare(dMaxAngle) ||
        !v_knot_mult        || !v_knot_mult->       Prepare(dMaxAngle) ||
        !bspline_vertices   || !bspline_vertices->  Prepare(dMaxAngle))
    {
        return false;
    }

    if( !u_closed && u_periodic ||
        !v_closed && v_periodic)
    {
        return false;
    }

    if( u_degree < 1 || u_degree > 7 ||
        v_degree < 1 || v_degree > 7)
    {
        return false;
    }

    return true;
}

// -------------
// SURFACE_DATA
// -------------
void SURFACE_DATA::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadInterval (original_uint);
    Reader.ReadInterval (original_vint);
    Reader.ReadInterval (extended_uint);
    Reader.ReadInterval (extended_vint);
    Reader.ReadEnum     (self_int);
    Reader.ReadChar     (original_u_start);
    Reader.ReadChar     (original_u_end);
    Reader.ReadChar     (original_v_start);
    Reader.ReadChar     (original_v_end);
    Reader.ReadChar     (extended_u_start);
    Reader.ReadChar     (extended_u_end);
    Reader.ReadChar     (extended_v_start);
    Reader.ReadChar     (extended_v_end);
    Reader.ReadChar     (analytic_form_type);
    Reader.ReadChar     (swept_form_type);
    Reader.ReadChar     (spun_form_type);
    Reader.ReadChar     (blend_form_type);
    Reader.ReadPtr      (analytic_form);
    Reader.ReadPtr      (swept_form);
    Reader.ReadPtr      (spun_form);
    Reader.ReadPtr      (blend_form);
}

void SURFACE_DATA::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, analytic_form);
    ResolvePtr(Map, swept_form);
    ResolvePtr(Map, spun_form);
    ResolvePtr(Map, blend_form);
}

// --------------
// HELIX_SU_FORM
// --------------
void HELIX_SU_FORM::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadVector   (axis_pt);
    Reader.ReadVector   (axis_dir);
    Reader.ReadChar     (hand);
    Reader.ReadInterval (turns);
    Reader.ReadReal     (pitch);
    Reader.ReadReal     (gap);
    Reader.ReadReal     (tol);
}

// ----------
// B_SURFACE
// ----------
void B_SURFACE::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadPtr(nurbs);
    Reader.ReadPtr(data);
}

void B_SURFACE::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, nurbs);
    ResolvePtr(Map, data);
}

void B_SURFACE::ParseVertex(unsigned i, unsigned j, vector& v, double& w) const
{
    const double* c = &nurbs->bspline_vertices->vertices[nurbs->vertex_dim * (i * nurbs->n_v_vertices + j)];

    v.Set(0, 0, 0);

    if(nurbs->rational)
    {
        if(nurbs->vertex_dim >= 2)
        {
            v.x = c[0];

            if(nurbs->vertex_dim >= 3)
            {
                v.y = c[1];

                if(nurbs->vertex_dim >= 4)
                    v.z = c[2];
            }
        }

        w = c[nurbs->vertex_dim - 1];
    }
    else
    {
        if(nurbs->vertex_dim >= 1)
        {
            v.x = c[0];

            if(nurbs->vertex_dim >= 2)
            {
                v.y = c[1];

                if(nurbs->vertex_dim >= 3)
                    v.z = c[2];
            }
        }

        w = 1.0;
    }
}

bool B_SURFACE::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    if(!nurbs || !nurbs->Prepare(dMaxAngle))
        return false;

    // Removing zero-mult knots
    if(nurbs->u_knots->knots.size() != nurbs->u_knot_mult->mult.size())
        return false;

    for(size_t i = 0 ; i < nurbs->u_knots->knots.size() ; i++)
    {
        if(!nurbs->u_knot_mult->mult[i])
        {
            nurbs->u_knots->    knots.erase(nurbs->u_knots->    knots.begin() + i);
            nurbs->u_knot_mult->mult. erase(nurbs->u_knot_mult->mult. begin() + i);

            i--;
        }
    }

    if(nurbs->v_knots->knots.size() != nurbs->v_knot_mult->mult.size())
        return false;

    for(size_t i = 0 ; i < nurbs->v_knots->knots.size() ; i++)
    {
        if(!nurbs->v_knot_mult->mult[i])
        {
            nurbs->v_knots->    knots.erase(nurbs->v_knots->    knots.begin() + i);
            nurbs->v_knot_mult->mult. erase(nurbs->v_knot_mult->mult. begin() + i);

            i--;
        }
    }

    // Asserting data integrity
    if( nurbs->bspline_vertices->vertices.size() != nurbs->n_u_vertices * nurbs->n_v_vertices * nurbs->vertex_dim ||
        nurbs->u_knots->    knots.size() != nurbs->n_u_knots ||
        nurbs->v_knots->    knots.size() != nurbs->n_v_knots ||
        nurbs->u_knot_mult->mult. size() != nurbs->n_u_knots ||
        nurbs->v_knot_mult->mult. size() != nurbs->n_v_knots)
    {
        return false;
    }

    // Expanding knots
    for(int i = 0 ; i < nurbs->n_u_knots ; i++)
        exp_u_knots.insert(exp_u_knots.end(), nurbs->u_knot_mult->mult[i], nurbs->u_knots->knots[i]);

    for(int i = 0 ; i < nurbs->n_v_knots ; i++)
        exp_v_knots.insert(exp_v_knots.end(), nurbs->v_knot_mult->mult[i], nurbs->v_knots->knots[i]);

    if( nurbs->n_u_vertices != exp_u_knots.size() - nurbs->u_degree - 1 ||
        nurbs->n_v_vertices != exp_v_knots.size() - nurbs->v_degree - 1)
    {
        return false;
    }

    unsigned uSubdivPrecision = (unsigned)Ceil(g_dPI / dMaxAngle);

    double dMaxAngCos = cos(dMaxAngle);

    // Filling valid U knots and segmentation
    {
        std::vector<vector> PrevTangents((nurbs->n_v_vertices - nurbs->v_degree) * uSubdivPrecision);
        std::vector<vector> CurTangents ((nurbs->n_v_vertices - nurbs->v_degree) * uSubdivPrecision);

        for(int i = nurbs->u_degree ; i < nurbs->n_u_vertices ; i++)
        {
            if(exp_u_knots[i] == exp_u_knots[i+1])
                continue;

            valid_u_segs.insert(std::map<double, unsigned>::value_type(exp_u_knots[i+1], i - nurbs->u_degree));

            double dUStep = (exp_u_knots[i+1] - exp_u_knots[i]) / uSubdivPrecision;

            for(unsigned ii = 0 ; ii < uSubdivPrecision ; ii++)
            {
                for(int j = nurbs->v_degree ; j < nurbs->n_v_vertices ; j++)
                {
                    if(exp_v_knots[j] == exp_v_knots[j+1])
                        continue;

                    double dVStep = (exp_v_knots[j+1] - exp_v_knots[j]) / uSubdivPrecision;

                    for(unsigned jj = 0 ; jj < uSubdivPrecision ; jj++)
                    {
                        CurTangents[(j - nurbs->v_degree) * uSubdivPrecision + jj] =
                            GetXYZdu(   T_UV_Point(exp_u_knots[i] + ii * dUStep, exp_v_knots[j] + jj * dVStep),
                                                i - nurbs->u_degree,
                                                j - nurbs->v_degree).Normalized();
                    }
                }

                bool bSingularity = true;
                bool bHasBigAngle = false;

                if(ii)
                {
                    for(size_t k = 0 ; k < CurTangents.size() ; k++)
                    {
                        if(!PrevTangents[k].IsZero() && !CurTangents[k].IsZero())
                        {
                            bSingularity = false;

                            if(Compare(PrevTangents[k] & CurTangents[k], dMaxAngCos) <= 0)
                            {
                                bHasBigAngle = true;
                                break;
                            }
                        }
                    }
                }

                if(bSingularity || bHasBigAngle)
                    u_values.insert(exp_u_knots[i] + ii * dUStep), PrevTangents = CurTangents;
            }
        }

        u_values.insert(exp_u_knots[nurbs->n_u_vertices]);
    }

    // Filling valid V knots and segmentation
    {
        std::vector<vector> PrevTangents((nurbs->n_u_vertices - nurbs->u_degree) * uSubdivPrecision);
        std::vector<vector> CurTangents ((nurbs->n_u_vertices - nurbs->u_degree) * uSubdivPrecision);

        for(int j = nurbs->v_degree ; j < nurbs->n_v_vertices ; j++)
        {
            if(exp_v_knots[j] == exp_v_knots[j+1])
                continue;

            valid_v_segs.insert(std::map<double, unsigned>::value_type(exp_v_knots[j+1], j - nurbs->v_degree));

            double dVStep = (exp_v_knots[j+1] - exp_v_knots[j]) / uSubdivPrecision;

            for(unsigned jj = 0 ; jj < uSubdivPrecision ; jj++)
            {
                for(int i = nurbs->u_degree ; i < nurbs->n_u_vertices ; i++)
                {
                    if(exp_u_knots[i] == exp_u_knots[i+1])
                        continue;

                    double dUStep = (exp_u_knots[i+1] - exp_u_knots[i]) / uSubdivPrecision;

                    for(unsigned ii = 0 ; ii < uSubdivPrecision ; ii++)
                    {
                        CurTangents[(i - nurbs->u_degree) * uSubdivPrecision + ii] =
                            GetXYZdv(   T_UV_Point(exp_u_knots[i] + ii * dUStep, exp_v_knots[j] + jj * dVStep),
                                                i - nurbs->u_degree,
                                                j - nurbs->v_degree).Normalized();
                    }
                }

                bool bSingularity = true;
                bool bHasBigAngle = false;

                if(jj)
                {
                    for(size_t k = 0 ; k < CurTangents.size() ; k++)
                    {
                        if(!PrevTangents[k].IsZero() && !CurTangents[k].IsZero())
                        {
                            bSingularity = false;

                            if(Compare(PrevTangents[k] & CurTangents[k], dMaxAngCos) <= 0)
                            {
                                bHasBigAngle = true;
                                break;
                            }
                        }
                    }
                }

                if(bSingularity || bHasBigAngle)
                    v_values.insert(exp_v_knots[j] + jj * dVStep), PrevTangents = CurTangents;
            }
        }

        v_values.insert(exp_v_knots[nurbs->n_v_vertices]);
    }

    return true;
}

void B_SURFACE::InternalPostPrepare()
{
    ANY_SURF::InternalPostPrepare();

    approx.Build(this);
}

vector B_SURFACE::GetRealXYZ(const T_UV_Point& _uv) const
{
    if(valid_u_segs.empty() || valid_v_segs.empty())
        return vector(0, 0, 0);

    T_UV_Point uv = NormalizeUV(_uv);

    TApproxParamValues::const_iterator u_iter = valid_u_segs.lower_bound(uv.u);
    TApproxParamValues::const_iterator v_iter = valid_v_segs.lower_bound(uv.v);

    size_t bi = u_iter->second;
    size_t bj = v_iter->second;

    vector cv[8][8];
    double cw[8][8];

    for(int i = 0 ; i <= nurbs->u_degree ; i++)
    {
        for(int j = 0 ; j <= nurbs->v_degree ; j++)
            ParseVertex(bi + i, bj + j, cv[i][j], cw[i][j]);
    }

    // Transforming (0,0) matrix to (0,deg_v) matrix
    for(int d = 1 ; d <= nurbs->v_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= 0 ; i--)
        {
            for(int j = nurbs->v_degree ; j >= d ; j--)
            {
                double c = SafeDiv( uv.v - exp_v_knots[bj + j],
                                    exp_v_knots[bj + j + nurbs->v_degree + 1 - d] - exp_v_knots[bj + j]);

                cv[i][j] = cv[i][j-1] + (cv[i][j] - cv[i][j-1]) * c;
                cw[i][j] = cw[i][j-1] + (cw[i][j] - cw[i][j-1]) * c;
            }
        }
    }

    // Transforming (0,deg_v) matrix to (deg_u,deg_v) matrix
    for(int d = 1 ; d <= nurbs->u_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= d ; i--)
        {
            for(int j = nurbs->v_degree ; j >= 0 ; j--)
            {
                double c = SafeDiv( uv.u - exp_u_knots[bi + i],
                                    exp_u_knots[bi + i + nurbs->u_degree + 1 - d] - exp_u_knots[bi + i]);

                cv[i][j] = cv[i-1][j] + (cv[i][j] - cv[i-1][j]) * c;
                cw[i][j] = cw[i-1][j] + (cw[i][j] - cw[i-1][j]) * c;
            }
        }
    }

    vector v = cv[nurbs->u_degree][nurbs->v_degree];
    double w = cw[nurbs->u_degree][nurbs->v_degree];

    return nurbs->rational && w ? v / w : v;
}

vector B_SURFACE::GetXYZdu(const T_UV_Point& uv, size_t bi, size_t bj) const
{
    vector cv[8][8];
    double cw[8][8];

    vector cvdu[8][8];
    double cwdu[8][8];

    for(int i = 0 ; i <= nurbs->u_degree ; i++)
    {
        for(int j = 0 ; j <= nurbs->v_degree ; j++)
        {
            ParseVertex(bi + i, bj + j, cv[i][j], cw[i][j]);

            cvdu[i][j] = vector(0, 0, 0);
            cwdu[i][j] = 0;
        }
    }

    // Transforming (0,0) matrix to (0,deg_v) matrix
    for(int d = 1 ; d <= nurbs->v_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= 0 ; i--)
        {
            for(int j = nurbs->v_degree ; j >= d ; j--)
            {
                double cd = SafeDiv(1, exp_v_knots[bj + j + nurbs->v_degree + 1 - d] - exp_v_knots[bj + j]);

                double c = (uv.v - exp_v_knots[bj + j]) * cd;

                cv[i][j] = cv[i][j-1] + (cv[i][j] - cv[i][j-1]) * c;
                cw[i][j] = cw[i][j-1] + (cw[i][j] - cw[i][j-1]) * c;
            }
        }
    }

    // Transforming (0,deg_v) matrix to (deg_u,deg_v) matrix
    for(int d = 1 ; d <= nurbs->u_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= d ; i--)
        {
            double cd = SafeDiv(1, exp_u_knots[bi + i + nurbs->u_degree + 1 - d] - exp_u_knots[bi + i]);

            double c = (uv.u - exp_u_knots[bi + i]) * cd;

            for(int j = nurbs->v_degree ; j >= 0 ; j--)
            {
                cvdu[i][j] = cvdu[i-1][j] + (cvdu[i][j] - cvdu[i-1][j]) * c + (cv[i][j] - cv[i-1][j]) * cd;
                cwdu[i][j] = cwdu[i-1][j] + (cwdu[i][j] - cwdu[i-1][j]) * c + (cw[i][j] - cw[i-1][j]) * cd;

                cv[i][j] = cv[i-1][j] + (cv[i][j] - cv[i-1][j]) * c;
                cw[i][j] = cw[i-1][j] + (cw[i][j] - cw[i-1][j]) * c;
            }
        }
    }

    vector v = cv[nurbs->u_degree][nurbs->v_degree];
    double w = cw[nurbs->u_degree][nurbs->v_degree];

    vector vdu = cvdu[nurbs->u_degree][nurbs->v_degree];
    double wdu = cwdu[nurbs->u_degree][nurbs->v_degree];

    return nurbs->rational ? vdu * w - v * wdu : vdu;
}

vector B_SURFACE::GetXYZdv(const T_UV_Point& uv, size_t bi, size_t bj) const
{
    vector cv[8][8];
    double cw[8][8];

    vector cvdv[8][8];
    double cwdv[8][8];

    for(int i = 0 ; i <= nurbs->u_degree ; i++)
    {
        for(int j = 0 ; j <= nurbs->v_degree ; j++)
        {
            ParseVertex(bi + i, bj + j, cv[i][j], cw[i][j]);

            cvdv[i][j] = vector(0, 0, 0);
            cwdv[i][j] = 0;
        }
    }

    // Transforming (0,0) matrix to (0,deg_v) matrix
    for(int d = 1 ; d <= nurbs->v_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= 0 ; i--)
        {
            for(int j = nurbs->v_degree ; j >= d ; j--)
            {
                double cd = SafeDiv(1, exp_v_knots[bj + j + nurbs->v_degree + 1 - d] - exp_v_knots[bj + j]);

                double c = (uv.v - exp_v_knots[bj + j]) * cd;

                cvdv[i][j] = cvdv[i][j-1] + (cvdv[i][j] - cvdv[i][j-1]) * c + (cv[i][j] - cv[i][j-1]) * cd;
                cwdv[i][j] = cwdv[i][j-1] + (cwdv[i][j] - cwdv[i][j-1]) * c + (cw[i][j] - cw[i][j-1]) * cd;

                cv[i][j] = cv[i][j-1] + (cv[i][j] - cv[i][j-1]) * c;
                cw[i][j] = cw[i][j-1] + (cw[i][j] - cw[i][j-1]) * c;
            }
        }
    }

    // Transforming (0,deg_v) matrix to (deg_u,deg_v) matrix
    for(int d = 1 ; d <= nurbs->u_degree ; d++)
    {
        for(int i = nurbs->u_degree ; i >= d ; i--)
        {
            double cd = SafeDiv(1, exp_u_knots[bi + i + nurbs->u_degree + 1 - d] - exp_u_knots[bi + i]);

            double c = (uv.u - exp_u_knots[bi + i]) * cd;

            for(int j = nurbs->v_degree ; j >= 0 ; j--)
            {
                cvdv[i][j] = cvdv[i-1][j] + (cvdv[i][j] - cvdv[i-1][j]) * c;
                cwdv[i][j] = cwdv[i-1][j] + (cwdv[i][j] - cwdv[i-1][j]) * c;

                cv[i][j] = cv[i-1][j] + (cv[i][j] - cv[i-1][j]) * c;
                cw[i][j] = cw[i-1][j] + (cw[i][j] - cw[i-1][j]) * c;
            }
        }
    }

    vector v = cv[nurbs->u_degree][nurbs->v_degree];
    double w = cw[nurbs->u_degree][nurbs->v_degree];

    vector vdv = cvdv[nurbs->u_degree][nurbs->v_degree];
    double wdv = cwdv[nurbs->u_degree][nurbs->v_degree];

    return nurbs->rational ? vdv * w - v * wdv : vdv;
}

vector B_SURFACE::GetRealNoSenseNormal(const T_UV_Point& _uv, int u_bias, int v_bias) const
{
    if(valid_u_segs.empty() || valid_v_segs.empty())
        return vector(0, 0, 1);

    T_UV_Point uv = NormalizeUV(_uv);

    TApproxParamValues::const_iterator u_iter;

    if(!Compare(uv.u, GetMinU()) && u_bias < 0 && GetUTopology() >= TPL_PERIODIC)
    {
        --(u_iter = valid_u_segs.end()), uv.u = GetMaxU();
    }
    else
    {
        u_iter = valid_u_segs.lower_bound(uv.u);

        if(!Compare(u_iter->first, uv.u) && u_bias >= 0)
        {
            if(++u_iter == valid_u_segs.end())
            {
                if(GetUTopology() == TPL_PERIODIC)
                    u_iter = valid_u_segs.begin(), uv.u = GetMinU();
                else
                    u_iter--;
            }
        }
    }

    TApproxParamValues::const_iterator v_iter;

    if(!Compare(uv.v, GetMinV()) && v_bias < 0 && GetVTopology() >= TPL_PERIODIC)
    {
        --(v_iter = valid_v_segs.end()), uv.v = GetMaxV();
    }
    else
    {
        v_iter = valid_v_segs.lower_bound(uv.v);

        if(!Compare(v_iter->first, uv.v) && v_bias >= 0)
        {
            if(++v_iter == valid_v_segs.end())
            {
                if(GetUTopology() == TPL_PERIODIC)
                    v_iter = valid_v_segs.begin(), uv.v = GetMinV();
                else
                    v_iter--;
            }
        }
    }

    return (GetXYZdu(uv, u_iter->second, v_iter->second) * GetXYZdv(uv, u_iter->second, v_iter->second)).Normalized();
}

// -----------
// SWEPT_SURF
// -----------
void SWEPT_SURF::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::LoadData(Reader);

    Reader.ReadPtr      (section);
    Reader.ReadVector   (sweep);
    Reader.ReadReal     (scale);
}

void SWEPT_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, section);
}

bool SWEPT_SURF::InternalPrepare(double dMaxAngle)
{
    if(!ANY_SURF::InternalPrepare(dMaxAngle))
        return false;

    if(!section || !section->Prepare(dMaxAngle))
        return false;

    vector x_axis = OrthogonalVector(sweep);

    fwd_transform.SetBasis(x_axis, sweep * x_axis, sweep);
    (inv_transform = fwd_transform).Inverse();

    approx_xy.Clear();
    approx_z. Clear();

    for(TParamValues::const_iterator iter = section->GetTValues().begin() ; iter != section->GetTValues().end() ; ++iter)
    {
        vector p = inv_transform * section->GetRealXYZ(*iter);

        vector tan1 = section->GetRealNoSenseTangent(*iter, -1);
        vector tan2 = section->GetRealNoSenseTangent(*iter, +1);

        approx_xy.AddPoint(*iter, vector(p.x, p.y,   0), tan1, tan2);
        approx_z. AddPoint(*iter, vector(  0,   0, p.z), vector(), vector());
    }

    u_values = section->GetTValues();

    return true;
}

// ----------
// SPUN_SURF
// ----------
void SPUN_SURF::LoadData(T_XT_Reader& Reader)
{
    ANY_SURF::Load(Reader);

    Reader.ReadPtr      (profile);
    Reader.ReadVector   (base);
    Reader.ReadVector   (axis);
    Reader.ReadVector   (start);
    Reader.ReadVector   (end);
    Reader.ReadReal     (start_param);
    Reader.ReadReal     (end_param);
    Reader.ReadVector   (x_axis);
    Reader.ReadReal     (scale);
}

void SPUN_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, profile);
}

bool SPUN_SURF::InternalPrepare(double dMaxAngle)
{
    return false;
}

// --------
// PE_SURF
// --------
void PE_SURF::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    ANY_SURF::LoadData(Reader);

    Reader.ReadChar (type);
    Reader.ReadPtr  (data);
    Reader.ReadPtr  (tf);

    internal_geom.reserve(n);

    for( ; n ; n--)
        internal_geom.push_back(Reader.ReadPtr<PE_INT_GEOM>());
}

void PE_SURF::ResolvePtrs(const TNodeIndexMap& Map)
{
    ANY_SURF::ResolvePtrs(Map);

    ResolvePtr(Map, data);
    ResolvePtr(Map, tf);

    for(std::vector<PE_INT_GEOM*>::iterator i = internal_geom.begin() ; i != internal_geom.end() ; ++i)
        ResolvePtr(Map, *i);
}

// ------
// POINT
// ------
void POINT::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr      (attributes_groups);
    Reader.ReadPtr      (owner);
    Reader.ReadPtr      (next);
    Reader.ReadPtr      (previous);
    Reader.ReadVector   (pvec);
}

void POINT::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
}

// ----------
// TRANSFORM
// ----------
void TRANSFORM::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr(owner);
    Reader.ReadPtr(next);
    Reader.ReadPtr(previous);

    for(size_t i = 0 ; i < 3 ; i++)
    {
        for(size_t j = 0 ; j < 3 ; j++)
            Reader.ReadReal(rotation_matrix.v[i][j]);
    }

    Reader.ReadVector   (translation_vector);
    Reader.ReadReal     (scale);
    Reader.ReadInt      (flag);
    Reader.ReadVector   (perspective_vector);
}

void TRANSFORM::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
}

TRANSFORM TRANSFORM::operator * (const TRANSFORM& Transform) const
{
    // x1 = (m1*x0 + t1)*s1
    // x2 = (m2*x1 + t2)*s2
    //
    // x2 = ((m2*m1)*x0 + m2*t1 + t2/s1)*s1*s2
    // x2 = (m3*x0 + t3)*s3
    //
    // m3 = m2*m1
    // t3 = m2*t1 + t2/s1
    // s3 = s1*s2

    TRANSFORM res;

    // m3 = m1*m2
    res.rotation_matrix = Transform.rotation_matrix * rotation_matrix;

    // t3 = m2*t1 + t2/s1
    res.translation_vector =
        Transform.rotation_matrix * translation_vector  +
        Transform.translation_vector / (scale ? scale : 1.0);

    // s3 = s1*s2
    res.scale = scale * Transform.scale;

    return res;
}

// ----------------
// GEOMETRIC_OWNER
// ----------------
void GEOMETRIC_OWNER::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadPtr(owner);
    Reader.ReadPtr(next);
    Reader.ReadPtr(previous);
    Reader.ReadPtr(shared_geometry);
}

void GEOMETRIC_OWNER::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, shared_geometry);
}

// ------
// WORLD
// ------
void WORLD::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadPtr      (assembly);
    Reader.ReadPtr      (attribute);
    Reader.ReadPtr      (body);
    Reader.ReadPtr      (transform);
    Reader.ReadPtr      (surface);
    Reader.ReadPtr      (curve);
    Reader.ReadPtr      (point);
    Reader.ReadLogical  (alive);
    Reader.ReadPtr      (attrib_def);
    Reader.ReadInt      (highest_id);
    Reader.ReadInt      (current_id);
}

void WORLD::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, assembly);
    ResolvePtr(Map, attribute);
    ResolvePtr(Map, body);
    ResolvePtr(Map, transform);
    ResolvePtr(Map, surface);
    ResolvePtr(Map, curve);
    ResolvePtr(Map, point);
    ResolvePtr(Map, attrib_def);
}

void WORLD::Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth) const
{
    RENDERABLE::Triangulate(RFaces, pTransform, szDepth);

    throw TException("WORLD rendering is not supported.");
}

// ---------
// ASSEMBLY
// ---------
void ASSEMBLY::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadInt  (highest_node_id);
    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (attribute_chains);
    Reader.ReadPtr  (list);
    Reader.ReadPtr  (surface);
    Reader.ReadPtr  (curve);
    Reader.ReadPtr  (point);
    Reader.ReadPtr  (key);
    Reader.ReadReal (res_size);
    Reader.ReadReal (res_linear);
    Reader.ReadPtr  (ref_instance);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadEnum (state);
    Reader.ReadPtr  (owner);
    Reader.ReadEnum (type);
    Reader.ReadPtr  (sub_instance);
}

void ASSEMBLY::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, attribute_chains);
    ResolvePtr(Map, list);
    ResolvePtr(Map, surface);
    ResolvePtr(Map, curve);
    ResolvePtr(Map, point);
    ResolvePtr(Map, key);
    ResolvePtr(Map, ref_instance);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, sub_instance);
}

void ASSEMBLY::Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth) const
{
    RENDERABLE::Triangulate(RFaces, pTransform, szDepth);

    for(const INSTANCE* pSubInstance = sub_instance ; pSubInstance ; pSubInstance = pSubInstance->next_in_part)
    {
        if(pSubInstance->part)
        {
            TRANSFORM OverallTransform;

            if(pTransform && pSubInstance->transform)
                OverallTransform = *pTransform * *pSubInstance->transform;

            pSubInstance->part->Triangulate(RFaces,
                                            pTransform && pSubInstance->transform   ? &OverallTransform :
                                            pTransform                              ? pTransform :
                                            pSubInstance->transform                 ? pSubInstance->transform :
                                                NULL,
                                            szDepth + 1);
        }
    }
}

// ----
// KEY
// ----
void KEY::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    string.reserve(n);

    for( ; n ; n--)
        string.push_back(Reader.ReadChar());
}

// ---------
// INSTANCE
// ---------
void INSTANCE::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadEnum (type);
    Reader.ReadPtr  (part);
    Reader.ReadPtr  (transform);
    Reader.ReadPtr  (assembly);
    Reader.ReadPtr  (next_in_part);
    Reader.ReadPtr  (prev_in_part);
    Reader.ReadPtr  (next_of_part);
    Reader.ReadPtr  (prev_of_part);
}

void INSTANCE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, part);
    ResolvePtr(Map, transform);
    ResolvePtr(Map, assembly);
    ResolvePtr(Map, next_in_part);
    ResolvePtr(Map, prev_in_part);
    ResolvePtr(Map, next_of_part);
    ResolvePtr(Map, prev_of_part);
}

// -----
// BODY
// -----
void BODY::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadInt  (highest_node_id);
    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (attribute_chains);
    Reader.ReadPtr  (surface);
    Reader.ReadPtr  (curve);
    Reader.ReadPtr  (point);
    Reader.ReadPtr  (key);
    Reader.ReadReal (res_size);
    Reader.ReadReal (res_linear);
    Reader.ReadPtr  (ref_instance);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadEnum (state);
    Reader.ReadPtr  (owner);
    Reader.ReadEnum (body_type);
    Reader.ReadEnum (nom_geom_state);
    Reader.ReadPtr  (shell);
    Reader.ReadPtr  (boundary_surface);
    Reader.ReadPtr  (boundary_curve);
    Reader.ReadPtr  (boundary_point);
    Reader.ReadPtr  (region);
    Reader.ReadPtr  (edge);
    Reader.ReadPtr  (vertex);
}

void BODY::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, attribute_chains);
    ResolvePtr(Map, surface);
    ResolvePtr(Map, curve);
    ResolvePtr(Map, point);
    ResolvePtr(Map, key);
    ResolvePtr(Map, ref_instance);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, shell);
    ResolvePtr(Map, boundary_surface);
    ResolvePtr(Map, boundary_curve);
    ResolvePtr(Map, boundary_point);
    ResolvePtr(Map, region);
    ResolvePtr(Map, edge);
    ResolvePtr(Map, vertex);
}

void BODY::Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth) const
{
    RENDERABLE::Triangulate(RFaces, pTransform, szDepth);

    for(const REGION* pRegion = region ; pRegion ; pRegion = pRegion->next)
    {
        if(pRegion->type != 'S') // getting only solid regions
            continue;

        for(const SHELL* pShell = pRegion->shell ; pShell ; pShell = pShell->next)
        {
            for(const FACE* pFace = pShell->face ; pFace ; pFace = pFace->next)
            {
                if(!pFace->surface) // invalid face
                    continue;

                const ANY_SURF* pSurface = pFace->surface;
                char cSurfaceSense = MulSense(pFace->sense, pSurface->sense);

                if(dynamic_cast<const BLEND_BOUND*>(pSurface))
                    continue;

//              if(dynamic_cast<const BLENDED_EDGE*>(pSurface))
//                  continue;

                if(!pSurface->IsValid())
                    continue;

                bool bValidFace = true;

                std::vector<std::vector<TTriLoopNode> > TriLoops;

                for(const LOOP* pLoop = pFace->loop ; pLoop ; pLoop = pLoop->next)
                {
                    if(!pLoop->fin) // empty loop
                        continue;

                    std::vector<TTriLoopNode> TriLoop;

                    for(const FIN* pFin = pLoop->fin ; ; )
                    {
                        if(!pFin->edge) // point fin
                        {
                            if(pFin->forward != pFin) // must be the only fin of the loop
                                bValidFace = false;

                            break;
                        }

                        const CURVE* pCurve = NULL;
                        char cCurveSense; // sense with respect to used curve parameter

                        if(pFin->curve)
                        {
                            pCurve = pFin->curve;
                            cCurveSense = pCurve->sense;
                        }
                        else if(pFin->edge->curve)
                        {
                            pCurve = pFin->edge->curve;
                            cCurveSense = MulSense(pCurve->sense, pFin->sense);
                        }

                        if(!pCurve) // invalid fin
                        {
                            bValidFace = false;
                            break;
                        }

                        if(!pCurve->IsValid())
                            continue;

                        if(pFin->forward == pFin) // single closed edge loop
                        {
                            if(pCurve->GetTopology() < TPL_CLOSED)
                            {
                                bValidFace = false;
                                break;
                            }

                            // Descending to bottommost basis curve
                            while(dynamic_cast<const TRIMMED_CURVE*>(pCurve))
                                pCurve = dynamic_cast<const TRIMMED_CURVE*>(pCurve)->basis_curve;

                            // Filling 't' values
                            std::vector<double> t;

                            if(cCurveSense == '-')
                            {
                                GenerateMidPoints(t, pCurve->GetMaxT(), pCurve->GetMinT(), pCurve->GetTValues());
                                t.push_back(pCurve->GetMinT());
                            }
                            else
                            {
                                t.push_back(pCurve->GetMinT());
                                GenerateMidPoints(t, pCurve->GetMinT(), pCurve->GetMaxT(), pCurve->GetTValues());
                            }

                            for(size_t i = 0 ; i < t.size() ; i++)
                                TriLoop.push_back(TTriLoopNode(pCurve->GetRealXYZ(t[i]), pSurface));
                        }
                        else // multi-edge face
                        {
                            if( !pFin->other ||
                                !pFin->vertex || !pFin->vertex->point ||
                                !pFin->other->vertex || !pFin->other->vertex->point) // invalid fin
                            {
                                bValidFace = false;
                                break;
                            }

                            // Adding endpoint as direct XYZ vertex
                            TriLoop.push_back(TTriLoopNode(pFin->other->vertex->point->pvec, pSurface));

                            // Getting curve internal points limits
                            double t1, t2;

                            if(const TRIMMED_CURVE* pTrimmedCurve = dynamic_cast<const TRIMMED_CURVE*>(pCurve)) // direct T getter
                            {
                                // Descending to bottommost basis curve
                                for(pCurve = pTrimmedCurve->basis_curve ;
                                    dynamic_cast<const TRIMMED_CURVE*>(pCurve) ;
                                    pCurve = dynamic_cast<const TRIMMED_CURVE*>(pCurve)->basis_curve);

                                t1 = pCurve->GetApproxT(pTrimmedCurve->point_1);
                                t2 = pCurve->GetApproxT(pTrimmedCurve->point_2);

                                if(cCurveSense == '-')
                                    std::swap(t1, t2);
                            }
                            else
                            {
                                t1 = pCurve->GetApproxT(pFin->other->vertex->point->pvec);
                                t2 = pCurve->GetApproxT(pFin->vertex->point->pvec);
                            }

                            // Adding curve internal points
                            std::vector<double> t;

                            if(pCurve->GetTopology() >= TPL_CLOSED)
                            {
                                if(cCurveSense == '-')
                                {
                                    if(Compare(t1, t2) >= 0)
                                    {
                                        GenerateMidPoints(t, t1, t2, pCurve->GetTValues());
                                    }
                                    else
                                    {
                                        GenerateMidPoints(t, t1, pCurve->GetMinT(), pCurve->GetTValues());

                                        if(Compare(t1, pCurve->GetMinT()) > 0)
                                            t.push_back(pCurve->GetMinT());

                                        GenerateMidPoints(t, pCurve->GetMaxT(), t2, pCurve->GetTValues());
                                    }
                                }
                                else
                                {
                                    if(Compare(t1, t2) <= 0)
                                    {
                                        GenerateMidPoints(t, t1, t2, pCurve->GetTValues());
                                    }
                                    else
                                    {
                                        GenerateMidPoints(t, t1, pCurve->GetMaxT(), pCurve->GetTValues());

                                        if(Compare(t2, pCurve->GetMinT()) > 0)
                                            t.push_back(pCurve->GetMinT());

                                        GenerateMidPoints(t, pCurve->GetMinT(), t2, pCurve->GetTValues());
                                    }
                                }
                            }
                            else
                            {
                                GenerateMidPoints(t, t1, t2, pCurve->GetTValues());
                            }

                            for(size_t i = 0 ; i < t.size() ; i++)
                                TriLoop.push_back(TTriLoopNode(pCurve->GetApproxXYZ(t[i]), pSurface));
                        }

                        if((pFin = pFin->forward) == pLoop->fin)
                            break;
                    }

                    if(!bValidFace)
                        break;

                    if(!TriLoop.empty()) // not point loop
                        TriLoops.push_back(TriLoop);
                }

                if(!bValidFace)
                    continue;

                size_t szBaseIndex = RFaces.size();

                TriangulateUV_Surface(RFaces, pSurface, cSurfaceSense, TriLoops);

                // Transforming
                if(pTransform)
                {
                    for(size_t i = szBaseIndex ; i < RFaces.size() ; i++)
                        RFaces[i] = (pTransform->rotation_matrix * RFaces[i] + pTransform->translation_vector) * pTransform->scale;
                }
            }
        }
    }
}

// -------
// REGION
// -------
void REGION::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (body);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (shell);
    Reader.ReadChar (type);
}

void REGION::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, body);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, shell);
}

// ------
// SHELL
// ------
void SHELL::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr(attributes_groups);
    Reader.ReadPtr(body);
    Reader.ReadPtr(next);
    Reader.ReadPtr(face);
    Reader.ReadPtr(edge);
    Reader.ReadPtr(vertex);
    Reader.ReadPtr(region);
    Reader.ReadPtr(front_face);
}

void SHELL::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, body);
    ResolvePtr(Map, next);
    ResolvePtr(Map, face);
    ResolvePtr(Map, edge);
    ResolvePtr(Map, vertex);
    ResolvePtr(Map, region);
    ResolvePtr(Map, front_face);
}

// -----
// FACE
// -----
void FACE::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadReal (tolerance);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (loop);
    Reader.ReadPtr  (shell);
    Reader.ReadPtr  (surface);
    Reader.ReadChar (sense);
    Reader.ReadPtr  (next_on_surface);
    Reader.ReadPtr  (previous_on_surface);
    Reader.ReadPtr  (next_front);
    Reader.ReadPtr  (previous_front);
    Reader.ReadPtr  (front_shell);
}

void FACE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, loop);
    ResolvePtr(Map, shell);
    ResolvePtr(Map, surface);
    ResolvePtr(Map, next_on_surface);
    ResolvePtr(Map, previous_on_surface);
    ResolvePtr(Map, next_front);
    ResolvePtr(Map, previous_front);
    ResolvePtr(Map, front_shell);
}

// -----
// LOOP
// -----
void LOOP::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr(attributes_groups);
    Reader.ReadPtr(fin);
    Reader.ReadPtr(face);
    Reader.ReadPtr(next);
}

void LOOP::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, fin);
    ResolvePtr(Map, face);
    ResolvePtr(Map, next);
}

// ----
// FIN
// ----
void FIN::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (loop);
    Reader.ReadPtr  (forward);
    Reader.ReadPtr  (backward);
    Reader.ReadPtr  (vertex);
    Reader.ReadPtr  (other);
    Reader.ReadPtr  (edge);
    Reader.ReadPtr  (curve);
    Reader.ReadPtr  (next_at_vx);
    Reader.ReadChar (sense);
}

void FIN::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, loop);
    ResolvePtr(Map, forward);
    ResolvePtr(Map, backward);
    ResolvePtr(Map, vertex);
    ResolvePtr(Map, other);
    ResolvePtr(Map, edge);
    ResolvePtr(Map, curve);
    ResolvePtr(Map, next_at_vx);
}

// -------
// VERTEX
// -------
void VERTEX::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (fin);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (point);
    Reader.ReadReal (tolerance);
    Reader.ReadPtr  (owner);
}

void VERTEX::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, fin);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, next);
    ResolvePtr(Map, point);
    ResolvePtr(Map, owner);
}

// -----
// EDGE
// -----
void EDGE::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadReal (tolerance);
    Reader.ReadPtr  (fin);
    Reader.ReadPtr  (previous);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (curve);
    Reader.ReadPtr  (next_on_curve);
    Reader.ReadPtr  (previous_on_curve);
    Reader.ReadPtr  (owner);
}

void EDGE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, fin);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, next);
    ResolvePtr(Map, curve);
    ResolvePtr(Map, next_on_curve);
    ResolvePtr(Map, previous_on_curve);
    ResolvePtr(Map, owner);
}

// -----
// LIST
// -----
void LIST::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr      (owner);
    Reader.ReadPtr      (next);
    Reader.ReadPtr      (previous);
    Reader.ReadEnum     (list_type);
    Reader.ReadInt      (list_length);
    Reader.ReadInt      (block_length);
    Reader.ReadInt      (size_of_entry);
    Reader.ReadPtr      (list_block);
    Reader.ReadPtr      (finger_block);
    Reader.ReadInt      (finger_index);
    Reader.ReadLogical  (notransmit);
}

void LIST::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, list_block);
    ResolvePtr(Map, finger_block);
}

// ------------------
// POINTER_LIS_BLOCK
// ------------------
void POINTER_LIS_BLOCK::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    Reader.ReadInt(n_entries);
    Reader.ReadPtr(next_block);

    entries.reserve(n);

    for( ; n ; n--)
        entries.push_back(Reader.ReadPtr<NODE>());
}

void POINTER_LIS_BLOCK::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    for(std::vector<NODE*>::iterator i = entries.begin() ; i != entries.end() ; ++i)
        ResolvePtr(Map, *i);
}

void POINTER_LIS_BLOCK::Triangulate(std::vector<vector>& RFaces, const TRANSFORM* pTransform, size_t szDepth) const
{
    RENDERABLE::Triangulate(RFaces, pTransform, szDepth);

    throw TException("POINTER_LIS_BLOCK rendering is not supported.");
}

// -----------
// ATT_DEF_ID
// -----------
void ATT_DEF_ID::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    String.reserve(n);

    for( ; n ; n--)
        String.push_back(Reader.ReadChar());
}

// ------------
// FIELD_NAMES
// ------------
void FIELD_NAMES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    names.reserve(n);

    for( ; n ; n--)
        names.push_back(Reader.ReadPtr<FIELD_NAME>());
}

void FIELD_NAMES::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    for(std::vector<FIELD_NAME*>::iterator i = names.begin() ; i != names.end() ; ++i)
        ResolvePtr(Map, *i);
}

// -----------
// ATTRIB_DEF
// -----------
void ATTRIB_DEF::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    Reader.ReadPtr(next);
    Reader.ReadPtr(identifier);
    Reader.ReadInt(type_id);

    for(size_t i = 0 ; i < (size_t)SCH_LE_max_logged_event ; i++)
        Reader.ReadEnum(actions[i]);

    for(size_t i = 0 ; i < (size_t)SCH_AO_max_owner ; i++)
        Reader.ReadLogical(legal_owners[i]);

    fields.reserve(n);

    for( ; n ; n--)
        fields.push_back(Reader.ReadEnum<SCH_field_type>());
}

void ATTRIB_DEF::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, next);
    ResolvePtr(Map, identifier);
}

// ----------
// ATTRIBUTE
// ----------
void ATTRIBUTE::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr(definition);
    Reader.ReadPtr(owner);
    Reader.ReadPtr(next);
    Reader.ReadPtr(previous);
    Reader.ReadPtr(next_of_type);
    Reader.ReadPtr(previous_of_type);

    fields.reserve(n);

    for( ; n ; n--)
        fields.push_back(Reader.ReadPtr<FIELD_VALUES>());
}

void ATTRIBUTE::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, definition);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, next_of_type);
    ResolvePtr(Map, previous_of_type);

    for(std::vector<FIELD_VALUES*>::iterator i = fields.begin() ; i != fields.end() ; ++i)
        ResolvePtr(Map, *i);
}

// -----------
// INT_VALUES
// -----------
void INT_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadInt());
}

// ------------
// REAL_VALUES
// ------------
void REAL_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadReal());
}

// ------------
// CHAR_VALUES
// ------------
void CHAR_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadChar());
}

// ---------------
// UNICODE_VALUES
// ---------------
void UNICODE_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadShort());
}

// -------------
// SHORT_VALUES
// -------------
void SHORT_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadShort());
}

// -------------
// POINT_VALUES
// -------------
void POINT_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadVector());
}

// --------------
// VECTOR_VALUES
// --------------
void VECTOR_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadVector());
}

// -----------------
// DIRECTION_VALUES
// -----------------
void DIRECTION_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadVector());
}

// ------------
// AXIS_VALUES
// ------------
void AXIS_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadVector());
}

// -----------
// TAG_VALUES
// -----------
void TAG_VALUES::LoadData(T_XT_Reader& Reader)
{
    size_t n = Reader.ReadInt();

    NODE::LoadData(Reader);

    values.reserve(n);

    for( ; n ; n--)
        values.push_back(Reader.ReadInt());
}

// ------
// GROUP
// ------
void GROUP::LoadData(T_XT_Reader& Reader)
{
    NODE_WITH_ID::LoadData(Reader);

    Reader.ReadPtr  (attributes_groups);
    Reader.ReadPtr  (owner);
    Reader.ReadPtr  (next);
    Reader.ReadPtr  (previous);
    Reader.ReadEnum (type);
    Reader.ReadPtr  (first_member);
}

void GROUP::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE_WITH_ID::ResolvePtrs(Map);

    ResolvePtr(Map, attributes_groups);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, first_member);
}

// ----------------
// MEMBER_OF_GROUP
// ----------------
void MEMBER_OF_GROUP::LoadData(T_XT_Reader& Reader)
{
    NODE::LoadData(Reader);

    Reader.ReadInt(dummy_node_id);
    Reader.ReadPtr(owning_group);
    Reader.ReadPtr(owner);
    Reader.ReadPtr(next);
    Reader.ReadPtr(previous);
    Reader.ReadPtr(next_member);
    Reader.ReadPtr(previous_member);
}

void MEMBER_OF_GROUP::ResolvePtrs(const TNodeIndexMap& Map)
{
    NODE::ResolvePtrs(Map);

    ResolvePtr(Map, owning_group);
    ResolvePtr(Map, owner);
    ResolvePtr(Map, next);
    ResolvePtr(Map, previous);
    ResolvePtr(Map, next_member);
    ResolvePtr(Map, previous_member);
}

// -------------
// Refined line
// -------------
struct TRefinedLine
{
public:
    vector p, d; // origin and direction

public:
    TRefinedLine(const vector& v1, const vector& v2)
    {
        p = v1, d = v2 - v1;

        // Bringing origin 'p' and direction 'd' to unique canonical form.
        // 'd' is scaled and oriented so that its first non-zero coordinate is one
        // 'p' is translated so that first non-zero coordinate in 'd' becomes zero in 'p'
        if(Sign(d.x))
        {
            d /= d.x;

            p -= d * p.x;
        }
        else if(Sign(d.y))
        {
            d /= d.y;

            p -= d * p.y;
        }
        else if(Sign(d.z))
        {
            d /= d.z;

            p -= d * p.z;
        }
        else
        {
            p.Set(0, 0, 0);
            d.Set(0, 0, 0);
        }
    }

    vector GetXYZ(double t) const
        {  return p + d * t; }

    double GetT(const vector& v) const
    {
        if(Sign(d.x))
            return (v.x - p.x) / d.x;

        if(Sign(d.y))
            return (v.y - p.y) / d.y;

        if(Sign(d.z))
            return (v.z - p.z) / d.z;

        return 0.0;
    }
};

inline int Compare(const TRefinedLine& l1, const TRefinedLine& l2)
{
    int d;

    // Ordering by origin
    if(d = Compare(l1.p.x, l2.p.x))
        return d;

    if(d = Compare(l1.p.y, l2.p.y))
        return d;

    if(d = Compare(l1.p.z, l2.p.z))
        return d;

    // Ordering by direction
    if(d = Compare(l1.d.x, l2.d.x))
        return d;

    if(d = Compare(l1.d.y, l2.d.y))
        return d;

    if(d = Compare(l1.d.z, l2.d.z))
        return d;

    return 0;
}

inline bool operator < (const TRefinedLine& l1, const TRefinedLine& l2)
{
    return Compare(l1, l2) < 0;
}

typedef std::set<double, precision_less> TRefinedLineParamSet;

typedef std::map<TRefinedLine, TRefinedLineParamSet> TRefinedLines;

// ----------------
// Global routines
// ----------------
void RefineCollinearEdges(std::vector<vector>& Faces)
{
    assert(!(Faces.size() % 3));

    std::vector<size_t> EdgeFaces                   (Faces.size()); // index of 1st face vertex (face-number * 3)
    std::vector<TRefinedLines::iterator> EdgeLines  (Faces.size()); // edge line iterator
    std::vector<double> EdgeT1                      (Faces.size()); // 1st vertex parameter along edge line
    std::vector<double> EdgeT2                      (Faces.size()); // 2nd vertex parameter along edge line

    TRefinedLines Lines;

    for(size_t i = 0 ; i < Faces.size() ; i++)
    {
        size_t v1 = i;
        size_t v2 = i % 3 == 2 ? i - 2 : i + 1;

        // Registering currently referenced face
        EdgeFaces[i] = i - i % 3;

        // Getting line of this edge and vertices parameters along this line
        TRefinedLine Line(Faces[v1], Faces[v2]);

        EdgeT1[i] = Line.GetT(Faces[v1]);
        EdgeT2[i] = Line.GetT(Faces[v2]);

        // Registering edge line
        EdgeLines[i] = Lines.insert(TRefinedLines::value_type(Line, TRefinedLineParamSet())).first;

        // Registering edge vertices on this line
        EdgeLines[i]->second.insert(EdgeT1[i]);
        EdgeLines[i]->second.insert(EdgeT2[i]);
    }

    size_t szOrigSize = Faces.size();

    for(size_t i = 0 ; i < szOrigSize ; i++)
    {
        std::vector<double> MidT;

        for(TRefinedLineParamSet::iterator j = EdgeLines[i]->second.upper_bound(Min(EdgeT1[i], EdgeT2[i])) ;
            j != EdgeLines[i]->second.end() && Compare(*j, Max(EdgeT1[i], EdgeT2[i])) < 0 ;
            ++j)
        {
            MidT.push_back(*j);
        }

        if(Compare(EdgeT1[i], EdgeT2[i]) > 0)
            std::reverse(MidT.begin(), MidT.end());

        size_t ei = i % 3; // this edge index within its face

        for(size_t j = 0 ; j < MidT.size() ; j++)
        {
            // Split-point coordinate
            vector v = EdgeLines[i]->first.GetXYZ(MidT[j]);

            // Creating new face
            size_t nf = Faces.size();
            Faces.resize(nf + 3);

            Faces[nf + (ei+0)%3] = v;
            Faces[nf + (ei+1)%3] = Faces[EdgeFaces[i] + (ei+1)%3];
            Faces[nf + (ei+2)%3] = Faces[EdgeFaces[i] + (ei+2)%3];

            // Updating current face
            Faces[EdgeFaces[i] + (ei+1)%3] = v;

            // Updating next edge face index if this edge is not last edge of the
            // face and if this is not one of new faces
            if(i % 3 < 2 && EdgeFaces[i] < szOrigSize)
                EdgeFaces[i+1] = nf;

            // Updating this edge face index
            EdgeFaces[i] = nf;
        }
    }
}

void SaveSTL_File(LPCTSTR pFileName, const std::vector<vector>& Faces)
{
    FILE* pFile = fopen(pFileName, "wb");
    char header[80];
    memset(header, 0, sizeof(header));
    fwrite(header, 1, sizeof(header), pFile);

    size_t szNFaces = Faces.size() / 3;
    fwrite(&szNFaces, sizeof(szNFaces), 1, pFile);

    size_t j = 0;

    for(size_t i = 0 ; i < szNFaces ; i++)
    {
        float nx = 0, ny = 0, nz = 0;

        float x1 = (float)Faces[j].x * 1000.0f;
        float y1 = (float)Faces[j].y * 1000.0f;
        float z1 = (float)Faces[j].z * 1000.0f;
        j++;

        float x2 = (float)Faces[j].x * 1000.0f;
        float y2 = (float)Faces[j].y * 1000.0f;
        float z2 = (float)Faces[j].z * 1000.0f;
        j++;

        float x3 = (float)Faces[j].x * 1000.0f;
        float y3 = (float)Faces[j].y * 1000.0f;
        float z3 = (float)Faces[j].z * 1000.0f;
        j++;

        unsigned short attr = 0;

        fwrite(&nx, sizeof(nx), 1, pFile);
        fwrite(&ny, sizeof(ny), 1, pFile);
        fwrite(&nz, sizeof(nz), 1, pFile);

        fwrite(&x1, sizeof(x1), 1, pFile);
        fwrite(&y1, sizeof(y1), 1, pFile);
        fwrite(&z1, sizeof(z1), 1, pFile);

        fwrite(&x2, sizeof(x2), 1, pFile);
        fwrite(&y2, sizeof(y2), 1, pFile);
        fwrite(&z2, sizeof(z2), 1, pFile);

        fwrite(&x3, sizeof(x3), 1, pFile);
        fwrite(&y3, sizeof(y3), 1, pFile);
        fwrite(&z3, sizeof(z3), 1, pFile);

        fwrite(&attr, sizeof(attr), 1, pFile);
    }

    fclose(pFile);
}

}
