#include "pch.h"
#include "expand_test.h"

// ----------------
// Global routines
// ----------------
void TestExpand()
{
    puts("Testing _expand...");

    #ifdef _MSC_VER
    {
        size_t i, j;

        for(i=1;i<=256;i++)
        {
            for(j=1;j<=i;j++)
            {
                void* p = malloc(i);
                tassert(p);

                void* q = realloc(p, j);
                tassert(q==p);

                free(p);
            }

            for(j=1;j<=i;j++)
            {
                void* p = malloc(i);
                tassert(p);

                void* q = _expand(p, j);
                tassert(q==p);

                free(p);
            }

            {
                void* p = malloc(i);

                for(j=i-1;j>=1;j--)
                {
                    void* q = realloc(p, j);
                    tassert(q==p);
                }

                free(p);
            }

            {
                void* p = malloc(i);

                for(j=i-1;j>=1;j--)
                {
                    void* q = _expand(p, j);
                    tassert(q==p);
                }

                free(p);
            }
        }
    }
    #endif // _MSC_VER

    puts("Done");
}
