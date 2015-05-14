#pragma once

#include"../../common/CommonDxSupport.hpp"
#include"GlobalWindow.hpp"
#include"../../common/CommonDef.hpp"

namespace aMazing
{
	class FrameBuffer
	{
	public:
		FrameBuffer()
		{
			is_inited = false;
			multiSampling = false;
			m_renderTargetTexture = nullptr;
			m_renderTargetView = nullptr;
			m_shaderResourceView = nullptr;
			m_pDepthStencil = nullptr;
			m_depthStencilView = nullptr;
			m_renderTargetTextureMS = nullptr;
		}
		~FrameBuffer()
		{
			aSAFE_RELEASE(m_shaderResourceView);
			aSAFE_RELEASE(m_renderTargetTexture);
			aSAFE_RELEASE(m_renderTargetView);
			aSAFE_RELEASE(m_pDepthStencil);
			aSAFE_RELEASE(m_depthStencilView);
			aSAFE_RELEASE(m_renderTargetTextureMS);
		}

		HRESULT Initialize(ID3D11Device* device, 
			ID3D11DeviceContext* context, 
			unsigned short imageWidth = -1, 
			unsigned short imageHeight = -1)
		{
			HRESULT hr;
			if (imageWidth == -1)
				imageWidth = RESWIDTH;
			if (imageHeight == -1)
				imageHeight = RESHEIGHT;
			D3D11_TEXTURE2D_DESC textureDesc;
			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			textureDesc.Width = imageWidth;
			textureDesc.Height = imageHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			hr = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
			if (FAILED(hr))
			{
				return hr;
			}
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
			hr = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
			if (FAILED(hr))
			{
				return hr;
			}
			shaderResourceViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
			if (FAILED(hr))
			{
				return hr;
			}

			D3D11_TEXTURE2D_DESC descDepth;
			ZeroMemory(&descDepth, sizeof(descDepth));
			descDepth.Width = imageWidth;
			descDepth.Height = imageHeight;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;
			hr = device->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
			if (FAILED(hr))
				return hr;

			// Create the depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			hr = device->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_depthStencilView);
			if (FAILED(hr))
				return hr;

			is_inited = true;
			return S_OK;
		}

		void setRenderTarget(ID3D11Device* , ID3D11DeviceContext* context)
		{
			context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		}

		void clearRenderTarget(ID3D11Device* device, ID3D11DeviceContext* context)
		{
			float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			context->ClearRenderTargetView(m_renderTargetView, clearColor);
			context->ClearDepthStencilView(m_depthStencilView,
				D3D11_CLEAR_DEPTH, 1.0f, NULL);
		}

		void bindVS(ID3D11Device* device,
			ID3D11DeviceContext* context,
			unsigned int textureSlot)
		{
			context->VSSetShaderResources(textureSlot, 1, &m_shaderResourceView);
		}
		void bindPS(ID3D11Device* device,
			ID3D11DeviceContext* context,
			unsigned int textureSlot)
		{
			//context->ResolveSubresource(m_renderTargetTextureMS,
			//	0,
			//	m_renderTargetTexture, 
			//	0, 
			//	DXGI_FORMAT_R32G32B32A32_FLOAT);
			context->PSSetShaderResources(textureSlot, 1, &m_shaderResourceView);
		}

		bool isInited() const
		{
			return is_inited;
		}

		void clearDepthBuffer(ID3D11Device* device, ID3D11DeviceContext* context)
		{
			context->ClearDepthStencilView(m_depthStencilView,
				D3D11_CLEAR_DEPTH, 1.0f, NULL);
		}
	private:
		friend class EffectClass;
		volatile bool is_inited;
		bool multiSampling;
		ID3D11Texture2D* m_renderTargetTexture;
		ID3D11Texture2D* m_renderTargetTextureMS;//Multi-sampled
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11Texture2D* m_pDepthStencil;
		ID3D11DepthStencilView* m_depthStencilView;
	};
}
