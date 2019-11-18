#ifndef __skyboxH__
#define __skyboxH__

#include <vector>
#include "D3DDevice.h"

class SkyBox{

public:
	SkyBox(const char* filename, int size, bool flipHorizontal, bool flipVertical, ID3D10Device1* pD3DDevice);
	~SkyBox();

	ID3D10ShaderResourceView* m_cubeMapTex;
	ID3D10Buffer* pVertexBuffer;
	unsigned int m_stride = 3 * sizeof(float), m_offset = 0;

private:

	void createCubeMap(const char* filename, bool flipHorizontal, bool flipVertical, ID3D10Device1* pD3DDevice);
	void createBuffer(ID3D10Device1* pD3DDevice);
	
	
	int m_size = 50;
	std::vector<float> m_postions;

	
};


#endif 