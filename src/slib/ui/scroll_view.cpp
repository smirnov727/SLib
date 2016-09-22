#include "../../../inc/slib/ui/scroll_view.h"
#include "../../../inc/slib/ui/core.h"

SLIB_UI_NAMESPACE_BEGIN

SLIB_DEFINE_OBJECT(ScrollView, View)

ScrollView::ScrollView()
{
	SLIB_REFERABLE_CONSTRUCTOR

	setCreatingNativeWidget(sl_true);
	setCreatingChildInstances(sl_false);
	setBorder(sl_true, sl_false);
	
	m_flagInitedScrollbars = sl_false;
	m_flagHorizontalScroll = sl_true;
	m_flagVerticalScroll = sl_true;
}

sl_bool ScrollView::isHorizontalScrolling()
{
	return m_flagHorizontalScroll;
}

void ScrollView::setHorizontalScrolling(sl_bool flagHorizontal)
{
	m_flagHorizontalScroll = flagHorizontal;
}

sl_bool ScrollView::isVerticalScrolling()
{
	return m_flagVerticalScroll;
}

void ScrollView::setVerticalScrolling(sl_bool flagVertical)
{
	m_flagVerticalScroll = flagVertical;
}


Ref<View> ScrollView::getContentView()
{
	return m_viewContent;
}

void ScrollView::setContentView(const Ref<slib::View>& view, sl_bool flagRedraw)
{
	ObjectLocker lock(this);
	_initScrollbars();
	if (m_viewContent != view) {
		Ref<View> viewOld = m_viewContent;
		removeChild(viewOld);
		m_viewContent = view;
		if (view.isNotNull()) {
			view->setParent(this);
			view->setCreatingInstance(sl_true);
			view->setAttachMode(UIAttachMode::NotAttach);
			addChild(view, sl_false);
			View::setContentSize(view->getWidth(), view->getHeight());
		} else {
			View::setContentSize(0, 0);
		}
		if (isNativeWidget()) {
			if (UI::isUiThread()) {
				_setContentView_NW_OnUiThread(view);
			} else {
				UI::dispatchToUiThread(SLIB_CALLBACK_WEAKREF(ScrollView, _setContentView_NW_OnUiThread, this, view));
			}
		} else {
			UIPoint pt = View::getScrollPosition();
			_scrollTo(pt.x, pt.y, sl_false);
			if (flagRedraw) {
				invalidate();
			}
		}
	}
}

void ScrollView::setContentSize(sl_scroll_pos _width, sl_scroll_pos _height, sl_bool flagRefresh)
{
	sl_ui_pos width = (sl_ui_pos)(_width);
	if (width < 0) {
		width = 0;
	}
	sl_ui_pos height = (sl_ui_pos)(_height);
	if (height < 0) {
		height = 0;
	}
	ObjectLocker lock(this);
	_initScrollbars();
	Ref<View> viewContent = m_viewContent;
	if (viewContent.isNotNull()) {
		viewContent->setSize(width, height);
	}
	View::setContentSize(_width, _height, flagRefresh);
	if (isNativeWidget()) {
		_refreshContentSize_NW();
	}
}

void ScrollView::setContentSize(const ScrollPoint& size, sl_bool flagRefresh)
{
	setContentSize(size.x, size.y, flagRefresh);
}

ScrollPoint ScrollView::getScrollPosition()
{
	if (isNativeWidget()) {
		return _getScrollPosition_NW();
	}
	return View::getScrollPosition();
}

ScrollPoint ScrollView::getScrollRange()
{
	if (isNativeWidget()) {
		return _getScrollRange_NW();
	}
	return View::getScrollRange();
}

void ScrollView::scrollTo(sl_scroll_pos x, sl_scroll_pos y, sl_bool flagRedraw)
{
	_scrollTo(x, y, sl_false);
	View::scrollTo(x, y, flagRedraw);
}

void ScrollView::scrollTo(const ScrollPoint& position, sl_bool flagRedraw)
{
	scrollTo(position.x, position.y, flagRedraw);
}

void ScrollView::onResize(sl_ui_len width, sl_ui_len height)
{
	if (isNativeWidget()) {
		_refreshContentSize_NW();
	}
}

void ScrollView::onResizeChild(View* child, sl_ui_len width, sl_ui_len height)
{
	if (child == m_viewContent) {
		View::setContentSize(width, height);
		if (isNativeWidget()) {
			_refreshContentSize_NW();
		}
	}
}

void ScrollView::onMeasureLayout(sl_bool flagHorizontal, sl_bool flagVertical)
{
}

void ScrollView::onMakeLayout()
{	
}

void ScrollView::_initScrollbars()
{
	if (m_flagInitedScrollbars) {
		return;
	}
	m_flagInitedScrollbars = sl_true;
	if (m_flagVerticalScroll) {
		if (m_flagHorizontalScroll) {
			createScrollBars(sl_false);
		} else {
			createVerticalScrollBar(sl_false);
		}
	} else {
		if (m_flagHorizontalScroll) {
			createHorizontalScrollBar(sl_false);
		}
	}
}

void ScrollView::_scrollTo(sl_scroll_pos x, sl_scroll_pos y, sl_bool flagRedraw)
{
	Ref<View> view = m_viewContent;
	if (view.isNotNull()) {
		if (isNativeWidget()) {
			_scrollTo_NW(x, y);
		} else {
			sl_ui_len w = getWidth();
			sl_ui_len cw = view->getWidth();
			if (cw > w) {
				sl_ui_pos rx = cw - w;
				if (x > (sl_scroll_pos)rx) {
					x = rx;
				}
				if (x < 0) {
					x = 0;
				}
			} else {
				x = 0;
			}
			sl_ui_len h = getHeight();
			sl_ui_len ch = view->getHeight();
			if (ch > h) {
				sl_ui_pos ry = ch - h;
				if (y > (sl_scroll_pos)ry) {
					y = ry;
				}
				if (y < 0) {
					y = 0;
				}
			} else {
				y = 0;
			}
			view->setPosition((sl_ui_pos)-x, (sl_ui_pos)-y, flagRedraw);
		}
	}
}

void ScrollView::_setContentView_NW_OnUiThread(const Ref<View> view)
{
	_setContentView_NW(view);
	if (view.isNotNull()) {
		UIPoint pt = View::getScrollPosition();
		_scrollTo_NW(pt.x, pt.y);
	}
}

void ScrollView::_onScroll_NW(sl_scroll_pos x, sl_scroll_pos y)
{
	View::scrollTo(x, y, sl_false);
}


#if !(defined(SLIB_PLATFORM_IS_OSX)) && !(defined(SLIB_PLATFORM_IS_IOS)) && !(defined(SLIB_PLATFORM_IS_WIN32)) && !(defined(SLIB_PLATFORM_IS_ANDROID))

Ref<ViewInstance> ScrollView::createNativeWidget(ViewInstance* parent)
{
	return Ref<ViewInstance>::null();
}

void ScrollView::_refreshContentSize_NW()
{
}

void ScrollView::_setContentView_NW(const Ref<View>& view)
{
}

void ScrollView::_scrollTo_NW(sl_scroll_pos x, sl_scroll_pos y)
{
}

ScrollPoint ScrollView::_getScrollPosition_NW()
{
	return ScrollPoint::zero();
}

ScrollPoint ScrollView::_getScrollRange_NW()
{
	return ScrollPoint::zero();
}

void ScrollView::_setBorder_NW(sl_bool flag)
{
}

void ScrollView::_setBackgroundColor_NW(const Color& color)
{
}

#endif


HorizontalScrollView::HorizontalScrollView()
{
	setHorizontalScrolling(sl_true);
	setVerticalScrolling(sl_false);
}

VerticalScrollView::VerticalScrollView()
{
	setHorizontalScrolling(sl_false);
	setVerticalScrolling(sl_true);
}

SLIB_UI_NAMESPACE_END