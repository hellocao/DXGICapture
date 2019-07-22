#include "DXGICapture.h"
#include <stdio.h>

int main()
{
#if 0
	CDXGICapture* pCapture = new CDXGICapture(1920, 1080);
	FILE* pf = NULL;
	while (true)
	{
		char* pImageData = NULL;
		int nLen = 0;
		BOOL bRet = pCapture->CaptureImage((void**)&pImageData, &nLen);
		if (bRet && nLen > 0)
		{
			fopen_s(&pf, "testBGRA", "wb");
			fwrite(pImageData, 1, nLen, pf);
			fclose(pf);
		}
	}
#endif
	printf("Hello world\n");
	system("pause");
	return 0;
}