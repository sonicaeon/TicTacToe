#include "pch.h"
#include "TicTacToeGame.h"
#include <ImGui\imgui_impl_dx11.h>
#include "Board.h"

using namespace std;
using namespace DirectX;
using namespace Library;
using namespace Microsoft::WRL;

IMGUI_API LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace TicTacToe
{
	Board::smallint human = 0;
	int32_t boardSize = 9;
	int move = -1;
	bool newGame = true;
	bool cpuTurn = false;
	const char X = 'x', O = 'o', empty = ' ';
	XMVECTORF32 TicTacToeGame::BackgroundColor = Colors::Black;

	Board root;
	Board* game = &root;

	TicTacToeGame::TicTacToeGame(function<void*()> getWindowCallback, function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mShowWindow(true)
	{
	}

	void TicTacToeGame::Initialize()
	{
		// Load all of the textures & components
		ComPtr<ID3D11Resource> textureResource;
		ComPtr<ID3D11Texture2D> texture;
		wstring textureName = L"Content\\Textures\\X-icon.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureX.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\O-icon.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureO.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		textureName = L"Content\\Textures\\empty-icon.png";
		ThrowIfFailed(CreateWICTextureFromFile(this->Direct3DDevice(), textureName.c_str(), textureResource.ReleaseAndGetAddressOf(), mTextureEmpty.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11Resource returned from CreateWICTextureFromFile. Should be a ID3D11Texture2D.");

		SpriteManager::Initialize(*this);
		BlendStates::Initialize(mDirect3DDevice.Get());

		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mImGui = make_shared<ImGuiComponent>(*this);
		mComponents.push_back(mImGui);
		mServices.AddService(ImGuiComponent::TypeIdClass(), mImGui.get());
		auto imGuiWndProcHandler = make_shared<UtilityWin32::WndProcHandler>(ImGui_ImplDX11_WndProcHandler);
		UtilityWin32::AddWndProcHandler(imGuiWndProcHandler);

		// render popup for player's character select (X or O)
		auto popUpMenuBlock = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			// popup menu prompting player to select their piece: X or O which will determine who will also go first
			if (newGame)
			{
				ImGui::OpenPopup("Select Your Piece");
				if (ImGui::BeginPopupModal("Select Your Piece", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Please select X or O.\nX will go first, O goes second\n\n");
					ImGui::Separator();

					// X goes first, if human picks X cpuTurn is 2nd
					if (ImGui::Button("X", ImVec2(120, 0))) 
					{
						human = Board::MAX;
						cpuTurn = false;
						newGame = false;
						ImGui::CloseCurrentPopup(); 
					}
					ImGui::SameLine();
					if (ImGui::Button("O", ImVec2(120, 0)))
					{ 
						human = Board::MIN;
						cpuTurn = true;
						newGame = false;
						ImGui::CloseCurrentPopup(); 
					}
					ImGui::EndPopup();
				}
			}
		});
		mImGui->AddRenderBlock(popUpMenuBlock);
		
		// Render space for the Grid and buttons
		auto gridRenderBlock = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			ImGui::Begin("TicTacToe", &mShowWindow, ImGuiWindowFlags_NoTitleBar);
			ImGui::SetWindowSize(ImVec2(800, 600));
			ImGui::SetWindowPos(ImVec2(150, 50));
			
			for (int i = 0; i < boardSize; i++)
			{
				// push unique button ID
				ImGui::PushID(i);
				if (TicTacToeGame::DrawPosition(game->board[i]) == X)
				{
					ImGui::ImageButton(mTextureX.Get(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), -1);
				}
				else if (TicTacToeGame::DrawPosition(game->board[i]) == O)
				{
					ImGui::ImageButton(mTextureO.Get(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), -1);
				}
				else if (ImGui::ImageButton(mTextureEmpty.Get(), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), -1))
				{
					// human's turn
					if (!cpuTurn)
					{
						// when empty button is pressed place X or O depending what the player's piece is
						if (human == Board::MAX)
						{
							game->board[i] = Board::MAX;
							move = i;
						}
						else if (human == Board::MIN)
						{
							game->board[i] = Board::MIN;
							move = i;
						}

						// update the board, it's now the cpu's turn
						cpuTurn = true;
						game = game->get_child((Board::smallint)move);
					}
				}
				else if (cpuTurn)
				{
					// Computer's turn
					if (human == Board::MAX)
					{
						Board::smallint min = +Board::INF;
						for (Board::smallint p = 0; p < Board::MAX_POSITIONS; ++p)
						{
							if (game->board[p] == Board::ZERO)
							{
								Board *child = game->get_child(p);
								if (child->value < min)
								{
									min = child->value;
									move = p;
								}
							}
						}
					}
					else
					{
						Board::smallint max = -Board::INF;
						for (Board::smallint p = 0; p < Board::MAX_POSITIONS; ++p)
						{
							if (game->board[p] == Board::ZERO)
							{
								Board *child = game->get_child(p);
								if (child->value > max)
								{
									max = child->value;
									move = p;
								}
							}
						}
					}
					// update the board, it's the player's turn
					cpuTurn = false;
					game = game->get_child((Board::smallint)move);
				}
				ImGui::PopID();
				ImGui::SameLine();
				if ((i + 1) % 3 == 0)
					ImGui::NewLine(); // new line for grid
			}
			ImGui::End();
		});
		mImGui->AddRenderBlock(gridRenderBlock);

		// render popup for game over state
		auto gameStateMenuBlock = make_shared<ImGuiComponent::RenderBlock>([this]()
		{
			// if game is in a terminal state (turn 5 to 9)
			if (game->depth >= 5)
			{
				// check for the win/draw/lose state of the game
				if (game->WinState() || (game->depth == Board::MAX_POSITIONS))
				{
					// evaluate the score
					Board::smallint score = human * game->value;
					ImGui::OpenPopup("Game Over");
					if (ImGui::BeginPopupModal("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						if (score > 0)
						{
							// player won
							ImGui::Text("Congratulations You Won!\n\n\n");
							ImGui::Separator();
						}
						else if (score < 0)
						{
							// player lost
							ImGui::Text("You Lost!\n\n\n");
							ImGui::Separator();
						}
						else
						{
							// draw
							ImGui::Text("Draw!\n\n\n");
							ImGui::Separator();
						}

						//if (ImGui::Button("Yes", ImVec2(120, 0))) { TicTacToeGame::Reset(); ImGui::CloseCurrentPopup(); }
						//ImGui::SameLine();
						if (ImGui::Button("Quit", ImVec2(120, 0))) { TicTacToeGame::Exit(); ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
				}
			}
		});
		mImGui->AddRenderBlock(gameStateMenuBlock);
		
		
		Game::Initialize();
	}

	void TicTacToeGame::Shutdown()
	{
		BlendStates::Shutdown();
		SpriteManager::Shutdown();

		Game::Shutdown();
	}

	void TicTacToeGame::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape))
		{
			Exit();
		}

		Game::Update(gameTime);
	}

	void TicTacToeGame::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
	}

	// When a space is selected by Max (X) or Min (O) return and display X or O respectively else display nothing in the grid space
	char TicTacToeGame::DrawPosition(int position)
	{
		//const char X = 'x', O = 'o', empty = ' ';

		if (position == Board::MAX)
			return X;
		else if (position == Board::MIN)
			return O;
		else // if (position == TicTacToe::ZERO)
			return empty;
	}

	void TicTacToeGame::Reset()
	{
		newGame = true;
		human = 0;
		move = -1;
	}

	void TicTacToeGame::Exit()
	{
		PostQuitMessage(0);
	}
}