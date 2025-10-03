#pragma once

#include "SceneView/SceneView.h"

namespace KaputEditor
{
	class GameView : public SceneView
	{
	public:
		GameView();
		
		void postRender() override;

		void setWindowFocus();

		~GameView() = default;
	};
}
