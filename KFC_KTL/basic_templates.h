#ifndef basic_templates_h
#define basic_templates_h

// ----------
// Null type
// ----------
struct null_t {};

// ------------------
// Null virtual type
// ------------------
struct null_virt_t { virtual ~null_virt_t(); };

// ---------
// Min type
// ---------
template <  size_t _v0,
            size_t _v1,
            size_t _v2 = -1,
            size_t _v3 = -1,
            size_t _v4 = -1,
            size_t _v5 = -1,
            size_t _v6 = -1,
            size_t _v7 = -1>

class min_t
{
private:
    template <size_t a, size_t b>
    struct min2_t
    {
        enum { res = a < b ? a : b };
    };

public:
    enum { res =    min2_t<
                    min2_t<
                    min2_t<
                    min2_t<
                    min2_t<
                    min2_t<
                    min2_t<_v0,
                        _v1>::res,
                        _v2>::res,
                        _v3>::res,
                        _v4>::res,
                        _v5>::res,
                        _v6>::res,
                        _v7>::res};
};

// ---------
// Max type
// ---------
template <  size_t _v0,
            size_t _v1,
            size_t _v2 = 0,
            size_t _v3 = 0,
            size_t _v4 = 0,
            size_t _v5 = 0,
            size_t _v6 = 0,
            size_t _v7 = 0>

class max_t
{
private:
    template <size_t a, size_t b>
    struct max2_t
    {
        enum { res = a > b ? a : b };
    };

public:
    enum { res =    max2_t<
                    max2_t<
                    max2_t<
                    max2_t<
                    max2_t<
                    max2_t<
                    max2_t<_v0,
                        _v1>::res,
                        _v2>::res,
                        _v3>::res,
                        _v4>::res,
                        _v5>::res,
                        _v6>::res,
                        _v7>::res};
};

// ---------
// GCD type
// ---------
template <  size_t _v0,
            size_t _v1,
            size_t _v2 = 0,
            size_t _v3 = 0,
            size_t _v4 = 0,
            size_t _v5 = 0,
            size_t _v6 = 0,
            size_t _v7 = 0>

class gcd_t
{
private:
    template <size_t a, size_t b>
    struct gcd2_t
    {
        enum { res = gcd2_t<b, a % b>::res };
    };

    template <size_t a>
    struct gcd2_t<a, 0>
    {
        enum { res = a };
    };

public:
    enum { res =    gcd2_t<
                    gcd2_t<
                    gcd2_t<
                    gcd2_t<
                    gcd2_t<
                    gcd2_t<
                    gcd2_t<_v0,
                        _v1>::res,
                        _v2>::res,
                        _v3>::res,
                        _v4>::res,
                        _v5>::res,
                        _v6>::res,
                        _v7>::res };
};

// ---------
// LCM type
// ---------
template <  size_t _v0,
            size_t _v1,
            size_t _v2 = 1,
            size_t _v3 = 1,
            size_t _v4 = 1,
            size_t _v5 = 1,
            size_t _v6 = 1,
            size_t _v7 = 1>

class lcm_t
{
private:
    template <size_t a, size_t b>
    struct lcm2_t
    {
        enum { res = a * b / gcd_t<a, b>::res };
    };

public:
    enum { res =    lcm2_t<
                    lcm2_t<
                    lcm2_t<
                    lcm2_t<
                    lcm2_t<
                    lcm2_t<
                    lcm2_t<_v0,
                        _v1>::res,
                        _v2>::res,
                        _v3>::res,
                        _v4>::res,
                        _v5>::res,
                        _v6>::res,
                        _v7>::res };
};

// ---------------
// Alignment type
// ---------------
template <class t>
class alignment_t
{
#ifdef _MSC_VER
    private:
        struct s
        {
            char _;
            t o;
        };

    public:
        enum { res = offsetof(s, o) };
#else // _MSC_VER
    public:
        enum { res = 8 }; // 16 might be better in some cases
#endif // _MSC_VER
};

// ----------------
// Union size type
// ----------------
template <  class _t0,
            class _t1,
            class _t2 = null_t,
            class _t3 = null_t,
            class _t4 = null_t,
            class _t5 = null_t,
            class _t6 = null_t>

class union_size_t
{
private:
    enum { al_size = lcm_t< alignment_t<_t0>::res,
                            alignment_t<_t1>::res,
                            alignment_t<_t2>::res,
                            alignment_t<_t3>::res,
                            alignment_t<_t4>::res,
                            alignment_t<_t5>::res,
                            alignment_t<_t6>::res>::res };

    enum { max_size = max_t<sizeof(_t0),
                            sizeof(_t1),
                            sizeof(_t2),
                            sizeof(_t3),
                            sizeof(_t4),
                            sizeof(_t5),
                            sizeof(_t6)>::res};

public:
    enum { res = (max_size + al_size - 1) / al_size * al_size };
};

#endif // basic_templates_h
