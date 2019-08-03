#ifndef _D3DDEVICE_H
#define _D3DDEVICE_H

#include <iostream>

//#include "d3d10.h"
#include "d3d10_1.h"
#include "d3dx10.h"



class D3DDevice{

public:

	D3DDevice(HWND* hWnd);
	~D3DDevice();

	void createSwapChainAndDevice(UINT width, UINT height);
	void createViewports(UINT width, UINT height);
	void initRasterizerState();
	void createRenderTargetsAndDepthBuffer(UINT width, UINT height);
	ID3D10Device1* getD3DDevice();
	IDXGISwapChain* getSwapChain();

	void clear();

private:

	ID3D10Device1*	m_pD3DDevice;
	IDXGISwapChain*	m_pSwapChain;
	ID3D10RenderTargetView*	pRenderTargetView;
	ID3D10DepthStencilView*		pDepthStencilView;

	HWND* m_hWnd;
	
};



#endif