#include <d3dx9.h>
#include <iostream>

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

const char*					g_ShaderPath = "Shader.bin";

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


struct VertexData
{
	float position[3];
};

const D3DVERTEXELEMENT9		g_VertexElements[] = {
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
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
		g_ShaderPath,				// Shader资源文件路径
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
	//g_pEffect = InitEffect(g_pDevice);
	g_pVertexDeclaration = InitVertexDeclaration(g_pDevice);

	/*D3DXHANDLE hEPTransform				= g_pEffect->GetParameterByName(nullptr, "g_WVP");
	D3DXHANDLE hEPViewOppositeDirection	= g_pEffect->GetParameterByName(nullptr, "g_ViewOppositeDirection");
	D3DXHANDLE hEPMaterial				= g_pEffect->GetParameterByName(nullptr, "g_Material");
	D3DXHANDLE hEPPointLight			= g_pEffect->GetParameterByName(nullptr, "g_PointLight");

	if (hEPTransform == nullptr)
	{
		cout << "Get shader param handle failed : g_WVP." << endl;
	}
	if (hEPViewOppositeDirection == nullptr)
	{
		cout << "Get shader param handle failed : g_ViewOppositeDirection." << endl;
	}
	if (hEPMaterial == nullptr)
	{
		cout << "Get shader param handle failed : g_Material." << endl;
	}
	if (hEPPointLight == nullptr)
	{
		cout << "Get shader param handle failed : g_PointLight." << endl;
	}*/

	D3DXMATRIX transform = {
		-0.805069f,		0.0f,			0.0000006f,		0.0000006f,
		0.000000000,	1.073426f,		0.0f,			0.0f,
		-0.0000005f,	0.0f,			-0.999999f,		-0.999999f,
		-0.0000025f,	0.0f,			-4.999999f,		-4.999999f
	};

	float viewOppositeDirection[3] = { -6.27833003e-007, -0.000000000, 1.00000000 };

	Material material = {
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1},
		1,
		1,
		1,
		1,
		1
	};

	PointLight light = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

	//g_pEffect->SetValue(hEPTransform, &transform, sizeof(transform));
	//g_pEffect->SetValue(hEPViewOppositeDirection, &viewOppositeDirection, sizeof(viewOppositeDirection));
	//g_pEffect->SetValue(hEPMaterial, &material, sizeof(material));
	//g_pEffect->SetValue(hEPPointLight, &light, sizeof(light));

	const unsigned int uVertexCount = 4;
	VertexData vertices[uVertexCount] = {
		{  0,  0, 0 },
		{  0, 10, 0 },
		{ 10, 10, 0 },
		{ 10,  0, 0 }
	};

	const unsigned int uIndexCount = 6;
	unsigned short indices[uIndexCount] = { 0, 1, 2, 2, 3, 0 };

	HRESULT hResult = g_pDevice->SetVertexDeclaration(g_pVertexDeclaration);
	if (FAILED(hResult))
	{
		cout << "Set vertex declaration failed : " << hResult << endl;
	}

	void* pBuffer;

	hResult = g_pVertexBuffer->Lock(0, 0, &pBuffer, 0);
	if (FAILED(hResult))
	{
		cout << "Lock vertex buffer failed : " << hResult << endl;
	}

	memcpy_s(pBuffer, sizeof(vertices), vertices, sizeof(vertices));

	hResult = g_pVertexBuffer->Unlock();
	if (FAILED(hResult))
	{
		cout << "Unlock vertex buffer failed : " << hResult << endl;
	}

	hResult = g_pIndexBuffer->Lock(0, 0, &pBuffer, 0);
	if (FAILED(hResult))
	{
		cout << "Lock index buffer failed : " << hResult << endl;
	}

	memcpy_s(pBuffer, sizeof(indices), indices, sizeof(indices));

	hResult = g_pIndexBuffer->Unlock();
	if (FAILED(hResult))
	{
		cout << "Unlock index buffer failed : " << hResult << endl;
	}

	hResult = g_pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VertexData));
	if (FAILED(hResult))
	{
		cout << "Set stream source failed : " << hResult << endl;
	}

	hResult = g_pDevice->SetIndices(g_pIndexBuffer);
	if (FAILED(hResult))
	{
		cout << "Set indices failed : " << hResult << endl;
	}

	ID3DXBuffer* pShaderBuffer = nullptr;
	ID3DXBuffer* pErrorBuffer = nullptr;

	IDirect3DVertexShader9* pVertexShader;
	ID3DXConstantTable* pVertexConstantTable;

	hResult = D3DXCompileShaderFromFile(
		"Vertex.hlsl", 
		nullptr, 
		nullptr, 
		"BaseVS", 
		"vs_3_0", 
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
		&pShaderBuffer,
		&pErrorBuffer,
		&pVertexConstantTable);
	if (FAILED(hResult))
	{
		cout << "Compile vertex shader failed : " << hResult << endl;
	}

	hResult = g_pDevice->CreateVertexShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pVertexShader);
	if (FAILED(hResult))
	{
		cout << "Create vertex shader failed : " << hResult << endl;
	}

	pVertexConstantTable->SetDefaults(g_pDevice);

	pShaderBuffer->Release();

	IDirect3DPixelShader9* pPixelShader;
	ID3DXConstantTable* pPixelConstantTable;

	hResult = D3DXCompileShaderFromFile(
		"Pixel.hlsl",
		nullptr,
		nullptr,
		"BasePS",
		"ps_3_0",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		&pShaderBuffer,
		&pErrorBuffer,
		&pPixelConstantTable);
	if (FAILED(hResult))
	{
		cout << "Compile pixel shader failed : " << hResult << endl;
	}

	hResult = g_pDevice->CreatePixelShader((DWORD*)pShaderBuffer->GetBufferPointer(), &pPixelShader);
	if (FAILED(hResult))
	{
		cout << "Create pixel shader failed : " << hResult << endl;
	}

	pPixelConstantTable->SetDefaults(g_pDevice);

	g_pDevice->SetVertexShader(pVertexShader);
	g_pDevice->SetPixelShader(pPixelShader);

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
			hResult = g_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);

			/*unsigned int uPassCount;
			hResult = g_pEffect->Begin(&uPassCount, 0);
			if (FAILED(hResult) && g_uFrameCount < g_uFrameErrorMaxOutputTimes)
			{
				cout << "Begin effect failed : " << hResult << endl;
				g_bFrameStarted = true;
			}

			hResult = g_pEffect->BeginPass(0);
			if (FAILED(hResult) && g_uFrameCount < g_uFrameErrorMaxOutputTimes)
			{
				cout << "Begin pass failed : " << hResult << endl;
				g_bFrameStarted = true;
			}*/

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

			/*hResult = g_pEffect->EndPass();
			if (FAILED(hResult) && g_uFrameCount < g_uFrameErrorMaxOutputTimes)
			{
				cout << "End pass failed : " << hResult << endl;
				g_bFrameStarted = true;
			}

			hResult = g_pEffect->End();
			if (FAILED(hResult) && g_uFrameCount < g_uFrameErrorMaxOutputTimes)
			{
				cout << "End effect failed : " << hResult << endl;
				g_bFrameStarted = true;
			}*/

			hResult = g_pDevice->Present(nullptr, nullptr, nullptr, nullptr);
		}

		if (g_bFrameStarted)
		{
			++g_uFrameCount;
		}
	}
}