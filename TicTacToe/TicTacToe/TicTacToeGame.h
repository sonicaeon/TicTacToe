#pragma once
#include "Game.h"
#include "DrawableGameComponent.h"
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace Library
{
	class KeyboardComponent;
	class ImGuiComponent;
}

namespace TicTacToe
{
	class TicTacToeGame : public Library::Game
	{
	public:
		TicTacToeGame(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

		char DrawPosition(int position);
		void Reset();

	private:
		void Exit();

		static DirectX::XMVECTORF32 BackgroundColor;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureX;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureO;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureEmpty;
		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::ImGuiComponent> mImGui;

		bool mShowWindow;
	};
}