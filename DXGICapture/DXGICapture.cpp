#include "DXGICapture.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
BOOL CDXGICapture::s_isAttach = FALSE;
CDXGICapture::CDXGICapture(int maxWidth, int maxHeight):
m_isInitSuccess(FALSE),
m_pImageData(NULL)
{
	m_pImageData = new unsigned char[maxWidth * maxHeight * 4];
	Init();
}
CDXGICapture::~CDXGICapture()
{

}

BOOL CDXGICapture::Init()
{
	do
	{
		HRESULT hr = S_OK;
		if (m_isInitSuccess)
		{
			//已初始化
			break;
		}
		//Driver types supported 支持的设备类型
		//类型说明 https://docs.microsoft.com/zh-cn/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,	//硬件驱动，硬件支持所有Direct3D功能
			D3D_DRIVER_TYPE_WARP,		//WARP驱动，这是一个搞性能软件光栅，这个软件光栅支持9_1到10.1的功能级别
			D3D_DRIVER_TYPE_REFERENCE	//参考驱动程序，是支持每个Direct3D功能的软件实现
		};
		unsigned int  numDriverTypes = ARRAYSIZE(driverTypes);
		//描述Direct3D设备所针对的功能集 https://docs.microsoft.com/zh-cn/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,		//Direct3D 11.0支持的目标功能，包括着色器模型5
			D3D_FEATURE_LEVEL_10_1,		//Direct3D 10.1支持的目标功能，包括着色器模型4
			D3D_FEATURE_LEVEL_10_0,		//Direct3D 10.0支持的目标功能，包括着色器模型4
			D3D_FEATURE_LEVEL_9_1		//目标功能[功能级别]（/ windows / desktop / direct3d11 / overviews-direct3d-11-devices-downlevel-intro）9.1支持，包括着色器模型2
		};
		unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);
		D3D_FEATURE_LEVEL featureLevel;
		//1.Create D3D device 创建D3D设备
		/*注意   
		如果计算机上存在Direct3D 11.1运行时且pFeatureLevels设置为NULL，
		则此函数不会创建D3D_FEATURE_LEVEL_11_1设备。要创建D3D_FEATURE_LEVEL_11_1设备，
		必须显式提供包含D3D_FEATURE_LEVEL_11_1的D3D_FEATURE_LEVEL数组。如果在未安装
		Direct3D 11.1运行时的计算机上提供包含D3D_FEATURE_LEVEL_11_1的D3D_FEATURE_LEVEL数组，
		则此函数会立即失败并显示E_INVALIDARG*/
		for (int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
		{
			hr = D3D11CreateDevice(NULL, driverTypes[driverTypeIndex], NULL, 0, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_deviceContext);
			if (SUCCEEDED(hr))
			{
				break;
			}
		}
		if (FAILED(hr))
		{
			break;
		}
		//2.创建DXGI设备
		IDXGIDevice* pDxgiDevice = NULL;
		hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
		if (FAILED(hr))
		{
			break;
		}
		//3.获取DXGI adapter
		IDXGIAdapter* pDxgiAdapter = NULL;
		hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
		RELEASE_OBJECT(pDxgiDevice);
		if (FAILED(hr))
		{
			break;
		}
		//4.获取IDXGIOutput
		int nOutput = 0;
		IDXGIOutput* pDxgiOutput = NULL;
		//枚举适配器(视频卡)输出
		hr = pDxgiAdapter->EnumOutputs(nOutput, &pDxgiOutput);
		RELEASE_OBJECT(pDxgiAdapter);
		//5.获取DXGI_OUTPUT_DESC 参数
		pDxgiOutput->GetDesc(&m_dxgiOutputDesc);
		if (FAILED(hr))
		{
			break;
		}
		//6.获取IDXGIOutput1
		IDXGIOutput1* pDxgiOutput1 = NULL;
		hr = pDxgiOutput->QueryInterface(_uuidof(pDxgiOutput1), reinterpret_cast<void**>(&pDxgiOutput1));
		RELEASE_OBJECT(pDxgiOutput);
		if (FAILED(hr))
		{
			break;
		}
		//7.创建复制桌面
		hr = pDxgiOutput1->DuplicateOutput(m_pDevice, &m_pDeskDuplication);
		RELEASE_OBJECT(pDxgiOutput1);
		if (FAILED(hr))
		{
			break;
		}
		//初始化成功
		m_isInitSuccess = TRUE;
	} while (0);
	return m_isInitSuccess;

}

void CDXGICapture::DelInit()
{
	if (!m_isInitSuccess)
	{
		return;
	}
	m_isInitSuccess = FALSE;
	RELEASE_OBJECT(m_pDeskDuplication);
	RELEASE_OBJECT(m_pDevice);
	RELEASE_OBJECT(m_deviceContext)
}	
BOOL CDXGICapture::AttachToThread()
{
	if (s_isAttach)
	{
		return TRUE;
	}
	HDESK hCurrentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (!hCurrentDesktop)
	{
		return FALSE;
	}
	//Attach desktop to this thread
	BOOL isDesktopAttached = SetThreadDesktop(hCurrentDesktop);
	CloseDesktop(hCurrentDesktop);
	hCurrentDesktop = NULL;
	s_isAttach = TRUE;
	return isDesktopAttached;
}
BOOL CDXGICapture::QueryFrame(void** pImageData, int* nImageSize)
{

	*nImageSize = 0;
	if (!m_isInitSuccess || !AttachToThread())
	{
		return FALSE;
	}

	IDXGIResource* pDesktopResource = NULL;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	HRESULT hr = m_pDeskDuplication->AcquireNextFrame(0, &frameInfo, &pDesktopResource);
	if (FAILED(hr))
	{
		//在一些win10的系统上，如果桌面没有发生变化的情况
		return TRUE;
	}
	//Query next frame staging buffer
	ID3D11Texture2D* pAcquiredDesktopImage = NULL;
	hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pAcquiredDesktopImage));
	RELEASE_OBJECT(pDesktopResource);
	if (FAILED(hr))
	{
		return FALSE;
	}
	//copy old description
	D3D11_TEXTURE2D_DESC frameDescriptor;
	pAcquiredDesktopImage->GetDesc(&frameDescriptor);

	//create a new stging buffer for fill frame image
	ID3D11Texture2D* pNewDesktopImage = NULL;
	frameDescriptor.Usage = D3D11_USAGE_STAGING;
	frameDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	frameDescriptor.BindFlags = 0;
	frameDescriptor.MiscFlags = 0;
	frameDescriptor.MipLevels = 1;
	frameDescriptor.ArraySize = 1;
	frameDescriptor.SampleDesc.Count = 1;
	hr = m_pDevice->CreateTexture2D(&frameDescriptor, NULL, &pNewDesktopImage);
	if (FAILED(hr))
	{
		RELEASE_OBJECT(pAcquiredDesktopImage);
		m_pDeskDuplication->ReleaseFrame();
		return FALSE;
	}

	//copy next staging buffer to new staging buffer
	m_deviceContext->CopyResource(pNewDesktopImage, pAcquiredDesktopImage);
	RELEASE_OBJECT(pAcquiredDesktopImage);
	m_pDeskDuplication->ReleaseFrame();

	//create staging buffer for map bits
	IDXGISurface* pStagingSurf = NULL;
	hr = pNewDesktopImage->QueryInterface(__uuidof(IDXGISurface), (void**)(&pStagingSurf));
	RELEASE_OBJECT(pNewDesktopImage);
	if (FAILED(hr))
	{
		return FALSE;
	}
	//copy bits to user space 
	DXGI_MAPPED_RECT mappedRect;
	hr = pStagingSurf->Map(&mappedRect, DXGI_MAP_READ);
	if (SUCCEEDED(hr))
	{
		*nImageSize = m_dxgiOutputDesc.DesktopCoordinates.right * m_dxgiOutputDesc.DesktopCoordinates.bottom * 4;
		memcpy((BYTE*)m_pImageData, mappedRect.pBits, *nImageSize);
		*pImageData = m_pImageData;
		pStagingSurf->Unmap();
	}
	RELEASE_OBJECT(pStagingSurf);
	return SUCCEEDED(hr);
}

BOOL CDXGICapture::CaptureImage(void** pImageData, int* nLen)
{
	return QueryFrame(pImageData, nLen);
}