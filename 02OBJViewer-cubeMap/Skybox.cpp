#include "Skybox.h"
#include "Bitmap.h"

SkyBox::SkyBox(const char* filename, int size, bool flipHorizontal, bool flipVertical, ID3D10Device1* pD3DDevice){

	m_size = size;

	createCubeMap(filename, flipHorizontal, flipVertical, pD3DDevice);
	createBuffer(pD3DDevice);
}

SkyBox::~SkyBox(){

}


void SkyBox::createCubeMap(const char* filename, bool flipHorizontal, bool flipVertical, ID3D10Device1* pD3DDevice){

	Bitmap *bitmap[6];

	std::string pattern(filename);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;

	int i = 0;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {

			if (std::string(data.cFileName) == "." || std::string(data.cFileName) == "..") continue;
			bitmap[i] = new Bitmap();
			bitmap[i]->loadBitmap24toRGBA((std::string(filename) + "/" + std::string(data.cFileName)).c_str());

			if (flipHorizontal){

				bitmap[i]->flipHorizontal();
			}

			if (flipVertical){
				bitmap[i]->flipVertical();
			}

			i++;
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}


	ID3D10Texture2D *cubeTexture = NULL;

	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Width = bitmap[0]->width;
	texDesc.Height = bitmap[0]->height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	
	D3D10_SUBRESOURCE_DATA initialData[6];

	for (int i = 0; i < 6; i++){

		initialData[i].pSysMem = bitmap[i]->data;
		initialData[i].SysMemPitch = bitmap[i]->width * 4;
		initialData[i].SysMemSlicePitch = 0;
	}

	pD3DDevice->CreateTexture2D(&texDesc, &initialData[0], &cubeTexture);

	//The Shader Resource view description
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = texDesc.MipLevels - 1;


	pD3DDevice->CreateShaderResourceView(cubeTexture, &srvDesc, &m_cubeMapTex);


	for (int i = 0; i < 6; i++){
		delete bitmap[i];
	}
}

void  SkyBox::createBuffer(ID3D10Device1* pD3DDevice){

	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);

	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);

	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);

	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);

	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size);

	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size);
	m_postions.push_back(-1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);
	m_postions.push_back(1.0 *m_size); m_postions.push_back(-1.0 *m_size); m_postions.push_back(1.0 *m_size);


	//set vertex buffer description
	D3D10_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D10_BUFFER_DESC));
	bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float)* m_postions.size();
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	
	pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pVertexBuffer);

	void *pData = 0;
	pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&pData);
	memcpy(pData, m_postions.data(), sizeof(float)* m_postions.size());
	pVertexBuffer->Unmap();

}