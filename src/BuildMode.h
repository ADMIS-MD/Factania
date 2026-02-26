// Sangbeom Kim
// 02/19/2026
 
#pragma once

#include <array>
#include <cstdint>

#include "entt.hpp"

#include "Camera.h"
#include "Math.h"

class BuildMode
{
public:
    BuildMode() = default;

    void Init();

    void Update(entt::registry& registry, const Camera& camera);

    void DrawMainOverlay();
    void DrawSubOverlay();

    bool IsTransitioning() const;
    bool IsBankBCapture() const;
    int GetFlashLevel() const;
    void SkipCapture();

private:
    enum class Request
    {
        None,
        Enter,
        Exit,
    };

    enum class DragMode
    {
        None,
        Place,
        Erase,
    };

    // Request Build Mode State
    Request m_request = Request::None;

    // Drag state of stylus
    bool m_dragActive = false;
    DragMode m_dragMode = DragMode::None;
    int32_t m_lastGridX = std::numeric_limits<int32_t>::max();
    int32_t m_lastGridY = std::numeric_limits<int32_t>::max();

    // Slide Transition State
    enum class SlideState
    {
        Idle,
        WaitingCapture,
        Sliding,
        Finished,
        ExitWaitingCapture,
        ExitSliding,
    };

    SlideState m_slideState = SlideState::Idle;

    static constexpr int kOverlayCols = 4;
    static constexpr int kOverlayRows = 3;
    static constexpr int kOverlayCount = kOverlayCols * kOverlayRows;

    static constexpr int kSubOverlayOamBase = 116;
    static constexpr int kMainOverlayOamBase = 116;

    std::array<u16*, kOverlayCount> m_subOverlayGfx{};
    std::array<u16*, kOverlayCount> m_mainOverlayGfx{};

    int m_subOverlayY = 0;
    int m_mainOverlayY = 0;

    // Flashing (might be reusable?)
    bool m_flashActive = false;
    int  m_flashLevel = 0;
    int  m_flashDecay = 1;

    // Transition Lerp
    fixed m_slidePos = FINT(0);
    fixed m_slideLerpFactor = FINT(0);
    int m_startDelay = 0;
    int m_exitDelay = 0;

    // Mirror Mode
    bool m_mirrorActive = false;
    bool m_mirrorInitialized = false;
    bool m_skipCapture = false;

    // Mirror/Background data toggle
    int  m_subBg3Id = -1;
    u16* m_subBg3Gfx = nullptr;

    // Main Screen Overlay Image
    int m_overlayId = -1;   // BG2
    int m_darkenId = -1;    // BG1

    // Helper
    void HandleToggle();
    void HandleTouchBuild(entt::registry& registry, const Camera& camera);

    void StartScreenCapture();

    void CopyRamToMainOverlay();
    void CopyRamToSubOverlay();

    void HideMainOverlaySprites();
    void HideSubOverlaySprites();

    void SubBg3ToBmp16();
    void SubBg3ToBmp8();

    void OverlayInit();
    void ToggleOverlay(bool on);
};
