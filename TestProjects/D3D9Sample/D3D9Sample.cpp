#include <d3dx9.h>
#include <iostream>
#include <string>

using namespace std;

const unsigned int g_uFrameErrorMaxOutputTimes = 10;
unsigned int g_uFrameCount = 0;
bool		g_bFrameStarted = false;

IDirect3D9*						g_pD3D9;
IDirect3DDevice9*				g_pDevice;
IDirect3DVertexBuffer9*			g_pVertexBuffer;
IDirect3DIndexBuffer9*			g_pIndexBuffer;
LPD3DXEFFECT					g_pEffect;
IDirect3DVertexDeclaration9*	g_pVertexDeclaration;

IDirect3DVertexShader9*			g_pVertexShader;
ID3DXConstantTable*				g_pVertexConstantTable;
IDirect3DVertexShader9*			g_pVSTest;
ID3DXConstantTable*				g_pVSTestConstantTable;
IDirect3DPixelShader9*			g_pPixelShader;
ID3DXConstantTable*				g_pPixelConstantTable;

const D3DDEVTYPE			g_DeviceType				= D3DDEVTYPE_HAL;
const unsigned int			g_AdapterID					= D3DADAPTER_DEFAULT;
const D3DFORMAT				g_BackBufferFormat			= D3DFMT_A8R8G8B8;
const unsigned int			g_BackBufferCount			= 1;
const D3DMULTISAMPLE_TYPE	g_MultiSampleType			= D3DMULTISAMPLE_NONE;
const DWORD					g_MultiSampleQuality		= 0;
const D3DSWAPEFFECT			g_SwapEffect				= D3DSWAPEFFECT_DISCARD;
const bool					g_EnableAutoDepthStencil	= true;
const D3DFORMAT				g_AutoDepthStencilFormat	= D3DFMT_D24S8;
const D3DFORMAT				g_AutoDepthStencilFormat2	= D3DFMT_D16;
const DWORD					g_Flags						= 0;
const unsigned int			g_FullScreenRefreshRate		= D3DPRESENT_RATE_DEFAULT;
const unsigned int			g_PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;	// 垂直同步，开启：D3DPRESENT_INTERVAL_ONE，关闭：D3DPRESENT_INTERVAL_IMMEDIATE

const unsigned int			g_uVertexBufferSize = 4 * 1024;
const unsigned int			g_uIndexBufferSize = 4 * 1024;

const char*					g_EffectShaderPath = "Shader.bin";
const char*					g_VertexShaderPath = "Vertex.hlsl";
const char*					g_PixelShaderPath = "Pixel.hlsl";
const char*					g_VSConstantBufferTest = "VertexConstantBufferTest.hlsl";

struct PointLight
{
	float diffuse[3];
	float ambient[3];
	float position[3];
};

struct Material
{
	float baseColor[3];
	float emissiveColor[3];
	float normal[3];
	float metallic;
	float specular;
	float roughness;
	float opacity;
	float opacityMask;
};


struct VertexDataP3fU2f
{
	float position[3];
	float texCoord[2];
};

struct VertexDataP4fU2f
{
	float position[4];
	float texCoord[2];
};

const D3DVERTEXELEMENT9		g_VertexElements[] = {
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

HINSTANCE					g_hInstance;
HWND						g_hWnd;

const unsigned short		g_uWndX = 300;
const unsigned short		g_uWndY = 200;
const unsigned short		g_uWndWidth = 800;
const unsigned short		g_uWndHeight = 600;
const unsigned short		g_bWndFullScreen = false;

IDirect3D9*			InitD3D9();
IDirect3DDevice9*	InitDevice(IDirect3D9* pD3D9, HWND hWnd);
IDirect3DVertexBuffer9* InitVertexBuffer(IDirect3DDevice9* pDevice);
IDirect3DIndexBuffer9* InitIndexBuffer(IDirect3DDevice9* pDevice);
LPD3DXEFFECT InitEffect(IDirect3DDevice9* pDevice);
IDirect3DVertexDeclaration9* InitVertexDeclaration(IDirect3DDevice9* pDevice);

IDirect3DVertexShader9* InitVertexShader(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable);
IDirect3DVertexShader9* InitVertexShaderTest(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable);
IDirect3DPixelShader9* InitPixelShader(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable);

HINSTANCE			InitApplication();
HWND				InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK	ProcessMessage(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IDirect3D9* InitD3D9()
{
	IDirect3D9* pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3D9 == nullptr)
	{
		cout << "Init D3D9 failed." << endl;
	}

	return pD3D9;
}

IDirect3DDevice9* InitDevice(IDirect3D9* pD3D9, HWND hWnd)
{
	IDirect3DDevice9* pDevice = nullptr;

	D3DCAPS9 caps;
	pD3D9->GetDeviceCaps(g_AdapterID, g_DeviceType, &caps);

	cout << "VS.Version: " << hex << caps.VertexShaderVersion << endl;
	cout << "PS.Version: " << hex << caps.PixelShaderVersion << endl;

	unsigned int uVertexProcessType = 0;

	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		uVertexProcessType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		uVertexProcessType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS presentParam;
	presentParam.BackBufferWidth = g_uWndWidth;
	presentParam.BackBufferHeight = g_uWndHeight;
	presentParam.hDeviceWindow = hWnd;
	presentParam.Windowed = !g_bWndFullScreen;
	presentParam.BackBufferFormat = g_BackBufferFormat;
	presentParam.BackBufferCount = g_BackBufferCount;
	presentParam.MultiSampleType = g_MultiSampleType;
	presentParam.MultiSampleQuality = g_MultiSampleQuality;
	presentParam.SwapEffect = g_SwapEffect;
	presentParam.EnableAutoDepthStencil = g_EnableAutoDepthStencil;
	presentParam.AutoDepthStencilFormat = g_AutoDepthStencilFormat;
	presentParam.Flags = g_Flags;
	presentParam.FullScreen_RefreshRateInHz = g_FullScreenRefreshRate;
	presentParam.PresentationInterval = g_PresentationInterval;

	HRESULT hResult = pD3D9->CreateDevice(
		g_AdapterID,					// primary adapter
		g_DeviceType,					// device type
		hWnd,							// window associated with device
		uVertexProcessType,				// vertex processing
		&presentParam,					// present parameters
		&pDevice);						// return created device

	if (FAILED(hResult))
	{
		cout << "Init D3D Device failed - 1 : " << hResult << "\nTrying again..." << endl;

		// try again using a 16-bit depth buffer
		presentParam.AutoDepthStencilFormat = g_AutoDepthStencilFormat2;

		hResult = pD3D9->CreateDevice(
			g_AdapterID,
			g_DeviceType,
			hWnd,
			uVertexProcessType,
			&presentParam,
			&pDevice);

		if (FAILED(hResult))
		{
			cout << "Init D3D Device failed - 2 : " << hResult << endl;
		}
	}

	/*IDirect3DSwapChain9* pSwapChain1;
	IDirect3DSwapChain9* pSwapChain2;
	hResult = pDevice->GetSwapChain(0, &pSwapChain1);
	if (FAILED(hResult))
	{
		cout << "Get swap chain failed." << endl;
	}

	IDirect3DSurface9* pSurface;
	presentParam.hDeviceWindow = hWnd;
	hResult = pDevice->CreateAdditionalSwapChain(&presentParam, &pSwapChain2);
	if (FAILED(hResult))
	{
		cout << "Create swap chain failed." << endl;
	}
	pSwapChain2->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	pDevice->SetRenderTarget(0, pSurface);

	pSwapChain1->Release();
	pSwapChain1->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	pDevice->SetRenderTarget(0, pSurface);*/

	return pDevice;
}

IDirect3DVertexBuffer9* InitVertexBuffer(IDirect3DDevice9* pDevice)
{
	IDirect3DVertexBuffer9* pVertexBuffer = nullptr;

	HRESULT hResult = pDevice->CreateVertexBuffer(
		g_uVertexBufferSize,							// 缓冲区字节数
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,			// 缓冲区用途
		D3DFMT_VERTEXDATA,								// 顶点格式
		D3DPOOL_DEFAULT,								// 资源池类型
		&pVertexBuffer,									// 缓冲区地址
		nullptr);										// 共享句柄

	if (FAILED(hResult))
	{
		cout << "Create vertex buffer failed : " << hResult << endl;
	}

	return pVertexBuffer;
}

IDirect3DIndexBuffer9* InitIndexBuffer(IDirect3DDevice9* pDevice)
{
	IDirect3DIndexBuffer9* pIndexBuffer = nullptr;

	HRESULT hResult = pDevice->CreateIndexBuffer(
		g_uIndexBufferSize,								// 缓冲区字节数
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,			// 缓冲区用途
		D3DFMT_INDEX16,									// 索引格式
		D3DPOOL_DEFAULT,								// 资源池类型
		&pIndexBuffer,									// 缓冲区地址
		nullptr);										// 共享句柄

	if (FAILED(hResult))
	{
		cout << "Create index buffer failed : " << hResult << endl;
	}

	return pIndexBuffer;
}

LPD3DXEFFECT InitEffect(IDirect3DDevice9* pDevice)
{
	LPD3DXEFFECT pEffect = nullptr;
	LPD3DXBUFFER pErrorBuffer = nullptr;

	HRESULT hResult = D3DXCreateEffectFromFile(
		pDevice,					// D3D9Device接口指针
		g_EffectShaderPath,				// Shader资源文件路径
		nullptr,					// 宏定义声明（二进制文件不需要）
		nullptr,					// Include声明（二进制文件不需要）
		0,							// Flag
		nullptr,					// EffectPool指针
		&pEffect,					// Effect指针的指针
		&pErrorBuffer);				// 编译Error信息指针

	if (FAILED(hResult))
	{
		cout << "Create effect failed : " << hResult << ". ErrorMessage :\n" << pErrorBuffer;
	}

	return pEffect;
}

IDirect3DVertexDeclaration9* InitVertexDeclaration(IDirect3DDevice9* pDevice)
{
	IDirect3DVertexDeclaration9* pDeclaration = nullptr;

	HRESULT hResult = pDevice->CreateVertexDeclaration(g_VertexElements, &pDeclaration);

	if (FAILED(hResult))
	{
		cout << "Create vertex declaration failed : " << hResult << endl;;
	}

	return pDeclaration;
}

IDirect3DVertexShader9* InitVertexShader(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable)
{
	ID3DXBuffer* pShaderBuffer = nullptr;
	ID3DXBuffer* pErrorBuffer = nullptr;
	IDirect3DVertexShader9* pVertexShader = nullptr;

	HRESULT hResult = D3DXCompileShaderFromFile(
		g_VertexShaderPath,
		nullptr,
		nullptr,
		"BaseVS",
		"vs_3_0",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		&pShaderBuffer,
		&pErrorBuffer,
		ppConstantTable);
	if (FAILED(hResult))
	{
		cout << "Compile vertex shader failed : " << hResult << endl;
	}

	hResult = g_pDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pVertexShader);
	if (FAILED(hResult))
	{
		cout << "Create vertex shader failed_ : " << hResult << endl;
	}

	(*ppConstantTable)->SetDefaults(pDevice);

	if (pShaderBuffer)
	{
		pShaderBuffer->Release();
	}

	if (pErrorBuffer)
	{
		pErrorBuffer->Release();
	}

	return pVertexShader;
}

IDirect3DVertexShader9* InitVertexShaderTest(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable)
{
	ID3DXBuffer* pShaderBuffer = nullptr;
	ID3DXBuffer* pErrorBuffer = nullptr;
	IDirect3DVertexShader9* pVertexShader = nullptr;

	HRESULT hResult = D3DXCompileShaderFromFile(
		g_VSConstantBufferTest,
		nullptr,
		nullptr,
		"VSTest",
		"vs_3_0",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		&pShaderBuffer,
		&pErrorBuffer,
		ppConstantTable);
	if (FAILED(hResult))
	{
		cout << "Compile vertex shader failed : " << hResult << endl;
	}

	hResult = g_pDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pVertexShader);
	if (FAILED(hResult))
	{
		cout << "Create vertex shader failed : " << hResult << endl;
	}

	(*ppConstantTable)->SetDefaults(pDevice);

	if (pShaderBuffer)
	{
		pShaderBuffer->Release();
	}

	if (pErrorBuffer)
	{
		pErrorBuffer->Release();
	}

	return pVertexShader;
}

IDirect3DPixelShader9* InitPixelShader(IDirect3DDevice9* pDevice, ID3DXConstantTable** ppConstantTable)
{
	ID3DXBuffer* pShaderBuffer = nullptr;
	ID3DXBuffer* pErrorBuffer = nullptr;
	IDirect3DPixelShader9* pPixelShader = nullptr;

	HRESULT hResult = D3DXCompileShaderFromFile(
		g_PixelShaderPath,
		nullptr,
		nullptr,
		"BasePS",
		"ps_3_0",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		&pShaderBuffer,
		&pErrorBuffer,
		ppConstantTable);
	if (FAILED(hResult))
	{
		cout << "Compile pixel shader failed : " << hResult << endl;
	}

	hResult = g_pDevice->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pPixelShader);
	if (FAILED(hResult))
	{
		cout << "Create pixel shader failed : " << hResult << ". Error Info :\n" << pErrorBuffer << endl;
	}

	(*ppConstantTable)->SetDefaults(pDevice);

	if (pShaderBuffer)
	{
		pShaderBuffer->Release();
	}

	if (pErrorBuffer)
	{
		pErrorBuffer->Release();
	}

	return pPixelShader;
}

HINSTANCE InitApplication()
{
	return GetModuleHandle(nullptr);
}

HWND InitWindow(HINSTANCE hInstance)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ProcessMessage;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "MainWindow";
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	// Create window
	HWND hWnd = CreateWindow(
		"MainWindow",				// 窗口别名
		"D3D9 Sample",				// 窗口标题
		WS_OVERLAPPEDWINDOW,		// 窗口风格
		g_uWndX,					// 窗口左上角X坐标（基于屏幕像素）
		g_uWndY,					// 窗口左上角Y坐标（基于屏幕像素）
		g_uWndWidth,				// 窗口宽度
		g_uWndHeight,				// 窗口高度
		nullptr,					// 父窗口
		nullptr,					// 菜单
		hInstance,					// 程序实例的句柄
		nullptr);					// 参数

	if (hWnd == nullptr)
	{
		cout << "Init window failed." << endl;
	}

	if (g_bWndFullScreen)
	{
		// 设置全屏的过程中会令窗口显示
		HWND hDesk = GetDesktopWindow();
		RECT rect;
		GetWindowRect(hDesk, &rect);

		unsigned short uX = 0;
		unsigned short uY = 0;
		unsigned short uWidth = rect.right;
		unsigned short uHeight = rect.bottom;

		SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
		SetWindowPos(hWnd, HWND_TOPMOST, uX, uY, uWidth, uHeight, SWP_SHOWWINDOW);
	}
	else
	{
		ShowWindow(hWnd, SW_SHOW);
	}

	return hWnd;
}

LRESULT CALLBACK ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Check if the window is being closed.
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		
		return 0;

		// Check if a key has been released on the keyboard.
	case WM_KEYUP:
		
		return 0;

		// All other messages pass to the message handler in the system class.
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	g_hInstance = InitApplication();
	g_hWnd = InitWindow(g_hInstance);

	g_pD3D9 = InitD3D9();
	g_pDevice = InitDevice(g_pD3D9, g_hWnd);

	g_pVertexBuffer = InitVertexBuffer(g_pDevice);
	g_pIndexBuffer = InitIndexBuffer(g_pDevice);
	g_pVertexDeclaration = InitVertexDeclaration(g_pDevice);

	float fSize = 300;
	const unsigned int uVertexCount = 4;
	/*VertexDataP4fU2f vertices[uVertexCount] = {
		{ fSize,     fSize,     0.1, 1, 0,  0 },
		{ fSize,     fSize * 2, 0.1, 1, 0, -1 },
		{ fSize * 2, fSize * 2, 0.1, 1, 1, -1 },
		{ fSize * 2, fSize,     0.1, 1, 1,  0 }
	};*/

	VertexDataP3fU2f vertices[uVertexCount] = {
		{ 0, 0, 0, 0, 0 },
		{ 0, fSize, 0, 0, -1 },
		{ fSize, fSize, 0, 1, -1 },
		{ fSize, 0, 0, 1, 0 }
	};

	const unsigned int uIndexCount = 6;
	unsigned short indices[uIndexCount] = { 0, 1, 2, 2, 3, 0 };

	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	HRESULT hResult = g_pDevice->SetVertexDeclaration(g_pVertexDeclaration);
	/*HRESULT hResult = g_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	
	g_pDevice->SetVertexShader(NULL);
	g_pDevice->SetPixelShader(NULL);

	g_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	g_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
	g_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);*/

	void* pBuffer;

	hResult = g_pVertexBuffer->Lock(0, 0, &pBuffer, 0);
	memcpy_s(pBuffer, sizeof(vertices), vertices, sizeof(vertices));
	hResult = g_pVertexBuffer->Unlock();

	hResult = g_pIndexBuffer->Lock(0, 0, &pBuffer, 0);
	memcpy_s(pBuffer, sizeof(indices), indices, sizeof(indices));
	hResult = g_pIndexBuffer->Unlock();

	hResult = g_pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexDataP4fU2f));
	hResult = g_pDevice->SetIndices(g_pIndexBuffer);

	IDirect3DTexture9* pTexture;
	D3DXCreateTextureFromFile(g_pDevice, "C:/Users/CHENGXU95/Pictures/{31C19DC3-0758-4AEE-8BBA-0ABCC4918BB1}.jpg", &pTexture);

	g_pVertexShader = InitVertexShader(g_pDevice, &g_pVertexConstantTable);
	g_pVSTest = InitVertexShaderTest(g_pDevice, &g_pVSTestConstantTable);
	g_pPixelShader = InitPixelShader(g_pDevice, &g_pPixelConstantTable);

	g_pDevice->SetVertexShader(g_pVertexShader);
	g_pDevice->SetPixelShader(g_pPixelShader);

	/*D3DXHANDLE hTex = g_pPixelConstantTable->GetConstantByName(0, "sampler0");
	UINT count;
	D3DXCONSTANT_DESC TexDesc;
	g_pPixelConstantTable->GetConstantDesc(hTex, &TexDesc, &count);
	g_pDevice->SetTexture(TexDesc.RegisterIndex, pTexture);*/
	g_pDevice->SetTexture(0, pTexture);

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// BeginScene和EndScene需要在调用DP之前和之后调用，否则DP会抛出long at memory location异常，且无法正常绘制图像
			/*
			EndScene调用并成功返回后，表明所有的图元都已经从CPU提交到GPU，但不一定已经绘制完成，此时若马上调用Present函数会导致CPU被GPU阻塞，
			因此更好的做法是调用EndScene后，计算程序的其他逻辑，计算完后再调用Present。
			*/
			g_pDevice->BeginScene();

			hResult = g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 255, 0), 1.0f, 0);

			hResult = g_pDevice->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,		// 图元类型
				0,						// 从第几个顶点开始匹配0号索引
				0,						// 最小顶点索引（？）
				uVertexCount,			// 顶点流中的顶点个数
				0,						// 从第几个索引开始绘制
				uIndexCount / 3);		// 图元个数

			if (FAILED(hResult) && g_uFrameCount < g_uFrameErrorMaxOutputTimes)
			{
				cout << "Draw indexed primitive failed : " << hResult << endl;
				g_bFrameStarted = true;
			}
			g_pDevice->EndScene();

			hResult = g_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
		}


		if (g_bFrameStarted)
		{
			++g_uFrameCount;
		}
	}
}