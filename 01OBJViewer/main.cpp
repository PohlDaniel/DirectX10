
#include <windows.h>			
#include <iostream>

#include "Model.h"
#include "D3DDevice.h"
#include "Camera.h"
#include "ModelMatrix.h"
#include "Shader.h"


//#pragma comment(lib, "D3D10.lib")
#pragma comment(lib, "D3D10_1.lib")
#pragma comment(lib, "d3dx10.lib")



enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};

int width = 640;
int height = 480;

POINT g_OldCursorPos;
bool capture = false;
ID3D10Device1*	g_pD3DDevice;
IDXGISwapChain*	g_pSwapChain;

D3DDevice* d3dDevice;
Camera* camera;
Model* model;

ShaderFX* shaderFX = NULL;
Shader* shader = NULL;


LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void processInput();
void setCursortoMiddle(HWND hwnd);
void render();
void render2();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");
	MoveWindow(GetConsoleWindow(), 790, 0, 500, 200, true);

	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "space             : release capture" << std::endl;

	D3DXVECTOR3 camPos(0.0f, 30.0f, 25.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, target, up);


	float fAspectRatio = width / (float)height;
	camera->perspective(45.0f, fAspectRatio, 0.1f, 4000.0f);

	WNDCLASSEX windowClass;		// window class
	HWND	   hwnd;			//window handle
	MSG		   msg;				// message
	HDC		   hdc;				// device context handle

	// fill out the window class structure
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = winProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// default icon
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// default arrow
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background
	windowClass.lpszMenuName = NULL;									// no menu
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

	// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	// class registered, so now create our window
	hwnd = CreateWindowEx(NULL,									// extended style
		"WINDOWCLASS",						// class name
		"OBJViewer",							// app name
		WS_OVERLAPPEDWINDOW,
		0, 0,									// x,y coordinate
		width,
		height,								// width, height
		NULL,									// handle to parent
		NULL,									// handle to menu
		hInstance,								// application instance
		NULL);									// no extra params

	// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the window

	
	d3dDevice = new D3DDevice(&hwnd);
	d3dDevice->createSwapChainAndDevice(width, height);
	d3dDevice->createViewports(width, height);
	d3dDevice->initRasterizerState();
	d3dDevice->createRenderTargetsAndDepthBuffer(width, height);
	g_pD3DDevice = d3dDevice->getD3DDevice(); 
	g_pSwapChain = d3dDevice->getSwapChain();

	model = new Model();
	model->loadObject("objs/res/room2.obj");
	//model->rotate(D3DXVECTOR3(1.0, 0.0, 0.0), -90.0);
	for (int j = 0; j < model->mesh.size(); j++){
		std::cout << model->mesh.size() << std::endl;


		//std::cout << model->mesh[j]->getMltName() << std::endl;

		model->mesh[j]->readMaterial((model->getModelDirectory() + "/" + model->getMltPath()).c_str());
		model->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
		model->mesh[j]->createBuffer(g_pD3DDevice);
		model->mesh[j]->determineInpuDescD3D10();
		model->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
	}

	//use the inputlayout from the first mesh of the model
	shader = new Shader(g_pD3DDevice);
	shader->compileFromFile("basicT.vs", "basicT.ps", g_pD3DDevice);
	shader->createConstBuffer();
	shader->createSamplerState();
	shader->createInputLayoutDescFromVertexShaderSignature();

	g_pD3DDevice->IASetInputLayout(shader->m_pInputLayout);

	/*shaderFX = new ShaderFX("basicEffectT.fx", g_pD3DDevice);
	shaderFX->createEffect();
	shaderFX->loadTechniquePointer("render");
	//shaderFX->createInputLayout();
	//shaderFX->createInputLayout(&model->mesh[0]->getInpuDescD3D10()[0], model->mesh[0]->getNumInputLayoutElements());
	shaderFX->createInputLayoutDescFromVertexShaderSignature();

	g_pD3DDevice->IASetInputLayout(shaderFX->m_pInputLayout);*/

	// Set primitive topology 
	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// main message loop
	while (true){

	
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){

			
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else{

			processInput();

			if (shaderFX){

				render();

			}else{

				render2();

			}
		} // end If messages waiting
	} // end while

	delete model;

	return msg.wParam;
}


LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	POINT pt;
	RECT rect;

	switch (message){

	case WM_DESTROY:{

						PostQuitMessage(0);
						return 0;
	}

	case WM_CREATE:{

					   GetClientRect(hWnd, &rect);
					   g_OldCursorPos.x = rect.right / 2;
					   g_OldCursorPos.y = rect.bottom / 2;
					   pt.x = rect.right / 2;
					   pt.y = rect.bottom / 2;
					   SetCursorPos(pt.x, pt.y);
					   // set the cursor to the middle of the window and capture the window via "SendMessage"
					   SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
					   capture = true;
					   return 0;
	}break;

	case WM_LBUTTONDOWN:{ // Capture the mouse

							
							setCursortoMiddle(hWnd);
							capture = true;

							return 0;
	} break;

	case WM_KEYDOWN:{

						switch (wParam){

						case VK_ESCAPE:{
										   PostQuitMessage(0);
										   return 0;

						}break;
						case VK_SPACE: {										 
										   capture = false;
										   return 0;

						}break;


							return 0;
						}break;
	default:
		break;
	}
	}

	return (DefWindowProc(hWnd, message, wParam, lParam));
}

void processInput(){

	if (capture){

		static UCHAR pKeyBuffer[256];
		ULONG        Direction = 0;
		POINT        CursorPos;
		float        X = 0.0f, Y = 0.0f, rotationSpeed = 0.1f;

		// Retrieve keyboard state
		if (!GetKeyboardState(pKeyBuffer)) return;

		// Check the relevant keys
		if (pKeyBuffer['W'] & 0xF0) Direction |= DIR_FORWARD;
		if (pKeyBuffer['S'] & 0xF0) Direction |= DIR_BACKWARD;
		if (pKeyBuffer['A'] & 0xF0) Direction |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) Direction |= DIR_RIGHT;
		if (pKeyBuffer['E'] & 0xF0) Direction |= DIR_UP;
		if (pKeyBuffer['Q'] & 0xF0) Direction |= DIR_DOWN;

		// Hide the mouse pointer
		SetCursor(NULL);

		// Retrieve the cursor position
		GetCursorPos(&CursorPos);

		// Calculate mouse rotational values
		X = (float)(CursorPos.x - g_OldCursorPos.x) * rotationSpeed;
		Y = (float)(CursorPos.y - g_OldCursorPos.y) * rotationSpeed;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos(g_OldCursorPos.x, g_OldCursorPos.y);

		if (Direction > 0 || X != 0.0f || Y != 0.0f){

			// Rotate camera
			if (X || Y) {

				camera->rotate(X, Y, 0.0f);
			} // End if any rotation

			if (Direction) {

				float dx = 0, dy = 0, dz = 0, speed = 0.5;

				if (Direction & DIR_FORWARD) dz = speed;
				if (Direction & DIR_BACKWARD) dz = -speed;
				if (Direction & DIR_LEFT) dx = -speed;
				if (Direction & DIR_RIGHT) dx = speed;
				if (Direction & DIR_UP) dy = speed;
				if (Direction & DIR_DOWN) dy = -speed;

				camera->move(dx, dy, dz);
			}
		}// End if any movement
	}// End if Captured
}


void setCursortoMiddle(HWND hwnd){

	RECT rect;
	GetWindowRect(hwnd, &rect);
	SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
}


void render(){

	//clear depth and stencil buffer
	d3dDevice->clear();

	shaderFX->loadMatrix("modelMatrix", model->getTransformationMatrix());
	shaderFX->loadMatrix("viewMatrix", camera->getViewMatrix());
	shaderFX->loadMatrix("projectionMatrix", camera->getProjectionMatrix());
	
	// Set primitive topology 
	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//send vertices down pipeline
	for (UINT p = 0; p < shaderFX->m_techDesc.Passes; p++){
		for (int j = 0; j < model->mesh.size(); j++){


			// binb vertex and index buffer
			g_pD3DDevice->IASetVertexBuffers(0, 1, &model->mesh[j]->pVertexBuffer, &model->mesh[j]->m_stride, &model->mesh[j]->m_offset);
			g_pD3DDevice->IASetIndexBuffer(model->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (model->mesh[j]->m_pDiffuseTex){
				shaderFX->setTexture("tex2D", *model->mesh[j]->m_pDiffuseTex);
			}

			//apply technique
			shaderFX->m_pTechnique->GetPassByIndex(p)->Apply(0);
			g_pD3DDevice->DrawIndexed(model->mesh[j]->getNumberOfIndices(), 0, 0);
		}
	}
	g_pSwapChain->Present(1, 0);
}

void render2(){
	//clear depth and stencil buffer
	d3dDevice->clear();

	shader->bindShader();

	shader->loadMatrix(Shader::Location::Model	   , model->getTransformationMatrix());
	shader->loadMatrix(Shader::Location::View	   , camera->getViewMatrix());
	shader->loadMatrix(Shader::Location::Projection, camera->getProjectionMatrix());
	shader->mapMVPBuffer();
	

		for (int j = 0; j < model->mesh.size(); j++){
			// binb vertex and index buffer
			g_pD3DDevice->IASetVertexBuffers(0, 1, &model->mesh[j]->pVertexBuffer, &model->mesh[j]->m_stride, &model->mesh[j]->m_offset);
			g_pD3DDevice->IASetIndexBuffer(model->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (model->mesh[j]->m_pDiffuseTex){
				shader->setTexture(model->mesh[j]->m_pDiffuseTex);
				shader->mapColorBuffer(D3DXVECTOR4(0.0, 1.0, 0.0, 1.0));
			}else {
				shader->mapColorBuffer(D3DXVECTOR4(0.0, 1.0, 0.0, 0.0));
			}
			//shader->mapSecondBuffer(D3DXVECTOR4(1.0, 0.0, 0.0, 0.0));
			
			g_pD3DDevice->DrawIndexed(model->mesh[j]->getNumberOfIndices(), 0, 0);
		}
	g_pSwapChain->Present(1, 0);
}


