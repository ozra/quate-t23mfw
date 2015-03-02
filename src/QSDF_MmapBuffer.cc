#include "QSDF_MmapBuffer.hh"
#ifdef INTERFACE
/**
* Created:  2015-02-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

//#include <cstring>
//#include <stdint.h>

#include "rfx11_types.hh"
#include "QSDF_Utils.hh"

#include "sys/mman.h"

/*
# define MADV_NORMAL      0 / * No further special treatment.  * /
# define MADV_RANDOM      1 / * Expect random page references.  * /
# define MADV_SEQUENTIAL  2 / * Expect sequential page references.  * /
# define MADV_WILLNEED    3 / * Will need these pages.  * /
# define MADV_DONTNEED    4 / * Don't need these pages.  * /
# define MADV_REMOVE      9 / * Remove these pages and resources.  * /
# define MADV_DONTFORK    10    / * Do not inherit across fork.  * /
# define MADV_DOFORK      11    / * Do inherit across fork.  * /
# define MADV_MERGEABLE   12    / * KSM may merge identical pages.  * /
# define MADV_UNMERGEABLE 13    / * KSM may not merge identical pages.  * /
# define MADV_HUGEPAGE    14    / * Worth backing with hugepages.  * /
# define MADV_NOHUGEPAGE  15    / * Not worth backing with hugepages.  * /
# define MADV_DONTDUMP    16    / * Explicity exclude from the core dump,
                                   overrides the coredump filter bits.  * /
# define MADV_DODUMP      17    / * Clear the MADV_DONTDUMP flag.  * /
# define MADV_HWPOISON    100   / * Poison a page for testing.  * /
*/

class QSDF_MmapBuffer
{


};

#endif
