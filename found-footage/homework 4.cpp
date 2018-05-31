//--------------------------------------------------------------------------------------
// File: lecture 8.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "sound.h"




int dist(XMFLOAT3 pos1, XMFLOAT3 pos2) {
	float dis = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.z - pos2.z, 2));

	return (int)dis * 100;
}


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
// XMFLOAT3 a,b; a = a + b;
XMFLOAT3 operator+(const XMFLOAT3 lhs, XMFLOAT3 rhs)
{
	XMFLOAT3 res;
	res.x = lhs.x + rhs.x;
	res.y = lhs.y + rhs.y;
	res.z = lhs.z + rhs.z;
	return res;
}
// XMFLOAT3 a,b; a = a - b;
XMFLOAT3 operator-(const XMFLOAT3 lhs, XMFLOAT3 rhs)
{
	XMFLOAT3 res;
	res.x = lhs.x - rhs.x;
	res.y = lhs.y - rhs.y;
	res.z = lhs.z - rhs.z;
	return res;
}
//multiplication with matrix
// XMFLOAT3 a; XMMATRIX M;a = M*a;
XMFLOAT3 operator*(const XMMATRIX &lhs, XMFLOAT3 rhs)
{
	XMFLOAT3 res;

	XMVECTOR f = XMLoadFloat3(&rhs);
	f = XMVector3TransformCoord(f, lhs);
	XMStoreFloat3(&res, f);
	return res;
}
// XMFLOAT3 a; XMMATRIX M;a = M*a;
XMFLOAT3 operator*(XMFLOAT3 rhs, const XMMATRIX &lhs)
{
	XMFLOAT3 res;

	XMVECTOR f = XMLoadFloat3(&rhs);
	f = XMVector3TransformCoord(f, lhs);
	XMStoreFloat3(&res, f);
	return res;
}
// XMFLOAT3 a; a = a*0.01;
XMFLOAT3 operator*(const XMFLOAT3 lhs, float rhs)
{
	XMFLOAT3 res;
	res.x = lhs.x * rhs;
	res.y = lhs.y * rhs;
	res.z = lhs.z * rhs;
	return res;
}
// XMFLOAT3 a; a = 0.01*a;
XMFLOAT3 operator*(float rhs, const XMFLOAT3 lhs)
{
	XMFLOAT3 res;
	res.x = lhs.x * rhs;
	res.y = lhs.y * rhs;
	res.z = lhs.z * rhs;
	return res;
}



//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;
ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pVertexBuffer_sky = NULL;
ID3D11PixelShader*                  myPs = NULL;
ID3D11BlendState*					g_BlendState;

music_ music;
int track1;
int track2;
int track3;
int track4;
int track5;
int track6;

ID3D11Buffer*                       g_pVertexBuffer_3ds = NULL;
int									model_vertex_anz = 0;
static int cassette = 0;

//tapes vertex buffer and vertex count
ID3D11Buffer*                       g_pVertexBuffer_tape = NULL;
int									model_vertex_tape = 0;

//bed model vertex buffer, vertex count, and texture
ID3D11Buffer*                       g_pVertexBuffer_bed = NULL;
int									model_vertex_bed = 0;

//table
//bed model vertex buffer, vertex count, and texture
ID3D11Buffer*                       g_pVertexBuffer_table = NULL;
int									model_vertex_table = 0;

//tv
ID3D11Buffer*                       g_pVertexBuffer_tv = NULL;
int									model_vertex_tv = 0;

//sofa
ID3D11Buffer*                       g_pVertexBuffer_sofa = NULL;
int									model_vertex_sofa = 0;

//fridge
ID3D11Buffer*                       g_pVertexBuffer_fridge = NULL;
int									model_vertex_fridge = 0;

ID3D11Buffer*                       g_pVertexBuffer_stove = NULL;
int									model_vertex_stove = 0;

ID3D11Buffer*                       g_pVertexBuffer_oven = NULL;
int									model_vertex_oven = 0;

//toilet
ID3D11Buffer*                       g_pVertexBuffer_toilet = NULL;
int									model_vertex_toilet = 0;

//sink
ID3D11Buffer*                       g_pVertexBuffer_sink = NULL;
int									model_vertex_sink = 0;

//sink
ID3D11Buffer*                       g_pVertexBuffer_shower = NULL;
int									model_vertex_shower = 0;

//arrow
ID3D11Buffer*                       g_pVertexBuffer_arrow = NULL;
int									model_vertex_arrow = 0;

//states for turning off and on the depth buffer
ID3D11DepthStencilState			*ds_on, *ds_off;

ID3D11Buffer*                       g_pCBuffer = NULL;

ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*           enemy_texture = NULL;
ID3D11ShaderResourceView*           bullet_texture = NULL;
ID3D11ShaderResourceView*           g_pTextureWood = NULL;
ID3D11ShaderResourceView*           g_pTextureMetal = NULL;
ID3D11ShaderResourceView*           g_pTextureBed = NULL;


ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMFLOAT4                            g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );

camera								cam;
level								level1;
float x = 0.0;
float mx = 0.0;
static billboard bullet;
XMFLOAT3 bulletImpulse;
billboard enemy[5];
Tape tapes[5];
Player player;
Object beds[5];
Object tables[7];
Object chairs[6];
Object tvs[2];
Object allObjects[29];
int sofa_size = 2;
Object sofas[3];
Object fridge;
Object stove;
Object oven;
Object toilet;
Object sink;
Object shower;
billboard drop_zone;
bool sight = false;
bool sightCont = true;
Arrow arrow;


//nodes and node list for Dijkstras
node A, B, C, D, E, F, G, H, I, J, K, L, M;
vector<node*> nodelist;




//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;
	

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	/* ==================== Initializing stuff for Dijkstras ===========================*/
	//initialize nodes
	A.pos = XMFLOAT3(0.5, -.75, 11.5); //node in spawn pos
	B.pos = XMFLOAT3(0.75, -.75, 26); //node at door to backyard
	C.pos = XMFLOAT3(28, -0.75, 11.5);//node to the right hallway of the spawn
	D.pos = XMFLOAT3(28, -0.75, 24);//node outside kitchen
	E.pos = XMFLOAT3(28, -.75, 35);//ndoe outside bathroom
	F.pos = XMFLOAT3(-10.5, -0.75, 11.5);//node to left of spawn
	G.pos = XMFLOAT3(-0.5, -0.75, 45);//first nde in backyard
	H.pos = XMFLOAT3(18, -0.75, 45);//2nd node in backyard
	I.pos = XMFLOAT3(14.25, -0.75, 11.5);//in between A and C in the right hallway
	K.pos = XMFLOAT3(-10.5, -.75, 25.75);
	M.pos = XMFLOAT3(-0.5, -0.75, 37);
									//adding edges

	A.init_double_connection(&F, dist(A.pos, F.pos));
	A.init_double_connection(&I, dist(A.pos, I.pos));
	B.init_double_connection(&M, dist(B.pos, M.pos));
	B.init_double_connection(&A, dist(B.pos, A.pos));
	B.init_double_connection(&K, dist(B.pos, K.pos));
	C.init_double_connection(&D, dist(C.pos, D.pos));
	C.init_double_connection(&I, dist(C.pos, I.pos));
	D.init_double_connection(&E, dist(D.pos, E.pos));
	G.init_double_connection(&H, dist(G.pos, H.pos));
	G.init_double_connection(&M, dist(G.pos, M.pos));
	F.init_double_connection(&K, dist(F.pos, K.pos));

	//put nodes in list
	nodelist.push_back(&A);
	nodelist.push_back(&B);
	nodelist.push_back(&C);
	nodelist.push_back(&D);
	nodelist.push_back(&E);
	nodelist.push_back(&F);
	nodelist.push_back(&G);
	nodelist.push_back(&H);
	nodelist.push_back(&I);
	nodelist.push_back(&K);
	nodelist.push_back(&L);
	nodelist.push_back(&M);

	

	//===========================================================================================
	

	//first enemy
	enemy[0].position = XMFLOAT3(-.5, -.75, 25.75);
	enemy[0].curr = &B;
	enemy[0].scale = .5;
	
	//second enemy
	enemy[1].position = XMFLOAT3(28, -0.75, 11.5);
	enemy[1].curr = &C;
	enemy[1].scale = .5;
	
	//third enemy
	enemy[2].position = XMFLOAT3(28, -.75, 35);;
	enemy[2].curr = &E;
	enemy[2].scale = .5;

	//fourth enemy
	enemy[3].position = XMFLOAT3(18, -0.75, 45);;
	enemy[3].curr = &H;
	enemy[3].scale = .5;
	

	//fifth enemy
	enemy[4].position = XMFLOAT3(-10.5, -0.75, 11.5);//node to left of spawn
	enemy[4].curr = &F;
	enemy[4].scale = .5;

	

	
	//tape 1
	tapes[0].pos = XMFLOAT3(-0.5, -0.75, 37);
	tapes[0].scale = .03;

	
	//tape 2
	tapes[1].pos.x = -8.0;
	tapes[1].pos.z = 10.0;
	tapes[1].pos.y = -.75;
	tapes[1].scale = .03;

	//tape 3
	
	tapes[2].pos.x = 20.0;
	tapes[2].pos.z = 35.0;
	tapes[2].pos.y = -.75;
	tapes[2].scale = .03;

	//tape 2
	tapes[3].pos.x = 18.0;
	tapes[3].pos.z = 25.0;
	tapes[3].pos.y = -.75;
	tapes[3].scale = .03;

	//tape 2
	tapes[4].pos.x = 36.5;
	tapes[4].pos.z = 33.0;
	tapes[4].pos.y = -.75;
	tapes[4].scale = .03;

	//init beds
	beds[0].pos = XMFLOAT3(10.0, -21, -1.3);
	beds[1].pos = XMFLOAT3(-5.75, -18, -1.3);
	beds[2].pos = XMFLOAT3(-4.5, -22, -1.3);
	beds[3].pos = XMFLOAT3(-4, -28, -1.3);
	beds[4].pos = XMFLOAT3(10, -26.5, -1.3);
	for (int i = 0; i < 5; i++) {
		beds[i].rot = XMFLOAT3(0, 0, 0);
		beds[i].scale = 1.3;
	}
	beds[3].rot.z = XM_PI / 2;
	allObjects[0] = beds[0];
	allObjects[1] = beds[1];
	allObjects[2] = beds[2];

	//inint tables
	tables[0].pos = XMFLOAT3(8.0, -15.5, -.5);
	tables[1].pos = XMFLOAT3(-12, -18, -.5);
	tables[2].pos = XMFLOAT3(-16, -18, -.5);
	tables[3].pos = XMFLOAT3(-11, -28, -.5);
	tables[4].pos = XMFLOAT3(9.0, -36, -.5);
	tables[5].pos = XMFLOAT3(20, -20, -.5);
	tables[6].pos = XMFLOAT3(37, -27.5, -.5);
	for (int i = 0; i < 7; i++) {
		tables[i].scale = .003;
		tables[i].rot = XMFLOAT3(0, 0, 0);
	}
	tables[5].scale = .006;
	tables[2].rot.z = XM_PI / 2;
	allObjects[3] = tables[0];
	allObjects[4] = tables[1];
	allObjects[5] = tables[2];

	//init chairs
	chairs[0].pos = XMFLOAT3(-11, -.75, 10);
	chairs[1].pos = XMFLOAT3(-11.25, -.75, 27);
	chairs[2].pos = XMFLOAT3(19.75, -.75, 20.75);
	chairs[3].pos = XMFLOAT3(20.75, -.75, 20.75);
	chairs[4].pos = XMFLOAT3(19, -.75, 19.25);
	chairs[5].pos = XMFLOAT3(20.25, -.75, 19.25);
	for (int i = 0; i < 6; i++) {
		chairs[i].scale = .05;
		chairs[i].rot = XMFLOAT3(0, 0, 0);
	}
	chairs[1].rot.y = XM_PI;
	chairs[4].rot.y = XM_PI;
	chairs[5].rot.y = XM_PI;

	allObjects[6] = chairs[0];

	//init tvs
	tvs[0].pos = XMFLOAT3(-16, -18, -.5);
	tvs[1].pos = XMFLOAT3(9, -34, -1.0);
	for (int i = 0; i < 2; i++) {
		tvs[i].scale = .03;
		tvs[i].rot = XMFLOAT3(0, 0, 0);
	}
	tvs[1].rot.z = XM_PI / 2;

	//sofas
	
	sofas[0].pos = XMFLOAT3(10.0, -37.0, -.8);
	sofas[2].pos = XMFLOAT3(22, -28, -.8);
	for (int i = 0; i < sofa_size; i++) {
		sofas[i].scale = .02;
		sofas[i].rot = XMFLOAT3(0, 0, 0);
	}
	sofas[0].rot.z = XM_PI;
	sofas[2].rot.z = XM_PI;
	
	//fridge
	fridge.pos = XMFLOAT3(24, -15.5, -1.0);
	fridge.rot = XMFLOAT3(0, 0, 0);
	fridge.scale = .0009;

	//stove
	stove.scale = .0115;
	stove.pos = XMFLOAT3(22, -15.5, -1.0);

	//oven
	oven.pos = XMFLOAT3(20, -15.5, -.75);
	oven.rot = XMFLOAT3(0, 0, 0);
	oven.scale = .001;

	//toilet
	toilet.pos = XMFLOAT3(33.5, -30, -1.0);
	toilet.rot = XMFLOAT3(0, 0, 0);
	toilet.scale = .005;

	//sink
	sink.pos = XMFLOAT3(33.2, -32, -.75);
	sink.rot = XMFLOAT3(0, 0, 0);
	sink.scale = .02;
	sink.rot.z = XM_PI / 2;

	//shower
	shower.pos = XMFLOAT3(42.5, -32, -.3);
	shower.rot = XMFLOAT3(0, 0, 0);
	shower.scale = .0005;
	shower.rot.z = XM_PI;

	//drop zone
	drop_zone.position = XMFLOAT3(0, 0, 0);
	drop_zone.rotation = XMFLOAT3(0, 0, 0);
	
	track1 = music.init_music("static.wav");

	


	allObjects[7] = tvs[0];
	allObjects[8] = beds[3];
	allObjects[9] = tables[3];
	allObjects[10] = chairs[1];
	allObjects[11] = sofas[0];
	allObjects[12] = tables[4];
	allObjects[13] = tvs[1];
	allObjects[14] = beds[4];
	allObjects[15] = fridge;
	allObjects[16] = stove;
	allObjects[17] = oven;
	allObjects[18] = tables[5];
	allObjects[19] = chairs[2];
	allObjects[20] = chairs[3];
	allObjects[21] = chairs[4];
	allObjects[22] = chairs[5];
	allObjects[23] = sofas[2];
	allObjects[24] = toilet;
	allObjects[25] = sink;
	allObjects[26] = shower;
	allObjects[27] = tables[6];

    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
		return hr;

	
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

	  

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"shader.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"shader.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

	hr = CompileShaderFromFile(L"shader.fx", "PS2", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &myPs);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
	//create skybox vertex buffer
	SimpleVertex vertices_skybox[] =
		{
		//top
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.5f, 0.0f) },
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.5f, 0.356f)  },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, .356f)  },
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.25f, 0.356f) },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(.25f, .0f) },
				{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, .5f)  },
		//bottom
				{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(.3f, .731f) },
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.5f, .731f)  },
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.5f, 1.0f) },				
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		//left
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)  },
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },				
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)  },
		//right
				{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },				
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },				
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)  },
		//back
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f)  },
				{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)  },
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
				{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
				{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		//front
		//COLLISION DETECTION
		//check value r for where you are at, pos=pos.imp apply to temp, check to see if temp is outside of space.
		//if it isn't valid don't apply the impulse. can calculate to the wall(not necessary)
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.25f, 0.333f) },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.333f) },
				{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.5, 0.666f) },
				{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.5f, 0.666f) },
				{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.25f, 0.666f) },
				{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.25f, 0.333f)  },
		};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 36;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices_skybox;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_sky);
	if (FAILED(hr))
		return hr;
    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { XMFLOAT3( -1.0f, -1.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 0.0f ), XMFLOAT2( 0.0f, 0.0f ) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3( -1.0f, 1.0f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) },

		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },		
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },		
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }

    };


    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 12;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;
	

	//chair model
	LoadOBJ("chair.obj", g_pd3dDevice, &g_pVertexBuffer_3ds, &model_vertex_anz);

	LoadOBJ("arrow.obj", g_pd3dDevice, &g_pVertexBuffer_arrow, &model_vertex_arrow);

	//tape model
	Load3DS("tape.3ds", g_pd3dDevice, &g_pVertexBuffer_tape, &model_vertex_tape);

	Load3DS("cama.3ds", g_pd3dDevice, &g_pVertexBuffer_bed, &model_vertex_bed);

	Load3DS("table.3ds", g_pd3dDevice, &g_pVertexBuffer_table, &model_vertex_table);

	Load3DS("tv.3DS", g_pd3dDevice, &g_pVertexBuffer_tv, &model_vertex_tv);

	Load3DS("sofa.3DS", g_pd3dDevice, &g_pVertexBuffer_sofa, &model_vertex_sofa);

	Load3DS("fridge.3DS", g_pd3dDevice, &g_pVertexBuffer_fridge, &model_vertex_fridge);

	Load3DS("gasstove.3DS", g_pd3dDevice, &g_pVertexBuffer_stove, &model_vertex_stove);

	Load3DS("range.3ds", g_pd3dDevice, &g_pVertexBuffer_oven, &model_vertex_oven);

	Load3DS("Toilet.3DS", g_pd3dDevice, &g_pVertexBuffer_toilet, &model_vertex_toilet);

	Load3DS("Sink.3DS", g_pd3dDevice, &g_pVertexBuffer_sink, &model_vertex_sink);

	Load3DS("Shower.3DS", g_pd3dDevice, &g_pVertexBuffer_shower, &model_vertex_shower);


	//blendstate:
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = TRUE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	g_pd3dDevice->CreateBlendState(&blendStateDesc, &g_BlendState);




	float blendFactor[] = { 0, 0, 0, 0 };
	UINT sampleMask = 0xffffffff;
	g_pImmediateContext->OMSetBlendState(g_BlendState, blendFactor, sampleMask);


    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

 
    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBuffer);
    if( FAILED( hr ) )
        return hr;
    

    // Load the Texture
    hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, L"nightsky.jpg", NULL, NULL, &g_pTextureRV, NULL );
    if( FAILED( hr ) )
        return hr;
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"pokeball.png", NULL, NULL, &bullet_texture, NULL);
	if (FAILED(hr))
		return hr;
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"doot copy.png", NULL, NULL, &enemy_texture, NULL);
	if (FAILED(hr))
		return hr;

	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"wood.jpg", NULL, NULL, &g_pTextureWood, NULL);
	if (FAILED(hr))
		return hr;

	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"metal.jpg", NULL, NULL, &g_pTextureMetal, NULL);
	if (FAILED(hr))
		return hr;

	

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet( 0.0f, 0.0f, -6.0f, 0.0f );//camera position
    XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );//look at
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );// normal vector on at vector (always up)
    g_View = XMMatrixLookAtLH( Eye, At, Up );

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f);

	ConstantBuffer constantbuffer;
	constantbuffer.View = XMMatrixTranspose( g_View );
	constantbuffer.Projection = XMMatrixTranspose(g_Projection);
	constantbuffer.World = XMMatrixTranspose(XMMatrixIdentity());
	constantbuffer.green = 0;
	constantbuffer.green_col - 0.0;
    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0 );


	//create the depth stencil states for turning the depth buffer on and of:
	D3D11_DEPTH_STENCIL_DESC		DS_ON, DS_OFF;
	DS_ON.DepthEnable = true;
	DS_ON.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DS_ON.DepthFunc = D3D11_COMPARISON_LESS;
	// Stencil test parameters
	DS_ON.StencilEnable = true;
	DS_ON.StencilReadMask = 0xFF;
	DS_ON.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing
	DS_ON.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DS_ON.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DS_ON.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DS_ON.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	DS_ON.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DS_ON.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DS_ON.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DS_ON.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Create depth stencil state
	DS_OFF = DS_ON;
	DS_OFF.DepthEnable = false;
	g_pd3dDevice->CreateDepthStencilState(&DS_ON, &ds_on);
	g_pd3dDevice->CreateDepthStencilState(&DS_OFF, &ds_off);

	level1.init("level.bmp");
	level1.init_texture(g_pd3dDevice, L"wall3.jpg");//255
	level1.init_texture(g_pd3dDevice, L"wall4.jpg");//254
	level1.init_texture(g_pd3dDevice,L"grass.jpg");//253
	level1.init_texture(g_pd3dDevice, L"floor2.jpg");//252
	level1.init_texture(g_pd3dDevice, L"ceiling.jpg");//251
	level1.init_texture(g_pd3dDevice, L"wall5.jpg");//250
	
	music.play(track1);
    return S_OK;

	
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pSamplerLinear ) g_pSamplerLinear->Release();
    if( g_pTextureRV ) g_pTextureRV->Release();
    if(g_pCBuffer) g_pCBuffer->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}
///////////////////////////////////
//		This Function is called every time the Left Mouse Button is down
///////////////////////////////////
void OnLBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
	{
	
	XMFLOAT3 shootDirection = XMFLOAT3(cam.position.x, 0, cam.position.z);
	XMMATRIX R = cam.get_matrix(&g_View);
	R._41 = R._42 = R._43 = 0.0;
	R = XMMatrixTranspose(R);

	shootDirection = shootDirection*R;

	bulletImpulse = shootDirection;
	bullet.position.x = (cam.position.x*-.5);
	bullet.position.z = (cam.position.z*-.78);
	}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is down
///////////////////////////////////
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
	{
	
	}
///////////////////////////////////
//		This Function is called every time a character key is pressed
///////////////////////////////////
void OnChar(HWND hwnd, UINT ch, int cRepeat)
	{

	}
///////////////////////////////////
//		This Function is called every time the Left Mouse Button is up
///////////////////////////////////
void OnLBU(HWND hwnd, int x, int y, UINT keyFlags)
	{


	}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is up
///////////////////////////////////
void OnRBU(HWND hwnd, int x, int y, UINT keyFlags)
	{


	}
///////////////////////////////////
//		This Function is called every time the Mouse Moves
///////////////////////////////////
bool mouse = false;
void OnMM(HWND hwnd, int x, int y, UINT keyFlags)
	{
	static int lastx = x;
	if (mouse) {
		mouse = false;
		lastx = x;
	}
	else {
		int diff = lastx - x;
		cam.rotation.y += diff*0.01;
		lastx = x;

		RECT window;

		GetWindowRect(hwnd, &window);
		SetCursorPos((window.left + window.right) / 2, (window.top + window.bottom / 2));
		mouse = true;
	}
	if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
		{
		}

	if ((keyFlags & MK_RBUTTON) == MK_RBUTTON)
		{
		}
	
	}


BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
	{
	cam.position.x = -0.5;
	cam.position.y = 0.0;
	cam.position.z = -10.0;
	return TRUE;
	}
void OnTimer(HWND hwnd, UINT id)
	{

	}
//*************************************************************************
void OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{
	switch (vk)
		{
			case 65:cam.a = 0;//a
				break;
			case 68: cam.d = 0;//d
				break;
			case 32: //space
				
				

				break;
			case 87: cam.w = 0; //w
				break;
			case 83:cam.s = 0; //s
			default:break;

		}

	}

void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
	{
		XMFLOAT3 temp = cam.position;
		temp.x = -cam.position.x;
		temp.y = -cam.position.y;
		temp.z = -cam.position.z - 5;

	switch (vk)
		{

			default:break;
			case 65:
				cam.a = 1;//a
				break;
			case 68: cam.d = 1;//d
				break;
			case 32: //space

				for (int i = 0; i < 5; i++) {
					//pciks up tape if it is in range of the player and the player is not already holding the tape and thhe tape isnt completed
					if (tapes[i].inRadius == true && !player.holdingTape && !tapes[i].completed) {
						music.play_fx("pickup.wav");
						player.holdingTape = true;	//player now holding tape
						tapes[i].pickedUp = true;	//set tape to picked up state
						player.heldTape = &tapes[i];	//set tehe players held tape to this tape
						arrow.find_closest(tapes);
						/////////////////////////
						break;
					}

					else if (player.holdingTape == true) {
						
						player.heldTape->drop(cam.position, cam.rotation.y, level1.get_bitmap());	//drop the tape the player is holding
						if (player.heldTape->completed)	//if this tape was dropped in the droped zone increemnt completed tapes
							player.completedTapes++;
						player.heldTape = NULL;	//reset players held tape
						player.holdingTape = false;	//player not holding tape
						music.play_fx("putdown.wav");
						sightCont = true;
						break;

					}
				}
				
			break;
			case 87: cam.w = 1; //w
				break;
			case 83:cam.s = 1; //s
				break;
			case 27: 
				PostQuitMessage(0);//escape
				break;
		}
	}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
#include <windowsx.h>
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
	HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLBD);
	HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLBU);
	HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMM);
	HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
	HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
	HANDLE_MSG(hWnd, WM_KEYDOWN, OnKeyDown);
	HANDLE_MSG(hWnd, WM_KEYUP, OnKeyUp);
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}



//--------------------------------------------------------------------------------------
// sprites
//--------------------------------------------------------------------------------------
class sprites
	{
	public:
		XMFLOAT3 position;
		XMFLOAT3 impulse;
		float rotation_x;
		float rotation_y;
		float rotation_z;
		sprites()
			{
			impulse = position = XMFLOAT3(0, 0, 0);
			rotation_x = rotation_y = rotation_z;
			}
		XMMATRIX animation() 
			{
			//update position:
			position.x = position.x + impulse.x; //newtons law
			position.y = position.y + impulse.y; //newtons law
			position.z = position.z + impulse.z; //newtons law

			XMMATRIX M;
			//make matrix M:
			XMMATRIX R,Rx,Ry,Rz,T;
			T = XMMatrixTranslation(position.x, position.y, position.z);
			Rx = XMMatrixRotationX(rotation_x);
			Ry = XMMatrixRotationX(rotation_y);
			Rz = XMMatrixRotationX(rotation_z);
			R = Rx*Ry*Rz;
			M = R*T;
			return M;
			}
	};
sprites mario;

//--------------------------------------------------------------------------------------
// Renderw a frame
//--------------------------------------------------------------------------------------

void sort_enemies(billboard *b, camera c) {
	XMFLOAT3 camPos;
	billboard tempB[5];
	camPos.x = -c.position.x;
	camPos.y = -c.position.y;
	camPos.z = -c.position.z - 5;

	float distances[5];
	float d;
	int min;
	int tempf;
	billboard tempb;

	//get distance of each billboard to the camera;
	for (int i = 0; i < 5; i++) {
		d = sqrt(pow(b->position.x - camPos.x, 2) + pow(b->position.z - camPos.z, 2));
		distances[i] = d;
	}

	for (int j = 0; j < 5; j++) {
		min = j;

		for (int i = j + 1; i < 5; i++) {
			if (distances[i] < distances[j])
				min = i;
		}

		if (min != j) {
			tempf = distances[min];
			distances[min] = distances[j];
			distances[j] = tempf;

			tempb = b[min];
			b[min] = b[j];
			b[j] = tempb;
		}
	}
}

void Render()
{

	
	
UINT stride = sizeof(SimpleVertex);
UINT offset = 0;
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // Update our time
    static float t = 0.0f;
	t += 0.001;
	
    // Clear the back buffer
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	cam.animation(level1.get_bitmap(), allObjects,29,tapes);//pass the bitmap to the camera *NEW*

	XMMATRIX view = cam.get_matrix(&g_View);

	static float green = .001;
	green += .005;
	if (green >= 1.0)
		green = 0.0;
	float col = sin(green);
	
	/*
	float prevPos;
	for (int i = 0; i < 5; i++) {
		
		if (enemy[i].hasLineOfSight(cam.position, view, level1.get_bitmap()) && enemy[i].draw == true)
			sight = true;
			enemy[i].forward(view, cam.position, level1.get_bitmap());

	}
	*/

	/*
	node *playerNode = cam.get_closest(nodelist);
	for (int i = 0; i < 5; i++) {
		if (enemy[i].going_to == NULL) {
			enemy[i].pathfind(nodelist,playerNode);
		}
		else if (enemy[i].going_to) {
			playerNode = cam.get_closest(nodelist);
			enemy[i].atNextNode(nodelist,playerNode);
			if (enemy[i].traveling)
				enemy[i].updateTravel(nodelist,playerNode);
			else {
				node *playerNode = cam.get_closest(nodelist);
				enemy[i].pathfind(nodelist, playerNode);
			}
		}
	}
	*/
	
	
	for (int i = 0; i < 5; i++) {
		node *playerNode = cam.get_closest(nodelist);
		enemy[i].pathfind(nodelist, playerNode, cam.position);
	}


    // Update skybox constant buffer
    ConstantBuffer constantbuffer;
	XMMATRIX WSK = XMMatrixTranslation(-cam.position.x, cam.position.y, -cam.position.z-6);
	constantbuffer.World	=	XMMatrixTranspose(WSK);
	constantbuffer.View		=	XMMatrixTranspose(view);
	constantbuffer.Projection = XMMatrixTranspose(g_Projection);
    g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0 );
	
    // Render skybox
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);	
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBuffer);   
    g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sky, &stride, &offset);
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );

	g_pImmediateContext->OMSetDepthStencilState(ds_off, 1);
    g_pImmediateContext->Draw( 36, 0 );

	g_pImmediateContext->OMSetDepthStencilState(ds_on, 1);

	//Y movement
	XMMATRIX TM = XMMatrixTranslation(0, 0, 5);
	XMMATRIX RotY = XMMatrixRotationY(t);
	constantbuffer.World = XMMatrixTranspose(XMMatrixRotationY(t)*TM);
	constantbuffer.World = XMMatrixTranspose(XMMatrixRotationY(t)*TM);//rotation around y axis with t
	XMMATRIX Vtemp = g_View;
	XMMATRIX T = XMMatrixTranslation(0, 0, x);
	XMMATRIX RX = XMMatrixRotationY(mx);
	Vtemp = Vtemp*RX*T;
	constantbuffer.View = XMMatrixTranspose(Vtemp);
	constantbuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, NULL, &constantbuffer, 0, 0);

	//render all the walls of the level
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	level1.render_level(g_pImmediateContext, g_pVertexBuffer, &view, &g_Projection, g_pCBuffer);
	
	XMMATRIX worldmatrix;

	ConstantBuffer constantBuffer;
	XMMATRIX S = XMMatrixTranslation(-cam.position.x, -cam.position.y, -cam.position.z);

//first enemy
	
	g_pImmediateContext->PSSetShader(myPs, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &enemy_texture);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	
	for (int ii = 0; ii < 5; ii++)
	{
		sort_enemies(enemy, cam);
		worldmatrix = enemy[ii].get_matrix(view);
		constantBuffer.World = XMMatrixTranspose(worldmatrix);		
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);

		g_pImmediateContext->Draw(12, 0);

	}

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	constantBuffer.green = 0;
	

	//drop_zone
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	
	constantBuffer.green = 1;
	constantBuffer.green_col = col;
	drop_zone.position = XMFLOAT3(-21, -.8,17);
	drop_zone.rotation = XMFLOAT3(0, 0, 0);
	drop_zone.scale = 4.0;
	worldmatrix = drop_zone.get_matrix_no_rotation();
	
	constantBuffer.World = XMMatrixTranspose(worldmatrix);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);

	g_pImmediateContext->PSSetShaderResources(0, 1, &enemy_texture);
	g_pImmediateContext->Draw(12, 0);

	constantBuffer.green = 0;
	
	//bullet
	//ConstantBuffer constant_buffer;
	worldmatrix = bullet.get_matrix(view);
	constantbuffer.World = XMMatrixTranspose(worldmatrix);
	constantbuffer.View = XMMatrixTranspose(view);
	constantbuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantbuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	bullet.position = bullet.position - (bulletImpulse * 0.01);
	bullet.scale = 0.15;
	g_pImmediateContext->PSSetShaderResources(0, 1, &bullet_texture);
	g_pImmediateContext->Draw(12, 0);

	//render chair
	for (int i = 0; i < 6; i++) {
		XMMATRIX chairWorld = chairs[i].get_matrix(view);

		constantBuffer.World = XMMatrixTranspose(chairWorld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureWood);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_3ds, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_anz, 0);
	}

	XMMATRIX Rx90 = XMMatrixRotationX(-XM_PI / 2);

	//render tape

	for (int i = 0; i < 5; i++) {
		tapes[i].update(cam.position);

		XMMATRIX tapeWorld = tapes[i].get_matrix(cam.position, view, cam.rotation.y, level1.get_bitmap());

		constantBuffer.World = XMMatrixTranspose(tapeWorld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		if (tapes[i].completed) {
			constantBuffer.green = 1;
			constantBuffer.green_col = 1.0;
		}
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_tape, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_tape, 0);

		

		constantBuffer.green = 0;
	}
	

	//render bed
	for (int i = 0; i < 5; i++) {
		XMMATRIX bedWorld = beds[i].get_matrix(view);
		bedWorld *= Rx90;

		constantBuffer.World = XMMatrixTranspose(bedWorld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureWood);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_bed, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_bed, 0);
	}

	//table
	for (int i = 0; i < 7; i++) {
		XMMATRIX tableWorld = tables[i].get_matrix(view);
		tableWorld *= Rx90;


		constantBuffer.World = XMMatrixTranspose(tableWorld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureWood);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_table, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_table, 0);
	}

	//tv
	for (int i = 0; i < 2; i++) {
		XMMATRIX tvWorld = tvs[i].get_matrix(view);
		tvWorld *= Rx90;


		constantBuffer.World = XMMatrixTranspose(tvWorld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_tv, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_tv, 0);
	}

	//sofa
	for (int i = 0; i < sofa_size; i++) {
		if (i == 1)
			continue;
		XMMATRIX sofaworld = sofas[i].get_matrix(view);
		sofaworld *= Rx90;


		constantBuffer.World = XMMatrixTranspose(sofaworld);
		constantBuffer.View = XMMatrixTranspose(view);
		constantBuffer.Projection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
		g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sofa, &stride, &offset);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


		g_pImmediateContext->Draw(model_vertex_sofa, 0);
	}

	//fridge
	

	XMMATRIX fridgeWorld = fridge.get_matrix(view);
	fridgeWorld *= Rx90;


	constantBuffer.World = XMMatrixTranspose(fridgeWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_fridge, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_fridge, 0);

	//stove
	

	
	XMMATRIX stoveWorld = stove.get_matrix(view);
	stoveWorld *= Rx90;
	

	constantBuffer.World = XMMatrixTranspose(stoveWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_stove, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_stove, 0);

	//oven
	
	XMMATRIX ovenWorld = oven.get_matrix(view);
	ovenWorld *= Rx90;


	constantBuffer.World = XMMatrixTranspose(ovenWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_oven, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_oven, 0);

	//toilet
	
	XMMATRIX toiletWorld = toilet.get_matrix(view);
	toiletWorld *= Rx90;


	constantBuffer.World = XMMatrixTranspose(toiletWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_toilet, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_toilet, 0);

	//sink
	
	XMMATRIX sinkWorld = sink.get_matrix(view);
	sinkWorld *= Rx90;


	constantBuffer.World = XMMatrixTranspose(sinkWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureMetal);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_sink, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_sink, 0);

	//shower
	
	XMMATRIX showerWorld = shower.get_matrix(view);
	showerWorld *= Rx90;


	constantBuffer.World = XMMatrixTranspose(showerWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureWood);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_shower, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_shower, 0);

	//arrow
	arrow.find_closest(tapes);
	XMMATRIX arrowWorld = arrow.get_matrix(cam.position,view,cam.rotation.y);
	


	constantBuffer.World = XMMatrixTranspose(arrowWorld);
	constantBuffer.View = XMMatrixTranspose(view);
	constantBuffer.Projection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pCBuffer, 0, 0, &constantBuffer, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureWood);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_arrow, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	g_pImmediateContext->Draw(model_vertex_arrow, 0);
	
	if (sight == true&&sightCont==true) {
		music.play_fx("enemySound.wav");
		sightCont = false;
	}

    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}
