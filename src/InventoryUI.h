#pragma once

#include <array>
#include <string>
#include <vector>

#include <nds.h>
#include <entt.hpp>

#include "Inventory.h"
#include "Player.h"
#include "Transform.h"

class BuildMode;

struct InvIcon { int slotIndex = 0; bool occupied = false; int frameIndex = -1; };
struct InvArrow { int direction = 0; };
struct InvDragIcon {};
struct InvOverlay {};

enum class InventoryScreenMode : int { Inventory = 0, Craft = 1 };

class InventoryUI
{
public:
    void Init(entt::registry& registry);
    void Update(entt::registry& registry, const BuildMode& buildMode);
    void Draw();

    static bool IsTempItemsOn();
    static void ToggleTempItems();

private:
    static constexpr int kItemsPerPage = 18;
    static constexpr int kIconSize = 32;
    static constexpr int kTextYOffsetPixels = 4;
    static constexpr int kArrowHoldFrames = 30;
    static constexpr int kTempTestItemCount = 40;

    static constexpr int kDragIconOamId = 80;
    static constexpr int kFirstIconOamId = 81;
    static constexpr int kLeftArrowOamId = 99;
    static constexpr int kRightArrowOamId = 100;
    static constexpr int kSelectionOamId = 101;

    static constexpr int kBgCount = 4;
    static constexpr int kBgSize = 256;
    static constexpr int kBgVisibleHeight = 192;

    static constexpr int kIconOverlay = 0;
    static constexpr int kIconArrow = 1;
    static constexpr int kIconTemp = 2;
    static constexpr int kIconCoal = 3;
    static constexpr int kIconCopper = 4;
    static constexpr int kIconIron = 5;

    static constexpr const char* kDefaultText = "FactaniaOS";

    struct Rect
    {
        int left;
        int top;
        int right;
        int bottom;

        bool Contains(int x, int y) const { return x >= left && x <= right && y >= top && y <= bottom; }
    };

    struct DisplayData
    {
        std::string id;
        std::string label;
        ItemType itemType;
        int quantity = 0;
        bool isTemp = false;
    };

    struct DisplaySlot
    {
        bool occupied = false;
        DisplayData data{};
    };

    struct PressState
    {
        int slot = -1;
        int arrow = 0;
        int mode = 0;

        void Reset() { slot = -1; arrow = 0; mode = 0; }
    };

    static constexpr Rect kHeaderBox{ 2, 2, 253, 29 };
    static constexpr Rect kLeftArrowBox{ 2, 32, 17, 159 };
    static constexpr Rect kRightArrowBox{ 237, 32, 253, 159 };
    static constexpr Rect kModeLeftButtonBox{ 2, 162, 126, 189 };
    static constexpr Rect kModeRightButtonBox{ 129, 162, 253, 189 };
    static constexpr std::array<Rect, kItemsPerPage> kSlotRects = {{
        {27, 39, 58, 70},   {61, 39, 92, 70},   {95, 39, 126, 70},
        {129, 39, 160, 70}, {163, 39, 194, 70}, {197, 39, 228, 70},
        {27, 80, 58, 111},  {61, 80, 92, 111},  {95, 80, 126, 111},
        {129, 80, 160, 111},{163, 80, 194, 111},{197, 80, 228, 111},
        {27, 121, 58, 152}, {61, 121, 92, 152}, {95, 121, 126, 152},
        {129, 121, 160, 152},{163, 121, 194, 152},{197, 121, 228, 152},
    }};

    static DisplayData MakeTempData(int index);

    bool m_textVisible = true;
    bool m_textDirty = true;
    bool m_previousRunNormally = true;
    bool m_previousUiVisible = false;
    bool m_dragFrozenForBuildMode = false;
    bool m_bgDirty = true;
    bool m_dragging = false;

    InventoryScreenMode m_screenMode = InventoryScreenMode::Inventory;
    PrintConsole m_textConsole{};

    std::array<entt::entity, kItemsPerPage> m_iconEntities{};
    std::array<u16*, kItemsPerPage> m_iconGfx{};
    entt::entity m_leftArrowEntity = entt::null;
    entt::entity m_rightArrowEntity = entt::null;
    entt::entity m_dragEntity = entt::null;
    entt::entity m_selectionEntity = entt::null;
    u16* m_leftArrowGfx = nullptr;
    u16* m_rightArrowGfx = nullptr;
    u16* m_dragGfx = nullptr;
    u16* m_selectionGfx = nullptr;

    std::vector<DisplaySlot> m_slots;
    int m_currentPage = 0;
    int m_selectedAbsoluteSlot = -1;
    int m_dragSourceAbsoluteSlot = -1;
    int m_lastTouchX = 0;
    int m_lastTouchY = 0;
    int m_arrowHoldDirection = 0;
    int m_arrowHoldFrames = 0;
    int m_currentBgFrame = 0;
    int m_appliedBgFrame = -1;
    int m_subBg3Id = -1;
    u8* m_subBg3Gfx = nullptr;
    std::string m_selectedText;
    PressState m_pressed{};

    void InitBackground();
    void EnsureBackgroundReady();
    void ApplyBackgroundFrame();
    void SetBackgroundFrame(int frameIndex);

    void InitTextConsole();
    void SetTextVisible(bool visible);
    void DrawTopText();

    void CreateEntities(entt::registry& registry);
    entt::entity CreateSpriteEntity(entt::registry& registry, u16*& gfx, int oamId, const Rect& rect, int frameIndex, bool hide, bool xFlip);

    void ApplyBuildModeState(const BuildMode& buildMode);
    bool IsInventoryMode() const { return m_screenMode == InventoryScreenMode::Inventory; }
    bool IsCraftMode() const { return m_screenMode == InventoryScreenMode::Craft; }

    entt::entity FindPlayer(entt::registry& registry) const;
    std::vector<DisplayData> BuildLiveEntries(const Inventory& inventory) const;
    void SyncFromInventory(const Inventory& inventory);
    void EnsureSlotSize(int absoluteSlot);
    void CutEmptyPages();
    void ClampPage();
    int PageCount() const;
    int FirstIndexOnCurrentPage() const { return m_currentPage * kItemsPerPage; }
    int SlotToAbsoluteIndex(int slotIndex) const { return FirstIndexOnCurrentPage() + slotIndex; }
    bool IsValidAbsoluteSlot(int absoluteSlot) const;
    bool IsOccupiedAbsoluteSlot(int absoluteSlot) const;
    void EnsureSelectionIsValid();
    void UpdateSelectedText();

    void RefreshSlotEntities(entt::registry& registry);
    void RefreshArrowEntities(entt::registry& registry);

    int FindTouchedSlot(int x, int y) const;
    bool AnyPageOnLeft() const;
    bool AnyPageOnRight() const;
    bool TouchInsideAbsoluteSlot(int absoluteSlot, int x, int y) const;

    void HandleTouch(entt::registry& registry);
    void OnTouchDown();
    void OnTouchHeld(entt::registry& registry);
    void OnTouchUp(entt::registry& registry);
    bool HandleModePressOnDown();
    bool HandleArrowPressOnDown();
    void UpdatePressedModeButton();
    void UpdatePressedArrow();
    void ApplyModeSwitchIfConfirmed();
    void ApplyPageTurnIfConfirmed();

    void StartDrag(entt::registry& registry);
    void UpdateDrag(entt::registry& registry);
    void FinishDrag(entt::registry& registry, int releaseX, int releaseY);
    void CancelDragToSource();
    void UpdateDragSprite(entt::registry& registry, int touchX, int touchY);
    void UpdateArrowHoldWhileDragging();

    void ResetPressState();
    void ResetArrowHold();

    int FrameIndexForEntry(const DisplayData& entry) const;
    void CopyItemFrame(u16* dstGfx, int frameIndex) const;
    static int CenterXInBox(const Rect& rect, int width);
    static int CenterYInBox(const Rect& rect, int height);
};
