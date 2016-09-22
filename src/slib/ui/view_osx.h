#ifndef CHECKHEADER_SLIB_UI_VIEW_OSX
#define CHECKHEADER_SLIB_UI_VIEW_OSX

#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_OSX)

#include "../../../inc/slib/ui/view.h"
#include "../../../inc/slib/ui/platform.h"

SLIB_UI_NAMESPACE_BEGIN

class OSX_ViewInstance : public ViewInstance
{
public:
	OSX_ViewInstance();
	~OSX_ViewInstance();
	
public:
	static Ref<OSX_ViewInstance> create(NSView* handle, sl_bool flagFreeOnRelease);
	
	static Ref<OSX_ViewInstance> create(NSView* handle, NSView* parent, View* view);
	
public:
	void release();
	
	static void freeHandle(NSView* handle);
	
	NSView* getHandle();
	
public:
	// override
	sl_bool isValid();
	
	// override
	void setFocus();
	
	// override
	void invalidate();
	
	// override
	void invalidate(const UIRect& rect);
	
	// override
	UIRect getFrame();
	
	// override
	void setFrame(const UIRect& frame);
	
	// override
	void setVisible(sl_bool flag);
	
	// override
	void setEnabled(sl_bool flag);
	
	// override
	void setOpaque(sl_bool flag);
	
	// override
	UIPointf convertCoordinateFromScreenToView(const UIPointf& ptScreen);
	
	// override
	UIPointf convertCoordinateFromViewToScreen(const UIPointf& ptView);
	
	// override
	void addChildInstance(const Ref<ViewInstance>& instance);
	
	// override
	void removeChildInstance(const Ref<ViewInstance>& instance);
	
public:
	void onDraw(NSRect rectDirty);
	
	sl_bool onEventKey(sl_bool flagDown, NSEvent* event);
	
	sl_bool onEventMouse(UIAction action, NSEvent* event);
	
	sl_bool onEventMouseWheel(NSEvent* event);
	
	sl_bool onEventUpdateCursor(NSEvent* event);
	
	void applyModifiers(UIEvent* ev, NSEvent* event);
	
protected:
	NSView* m_handle;
	sl_bool m_flagFreeOnRelease;
	
};

SLIB_UI_NAMESPACE_END

@interface Slib_OSX_ViewBase : NSView {
	@public sl_bool m_flagOpaque;
}
@end

@interface Slib_OSX_ViewHandle : Slib_OSX_ViewBase {

	@public slib::WeakRef<slib::OSX_ViewInstance> m_viewInstance;

	NSTrackingArea* m_trackingArea;
}
@end


#define OSX_VIEW_CREATE_INSTANCE_BEGIN \
	Ref<OSX_ViewInstance> ret; \
	NSView* parent = UIPlatform::getViewHandle(_parent); \
	NSRect frame; \
	UIRect _frame = getFrame(); \
	frame.origin.x = (CGFloat)(_frame.left); \
	frame.origin.y = (CGFloat)(_frame.top); \
	frame.size.width = (CGFloat)(_frame.getWidth()); \
	frame.size.height = (CGFloat)(_frame.getHeight());

#define OSX_VIEW_CREATE_INSTANCE_END \
	if (handle != nil) { \
		ret = OSX_ViewInstance::create(handle, parent, this); \
		if (ret.isNotNull()) { \
			handle->m_viewInstance = ret; \
		} \
	}


#endif

#endif