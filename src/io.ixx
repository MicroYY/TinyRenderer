module;

#include <d3d12.h>
#include "imgui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <format>

export module io;

export namespace io
{
#pragma pack(push,1)
	struct TgaHeader
	{
		char idlength;
		char colormaptype;
		char datatypecode;
		short colormaporigin;
		short colormaplength;
		char colormapdepth;
		short x_origin;
		short y_origin;
		short width;
		short height;
		char  bitsperpixel;
		char  imagedescriptor;
	};
#pragma pack(pop)

	struct TgaColor
	{
		unsigned char bgra[4];
		unsigned char bytespp;

		TgaColor() : bgra(), bytespp(1) {
			for (int i = 0; i < 4; i++) bgra[i] = 0;
		}

		TgaColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : bgra(), bytespp(4) {
			bgra[0] = B;
			bgra[1] = G;
			bgra[2] = R;
			bgra[3] = A;
		}

		TgaColor(unsigned char v) : bgra(), bytespp(1) {
			for (int i = 0; i < 4; i++) bgra[i] = 0;
			bgra[0] = v;
		}


		TgaColor(const unsigned char* p, unsigned char bpp) : bgra(), bytespp(bpp) {
			for (int i = 0; i < (int)bpp; i++) {
				bgra[i] = p[i];
			}
			for (int i = bpp; i < 4; i++) {
				bgra[i] = 0;
			}
		}

		unsigned char& operator[](const int i) { return bgra[i]; }

		TgaColor operator *(float intensity) const {
			TgaColor res = *this;
			intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
			for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
			return res;
		}
	};

	class TgaImage {
	protected:
		unsigned char* m_data;
		int m_width;
		int m_height;
		int m_bytespp;

		bool LoadRleData(std::ifstream& in);
		bool UnloadRleData(std::ofstream& out);
	public:
		enum Format {
			GRAYSCALE = 1, RGB = 3, RGBA = 4
		};

		TgaImage() : m_data(nullptr), m_width(0), m_height(0), m_bytespp(0) {};
		TgaImage(int w, int h, int bpp) : m_data(nullptr), m_width(w), m_height(h), m_bytespp(bpp)
		{
			unsigned long nbytes = m_width * m_height * m_bytespp;
			m_data = new unsigned char[nbytes];
			memset(m_data, 0, nbytes);
		}

		TgaImage(const TgaImage& img);
		bool ReadTgaFile(const char* filename);
		bool WriteTgaFile(const char* filename, bool rle = true);
		bool FlipHorizontally();
		bool FlipVertically();
		bool Scale(int w, int h);
		TgaColor Get(int x, int y);
		bool Set(int x, int y, TgaColor& c);
		bool Set(int x, int y, const TgaColor& c);
		~TgaImage();
		TgaImage& operator =(const TgaImage& img);
		
		int GetWidth()          const { return m_width;   }
		int GetHeight()         const { return m_height;  }
		int GetBytespp()        const { return m_bytespp; }
		unsigned char* Buffer() const { return m_data;    }

		void Clear();
	};

	bool TgaImage::LoadRleData(std::ifstream& in)
	{
		unsigned long pixelcount = m_width * m_height;
		unsigned long currentpixel = 0;
		unsigned long currentbyte = 0;
		TgaColor colorbuffer;
		do {
			unsigned char chunkheader = 0;
			chunkheader = in.get();
			if (!in.good()) {
				std::cerr << "an error occured while reading the data\n";
				return false;
			}
			if (chunkheader < 128) {
				chunkheader++;
				for (int i = 0; i < chunkheader; i++) {
					in.read((char*)colorbuffer.bgra, m_bytespp);
					if (!in.good()) {
						std::cerr << "an error occured while reading the header\n";
						return false;
					}
					for (int t = 0; t < m_bytespp; t++)
						m_data[currentbyte++] = colorbuffer.bgra[t];
					currentpixel++;
					if (currentpixel > pixelcount) {
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
			else {
				chunkheader -= 127;
				in.read((char*)colorbuffer.bgra, m_bytespp);
				if (!in.good()) {
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (int i = 0; i < chunkheader; i++) {
					for (int t = 0; t < m_bytespp; t++)
						m_data[currentbyte++] = colorbuffer.bgra[t];
					currentpixel++;
					if (currentpixel > pixelcount) {
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
		} while (currentpixel < pixelcount);
		return true;
	}

	bool TgaImage::ReadTgaFile(const char* filename)
	{
		if (m_data) delete[] m_data;
		m_data = nullptr;
		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in.is_open())
		{
			std::cerr << "cannot open file " << filename << "\n";
			in.close();
			return false;
		}

		TgaHeader header;
		in.read((char*)&header, sizeof(header));
		if (!in.good())
		{
			in.close();
			std::cerr << "an error occurred while reading the header\n";
			return false;
		}

		m_width   = header.width;
		m_height  = header.height;
		m_bytespp = header.bitsperpixel >> 3;
		if (m_width <= 0 || m_height <= 0 || (m_bytespp != GRAYSCALE && m_bytespp != RGBA && m_bytespp != RGB))
		{
			in.close();
			std::cerr << std::format("invalid image info: width {}, height {}, bpp {}\n", m_width, m_height, m_bytespp);
			return false;
		}

		unsigned long nbytes = m_bytespp * m_width * m_height;
		m_data = new unsigned char[nbytes];
		if (header.datatypecode == 3 || header.datatypecode == 2)
		{
			in.read((char*)m_data, nbytes);
			if (!in.good())
			{
				in.close();
				std::cerr << "an error occurred while reading the data\n";
				return false;
			}
		}
		else if (header.datatypecode == 10 || header.datatypecode == 11)
		{
			if (!LoadRleData(in))
			{
				in.close();
				std::cerr << "an error occurred while reading the data\n";
				return false;
			}
		}
		else
		{
			in.close();
			std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
		}

		// do not flip
		if ((header.imagedescriptor & 0x20))
		{
			FlipVertically();
		}
		if (header.imagedescriptor & 0x10)
		{
			FlipHorizontally();
		}

		std::cout << m_width << "x" << m_height << "/" << m_bytespp * 8 << "\n";
		in.close();
		return true;
	}

	bool TgaImage::FlipVertically()
	{
		if (!m_data) return false;
		unsigned long bytes_per_line = m_width * m_bytespp;
		unsigned char* line = new unsigned char[bytes_per_line];
		int half = m_height >> 1;
		for (int j = 0; j < half; j++) {
			unsigned long l1 = j * bytes_per_line;
			unsigned long l2 = (m_height - 1 - j) * bytes_per_line;
			memmove((void*)line, (void*)(m_data + l1), bytes_per_line);
			memmove((void*)(m_data + l1), (void*)(m_data + l2), bytes_per_line);
			memmove((void*)(m_data + l2), (void*)line, bytes_per_line);
		}
		delete[] line;
		return true;
	}

	bool TgaImage::FlipHorizontally()
	{
		if (!m_data) return false;
		int half = m_width >> 1;
		for (int i = 0; i < half; i++) {
			for (int j = 0; j < m_height; j++) {
				TgaColor c1 = Get(i, j);
				TgaColor c2 = Get(m_width - 1 - i, j);
				Set(i, j, c2);
				Set(m_width - 1 - i, j, c1);
			}
		}
		return true;
	}

	TgaColor TgaImage::Get(int x, int y)
	{
		if (!m_data || x < 0 || y < 0 || x >= m_width || y >= m_height) {
			return TgaColor();
		}
		return TgaColor(m_data + (x + y * m_width) * m_bytespp, m_bytespp);
	}

	bool TgaImage::Set(int x, int y, TgaColor& c)
	{
		if (!m_data || x < 0 || y < 0 || x >= m_width || y >= m_height) {
			return false;
		}
		memcpy(m_data + (x + y * m_width) * m_bytespp, c.bgra, m_bytespp);
		return true;
	}

	TgaImage::~TgaImage()
	{
		if (m_data) delete[] m_data;
	}

	// Simple helper function to load an image into a DX12 texture with common settings
	// Returns true on success, with the SRV CPU handle having an SRV for the newly-created texture placed in it (srv_cpu_handle must be a handle in a valid descriptor heap)
	bool LoadTextureFromFile(const char* filename, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
	{
		// Load from disk into a raw RGBA buffer
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create texture resource
		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_DEFAULT;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = image_width;
		desc.Height = image_height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ID3D12Resource* pTexture = NULL;
		d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));

		// Create a temporary upload resource to move the data in
		UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		UINT uploadSize = image_height * uploadPitch;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = uploadSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		ID3D12Resource* uploadBuffer = NULL;
		HRESULT hr = d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
		IM_ASSERT(SUCCEEDED(hr));

		// Write pixels into the upload resource
		void* mapped = NULL;
		D3D12_RANGE range = { 0, uploadSize };
		hr = uploadBuffer->Map(0, &range, &mapped);
		IM_ASSERT(SUCCEEDED(hr));
		for (int y = 0; y < image_height; y++)
			memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
		uploadBuffer->Unmap(0, &range);

		// Copy the upload resource content into the real resource
		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer;
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srcLocation.PlacedFootprint.Footprint.Width = image_width;
		srcLocation.PlacedFootprint.Footprint.Height = image_height;
		srcLocation.PlacedFootprint.Footprint.Depth = 1;
		srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = pTexture;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = 0;

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pTexture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		// Create a temporary command queue to do the copy with
		ID3D12Fence* fence = NULL;
		hr = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		IM_ASSERT(SUCCEEDED(hr));

		HANDLE event = CreateEvent(0, 0, 0, 0);
		IM_ASSERT(event != NULL);

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 1;

		ID3D12CommandQueue* cmdQueue = NULL;
		hr = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
		IM_ASSERT(SUCCEEDED(hr));

		ID3D12CommandAllocator* cmdAlloc = NULL;
		hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
		IM_ASSERT(SUCCEEDED(hr));

		ID3D12GraphicsCommandList* cmdList = NULL;
		hr = d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
		IM_ASSERT(SUCCEEDED(hr));

		cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
		cmdList->ResourceBarrier(1, &barrier);

		hr = cmdList->Close();
		IM_ASSERT(SUCCEEDED(hr));

		// Execute the copy
		cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
		hr = cmdQueue->Signal(fence, 1);
		IM_ASSERT(SUCCEEDED(hr));

		// Wait for everything to complete
		fence->SetEventOnCompletion(1, event);
		WaitForSingleObject(event, INFINITE);

		// Tear down our temporary command queue and release the upload resource
		cmdList->Release();
		cmdAlloc->Release();
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();

		// Create a shader resource view for the texture
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3d_device->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

		// Return results
		*out_tex_resource = pTexture;
		*out_width = image_width;
		*out_height = image_height;
		stbi_image_free(image_data);

		return true;
	}

	bool LoadTextureFromFile(const std::string& filename, int& out_width, int& out_height, int& out_nChannels, unsigned int& texture)
	{
		auto found = filename.find("png");
		if (found != std::string::npos)
		{
			stbi_set_flip_vertically_on_load(true);
		}
		unsigned char* data = stbi_load(filename.c_str(), &out_width, &out_height, &out_nChannels, 0);
		if (!data) return false;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		if (out_nChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, out_width, out_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (out_nChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_width, out_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		return true;
	}
}