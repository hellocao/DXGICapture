#ifndef __DXGICAPTURE_H__
#define __DXGICAPTURE_H__
#include <d3d11.h>
#include <dxgi1_2.h>
#define RELEASE_OBJECT(obj) {if(obj) obj->Release(); obj = NULL;}
class CDXGICapture
{
public:
	CDXGICapture(int maxWidth, int maxHeight);
	virtual ~CDXGICapture();

	virtual BOOL CaptureImage(void** pImageData, int* nLen);
private:
	BOOL Init();
	void DelInit();
	//Attach desktop to this thread
	static BOOL AttachToThread();
	BOOL QueryFrame(void** pImageData, int* nImageSize);
private:
	BOOL m_isInitSuccess;
	ID3D11Device*	m_pDevice;
	ID3D11DeviceContext* m_deviceContext;
	DXGI_OUTPUT_DESC m_dxgiOutputDesc;
	IDXGIOutputDuplication* m_pDeskDuplication;
	static BOOL s_isAttach;
	unsigned char* m_pImageData;
	int m_maxWidth;
	int m_maxHeight;
};

#endif