#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define FULLSCRENMODE_RELEASE
#define CONST_METRICS

#ifdef FULLSCRENMODE_RELEASE
	
	#ifdef CONST_METRICS
		const int XRES = 1366;
		const int YRES = 768;
	#else
		int XRES;
		int YRES;
	#endif

	#define WINDOW_TYPE WS_EX_TOPMOST | WS_POPUP // full screen window
#else
	const int XRES = 640;
	const int YRES = 480; 
	
	#define WINDOW_TYPE WS_OVERLAPPEDWINDOW

	LRESULT CALLBACK WindowProc(HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);
#endif

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

const CUSTOMVERTEX vertices[] =
{
	{ -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, },    // side 1
	{ 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, },
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, },

	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, },    // side 2
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, },
	{ 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, },

	{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, },    // side 3
	{ -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, },

	{ -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, },    // side 4
	{ 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, },
	{ -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, },
	{ 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, },

	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, },    // side 5
	{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, },
	{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, },
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, },

	{ -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, },    // side 6
	{ -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, },
	{ -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, },
	{ -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, },
};

const short indices[] =
{
	0, 1, 2,    // side 1
	2, 1, 3,
	4, 5, 6,    // side 2
	6, 5, 7,
	8, 9, 10,    // side 3
	10, 9, 11,
	12, 13, 14,    // side 4
	14, 13, 15,
	16, 17, 18,    // side 5
	18, 17, 19,
	20, 21, 22,    // side 6
	22, 21, 23,
};

// get rid of memset calls by placing structures in .bss section

HWND hWnd;
WNDCLASSA wc;
MSG msg;

LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class

D3DPRESENT_PARAMETERS d3dpp = // create a struct to hold various device information
{
	0,		// UINT                BackBufferWidth;
	0,		// UINT                BackBufferHeight;
	{},		// D3DFORMAT           BackBufferFormat;
	0,		// UINT                BackBufferCount;

	{},		// D3DMULTISAMPLE_TYPE MultiSampleType;
	0,		// DWORD               MultiSampleQuality;

	D3DSWAPEFFECT_DISCARD, // D3DSWAPEFFECT       SwapEffect;
	0,		// HWND                hDeviceWindow;
	TRUE,	// BOOL                Windowed;
	TRUE,	// BOOL                EnableAutoDepthStencil;
	D3DFMT_D16, // D3DFORMAT           AutoDepthStencilFormat;
	0,		// DWORD               Flags;

	0,		// UINT                FullScreen_RefreshRateInHz; // must be zero for Windowed mode
	0		// UINT                PresentationInterval;
};

#if 0
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	//d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
	//d3dpp.BackBufferWidth = XRES;
	//d3dpp.BackBufferHeight = YRES;
	d3dpp.EnableAutoDepthStencil = TRUE;    // automatically run the z-buffer for us
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;    // 16-bit pixel format for the z-buffer
#endif

LPDIRECT3DVERTEXBUFFER9 v_buffer;    // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer;

D3DLIGHT9 light;    // create the light struct
D3DMATERIAL9 material;    // create the material struct

#if 0
D3DLIGHT9 light =
{
	D3DLIGHT_DIRECTIONAL, // D3DLIGHTTYPE    Type;            
		D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), // D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
		{}, // D3DCOLORVALUE   Specular;        /* Specular color of light */
		{}, // D3DCOLORVALUE   Ambient;         /* Ambient color of light */
		{}, // D3DVECTOR       Position;         /* Position in world space */
		D3DXVECTOR3(-1.0f, -0.3007812500f, -1.0f), // D3DVECTOR       Direction;        /* Direction in world space */
		0, // float           Range;            /* Cutoff range */
		0,// float           Falloff;          /* Falloff */
		0, // float           Attenuation0;     /* Constant attenuation */
		0, // float           Attenuation1;     /* Linear attenuation */
		0, // float           Attenuation2;     /* Quadratic attenuation */
		0, // float           Theta;            /* Inner angle of spotlight cone */
		0 // float           Phi;              /* Outer angle of spotlight cone */
};

D3DMATERIAL9 material =
{
	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), // D3DCOLORVALUE   Diffuse;        /* Diffuse color RGBA */
	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), // D3DCOLORVALUE   Ambient;        /* Ambient color RGB */
	0, // D3DCOLORVALUE   Specular;       /* Specular 'shininess' */
	0, // D3DCOLORVALUE   Emissive;       /* Emissive color RGB */
	0 // float           Power;          /* Sharpness if specular highlight */
};
#endif

VOID* pVoid;    // the void pointer
float index;
D3DXMATRIX matView;

void initD3D(void);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory
void init_graphics(void);
void init_light(void);

	__declspec(naked)  void ___tmainCRTStartup(void)  //HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
	{
		//__asm enter 0x10, 0;
		//__asm pushad; // sometimes neccesary

	#ifndef FULLSCRENMODE_RELEASE
		//wc.cbSize = sizeof(WNDCLASSEX); // it have to be passed if using WNDCLASSEX
		wc.lpszClassName = "W";
		wc.lpfnWndProc = WindowProc;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		RegisterClassA(&wc);

		hWnd = CreateWindowA(wc.lpszClassName,  // string polling they say ...
			NULL, // title
			WINDOW_TYPE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			XRES,
			YRES,
			NULL,
			NULL,
			NULL, //GetModuleHandle(NULL),
			NULL);
	#else

		#ifndef CONST_METRICS
			XRES = GetSystemMetrics(SM_CXSCREEN);
			YRES = GetSystemMetrics(SM_CYSCREEN);
		#endif

		hWnd = CreateWindowA("edit", 0, WINDOW_TYPE, 0, 0, XRES, YRES, 0, 0, 0, 0); //static
		//ShowCursor(false);
	#endif
		ShowWindow(hWnd, SW_NORMAL);
		
		initD3D();

	#ifndef FULLSCRENMODE_RELEASE
		while (TRUE)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				//TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		
			render_frame();
		}
	#else
		do
		{
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); // remove windows messages, to not crash app after few seconds

			render_frame();

		} while (!GetAsyncKeyState(VK_ESCAPE));

		//cleanD3D();
		ExitProcess(0); // clean enough exit (kill process)
	#endif
	}

#ifndef FULLSCRENMODE_RELEASE
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
		case WM_KEYDOWN:
		
			//cleanD3D();
			ExitProcess(0);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
#endif

	void initD3D(void)
	{
		d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

		/*d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
		//d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
		//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
		//d3dpp.BackBufferWidth = XRES;
		//d3dpp.BackBufferHeight = YRES;
		d3dpp.EnableAutoDepthStencil = TRUE;    // automatically run the z-buffer for us
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;    // 16-bit pixel format for the z-buffer
		*/

		// create a device class using this information and information from the d3dpp stuct
		d3d->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL, // HAL returns INVALIDCALL
				hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING, // D3DCREATE_MIXED_VERTEXPROCESSING 
				&d3dpp,
				&d3ddev);

		init_graphics();
		init_light();    // call the function to initialize the light and material
	
		d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
		//d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50));    // ambient light
		d3ddev->SetFVF(CUSTOMFVF); // select which vertex format we are using

		/*
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 2.0f, 8.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		D3DXMatrixPerspectiveFovLH(&matView,
			D3DXToRadian(45),    // the horizontal field of view
			(FLOAT)XRES / (FLOAT)YRES, // aspect ratio
			1.0f,    // the near view-plane
			128.0f);    // the far view-plane
		d3ddev->SetTransform(D3DTS_PROJECTION, &matView);    // set the projection
		*/
	}
	
	void init_graphics(void)
	{
		// create a index buffer interface called i_buffer
		d3ddev->CreateIndexBuffer(36 * sizeof(short),
					0, // usage
					D3DFMT_INDEX16,
					D3DPOOL_MANAGED,
					&i_buffer,
					NULL);
		
		// create the vertex and store the pointer into v_buffer, which is created globally
		d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX), // 8 cube
					0,
					CUSTOMFVF,
					D3DPOOL_MANAGED,
					&v_buffer,
					NULL);
		
		// lock i_buffer and load the indices into it
		i_buffer->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, indices, sizeof(indices));
		i_buffer->Unlock();

		v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
		memcpy(pVoid, vertices, sizeof(vertices));    // copy the vertices to the locked buffer
		v_buffer->Unlock();    // unlock the vertex buffer
	}

	void init_light(void)
	{
		light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
		light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
		light.Direction = D3DXVECTOR3(-1.0f, -0.3007812500f, -1.0f);
		
		d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
		d3ddev->LightEnable(0, TRUE);    // turn on light #0

		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
		material.Ambient = material.Diffuse; // D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white
		
		d3ddev->SetMaterial(&material);    // set the globably-used material to &material
	}

	void render_frame(void)
	{
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		d3ddev->BeginScene();    // begins the 3D scene

		//d3ddev->SetFVF(CUSTOMFVF); // select which vertex format we are using

		///*
		D3DXMatrixLookAtLH(&matView,
			&D3DXVECTOR3(0.0f, 2.0f, 8.0f),    // the camera position
			&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
			&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction
		d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

		index += 0.0300292969f; // an ever-increasing float value

		D3DXMatrixPerspectiveFovLH(&matView,
			index/4.0f, //D3DXToRadian(45),    // the horizontal field of view
			(FLOAT)XRES / (FLOAT)YRES, // aspect ratio
			1.0f,    // the near view-plane
			128.0f);    // the far view-plane
		d3ddev->SetTransform(D3DTS_PROJECTION, &matView);    // set the projection
		//*/

		D3DXMatrixRotationY(&matView, index);    // the rotation matrix
		d3ddev->SetTransform(D3DTS_WORLD, &(matView));    // set the world transform

		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX)); // select the vertex buffer to display
		d3ddev->SetIndices(i_buffer);

		//d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12); //cube
		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);    // displays the created frame
	}