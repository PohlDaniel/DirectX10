
#include <windows.h>			
#include <iostream>

#include "Model.h"
#include "D3DDevice.h"
#include "Camera.h"
#include "ModelMatrix.h"
#include "Shader.h"
#include "Bitmap.h"
#include "Skybox.h"

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
SkyBox* skybox;
Model  *teapot1, *teapot2, *teapot3, *quad;

ShaderFX* skyboxShader;
ShaderFX *teapotShader1, *teapotShader2, *teapotShader3, *quadShader;

LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void processInput();
void setCursortoMiddle(HWND hwnd);
void render();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow){

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");
	MoveWindow(GetConsoleWindow(), 790, 0, 500, 200, true);

	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "space             : release capture" << std::endl;

	D3DXVECTOR3 camPos(0.0, 0.0, 200.0);
	D3DXVECTOR3 target(0.0, 0.0, 0.0);
	D3DXVECTOR3 up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, target, up);


	float fAspectRatio = width / (float)height;
	camera->perspective(45.0f, fAspectRatio, 0.1f, 1000.0f);

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


	///////////////////////////////////////////////////////////////////////////
	skybox = new SkyBox("../skyboxes/sor_sea", 500, true, false, g_pD3DDevice);

	skyboxShader = new ShaderFX("shader/skybox.fx", g_pD3DDevice);
	skyboxShader->createEffect();
	skyboxShader->loadTechniquePointer("render");
	skyboxShader->createInputLayoutDescFromVertexShaderSignature();
	
	// Set primitive topology 
	g_pD3DDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	teapot1 = new Model();
	teapot1->loadObject("objs/Teapot/teapot.obj");
	teapot1->scale(4.0, 4.0, 4.0);
	teapot1->setRotPos(D3DXVECTOR3(0.0, 1.0, 0.0), 225, 100.0, 0.0, 0.0);
	
	for (int j = 0; j < teapot1->mesh.size(); j++){
		teapot1->mesh[j]->readMaterial((teapot1->getModelDirectory() + "/" + teapot1->getMltPath()).c_str());
		teapot1->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
		teapot1->mesh[j]->generateTangents();
		teapot1->mesh[j]->createBuffer(g_pD3DDevice);
	}

	teapot2 = new Model();
	teapot2->loadObject("objs/Teapot/teapot.obj");
	teapot2->scale(4.0, 4.0, 4.0);
	teapot2->setRotPos(D3DXVECTOR3(0.0, 1.0, 0.0), 225, 0.0, 0.0, 0.0);

	for (int j = 0; j < teapot2->mesh.size(); j++){
		teapot2->mesh[j]->readMaterial((teapot2->getModelDirectory() + "/" + teapot2->getMltPath()).c_str());
		teapot2->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
		teapot2->mesh[j]->generateTangents();
		teapot2->mesh[j]->createBuffer(g_pD3DDevice);
	}

	teapot3 = new Model();
	teapot3->loadObject("objs/Teapot/teapot.obj");
	teapot3->scale(4.0, 4.0, 4.0);
	teapot3->setRotPos(D3DXVECTOR3(0.0, 1.0, 0.0), 225, -100.0, 0.0, 0.0);

	for (int j = 0; j < teapot3->mesh.size(); j++){
		teapot3->mesh[j]->readMaterial((teapot3->getModelDirectory() + "/" + teapot3->getMltPath()).c_str());
		teapot3->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
		teapot3->mesh[j]->generateTangents();
		teapot3->mesh[j]->createBuffer(g_pD3DDevice);
	}

	quad = new Model();
	quad->loadObject("objs/Quad/quad.obj");
	quad->scale(200, 200, 4.0);
	quad->setRotPos(D3DXVECTOR3(1.0, 0.0, 0.0), 90, 0.0, 0.0, -50.0);

	for (int j = 0; j < quad->mesh.size(); j++){
		quad->mesh[j]->readMaterial((quad->getModelDirectory() + "/" + quad->getMltPath()).c_str());
		quad->mesh[j]->loadTextureFromMlt(g_pD3DDevice);
		quad->mesh[j]->generateTangents();
		quad->mesh[j]->createBuffer(g_pD3DDevice);
	}


	teapotShader1 = new ShaderFX("shader/basicEffectT.fx", g_pD3DDevice);
	teapotShader1->createEffect();
	teapotShader1->loadTechniquePointer("render");
	teapotShader1->createInputLayoutDescFromVertexShaderSignature();

	teapotShader2 = new ShaderFX("shader/basicEffectT.fx", g_pD3DDevice);
	teapotShader2->createEffect();
	teapotShader2->loadTechniquePointer("render");
	teapotShader2->createInputLayoutDescFromVertexShaderSignature();

	teapotShader3 = new ShaderFX("shader/envmap.fx", g_pD3DDevice);
	teapotShader3->createEffect();
	teapotShader3->loadTechniquePointer("render");
	teapotShader3->createInputLayoutDescFromVertexShaderSignature();

	quadShader = new ShaderFX("shader/basicEffectT.fx", g_pD3DDevice);
	quadShader->createEffect();
	quadShader->loadTechniquePointer("render");
	quadShader->createInputLayoutDescFromVertexShaderSignature();

	// main message loop
	while (true){

	
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){

			
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}else{

			processInput();

			render();
		} // end If messages waiting
	} // end while



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

				float dx = 0, dy = 0, dz = 0, speed = 2.0;

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

	skyboxShader->setMatrix("modelMatrix", skybox->getTransformationMatrix());
	skyboxShader->setMatrix("viewMatrix", camera->getViewMatrix());
	skyboxShader->setMatrix("projectionMatrix", camera->getProjectionMatrix());


	teapotShader1->setMatrix("modelMatrix", teapot1->getTransformationMatrix());
	teapotShader1->setMatrix("viewMatrix", camera->getViewMatrix());
	teapotShader1->setMatrix("projectionMatrix", camera->getProjectionMatrix());
	teapotShader1->setMatrix("normalMatrix", Matrix::getNormalMatrix(teapot1->getTransformationMatrix() * camera->getViewMatrix()));
	teapotShader1->setFloatVector("lightPos", camera->getPosition(), sizeof(D3DXVECTOR3));

	teapotShader1->setFloatVectorInStruct("light", "pos", camera->getPosition(), sizeof(D3DXVECTOR3));
	teapotShader1->setFloatVectorInStruct("light", "ambient", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader1->setFloatVectorInStruct("light", "diffuse", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader1->setFloatVectorInStruct("light", "specular", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));

	teapotShader2->setMatrix("modelMatrix", teapot2->getTransformationMatrix());
	teapotShader2->setMatrix("viewMatrix", camera->getViewMatrix());
	teapotShader2->setMatrix("projectionMatrix", camera->getProjectionMatrix());
	teapotShader2->setMatrix("normalMatrix", Matrix::getNormalMatrix(teapot2->getTransformationMatrix() * camera->getViewMatrix()));
	teapotShader2->setFloatVector("lightPos", camera->getPosition(), sizeof(D3DXVECTOR3));

	teapotShader2->setFloatVectorInStruct("light", "pos", camera->getPosition(), sizeof(D3DXVECTOR3));
	teapotShader2->setFloatVectorInStruct("light", "ambient", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader2->setFloatVectorInStruct("light", "diffuse", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader2->setFloatVectorInStruct("light", "specular", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));

	teapotShader3->setMatrix("modelMatrix", teapot3->getTransformationMatrix());
	teapotShader3->setMatrix("viewMatrix", camera->getViewMatrix());
	teapotShader3->setMatrix("projectionMatrix", camera->getProjectionMatrix());
	teapotShader3->setMatrix("normalMatrix", Matrix::getNormalMatrix(teapot3->getTransformationMatrix() * camera->getViewMatrix()));
	teapotShader3->setMatrix("invModelMatrix", teapot3->getInvTransformationMatrix());
	
	teapotShader3->setFloatVector("lightPos", camera->getPosition(), sizeof(D3DXVECTOR3));

	teapotShader3->setFloatVectorInStruct("light", "pos", camera->getPosition(), sizeof(D3DXVECTOR3));
	teapotShader3->setFloatVectorInStruct("light", "ambient", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader3->setFloatVectorInStruct("light", "diffuse", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	teapotShader3->setFloatVectorInStruct("light", "specular", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	
	quadShader->setMatrix("modelMatrix", quad->getTransformationMatrix());
	quadShader->setMatrix("viewMatrix", camera->getViewMatrix());
	quadShader->setMatrix("projectionMatrix", camera->getProjectionMatrix());
	quadShader->setMatrix("normalMatrix", Matrix::getNormalMatrix(quad->getTransformationMatrix() * camera->getViewMatrix()));
	quadShader->setFloatVector("lightPos", camera->getPosition(), sizeof(D3DXVECTOR3));

	quadShader->setFloatVectorInStruct("light", "pos", camera->getPosition(), sizeof(D3DXVECTOR3));
	quadShader->setFloatVectorInStruct("light", "ambient", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	quadShader->setFloatVectorInStruct("light", "diffuse", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));
	quadShader->setFloatVectorInStruct("light", "specular", D3DXVECTOR3(1.0f, 1.0f, 1.0f), sizeof(D3DXVECTOR3));

	///////////////////////////////////////////teapot1//////////////////////////////
	g_pD3DDevice->IASetInputLayout(teapotShader1->m_pInputLayout);
	for (int j = 0; j < teapot1->mesh.size(); j++){
		
		// bind vertex and index buffer
		g_pD3DDevice->IASetVertexBuffers(0, 1, &teapot1->mesh[j]->pVertexBuffer, &teapot1->mesh[j]->m_stride, &teapot1->mesh[j]->m_offset);
		g_pD3DDevice->IASetIndexBuffer(teapot1->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		if (teapot1->mesh[j]->m_pDiffuseTex){
			teapotShader1->setTexture("colorMapTexture", teapot1->mesh[j]->m_pDiffuseTex);
		}

		teapotShader1->m_pTechnique->GetPassByIndex(0)->Apply(0);
		g_pD3DDevice->DrawIndexed(teapot1->mesh[j]->getNumberOfIndices(), 0, 0);
	}

	///////////////////////////////////////////teapot2//////////////////////////////
	g_pD3DDevice->IASetInputLayout(teapotShader2->m_pInputLayout);
	for (int j = 0; j < teapot2->mesh.size(); j++){

		// bind vertex and index buffer
		g_pD3DDevice->IASetVertexBuffers(0, 1, &teapot2->mesh[j]->pVertexBuffer, &teapot2->mesh[j]->m_stride, &teapot2->mesh[j]->m_offset);
		g_pD3DDevice->IASetIndexBuffer(teapot2->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		if (teapot2->mesh[j]->m_pDiffuseTex){
			teapotShader2->setTexture("colorMapTexture", teapot2->mesh[j]->m_pDiffuseTex);
		}

		teapotShader2->m_pTechnique->GetPassByIndex(0)->Apply(0);
		g_pD3DDevice->DrawIndexed(teapot2->mesh[j]->getNumberOfIndices(), 0, 0);
	}

	///////////////////////////////////////////teapot3//////////////////////////////
	g_pD3DDevice->IASetInputLayout(teapotShader3->m_pInputLayout);
	for (int j = 0; j < teapot3->mesh.size(); j++){

		// bind vertex and index buffer
		g_pD3DDevice->IASetVertexBuffers(0, 1, &teapot3->mesh[j]->pVertexBuffer, &teapot3->mesh[j]->m_stride, &teapot3->mesh[j]->m_offset);
		g_pD3DDevice->IASetIndexBuffer(teapot3->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			if (teapot3->mesh[j]->m_pDiffuseTex){
				teapotShader3->setTexture("colorMapTexture", teapot2->mesh[j]->m_pDiffuseTex);
			}

			if (skybox->m_cubeMapTex){
				teapotShader3->setTexture("cubeMapTexture", skybox->m_cubeMapTex);
			}

		teapotShader3->m_pTechnique->GetPassByIndex(0)->Apply(0);
		g_pD3DDevice->DrawIndexed(teapot3->mesh[j]->getNumberOfIndices(), 0, 0);
	}
	///////////////////////////////////////////quad//////////////////////////////
	g_pD3DDevice->IASetInputLayout(quadShader->m_pInputLayout);
	for (int j = 0; j < quad->mesh.size(); j++){

		// bind vertex and index buffer
		g_pD3DDevice->IASetVertexBuffers(0, 1, &quad->mesh[j]->pVertexBuffer, &quad->mesh[j]->m_stride, &quad->mesh[j]->m_offset);
		g_pD3DDevice->IASetIndexBuffer(quad->mesh[j]->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		if (quad->mesh[j]->m_pDiffuseTex){
			quadShader->setTexture("colorMapTexture", quad->mesh[j]->m_pDiffuseTex);
		}

		quadShader->m_pTechnique->GetPassByIndex(0)->Apply(0);
		g_pD3DDevice->DrawIndexed(quad->mesh[j]->getNumberOfIndices(), 0, 0);
	}
	///////////////////////////////////////////skybox//////////////////////////////
	g_pD3DDevice->IASetInputLayout(skyboxShader->m_pInputLayout);

	g_pD3DDevice->IASetVertexBuffers(0, 1, &skybox->pVertexBuffer, &skybox->m_stride, &skybox->m_offset);

	if (skybox->m_cubeMapTex){
		skyboxShader->setTexture("cubeMapTexture", skybox->m_cubeMapTex);
	}

	skyboxShader->m_pTechnique->GetPassByIndex(0)->Apply(0);
	g_pD3DDevice->Draw(36, 0);


	g_pSwapChain->Present(1, 0);
}