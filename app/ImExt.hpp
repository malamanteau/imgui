/// =============================================================================
/// LICENSE - MIT (https://opensource.org/licenses/MIT)
///
/// imgui (https://github.com/ocornut/imgui)
/// 
/// Copyright (c) 2014-2020 Omar Cornut
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// =============================================================================

#pragma once

#include "../imgui_internal.h"

enum class StyleVar : ImGuiStyleVar
{
	Alpha               = ImGuiStyleVar_Alpha,               // float     Alpha
	WindowPadding       = ImGuiStyleVar_WindowPadding,       // ImVec2    WindowPadding
	WindowRounding      = ImGuiStyleVar_WindowRounding,      // float     WindowRounding
	WindowBorderSize    = ImGuiStyleVar_WindowBorderSize,    // float     WindowBorderSize
	WindowMinSize       = ImGuiStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
	WindowTitleAlign    = ImGuiStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
	ChildRounding       = ImGuiStyleVar_ChildRounding,       // float     ChildRounding
	ChildBorderSize     = ImGuiStyleVar_ChildBorderSize,     // float     ChildBorderSize
	PopupRounding       = ImGuiStyleVar_PopupRounding,       // float     PopupRounding
	PopupBorderSize     = ImGuiStyleVar_PopupBorderSize,     // float     PopupBorderSize
	FramePadding        = ImGuiStyleVar_FramePadding,        // ImVec2    FramePadding
	FrameRounding       = ImGuiStyleVar_FrameRounding,       // float     FrameRounding
	FrameBorderSize     = ImGuiStyleVar_FrameBorderSize,     // float     FrameBorderSize
	ItemSpacing         = ImGuiStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
	ItemInnerSpacing    = ImGuiStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
	IndentSpacing       = ImGuiStyleVar_IndentSpacing,       // float     IndentSpacing
	ScrollbarSize       = ImGuiStyleVar_ScrollbarSize,       // float     ScrollbarSize
	ScrollbarRounding   = ImGuiStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
	GrabMinSize         = ImGuiStyleVar_GrabMinSize,         // float     GrabMinSize
	GrabRounding        = ImGuiStyleVar_GrabRounding,        // float     GrabRounding
	TabRounding         = ImGuiStyleVar_TabRounding,         // float     TabRounding
	ButtonTextAlign     = ImGuiStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
	SelectableTextAlign = ImGuiStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
	COUNT               = ImGuiStyleVar_COUNT
};

enum class StyleCol : ImGuiCol
{
	Text                  = ImGuiCol_Text,
	TextDisabled          = ImGuiCol_TextDisabled,
	WindowBg              = ImGuiCol_WindowBg,              // Background of normal windows
	ChildBg               = ImGuiCol_ChildBg,               // Background of child windows
	PopupBg               = ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
	Border                = ImGuiCol_Border,
	BorderShadow          = ImGuiCol_BorderShadow,
	FrameBg               = ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
	FrameBgHovered        = ImGuiCol_FrameBgHovered,
	FrameBgActive         = ImGuiCol_FrameBgActive,
	TitleBg               = ImGuiCol_TitleBg,
	TitleBgActive         = ImGuiCol_TitleBgActive,
	TitleBgCollapsed      = ImGuiCol_TitleBgCollapsed,
	MenuBarBg             = ImGuiCol_MenuBarBg,
	ScrollbarBg           = ImGuiCol_ScrollbarBg,
	ScrollbarGrab         = ImGuiCol_ScrollbarGrab,
	ScrollbarGrabHovered  = ImGuiCol_ScrollbarGrabHovered,
	ScrollbarGrabActive   = ImGuiCol_ScrollbarGrabActive,
	CheckMark             = ImGuiCol_CheckMark,
	SliderGrab            = ImGuiCol_SliderGrab,
	SliderGrabActive      = ImGuiCol_SliderGrabActive,
	Button                = ImGuiCol_Button,
	ButtonHovered         = ImGuiCol_ButtonHovered,
	ButtonActive          = ImGuiCol_ButtonActive,
	Header                = ImGuiCol_Header,
	HeaderHovered         = ImGuiCol_HeaderHovered,
	HeaderActive          = ImGuiCol_HeaderActive,
	Separator             = ImGuiCol_Separator,
	SeparatorHovered      = ImGuiCol_SeparatorHovered,
	SeparatorActive       = ImGuiCol_SeparatorActive,
	ResizeGrip            = ImGuiCol_ResizeGrip,
	ResizeGripHovered     = ImGuiCol_ResizeGripHovered,
	ResizeGripActive      = ImGuiCol_ResizeGripActive,
	Tab                   = ImGuiCol_Tab,
	TabHovered            = ImGuiCol_TabHovered,
	TabActive             = ImGuiCol_TabActive,
	TabUnfocused          = ImGuiCol_TabUnfocused,
	TabUnfocusedActive    = ImGuiCol_TabUnfocusedActive,
	DockingPreview        = ImGuiCol_DockingPreview,
	DockingEmptyBg        = ImGuiCol_DockingEmptyBg,        // Background color for empty node (e.g. CentralNode with no window docked into it)
	PlotLines             = ImGuiCol_PlotLines,
	PlotLinesHovered      = ImGuiCol_PlotLinesHovered,
	PlotHistogram         = ImGuiCol_PlotHistogram,
	PlotHistogramHovered  = ImGuiCol_PlotHistogramHovered,
	TextSelectedBg        = ImGuiCol_TextSelectedBg,
	DragDropTarget        = ImGuiCol_DragDropTarget,
	NavHighlight          = ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
	NavWindowingHighlight = ImGuiCol_NavWindowingHighlight, // Gamepad/keyboard: when holding NavMenu to focus/move/resize windows
	NavWindowingDimBg     = ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	ModalWindowDimBg      = ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
	COUNT                 = ImGuiCol_COUNT
};

enum class StyleItem
{
	Width,

	COUNT
};


enum class WindowFlags
{
	None                   = ImGuiWindowFlags_None,
	NoTitleBar             = ImGuiWindowFlags_NoTitleBar,                  // Disable title-bar
	NoResize               = ImGuiWindowFlags_NoResize,                    // Disable user resizing with the lower-right grip
	NoMove                 = ImGuiWindowFlags_NoMove,                      // Disable user moving the window
	NoScrollbar            = ImGuiWindowFlags_NoScrollbar,                 // Disable scrollbars (window can still scroll with mouse or programatically)
	NoScrollWithMouse      = ImGuiWindowFlags_NoScrollWithMouse,           // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
	NoCollapse             = ImGuiWindowFlags_NoCollapse,                  // Disable user collapsing window by double-clicking on it
	AlwaysAutoResize       = ImGuiWindowFlags_AlwaysAutoResize,            // Resize every window to its content every frame
	NoBackground           = ImGuiWindowFlags_NoBackground,                // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
	NoSavedSettings        = ImGuiWindowFlags_NoSavedSettings,             // Never load/save settings in .ini file
	NoMouseInputs          = ImGuiWindowFlags_NoMouseInputs,               // Disable catching mouse, hovering test with pass through.
	MenuBar                = ImGuiWindowFlags_MenuBar,                     // Has a menu-bar
	HorizontalScrollbar    = ImGuiWindowFlags_HorizontalScrollbar,         // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
	NoFocusOnAppearing     = ImGuiWindowFlags_NoFocusOnAppearing,          // Disable taking focus when transitioning from hidden to visible state
	NoBringToFrontOnFocus  = ImGuiWindowFlags_NoBringToFrontOnFocus,       // Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
	AlwaysVerticalScrollbar= ImGuiWindowFlags_AlwaysVerticalScrollbar,     // Always show vertical scrollbar (even if ContentSize.y < Size.y)
	AlwaysHorizontalScrollbar=ImGuiWindowFlags_AlwaysHorizontalScrollbar,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
	AlwaysUseWindowPadding = ImGuiWindowFlags_AlwaysUseWindowPadding,      // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
	NoNavInputs            = ImGuiWindowFlags_NoNavInputs,                 // No gamepad/keyboard navigation within the window
	NoNavFocus             = ImGuiWindowFlags_NoNavFocus,                  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
	UnsavedDocument        = ImGuiWindowFlags_UnsavedDocument,             // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
	NoDocking              = ImGuiWindowFlags_NoDocking,                   // Disable docking of this window
	
	NoNav        = ImGuiWindowFlags_NoNav,        // = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
	NoDecoration = ImGuiWindowFlags_NoDecoration, // = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
	NoInputs     = ImGuiWindowFlags_NoInputs,     // = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus

	// [Internal]
	zInternal_NavFlattened = ImGuiWindowFlags_NavFlattened,  // [BETA] Allow gamepad/keyboard navigation to cross over parent border to this child (only use on child that have no scrolling!)
	zInternal_ChildWindow  = ImGuiWindowFlags_ChildWindow,   // Don't use! For internal use by BeginChild()
	zInternal_Tooltip      = ImGuiWindowFlags_Tooltip,       // Don't use! For internal use by BeginTooltip()
	zInternal_Popup        = ImGuiWindowFlags_Popup,         // Don't use! For internal use by BeginPopup()
	zInternal_Modal        = ImGuiWindowFlags_Modal,         // Don't use! For internal use by BeginPopupModal()
	zInternal_ChildMenu    = ImGuiWindowFlags_ChildMenu,     // Don't use! For internal use by BeginMenu()
	zInternal_DockNodeHost = ImGuiWindowFlags_DockNodeHost   // Don't use! For internal use by Begin()/NewFrame()

	// [Obsolete]
	// ShowBorders       // --> Set style.FrameBorderSize=1.0f / style.WindowBorderSize=1.0f to enable borders around windows and items
	// ResizeFromAnySide // --> Set io.ConfigResizeWindowsFromEdges and make sure mouse cursors are supported by back-end (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)
};

enum class Condition
{
	Always       = ImGuiCond_Always,       // Set the variable
	Once         = ImGuiCond_Once,         // Set the variable once per runtime session (only the first call with succeed)
	FirstUseEver = ImGuiCond_FirstUseEver, // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
	Appearing    = ImGuiCond_Appearing     // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
};


HANDY_ENABLE_BITMASK_OPERATORS(WindowFlags)

namespace ImExt
{
	//Singleton Class to keep track of which Platform Windows are holding on to which ImGui Windows.
	//This is used to support proper window focusing (i.e. also focus the platform window containing an ImGui window)
	class PlatformWindowRegistry {
		public:
			static PlatformWindowRegistry & Instance() { static PlatformWindowRegistry obj; return obj; }
			std::unordered_map<std::string, ImGuiViewport *> m_map;
	};
	
	class Window
	{
	public:
		class Options
		{
			std::optional<Math::Vector2> m_size          = Math::Vector2(100, 100); 
			Condition                    m_sizeCond      = Condition::FirstUseEver;

			std::optional<Math::Vector2> m_position      = Math::Vector2(100, 100);
			Math::Vector2                m_positionPivot = Math::Vector2(0.0f);
			Condition                    m_positionCond  = Condition::FirstUseEver;
			
			std::optional<bool>          m_collapsed;
			Condition                    m_collapsedCond = Condition::FirstUseEver;

			std::optional<ImGuiID>       m_dockID;
			Condition                    m_dockIDCond    = Condition::FirstUseEver;
			
		public:
			WindowFlags Flags = WindowFlags::None;

			bool * POpen = NULL;

			void Size     (Math::Vector2 size,                                           Condition cond = Condition::FirstUseEver) { m_size      = size;                               m_sizeCond      = cond; }
			void Position (Math::Vector2 pos, Math::Vector2 pivot = Math::Vector2(0.0f), Condition cond = Condition::FirstUseEver) { m_position  = pos;       m_positionPivot = pivot; m_positionCond  = cond; }
			void Collapsed(bool collapsed,                                               Condition cond = Condition::FirstUseEver) { m_collapsed = collapsed;                          m_collapsedCond = cond; }
			void DockID   (ImGuiID id,                                                   Condition cond = Condition::FirstUseEver) { m_dockID    = id;                                 m_dockIDCond    = cond; }

			std::optional<Math::Vector2> ContentSize; // Size and ContentSize are mutually exclusive.

			std::optional<Math::Vector4> SizeConstraintMinXYMaxXY;

			bool                         ForceFocused = false;

			std::optional<float>         BGAlpha;

			std::optional<ImGuiID>       Viewport;

			std::optional<ImGuiWindowClass *> Class;

			/// Style Options
			std::optional<Math::Vector2> MinSize;
			std::optional<Math::Vector2> Padding;
			std::optional<float>         Rounding;
			std::optional<float>         BorderSize;
			std::optional<Math::Vector2> TitleAlign;
			std::optional<Math::Vector4> BGColor;

			friend class Window;
		};
			
	private:
		bool m_shouldDrawContents = false;
		bool m_isBegun = false;

	public:

		explicit Window(std::string id)
			: Window(id, Options()) { }

		Window(std::string id, Options const & options)
		{
			if (options.m_position) ImGui::SetNextWindowPos (options.m_position.value(), (ImGuiCond)options.m_positionCond, options.m_positionPivot);
			if (options.m_size)     ImGui::SetNextWindowSize(options.m_size.value(),     (ImGuiCond)options.m_sizeCond);

			if (options.SizeConstraintMinXYMaxXY)
				ImGui::SetNextWindowSizeConstraints(
					options.SizeConstraintMinXYMaxXY.value().XY(), 
					options.SizeConstraintMinXYMaxXY.value().ZW());

			if (options.ContentSize)  ImGui::SetNextWindowContentSize(options.ContentSize .value());
			if (options.m_collapsed)  ImGui::SetNextWindowCollapsed  (options.m_collapsed .value(), (ImGuiCond)options.m_collapsedCond);
			if (options.BGAlpha)      ImGui::SetNextWindowBgAlpha    (options.BGAlpha     .value());
			if (options.Viewport)     ImGui::SetNextWindowViewport   (options.Viewport    .value());
			if (options.Class)        ImGui::SetNextWindowClass      (options.Class       .value());
			if (options.m_dockID)     ImGui::SetNextWindowDockID     (options.m_dockID    .value(), (ImGuiCond)options.m_dockIDCond);
			if (options.ForceFocused) ImGui::SetNextWindowFocus();
			
			/// Style Options:
			if (options.MinSize)    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize,    options.MinSize.value());
			if (options.Padding)    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    options.Padding.value());
			if (options.Rounding)   ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   options.Rounding.value());
			if (options.BorderSize) ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, options.BorderSize.value());
			if (options.TitleAlign) ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, options.TitleAlign.value());
			if (options.BGColor)    ImGui::PushStyleColor(ImGuiCol_WindowBg,            options.BGColor.value());

			//Note: Fixed a bug here - segfault when POpen is NULL
			if ((options.POpen == nullptr) || *(options.POpen))
			{
				m_isBegun = true;
				m_shouldDrawContents = ImGui::Begin(id.c_str(), options.POpen, (ImGuiWindowFlags)options.Flags);
				PlatformWindowRegistry::Instance().m_map[id] = ImGui::GetWindowViewport(); //Update the registry with our current platform window
			}

			if (options.MinSize)    ImGui::PopStyleVar();
			if (options.Padding)    ImGui::PopStyleVar();
			if (options.Rounding)   ImGui::PopStyleVar();
			if (options.BorderSize) ImGui::PopStyleVar();
			if (options.TitleAlign) ImGui::PopStyleVar();
			if (options.BGColor)    ImGui::PopStyleColor();
		}

		bool ShouldDrawContents() const { return m_shouldDrawContents; }

		auto ContentRegionMax()   { return ImGui::GetWindowContentRegionMax(); }
		auto ContentRegionMin()   { return ImGui::GetWindowContentRegionMin(); }
		auto ContentRegionWidth() { return ImGui::GetWindowContentRegionWidth(); }
		auto DockID()             { return ImGui::GetWindowDockID(); }
		auto DpiScale()           { return ImGui::GetWindowDpiScale(); }
		auto DrawList()           { return ImGui::GetWindowDrawList(); }
		auto Height()             { return ImGui::GetWindowHeight(); }
		auto Position()           { return ImGui::GetWindowPos(); }
		auto Size()               { return ImGui::GetWindowSize(); }
		auto Viewport()           { return ImGui::GetWindowViewport(); }
		auto Width()              { return ImGui::GetWindowWidth(); }

		auto IsAppearing() { return ImGui::IsWindowAppearing(); }
		auto IsCollapsed() { return ImGui::IsWindowCollapsed(); }
		auto IsDocked()    { return ImGui::IsWindowDocked(); }
		auto IsFocused(ImGuiFocusedFlags flags) { return ImGui::IsWindowFocused(flags); }
		auto IsHovered(ImGuiHoveredFlags flags) { return ImGui::IsWindowHovered(flags); }

		~Window()
		{
			if (m_isBegun)
				ImGui::End();
		}
		
		//Set focus on a given window (by name) *and* set focus on the viewport containing that window.
		//Note that the viewport will only focus if the window was created using ImExt::Window, as opposed to classic Begin() and End() calls.
		static void FocusWindow(std::string const & WinName) {
			auto & registry = PlatformWindowRegistry::Instance().m_map;
			
			//Clean up registry by removing invalid viewport pointers - technically we should do this
			//periodically, but the registry is generally very small so cleanup when we focus is just fine.
			std::unordered_set<ImGuiViewport *> validViewports;
			for (auto const item : ImGui::GetPlatformIO().Viewports)
				validViewports.insert(item);
			std::unordered_map<std::string, ImGuiViewport *> validItems;
			for (auto & kv : registry) {
				if (validViewports.count(kv.second) > 0U)
					validItems[kv.first] = kv.second;
			}
			registry.swap(validItems);
			
			//Now, if the given WinName is in the registry we know it points to a valid viewport
			if (registry.count(WinName) > 0)
				ImGui::GetPlatformIO().Platform_SetWindowFocus(registry.at(WinName));
			
			ImGui::SetWindowFocus(WinName.c_str()); //Set focus within platform window
			//std::cerr << "Registry size: " << registry.size() << "\r\n\r\n";
		}
	};

	class ID
	{
		bool m_ended = false;

	public:
		ID(int32_t in)      { ImGui::PushID(in); }
		ID(std::string s)   { ImGui::PushID(s.c_str()); }
		ID(char const * cs) { ImGui::PushID(cs); }

		ID(char const * csBegin, char const * csEnd) { ImGui::PushID(csBegin, csEnd); }

		void EndHere() 
		{ 
			if (!m_ended)
				ImGui::PopID(); 
			m_ended = true;
		}

		~ID() { EndHere(); }

		ID            (ID const &)  = delete;
		ID            (ID const &&) = delete;
		ID            (ID &&)       = delete;
		ID & operator=(ID const &)  = delete;
	};

	class Font
	{
		bool m_ended = false;

	public:
		Font(ImFont * imf) { ImGui::PushFont(imf); }

		void EndHere()     
		{
			if (!m_ended)
				ImGui::PopFont(); 
			m_ended = true;
		}

		~Font() { EndHere(); }

		Font            (Font const &)  = delete;
		Font            (Font const &&) = delete;
		Font            (Font &&)       = delete;
		Font & operator=(Font const &)  = delete;
	};

	class Group
	{
		bool m_ended = false;

	public:
		Group() { ImGui::BeginGroup(); }

		void EndHere()
		{
			if (!m_ended)
				ImGui::EndGroup(); 
			m_ended = true;
		}

		~Group() { EndHere(); }

		Group            (Group const &)  = delete;
		Group            (Group const &&) = delete;
		Group            (Group &&)       = delete;
		Group & operator=(Group const &)  = delete;
	};


	class Style
	{
		int32_t varPushCount = 0;
		int32_t colPushCount = 0;
		int32_t iwdPushCount = 0;

	public:

		Style & operator()(StyleItem var,  float f);
		Style & operator()(StyleVar  var,  float f);
		Style & operator()(StyleVar  var,  Math::Vector2 v2);
		Style & operator()(StyleCol  col,  Math::Vector4 v4);

		Style() = default;

		Style(StyleItem var,  float f)          { (*this)(var, f); }
		Style(StyleVar  var,  float f)          { (*this)(var, f); }
		Style(StyleVar  var,  Math::Vector2 v2) { (*this)(var, v2); }
		Style(StyleCol  col,  Math::Vector4 v4) { (*this)(col, v4); }

		void Reset();
		~Style();

		Style            (Style const &)  = delete;
		Style            (Style const &&) = delete;
		Style            (Style &&)       = delete;
		Style & operator=(Style const &)  = delete;
	};

	inline Style & Style::operator()(StyleItem var, float f)
	{

		switch (var)
		{
			case StyleItem::Width:
				ImGui::PushItemWidth(f);
				iwdPushCount++;
				break;
			default:
				std::cerr << "Bad Style/Type Push.";
		}

		return *this;
	}

	inline Style & Style::operator()(StyleVar var, float f)
	{
		varPushCount++;
		ImGui::PushStyleVar((ImGuiStyleVar)var, f);
		return *this;
	}

	inline Style & Style::operator()(StyleVar var, Math::Vector2 v2)
	{
		varPushCount++;
		ImGui::PushStyleVar((ImGuiStyleVar)var, v2);
		return *this;
	}

	inline Style & Style::operator()(StyleCol col, Math::Vector4 v4)
	{
		colPushCount++;
		ImGui::PushStyleColor((ImGuiCol)col, v4);
		return *this;
	}

	inline void Style::Reset()
	{
		ImGui::PopStyleColor(colPushCount);
		ImGui::PopStyleVar(varPushCount);
		colPushCount = 0;
		varPushCount = 0;

		for (auto i : Handy::range(0, iwdPushCount))
			ImGui::PopItemWidth();

		iwdPushCount = 0;
	}

	inline Style::~Style()
	{
		Reset();
	}

	inline void Dummy(float sizeXY = 5.0f)
	{
		ImGui::Dummy(ImVec2(sizeXY, sizeXY));
	}

	inline void Dummy(Math::Vector2 size)
	{
		ImGui::Dummy(size);
	}

	inline void Dummy(float sizeX, float sizeY)
	{
		ImGui::Dummy(ImVec2(sizeX, sizeY));
	}

	inline void DummyB(bool enabled, float sizeXY)
	{
		if (enabled)
			ImGui::Dummy(ImVec2(sizeXY, sizeXY));
	}

	inline void DummyB(bool enabled, Math::Vector2 size)
	{
		if (enabled)
			ImGui::Dummy(size);
	}

	inline void DummyB(bool enabled, float sizeX = 5_f, float sizeY = 5_f)
	{
		if (enabled)
			ImGui::Dummy(ImVec2(sizeX, sizeY));
	}

	inline void DummyBSL(bool enabled, float sizeXY)
	{
		if (enabled)
		{
			ImGui::Dummy(ImVec2(sizeXY, sizeXY));
			ImGui::SameLine();
		}
	}

	inline void DummyBSL(bool enabled, Math::Vector2 size)
	{
		if (enabled)
		{
			ImGui::Dummy(size);
			ImGui::SameLine();
		}
	}

	inline void DummyBSL(bool enabled, float sizeX = 5_f, float sizeY = 5_f)
	{
		if (enabled)
		{
			ImGui::Dummy(ImVec2(sizeX, sizeY));
			ImGui::SameLine();
		}
	}

	inline void SeparatorSpacer()
	{
		Dummy(5_f, 5_f);
		ImGui::Separator();
		Dummy(5_f, 5_f);
	}

	inline void Animate(float periodSeconds, float & valueCurrentNext)
	{
		float deltaTime = ImGui::GetIO().DeltaTime;

		valueCurrentNext += deltaTime / periodSeconds;
		valueCurrentNext = Handy::FastMax(0_f, valueCurrentNext);
		valueCurrentNext = Handy::FastMin(1_f, valueCurrentNext);
	}

	inline void AnimateReverse(float periodSeconds, float & valueCurrentNext)
	{
		float deltaTime = ImGui::GetIO().DeltaTime;

		valueCurrentNext -= deltaTime / periodSeconds;
		valueCurrentNext = Handy::FastMax(0_f, valueCurrentNext);
		valueCurrentNext = Handy::FastMin(1_f, valueCurrentNext);
	}

	inline bool WrapNext(float item_w)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		float last_item_x2 = ImGui::GetItemRectMax().x;
		float next_item_x2 = last_item_x2 + style.ItemSpacing.x + item_w; // Expected position if next button was on same line

		if (next_item_x2 < window_visible_x2)
		{
			ImGui::SameLine();
			return false;
		}

		return true;
	}


	inline void PathRightAngleSegment(ImDrawList * draw_list, const ImVec2& mn, const ImVec2& mx, float rounding, int rounding_corners, bool ccw = false)
	{
		ImVec2 a = mn;
		ImVec2 b = mx;

		float cond1 = (((rounding_corners & ImDrawCornerFlags_Top)  == ImDrawCornerFlags_Top)  || ((rounding_corners & ImDrawCornerFlags_Bot)   == ImDrawCornerFlags_Bot)   ? 0.5f : 1.0f);
		float cond2 = (((rounding_corners & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((rounding_corners & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f);

		rounding = Handy::FastMin(rounding, std::abs(b.x - a.x) * cond1 - 1.0f);
		rounding = Handy::FastMin(rounding, std::abs(b.y - a.y) * cond2 - 1.0f);

		using namespace std::placeholders;

		auto ptaf = ccw
			? std::bind(&ImDrawList::PathArcToFastCCW, draw_list, _1, _2, _3, _4)
			: std::bind(&ImDrawList::PathArcToFast,    draw_list, _1, _2, _3, _4);

		switch (rounding_corners)
		{
			case ImDrawCornerFlags_TopRight: ptaf(ImVec2(b.x - rounding, a.y + rounding), rounding, 9, 12); break;
			case ImDrawCornerFlags_BotRight: ptaf(ImVec2(b.x - rounding, b.y - rounding), rounding, 0,  3); break;
			case ImDrawCornerFlags_BotLeft:  ptaf(ImVec2(a.x + rounding, b.y - rounding), rounding, 3,  6); break;
			case ImDrawCornerFlags_TopLeft:  ptaf(ImVec2(a.x + rounding, a.y + rounding), rounding, 6,  9); break;
		}
	}

	inline void DrawRoundedOrthoLinearPolyline(ImDrawList * draw_list, std::vector<Math::Vector2> points, float rounding, ImColor color, float thickness)
	{
		if (points.size() >= 3 && rounding > 1.0f)
		{
			draw_list->PathLineTo(points[0]);
			for (size_t idxPoint = 2; idxPoint < points.size(); idxPoint++)
			{
				Math::Vector2 a = points[idxPoint - 2];
				Math::Vector2 b = points[idxPoint - 1];
				Math::Vector2 c = points[idxPoint];

				Math::AABB2 aabb;
				aabb.AddPoint(a);
				aabb.AddPoint(b);
				aabb.AddPoint(c);

				Math::Vector2 mn = aabb.Min;
				Math::Vector2 mx = aabb.Max;

				if (aabb.Area() < 1.0f)
				{
					draw_list->PathLineTo(b);
				}
				else
				{
					int cornerflags = 0;
					bool ccw = false;

					if      (b.Compare<Math::CMP::AllWE>(mn))                        { cornerflags |= ImDrawCornerFlags_TopLeft;  ccw = a.X > c.X; }
					else if (b.Compare<Math::CMP::AllWE>(mx))                        { cornerflags |= ImDrawCornerFlags_BotRight; ccw = a.X < c.X; }
					else if (b.Compare<Math::CMP::AllWE>(Math::Vector2(mx.X, mn.Y))) { cornerflags |= ImDrawCornerFlags_TopRight; ccw = a.X > c.X; }
					else if (b.Compare<Math::CMP::AllWE>(Math::Vector2(mn.X, mx.Y))) { cornerflags |= ImDrawCornerFlags_BotLeft;  ccw = a.X < c.X; }
					else
						std::cerr << "Failed to detect right angle case." << std::endl;

					if (cornerflags == 0)
						draw_list->PathLineTo(b);
					else
						PathRightAngleSegment(draw_list, mn, mx, rounding, cornerflags, ccw);
				}
			}
			draw_list->PathLineTo(points.back());

			draw_list->PathStroke(color, false, thickness);
		}
		else
		{
			for (size_t idxPoint = 1; idxPoint < points.size(); idxPoint++)
				draw_list->AddLine(points[idxPoint - 1], points[idxPoint], color, thickness);
		}
	}
}


