#include "InventoryUI.h"

#include <algorithm>
#include <cstdio>

#include <nds.h>

#include "BG.h"
#include "BuildMode.h"
#include "Console.h"
#include "Items.h"
#include "Pause.h"
#include "Sprite.h"

extern PrintConsole g_console;

bool g_tempTestItemsEnabled = false;

template <typename T>
T ClampValue(T value, T minValue, T maxValue)
{
    return std::max(minValue, std::min(value, maxValue));
}

InventoryUI::DisplayData InventoryUI::MakeTempData(int index)
{
    DisplayData data;
    data.id = "temp:" + std::to_string(index);
    data.label = "temp" + std::to_string(index);
    data.quantity = 1;
    data.isTemp = true;
    return data;
}

bool InventoryUI::IsTempItemsOn() { return g_tempTestItemsEnabled; }
void InventoryUI::ToggleTempItems() { g_tempTestItemsEnabled = !g_tempTestItemsEnabled; }

void InventoryUI::Init(entt::registry& registry)
{
    dmaCopy(ItemsPal, SPRITE_PALETTE_SUB, ItemsPalLen);
    InitBackground();
    InitTextConsole();
    CreateEntities(registry);

    m_textDirty = true;
}

void InventoryUI::Update(entt::registry& registry, const BuildMode& buildMode)
{
    const bool runNormally = buildMode.ShouldInventoryRunNormally();
    const bool uiVisible = buildMode.ShouldInventoryUiVisible();

    if (!runNormally && m_previousRunNormally && m_dragging) {
        m_dragFrozenForBuildMode = true;
    }

    ApplyBuildModeState(buildMode);

    if (uiVisible && !m_previousUiVisible) {
        m_appliedBgFrame = -1;
        m_bgDirty = true;
    }

    if (!uiVisible) {
        if (IsInventoryMode()) {
            SetBackgroundFrame(0);
        }
    }
    else if (m_bgDirty) {
        ApplyBackgroundFrame();
    }

    if (runNormally) {
        if (const entt::entity player = FindPlayer(registry); player != entt::null) {
            const Inventory& inventory = registry.get<Inventory>(player);
            SyncFromInventory(inventory);
            ClampPage();

            if (m_dragging && !IsOccupiedAbsoluteSlot(m_dragSourceAbsoluteSlot)) {
                registry.get<SubSprite>(m_dragEntity).hide = true;
                m_dragging = false;
                m_dragSourceAbsoluteSlot = -1;
                ResetPressState();
            }

            HandleTouch(registry);
            UpdateSelectedText();
        }
    }
    else if (!uiVisible && m_dragFrozenForBuildMode) {
        CancelDragToSource();
        m_dragFrozenForBuildMode = false;
        UpdateSelectedText();
    }

    if (m_bgDirty) {
        ApplyBackgroundFrame();
    }

    RefreshSlotEntities(registry);
    RefreshArrowEntities(registry);

    m_previousRunNormally = runNormally;
    m_previousUiVisible = uiVisible;
}

void InventoryUI::Draw()
{
    DrawTopText();
}

void InventoryUI::InitBackground()
{
    EnsureBackgroundReady();
    m_currentBgFrame = 0;
    m_appliedBgFrame = -1;
    m_bgDirty = true;
    ApplyBackgroundFrame();
}

void InventoryUI::EnsureBackgroundReady()
{
    m_subBg3Id = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    bgSetPriority(m_subBg3Id, 3);
    m_subBg3Gfx = reinterpret_cast<u8*>(bgGetGfxPtr(m_subBg3Id));
    REG_DISPCNT_SUB |= DISPLAY_BG3_ACTIVE;
}

void InventoryUI::ApplyBackgroundFrame()
{
    if (!m_bgDirty && m_appliedBgFrame == m_currentBgFrame) return;

    EnsureBackgroundReady();

    const int frame = ClampValue(m_currentBgFrame, 0, kBgCount - 1);
    const unsigned char* srcBase = reinterpret_cast<const unsigned char*>(BGBitmap);
    const int srcStrideBytes = kBgSize * kBgCount;
    const int frameXOffsetBytes = frame * kBgSize;

    for (int y = 0; y < kBgVisibleHeight; ++y) {
        dmaCopy(srcBase + y * srcStrideBytes + frameXOffsetBytes,
            m_subBg3Gfx + y * kBgSize,
            kBgSize);
    }

    dmaCopy(BGPal, BG_PALETTE_SUB, BGPalLen);
    BG_PALETTE_SUB[255] = RGB15(0, 0, 0);

    m_appliedBgFrame = frame;
    m_bgDirty = false;
}

void InventoryUI::SetBackgroundFrame(int frameIndex)
{
    frameIndex = ClampValue(frameIndex, 0, kBgCount - 1);
    if (m_currentBgFrame == frameIndex && !m_bgDirty) return;
    m_currentBgFrame = frameIndex;
    m_bgDirty = true;
}

void InventoryUI::InitTextConsole()
{
    consoleInit(&m_textConsole, 1, BgType_Text4bpp, BgSize_T_256x256, 28, 1, false, true);
    BG_PALETTE_SUB[255] = RGB15(0, 0, 0);
    bgSetPriority(m_textConsole.bgId, 2);
    bgSetScroll(m_textConsole.bgId, 0, -kTextYOffsetPixels);
    bgUpdate();

    REG_DISPCNT_SUB |= DISPLAY_BG1_ACTIVE;
    bgShow(m_textConsole.bgId);

    consoleSelect(&m_textConsole);
    consoleClear();
    consoleSelect(&g_console);
}

void InventoryUI::SetTextVisible(bool visible)
{
    if (m_textVisible == visible) return;

    m_textVisible = visible;
    if (visible) {
        InitTextConsole();
        REG_DISPCNT_SUB |= DISPLAY_BG1_ACTIVE;
        bgShow(m_textConsole.bgId);
        m_textDirty = true;
    }
    else {
        bgHide(m_textConsole.bgId);
        REG_DISPCNT_SUB &= ~DISPLAY_BG1_ACTIVE;
    }
}

entt::entity InventoryUI::CreateSpriteEntity(entt::registry& registry, u16*& gfx, int oamId, const Rect& rect, int frameIndex, bool hide, bool xFlip)
{
    gfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    CopyItemFrame(gfx, frameIndex);

    const entt::entity entity = registry.create();
    registry.emplace<Transform>(entity, Vec2(FINT(rect.left), FINT(rect.top)), 0);
    registry.emplace<SubSprite>(entity, gfx, oamId, SpriteSize_32x32, kIconSize, hide, xFlip);
    return entity;
}

void InventoryUI::CreateEntities(entt::registry& registry)
{
    for (int i = 0; i < kItemsPerPage; ++i) {
        m_iconEntities[i] = CreateSpriteEntity(registry, m_iconGfx[i], kFirstIconOamId + i, kSlotRects[i], kIconTemp, true, false);
        registry.emplace<InvIcon>(m_iconEntities[i], i, false, -1);
    }

    const Rect leftRect{ CenterXInBox(kLeftArrowBox, kIconSize), CenterYInBox(kLeftArrowBox, kIconSize), 0, 0 };
    const Rect rightRect{ CenterXInBox(kRightArrowBox, kIconSize), CenterYInBox(kRightArrowBox, kIconSize), 0, 0 };
    const Rect zeroRect{ 0, 0, 0, 0 };

    m_leftArrowEntity = CreateSpriteEntity(registry, m_leftArrowGfx, kLeftArrowOamId, leftRect, kIconArrow, true, true);
    registry.emplace<InvArrow>(m_leftArrowEntity, -1);

    m_rightArrowEntity = CreateSpriteEntity(registry, m_rightArrowGfx, kRightArrowOamId, rightRect, kIconArrow, true, false);
    registry.emplace<InvArrow>(m_rightArrowEntity, 1);

    m_dragEntity = CreateSpriteEntity(registry, m_dragGfx, kDragIconOamId, zeroRect, kIconTemp, true, false);
    registry.emplace<InvDragIcon>(m_dragEntity);

    m_selectionEntity = CreateSpriteEntity(registry, m_selectionGfx, kSelectionOamId, zeroRect, kIconOverlay, true, false);
    registry.emplace<InvOverlay>(m_selectionEntity);
}

void InventoryUI::ApplyBuildModeState(const BuildMode& buildMode)
{
    SetTextVisible(buildMode.ShouldInventoryUiVisible() && IsInventoryMode());
}

std::vector<InventoryUI::DisplayData> InventoryUI::BuildLiveEntries(const Inventory& inventory) const
{
    std::vector<DisplayData> entries;
    for (int i = 0; i < Inventory::MAX_ITEMS; ++i) {
        const ItemType type = static_cast<ItemType>(i);
        const int quantity = inventory.GetItem(type);
        if (quantity <= 0) continue;
        entries.push_back({ "item:" + std::to_string(i), ItemName(type), type, quantity, false });
    }
    if (IsTempItemsOn()) {
        for (int i = 0; i <= kTempTestItemCount; ++i)
            entries.push_back(MakeTempData(i));
    }
    return entries;
}

void InventoryUI::EnsureSlotSize(int absoluteSlot)
{
    if (absoluteSlot < 0 || absoluteSlot < static_cast<int>(m_slots.size())) return;
    const int requiredPages = (absoluteSlot + kItemsPerPage) / kItemsPerPage;
    m_slots.resize(static_cast<size_t>(requiredPages * kItemsPerPage));
}

void InventoryUI::CutEmptyPages()
{
    int highestOccupied = -1;
    for (int i = static_cast<int>(m_slots.size()) - 1; i >= 0; --i) {
        if (m_slots[i].occupied) { highestOccupied = i; break; }
    }

    const int minSlots = std::max(kItemsPerPage, ((highestOccupied + kItemsPerPage) / kItemsPerPage) * kItemsPerPage);
    m_slots.resize(static_cast<size_t>(std::max(minSlots, kItemsPerPage)));
}

void InventoryUI::ClampPage()
{
    m_currentPage = ClampValue(m_currentPage, 0, std::max(0, PageCount() - 1));
}

int InventoryUI::PageCount() const
{
    return std::max(1, static_cast<int>(m_slots.empty() ? 0 : m_slots.size()) / kItemsPerPage);
}

bool InventoryUI::IsValidAbsoluteSlot(int absoluteSlot) const
{
    return absoluteSlot >= 0 && absoluteSlot < static_cast<int>(m_slots.size());
}

bool InventoryUI::IsOccupiedAbsoluteSlot(int absoluteSlot) const
{
    return IsValidAbsoluteSlot(absoluteSlot) && m_slots[absoluteSlot].occupied;
}

void InventoryUI::EnsureSelectionIsValid()
{
    if (!IsOccupiedAbsoluteSlot(m_selectedAbsoluteSlot)) {
        m_selectedAbsoluteSlot = -1;
    }
}

void InventoryUI::SyncFromInventory(const Inventory& inventory)
{
    const std::vector<DisplayData> liveEntries = BuildLiveEntries(inventory);
    if (m_slots.empty()) {
        m_slots.resize(static_cast<size_t>(std::max(1, static_cast<int>((liveEntries.size() + kItemsPerPage - 1) / kItemsPerPage)) * kItemsPerPage));
        for (int i = 0; i < static_cast<int>(liveEntries.size()); ++i) {
            m_slots[i] = { true, liveEntries[i] };
        }
        EnsureSelectionIsValid();
        return;
    }

    std::vector<bool> used(liveEntries.size(), false);
    for (DisplaySlot& slot : m_slots) {
        if (!slot.occupied) {
            continue;
        }
        int found = -1;
        for (int i = 0; i < static_cast<int>(liveEntries.size()); ++i) {
            if (!used[i] && liveEntries[i].id == slot.data.id) { found = i; break; }
        }
        if (found >= 0) {
            slot.data = liveEntries[found];
            used[found] = true;
        }
        else {
            slot.occupied = false;
        }
    }

    for (int i = 0; i < static_cast<int>(liveEntries.size()); ++i) {
        if (used[i]) continue;
        auto it = std::find_if(m_slots.begin(), m_slots.end(), [](const DisplaySlot& slot) { return !slot.occupied; });
        int index = (it != m_slots.end()) ? static_cast<int>(it - m_slots.begin()) : static_cast<int>(m_slots.size());
        EnsureSlotSize(index);
        m_slots[index] = { true, liveEntries[i] };
    }

    CutEmptyPages();
    EnsureSelectionIsValid();
}

void InventoryUI::UpdateSelectedText()
{
    const std::string next = IsOccupiedAbsoluteSlot(m_selectedAbsoluteSlot)
        ? (m_slots[m_selectedAbsoluteSlot].data.label + " x" + std::to_string(m_slots[m_selectedAbsoluteSlot].data.quantity))
        : kDefaultText;
    if (next != m_selectedText) {
        m_selectedText = next;
        m_textDirty = true;
    }
}

entt::entity InventoryUI::FindPlayer(entt::registry& registry) const
{
    auto view = registry.view<PlayerState, Inventory>();
    return (view.begin() != view.end()) ? *view.begin() : entt::null;
}

void InventoryUI::RefreshSlotEntities(entt::registry& registry)
{
    const int firstIndex = FirstIndexOnCurrentPage();
    for (int slotIndex = 0; slotIndex < kItemsPerPage; ++slotIndex) {
        auto& icon = registry.get<InvIcon>(m_iconEntities[slotIndex]);
        auto& sprite = registry.get<SubSprite>(m_iconEntities[slotIndex]);
        const int absoluteIndex = firstIndex + slotIndex;
        const bool visible = IsInventoryMode() && IsOccupiedAbsoluteSlot(absoluteIndex) && (!m_dragging || absoluteIndex != m_dragSourceAbsoluteSlot);

        if (!visible) {
            icon.occupied = false;
            icon.frameIndex = -1;
            sprite.hide = true;
            continue;
        }

        const int frame = FrameIndexForEntry(m_slots[absoluteIndex].data);
        if (icon.frameIndex != frame) {
            CopyItemFrame(sprite.gfx, frame);
            icon.frameIndex = frame;
        }
        icon.occupied = true;
        sprite.hide = false;
    }

    registry.get<SubSprite>(m_dragEntity).hide = !m_dragging || IsCraftMode();

    auto& selectionTransform = registry.get<Transform>(m_selectionEntity);
    auto& selectionSprite = registry.get<SubSprite>(m_selectionEntity);
    const bool selectedOnPage = IsInventoryMode() && IsOccupiedAbsoluteSlot(m_selectedAbsoluteSlot) && (m_selectedAbsoluteSlot / kItemsPerPage) == m_currentPage;
    if (!selectedOnPage) {
        selectionSprite.hide = true;
        return;
    }

    const Rect& rect = kSlotRects[m_selectedAbsoluteSlot % kItemsPerPage];
    selectionTransform.pos.X() = fixed(static_cast<int32>(rect.left));
    selectionTransform.pos.Y() = fixed(static_cast<int32>(rect.top));
    selectionSprite.hide = false;
}

void InventoryUI::RefreshArrowEntities(entt::registry& registry)
{
    registry.get<SubSprite>(m_leftArrowEntity).hide = IsCraftMode() || !AnyPageOnLeft();
    registry.get<SubSprite>(m_rightArrowEntity).hide = IsCraftMode() || !AnyPageOnRight();
}

int InventoryUI::FindTouchedSlot(int x, int y) const
{
    for (int i = 0; i < kItemsPerPage; ++i) {
        if (kSlotRects[i].Contains(x, y)) return i;
    }
    return -1;
}

bool InventoryUI::AnyPageOnLeft() const { return m_currentPage > 0; }
bool InventoryUI::AnyPageOnRight() const { return m_currentPage + 1 < PageCount(); }

bool InventoryUI::TouchInsideAbsoluteSlot(int absoluteSlot, int x, int y) const
{
    return absoluteSlot >= 0 && kSlotRects[absoluteSlot % kItemsPerPage].Contains(x, y);
}

void InventoryUI::HandleTouch(entt::registry& registry)
{
    if (registry.ctx().get<PauseControl>().PauseEntity()) {
        if (IsInventoryMode()) SetBackgroundFrame(0);
        ResetArrowHold();
        ResetPressState();
        return;
    }

    const uint16_t down = keysDown();
    const uint16_t held = keysHeld();
    const uint16_t up = keysUp();

    if ((down & KEY_TOUCH) || (held & KEY_TOUCH)) {
        touchPosition touch;
        touchRead(&touch);
        m_lastTouchX = touch.px;
        m_lastTouchY = touch.py;
    }

    if (down & KEY_TOUCH) {
        OnTouchDown();
    }
    if (held & KEY_TOUCH) {
        OnTouchHeld(registry);
    }
    if (up & KEY_TOUCH) {
        OnTouchUp(registry);
    }
}

void InventoryUI::OnTouchDown()
{
    if (IsCraftMode()) {
        if (kModeLeftButtonBox.Contains(m_lastTouchX, m_lastTouchY)) {
            m_pressed.mode = -1;
        }
        return;
    }

    if (HandleModePressOnDown() || HandleArrowPressOnDown()) return;

    const int slotIndex = FindTouchedSlot(m_lastTouchX, m_lastTouchY);
    const int absoluteIndex = slotIndex >= 0 ? SlotToAbsoluteIndex(slotIndex) : -1;
    m_selectedAbsoluteSlot = IsOccupiedAbsoluteSlot(absoluteIndex) ? absoluteIndex : -1;
    m_pressed.slot = m_selectedAbsoluteSlot;
    m_textDirty = true;
}

void InventoryUI::OnTouchHeld(entt::registry& registry)
{
    if (m_pressed.mode) { 
        UpdatePressedModeButton(); 
        return; 
    }
    if (!m_dragging && m_pressed.arrow) { 
        UpdatePressedArrow();
        return;
    }
    if (IsCraftMode()) return;

    if (!m_dragging && m_pressed.slot >= 0 && !TouchInsideAbsoluteSlot(m_pressed.slot, m_lastTouchX, m_lastTouchY)) {
        StartDrag(registry);
    }
    if (m_dragging) {
        UpdateDrag(registry);
    }
}

void InventoryUI::OnTouchUp(entt::registry& registry)
{
    if (m_dragging) {
        FinishDrag(registry, m_lastTouchX, m_lastTouchY);
    }
    else if (m_pressed.mode) {
        ApplyModeSwitchIfConfirmed();
    }
    else if (m_pressed.arrow) {
        ApplyPageTurnIfConfirmed();
    }

    ResetPressState();
}

bool InventoryUI::HandleModePressOnDown()
{
    if (kModeRightButtonBox.Contains(m_lastTouchX, m_lastTouchY)) {
        m_pressed.mode = 1;
        return true;
    }
    return false;
}

bool InventoryUI::HandleArrowPressOnDown()
{
    if (kLeftArrowBox.Contains(m_lastTouchX, m_lastTouchY) && AnyPageOnLeft()) {
        m_selectedAbsoluteSlot = -1;
        m_textDirty = true;
        m_pressed.arrow = -1;
        SetBackgroundFrame(1);
        return true;
    }
    if (kRightArrowBox.Contains(m_lastTouchX, m_lastTouchY) && AnyPageOnRight()) {
        m_selectedAbsoluteSlot = -1;
        m_textDirty = true;
        m_pressed.arrow = 1;
        SetBackgroundFrame(2);
        return true;
    }
    return false;
}

void InventoryUI::UpdatePressedModeButton()
{
    const Rect& rect = m_pressed.mode < 0 ? kModeLeftButtonBox : kModeRightButtonBox;
    if (!rect.Contains(m_lastTouchX, m_lastTouchY)) {
        m_pressed.mode = 0;
    }
}

void InventoryUI::UpdatePressedArrow()
{
    const Rect& rect = m_pressed.arrow < 0 ? kLeftArrowBox : kRightArrowBox;
    if (!rect.Contains(m_lastTouchX, m_lastTouchY)) {
        m_pressed.arrow = 0;
        if (IsInventoryMode()) {
            SetBackgroundFrame(0);
        }
    }
}

void InventoryUI::ApplyModeSwitchIfConfirmed()
{
    const Rect& rect = m_pressed.mode < 0 ? kModeLeftButtonBox : kModeRightButtonBox;
    if (!rect.Contains(m_lastTouchX, m_lastTouchY)) return;

    if (m_pressed.mode > 0 && IsInventoryMode()) {
        m_screenMode = InventoryScreenMode::Craft;
        SetBackgroundFrame(3);
    }
    else if (m_pressed.mode < 0 && IsCraftMode()) {
        m_screenMode = InventoryScreenMode::Inventory;
        SetBackgroundFrame(0);
    }
}

void InventoryUI::ApplyPageTurnIfConfirmed()
{
    const Rect& rect = m_pressed.arrow < 0 ? kLeftArrowBox : kRightArrowBox;
    if (!rect.Contains(m_lastTouchX, m_lastTouchY)) return;

    m_currentPage += m_pressed.arrow;
    ClampPage();
    m_selectedAbsoluteSlot = -1;
    m_textDirty = true;
    SetBackgroundFrame(0);
}

void InventoryUI::StartDrag(entt::registry& registry)
{
    if (!IsOccupiedAbsoluteSlot(m_pressed.slot)) return;

    m_dragging = true;
    m_dragSourceAbsoluteSlot = m_pressed.slot;
    ResetArrowHold();
    CopyItemFrame(m_dragGfx, FrameIndexForEntry(m_slots[m_dragSourceAbsoluteSlot].data));
    UpdateDragSprite(registry, m_lastTouchX, m_lastTouchY);
}

void InventoryUI::UpdateDrag(entt::registry& registry)
{
    UpdateDragSprite(registry, m_lastTouchX, m_lastTouchY);
    UpdateArrowHoldWhileDragging();
}

void InventoryUI::FinishDrag(entt::registry& registry, int releaseX, int releaseY)
{
    const int source = m_dragSourceAbsoluteSlot;
    const int targetSlot = FindTouchedSlot(releaseX, releaseY);
    if (targetSlot >= 0 && IsOccupiedAbsoluteSlot(source)) {
        const int target = SlotToAbsoluteIndex(targetSlot);
        EnsureSlotSize(target);
        if (target != source) {
            if (IsOccupiedAbsoluteSlot(target)) std::swap(m_slots[source], m_slots[target]);
            else { m_slots[target] = m_slots[source]; m_slots[source].occupied = false; }
            m_selectedAbsoluteSlot = target;
        }
        else {
            m_selectedAbsoluteSlot = source;
        }
    }
    else {
        m_selectedAbsoluteSlot = source;
    }

    registry.get<SubSprite>(m_dragEntity).hide = true;
    m_dragging = false;
    m_dragSourceAbsoluteSlot = -1;
    ResetArrowHold();
    SetBackgroundFrame(0);
    CutEmptyPages();
    ClampPage();
    UpdateSelectedText();
}

void InventoryUI::CancelDragToSource()
{
    m_dragging = false;
    m_dragSourceAbsoluteSlot = -1;
    ResetArrowHold();
    if (m_pressed.slot >= 0) {
        m_selectedAbsoluteSlot = m_pressed.slot;
    }
    ResetPressState();
}

void InventoryUI::UpdateDragSprite(entt::registry& registry, int touchX, int touchY)
{
    auto& tr = registry.get<Transform>(m_dragEntity);
    auto& sprite = registry.get<SubSprite>(m_dragEntity);
    tr.pos.X() = fixed(static_cast<int32>(touchX - (kIconSize / 2)));
    tr.pos.Y() = fixed(static_cast<int32>(touchY - (kIconSize / 2)));
    sprite.hide = false;
}

void InventoryUI::UpdateArrowHoldWhileDragging()
{
    int direction = 0;
    if (kLeftArrowBox.Contains(m_lastTouchX, m_lastTouchY) && AnyPageOnLeft()) {
        direction = -1;
    }
    else if (kRightArrowBox.Contains(m_lastTouchX, m_lastTouchY) && AnyPageOnRight()) {
        direction = 1;
    }

    if (direction == 0) {
        ResetArrowHold();
        if (IsInventoryMode()) SetBackgroundFrame(0);
        return;
    }

    SetBackgroundFrame(direction < 0 ? 1 : 2);
    if (direction != m_arrowHoldDirection) {
        m_arrowHoldDirection = direction;
        m_arrowHoldFrames = 1;
        return;
    }

    if (++m_arrowHoldFrames < kArrowHoldFrames) return;

    m_currentPage += direction;
    ClampPage();
    m_selectedAbsoluteSlot = -1;
    m_textDirty = true;
    m_arrowHoldFrames = 0;
}

void InventoryUI::ResetPressState()
{
    m_pressed.Reset();
    ResetArrowHold();
}

void InventoryUI::ResetArrowHold()
{
    m_arrowHoldDirection = 0;
    m_arrowHoldFrames = 0;
}

void InventoryUI::DrawTopText()
{
    if (!m_textDirty) return;

    static constexpr int kConsoleWidthChars = 32;
    const int boxWidth = kHeaderBox.right - kHeaderBox.left + 1;
    const int maxChars = std::max(0, boxWidth / 8);

    std::string clipped = m_selectedText;
    if (static_cast<int>(clipped.size()) > maxChars) {
        clipped.resize(static_cast<size_t>(maxChars));
    }

    int charX = 0;
    int subPixelOffsetX = 0;
    if (!clipped.empty()) {
        const int textPixelWidth = static_cast<int>(clipped.size()) * 8;
        const int startPixelX = kHeaderBox.left + (boxWidth - textPixelWidth) / 2;
        charX = ClampValue(startPixelX / 8, 0, kConsoleWidthChars - 1);
        subPixelOffsetX = startPixelX % 8;
    }

    std::string line(static_cast<size_t>(kConsoleWidthChars), ' ');
    if (!clipped.empty()) {
        line.replace(static_cast<size_t>(charX), static_cast<size_t>(std::min<int>(clipped.size(), kConsoleWidthChars - charX)), clipped);
    }

    consoleSelect(&m_textConsole);
    consoleSetCursor(&m_textConsole, 0, 1);
    std::printf("%.*s", kConsoleWidthChars, line.c_str());
    bgSetScroll(m_textConsole.bgId, -subPixelOffsetX, -kTextYOffsetPixels);
    bgUpdate();
    consoleSelect(&g_console);
    m_textDirty = false;
}

int InventoryUI::FrameIndexForEntry(const DisplayData& entry) const
{
    if (entry.isTemp) return kIconTemp;
    switch (entry.itemType)
    {
    case ItemType::Coal: return kIconCoal;
    case ItemType::Copper: return kIconCopper;
    case ItemType::Iron: return kIconIron;
    }
    return kIconTemp;
}

void InventoryUI::CopyItemFrame(u16* dstGfx, int frameIndex) const
{
    if (!dstGfx) return;

    constexpr int kTilesPerSpriteRow = 4;
    constexpr int kTilesPerImageRow = 32;
    constexpr int kTileBytes = 64;
    constexpr int kTileRowsPerSprite = 4;

    frameIndex = ClampValue(frameIndex, 0, 7);
    const int srcTileColumn = frameIndex * kTilesPerSpriteRow;
    const unsigned char* src = reinterpret_cast<const unsigned char*>(ItemsTiles);
    unsigned char* dst = reinterpret_cast<unsigned char*>(dstGfx);

    for (int tileRow = 0; tileRow < kTileRowsPerSprite; ++tileRow) {
        dmaCopy(src + ((tileRow * kTilesPerImageRow + srcTileColumn) * kTileBytes),
                dst + ((tileRow * kTilesPerSpriteRow) * kTileBytes),
                kTilesPerSpriteRow * kTileBytes);
    }
}

int InventoryUI::CenterXInBox(const Rect& rect, int width)
{
    return rect.left + ((rect.right - rect.left + 1 - width) / 2);
}

int InventoryUI::CenterYInBox(const Rect& rect, int height)
{
    return rect.top + ((rect.bottom - rect.top + 1 - height) / 2);
}
