#include "pch.h"

#include <KFC_KTL/ktl_globals.h>
#include <KFC_KTL/module_globals.h>
#include <KFC_KTL/ktl_consts.h>
#include <KFC_Common/common_globals.h>
#include <KFC_Common/common_consts.h>
#include <KFC_Formats/formats_globals.h>
#include <KFC_CGI/cgi_globals.h>

#include "date_time_test.h"
#include "expand_test.h"
#include "fixed_item_heap_test.h"
#include "flexible_item_heap_test.h"
#include "avl_storage_test.h"
#include "fixed_item_heap_list_test.h"
#include "fixed_item_heap_avl_storage_test.h"
#include "file_chars_tree_test.h"
#include "persistent_fixed_item_heap_test.h"
#include "auto_holder_test.h"
#include "cgi_test.h"

// ----------------
// Project globals
// ----------------
class TProjectGlobals : public TModuleGlobals
{
public:
	TProjectGlobals() : TModuleGlobals(TEXT("Project globals"))
	{
		AddSubGlobals(g_KTL_Globals);
		AddSubGlobals(g_CommonGlobals);
		AddSubGlobals(g_FormatsGlobals);
		AddSubGlobals(g_CGI_Globals);
	}
};

TProjectGlobals g_ProjectGlobals;

// ----------
// Main proc
// ----------
int main(int argc, char* argv[])
{
	REPORT_MAIN_PROC_ARGS;

	TRACK_MEMORY_LEAKS;

	KFC_OUTER_BLOCK_BEGIN
	{
		g_KTL_Consts.m_bConsole = true;

		TGlobalsInitializer Initializer0(g_ProjectGlobals);

		srand(0x604);		

		TestDateTime();

		TestExpand();

		TestArrayFixedItemHeap		();
		TestDancerFixedItemHeap		();
		TestFileMappingFixedItemHeap();

		TestArrayFlexibleItemHeap		();
		TestDancerFlexibleItemHeap		();
		TestFileMappingFlexibleItemHeap	();

		Test_AVL_Storage();

		TestArrayFixedItemHeapList		();
		TestDancerFixedItemHeapList		();
		TestFileMappingFixedItemHeapList();

		TestArrayFixedItemHeapAVL_Storage		();
		TestDancerFixedItemHeapAVL_Storage		();
		TestFileMappingFixedItemHeapAVL_Storage	();

		TestFileCharsTree();

		TestPersistentFixedItemHeap();

		TestPtrAutoHolder			();
		TestListAutoHolder			();
		TestAVL_StorageAutoHolder	();

		TestCGI();
	}
	KFC_OUTER_BLOCK_END

	return 0;
}
