// Sangbeom Kim
// 02/25/2026

#pragma once

struct PauseControl {
	bool pause = false;
	bool transition = false;
	bool PauseEntity() const { return pause || transition; }
};

namespace PauseUI {
	void PauseImageInit();
	void DrawPauseSpr();
	void DrawPauseObj();
	void HidePauseObj();
}