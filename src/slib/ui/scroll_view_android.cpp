#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_ANDROID)

#include "../../../inc/slib/ui/scroll_view.h"

#include "view_android.h"

SLIB_UI_NAMESPACE_BEGIN

void JNICALL _AndroidScrollView_nativeOnScroll(JNIEnv* env, jobject _this, jlong instance, int x, int y);

SLIB_JNI_BEGIN_CLASS(_JAndroidScrollView, "slib/platform/android/ui/view/UiScrollView")

	SLIB_JNI_STATIC_METHOD(create, "_create", "(Landroid/content/Context;ZZ)Landroid/view/View;");

	SLIB_JNI_STATIC_METHOD(setBackgroundColor, "_setBackgroundColor", "(Landroid/view/View;I)V");
	SLIB_JNI_STATIC_METHOD(scrollTo, "_scrollTo", "(Landroid/view/View;II)V");
	SLIB_JNI_STATIC_METHOD(getScrollX, "_getScrollX", "(Landroid/view/View;)I");
	SLIB_JNI_STATIC_METHOD(getScrollY, "_getScrollY", "(Landroid/view/View;)I");

	SLIB_JNI_NATIVE(nativeOnScroll, "nativeOnScroll", "(JII)V", _AndroidScrollView_nativeOnScroll);

SLIB_JNI_END_CLASS

class _ScrollView : public ScrollView
{
public:
	void __applyContent(jobject sv, ViewInstance* scrollViewInstance)
	{
		Ref<View> viewContent = m_viewContent;
		if (viewContent.isNotNull()) {
			viewContent->attachToNewInstance(scrollViewInstance);
			viewContent->setParent(this);
		}
	}
	
	void __applyProperties(jobject handle, ViewInstance* scrollViewInstance)
	{
		_JAndroidScrollView::setBackgroundColor.call(sl_null, handle, getBackgroundColor().getARGB());
		__applyContent(handle, scrollViewInstance);
	}

	void __onScroll(int x, int y)
	{
		_onScroll_NW((sl_scroll_pos)x, (sl_scroll_pos)y);
	}
};

void JNICALL _AndroidScrollView_nativeOnScroll(JNIEnv* env, jobject _this, jlong instance, int x, int y)
{
	Ref<View> _view = Android_ViewInstance::getAndroidView(instance);
	if (ScrollView::checkInstance(_view.ptr)) {
		_ScrollView* view = (_ScrollView*)(_view.ptr);
		view->__onScroll(x, y);
	}
}

Ref<ViewInstance> ScrollView::createNativeWidget(ViewInstance* _parent)
{
	Ref<Android_ViewInstance> ret;
	Android_ViewInstance* parent = (Android_ViewInstance*)_parent;
	if (parent) {
		JniLocal<jobject> handle = _JAndroidScrollView::create.callObject(sl_null, parent->getContext(), m_flagVerticalScroll && m_flagHorizontalScroll, m_flagVerticalScroll);
		ret = Android_ViewInstance::create<Android_ViewInstance>(this, parent, handle.get());
		if (ret.isNotNull()) {
			jobject handle = ret->getHandle();
			((_ScrollView*)this)->__applyProperties(handle, ret.ptr);
		}
	}
	return ret;
}

void ScrollView::_refreshContentSize_NW()
{
}

void ScrollView::_setContentView_NW(const Ref<View>& view)
{
	Ref<ViewInstance> instance = getViewInstance();
	if (instance.isNotNull()) {
		jobject handle = UIPlatform::getViewHandle(instance.ptr);
		if (handle) {
			((_ScrollView*)this)->__applyContent(handle, instance.ptr);
		}
	}
}

void ScrollView::_scrollTo_NW(sl_scroll_pos x, sl_scroll_pos y)
{
	jobject handle = UIPlatform::getViewHandle(this);
	if (handle) {
		_JAndroidScrollView::scrollTo.call(sl_null, handle, (int)x, (int)y);
	}
}

ScrollPoint ScrollView::_getScrollPosition_NW()
{
	jobject handle = UIPlatform::getViewHandle(this);
	if (handle) {
		ScrollPoint ret;
		ret.x = (sl_scroll_pos)(_JAndroidScrollView::getScrollX.callInt(sl_null, handle));
		ret.y = (sl_scroll_pos)(_JAndroidScrollView::getScrollY.callInt(sl_null, handle));
		return ret;
	}
	return ScrollPoint::zero();
}

ScrollPoint ScrollView::_getScrollRange_NW()
{
	Ref<View> content = m_viewContent;
	if (content.isNotNull()) {
		ScrollPoint ret;
		ret.x = (sl_scroll_pos)(content->getWidth() - getWidth());
		if (ret.x < 0) {
			ret.x = 0;
		}
		ret.y = (sl_scroll_pos)(content->getHeight() - getHeight());
		if (ret.y < 0) {
			ret.y = 0;
		}
		return ret;
	}
	return ScrollPoint::zero();
}

void ScrollView::_setBorder_NW(sl_bool flag)
{
}

void ScrollView::_setBackgroundColor_NW(const Color& color)
{
	jobject handle = UIPlatform::getViewHandle(this);
	if (handle) {
		_JAndroidScrollView::setBackgroundColor.call(sl_null, handle, color.getARGB());
	}
}

SLIB_UI_NAMESPACE_END

#endif