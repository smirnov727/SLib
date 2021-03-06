/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_IOS)

#include "../../../inc/slib/ui/window.h"
#include "../../../inc/slib/ui/view.h"
#include "../../../inc/slib/ui/screen.h"
#include "../../../inc/slib/ui/core.h"
#include "../../../inc/slib/ui/platform.h"

#include "view_ios.h"

namespace slib
{
	class _iOS_Window;
}

@interface _slib_iOS_Window : UIWindow {
@public slib::WeakRef<slib::_iOS_Window> m_window;
}
@end

@interface _slib_iOS_Window_RootViewController : UIViewController
{
}
@end

namespace slib
{
	class _iOS_Window : public WindowInstance
	{
	public:
		UIView* m_window;
		AtomicRef<ViewInstance> m_viewContent;
		
	public:
		_iOS_Window()
		{
		}
		
		~_iOS_Window()
		{
			release();
		}
		
	public:
		static Ref<_iOS_Window> create(UIView* window)
		{
			if (window != nil) {
				Ref<_iOS_Window> ret = new _iOS_Window();
				if (ret.isNotNull()) {
					ret->m_window = window;
					UIView* view;
					if ([window isKindOfClass:[UIWindow class]]) {
						view = ((UIWindow*)window).rootViewController.view;
					} else {
						view = window;
					}
					if (view != nil) {
						Ref<ViewInstance> content = UIPlatform::createViewInstance(view);
						if (content.isNotNull()) {
							content->setWindowContent(sl_true);
							ret->m_viewContent = content;
							if ([view isKindOfClass:[Slib_iOS_ViewHandle class]]) {
								((Slib_iOS_ViewHandle*)view)->m_viewInstance = Ref<iOS_ViewInstance>::from(content);
							}
						}
					}
					return ret;
				}
			}
			return sl_null;
		}
		
		static Ref<WindowInstance> create(const WindowInstanceParam& param)
		{
			UIRect screenFrame;
			Ref<Screen> _screen = param.screen;
			if (_screen.isNotNull()) {
				screenFrame = _screen->getRegion();
			} else {
				_screen = UI::getPrimaryScreen();
				if (_screen.isNotNull()) {
					screenFrame = _screen->getRegion();
				} else {
					screenFrame = UIRect::zero();
				}
			}
			UIRect _rect = param.calculateRegion(screenFrame);
			CGRect rect;
			CGFloat f = UIPlatform::getGlobalScaleFactor();
			rect.origin.x = (CGFloat)(_rect.left) / f;
			rect.origin.y = (CGFloat)(_rect.top) / f;
			rect.size.width = (CGFloat)(_rect.getWidth()) / f;
			rect.size.height = (CGFloat)(_rect.getHeight()) / f;
			_slib_iOS_Window* window = [[_slib_iOS_Window alloc] initWithFrame:rect];
			if (window != nil) {
				UIScreen* screen = UIPlatform::getScreenHandle(_screen.get());
				if (screen != nil) {
					window.screen = screen;
				}
				window.windowLevel = UIWindowLevelNormal + 1;
				_slib_iOS_Window_RootViewController* controller = [[_slib_iOS_Window_RootViewController alloc] init];
				if (controller != nil) {
					Slib_iOS_ViewHandle* view = [[Slib_iOS_ViewHandle alloc] init];
					if (view != nil) {
						view.opaque = NO;
						controller.view = view;
						window.rootViewController = controller;
						Ref<_iOS_Window> ret = Ref<_iOS_Window>::from(UIPlatform::createWindowInstance(window));
						if (ret.isNotNull()) {
							window->m_window = ret;
							ret->setFocus();
							return ret;
						}
					}
				}
			}
			return sl_null;
		}
		
		void release()
		{
			UIView* window = m_window;
			if (window != nil) {
				UIPlatform::removeWindowInstance(window);
			}
			if (!([window isKindOfClass:[UIWindow class]])) {
				[window removeFromSuperview];
			}
			m_viewContent.setNull();
			m_window = nil;
		}
		
		Ref<ViewInstance> getContentView()
		{
			return m_viewContent;
		}
		
		void close()
		{
			UIView* view = m_window;
			if (view != nil) {
				if ([view isKindOfClass:[UIWindow class]]) {
					UIWindow* window = (UIWindow*)view;
					window.hidden = TRUE;
				} else {
					[view removeFromSuperview];
				}
			}
			m_window = nil;
			m_viewContent.setNull();
		}
		
		sl_bool isClosed()
		{
			return m_window == nil;
		}
		
		Ref<WindowInstance> getParent()
		{
			return sl_null;
		}
		
		sl_bool setParent(const Ref<WindowInstance>& window)
		{
			return sl_false;
		}
		
		sl_bool setFocus()
		{
			UIView* view = m_window;
			if (view != nil) {
				dispatch_async(dispatch_get_main_queue(), ^{
					if ([view isKindOfClass:[UIWindow class]]) {
						UIWindow* window = (UIWindow*)view;
						[window makeKeyAndVisible];
					} else {
						[view becomeFirstResponder];
					}
				});
				return sl_true;
			}
			return sl_false;
		}
		
		UIRect getFrame()
		{
			UIView* window = m_window;
			if (window != nil) {
				CGRect rect = [window frame];
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				UIRect frame;
				frame.left = (sl_ui_pos)(rect.origin.x * f);
				frame.top = (sl_ui_pos)(rect.origin.y * f);
				frame.right = (sl_ui_pos)((rect.origin.x + rect.size.width) * f);
				frame.bottom = (sl_ui_pos)((rect.origin.y + rect.size.height) * f);
				return frame;
			} else {
				return UIRect::zero();
			}
		}
		
		sl_bool setFrame(const UIRect& _frame)
		{
			UIRect frame = _frame;
			UIView* window = m_window;
			if (window != nil) {
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				dispatch_async(dispatch_get_main_queue(), ^{
					CGRect rect;
					rect.origin.x = (CGFloat)(frame.left) / f;
					rect.origin.y = (CGFloat)(frame.top) / f;
					rect.size.width = (CGFloat)(frame.getWidth()) / f;
					rect.size.height = (CGFloat)(frame.getHeight()) / f;
					[window setFrame:rect];
				});
				return sl_true;
			}
			return sl_false;
		}
		
		UIRect getClientFrame()
		{
			return getFrame();
		}
		
		UISize getClientSize()
		{
			UIView* window = m_window;
			if (window != nil) {
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				CGRect rect = [window frame];
				UISize ret;
				ret.x = (sl_ui_pos)(rect.size.width * f);
				ret.y = (sl_ui_pos)(rect.size.height * f);
				return ret;
			} else {
				return UISize::zero();
			}
		}
		
		sl_bool setClientSize(const UISize& _size)
		{
			UISize size = _size;
			UIView* window = m_window;
			if (window != nil) {
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				dispatch_async(dispatch_get_main_queue(), ^{
					CGRect frame = [window frame];
					frame.size.width = (CGFloat)(size.x) / f;
					frame.size.height = (CGFloat)(size.y) / f;
					[window setFrame:frame];
				});
				return sl_true;
			}
			return sl_false;
		}
		
		String getTitle()
		{
			return sl_null;
		}
		
		sl_bool setTitle(const String& title)
		{
			return sl_false;
		}
		
		Color getBackgroundColor()
		{
			UIView* window = m_window;
			if (window != nil) {
				UIColor* color = [window backgroundColor];
				if (color == nil) {
					return Color::zero();
				}
				return GraphicsPlatform::getColorFromUIColor(color);
			}
			return Color::Transparent;
		}
		
		sl_bool setBackgroundColor(const Color& _color)
		{
			UIView* window = m_window;
			if (window != nil) {
				dispatch_async(dispatch_get_main_queue(), ^{
					UIColor* color;
					if (_color.isZero()) {
						color = nil;
					} else {
						color = GraphicsPlatform::getUIColorFromColor(_color);
					}
					[window setBackgroundColor:color];
				});
				return sl_true;
			}
			return sl_false;
		}
		
		sl_bool isMinimized()
		{
			return sl_false;
		}
		
		sl_bool setMinimized(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_bool isMaximized()
		{
			return sl_false;
		}
		
		sl_bool setMaximized(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_bool isVisible()
		{
			UIView* window = m_window;
			if (window != nil) {
				BOOL flag = [window isHidden];
				if (flag) {
					return sl_false;
				} else {
					return sl_true;
				}
			} else {
				return sl_false;
			}
		}
		
		sl_bool setVisible(sl_bool flag)
		{
			UIView* window = m_window;
			if (window != nil) {
				dispatch_async(dispatch_get_main_queue(), ^{
					[window setHidden:(flag?NO:YES)];
				});
				return sl_true;
			} else {
				return sl_false;
			}
		}
		
		sl_bool isAlwaysOnTop()
		{
			UIView* view = m_window;
			if (view != nil) {
				if ([view isKindOfClass:[UIWindow class]]) {
					UIWindow* window = (UIWindow*)view;
					if (window.windowLevel >= UIWindowLevelAlert) {
						return sl_true;
					} else {
						return sl_false;
					}
				}
			}
			return sl_false;
		}
		
		sl_bool setAlwaysOnTop(sl_bool flag)
		{
			UIView* view = m_window;
			if (view != nil) {
				if ([view isKindOfClass:[UIWindow class]]) {
					dispatch_async(dispatch_get_main_queue(), ^{
						UIWindow* window = (UIWindow*)view;
						if (flag) {
							window.windowLevel = UIWindowLevelAlert + 1;
						} else {
							window.windowLevel = UIWindowLevelNormal + 1;
						}
					});
					return sl_true;
				}
			}
			return sl_false;
		}
		
		sl_bool isCloseButtonEnabled()
		{
			return sl_false;
		}
		
		sl_bool setCloseButtonEnabled(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_bool isMinimizeButtonEnabled()
		{
			return sl_false;
		}
		
		sl_bool setMinimizeButtonEnabled(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_bool isMaximizeButtonEnabled()
		{
			return sl_false;
		}
		
		sl_bool setMaximizeButtonEnabled(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_bool isResizable()
		{
			return sl_false;
		}
		
		sl_bool setResizable(sl_bool flag)
		{
			return sl_false;
		}
		
		sl_real getAlpha()
		{
			UIView* window = m_window;
			if (window != nil) {
				sl_real alpha = (sl_real)(window.alpha);
				return alpha;
			}
			return 1;
		}
		
		sl_bool setAlpha(sl_real _alpha)
		{
			UIView* window = m_window;
			if (window != nil) {
				dispatch_async(dispatch_get_main_queue(), ^{
					sl_real alpha = _alpha;
					if (alpha < 0) {
						alpha = 0;
					}
					if (alpha > 1) {
						alpha = 1;
					}
					window.alpha = alpha;
				});
				return sl_true;
			} else {
				return sl_false;
			}
		}
		
		sl_bool isTransparent()
		{
			return sl_false;
		}
		
		sl_bool setTransparent(sl_bool flag)
		{
			return sl_false;
		}
		
		UIPointf convertCoordinateFromScreenToWindow(const UIPointf& ptScreen)
		{
			UIView* view = m_window;
			if (view != nil) {
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				if ([view isKindOfClass:[UIWindow class]]) {
					UIWindow* window = (UIWindow*)view;
					CGPoint pt;
					pt.x = (CGFloat)(ptScreen.x) / f;
					pt.y = (CGFloat)(ptScreen.y) / f;
					pt = [window convertPoint:pt fromWindow:nil];
					UIPointf ret;
					ret.x = (sl_ui_posf)(pt.x * f);
					ret.y = (sl_ui_posf)(pt.y * f);
					return ret;
				} else {
					UIWindow* window = [view window];
					if (window != nil) {
						CGPoint pt;
						pt.x = (CGFloat)(ptScreen.x) / f;
						pt.y = (CGFloat)(ptScreen.y) / f;
						pt = [window convertPoint:pt fromWindow:nil];
						pt = [window convertPoint:pt toView:view];
						UIPointf ret;
						ret.x = (sl_ui_posf)(pt.x * f);
						ret.y = (sl_ui_posf)(pt.y * f);
						return ret;
					}
				}
			}
			return ptScreen;
		}
		
		UIPointf convertCoordinateFromWindowToScreen(const UIPointf& ptWindow)
		{
			UIView* view = m_window;
			if (view != nil) {
				CGFloat f = UIPlatform::getGlobalScaleFactor();
				if ([view isKindOfClass:[UIWindow class]]) {
					UIWindow* window = (UIWindow*)view;
					CGPoint pt;
					pt.x = (CGFloat)(ptWindow.x) / f;
					pt.y = (CGFloat)(ptWindow.y) / f;
					pt = [window convertPoint:pt toWindow:nil];
					UIPointf ret;
					ret.x = (sl_ui_posf)(pt.x * f);
					ret.y = (sl_ui_posf)(pt.y * f);
					return ret;
				} else {
					UIWindow* window = [view window];
					if (window != nil) {
						CGPoint pt;
						pt.x = (CGFloat)(ptWindow.x) / f;
						pt.y = (CGFloat)(ptWindow.y) / f;
						pt = [window convertPoint:pt fromView:view];
						pt = [window convertPoint:pt toWindow:nil];
						UIPointf ret;
						ret.x = (sl_ui_posf)(pt.x * f);
						ret.y = (sl_ui_posf)(pt.y * f);
						return ret;
					}
				}
			}
			return ptWindow;
		}
		
		UIPointf convertCoordinateFromScreenToClient(const UIPointf& ptScreen)
		{
			return convertCoordinateFromScreenToWindow(ptScreen);
		}
		
		UIPointf convertCoordinateFromClientToScreen(const UIPointf& ptClient)
		{
			return convertCoordinateFromWindowToScreen(ptClient);
		}
		
		UIPointf convertCoordinateFromWindowToClient(const UIPointf& ptWindow)
		{
			return ptWindow;
		}
		
		UIPointf convertCoordinateFromClientToWindow(const UIPointf& ptClient)
		{
			return ptClient;
		}
		
		UISize getWindowSizeFromClientSize(const UISize& sizeClient)
		{
			return sizeClient;
		}
		
		UISize getClientSizeFromWindowSize(const UISize& sizeWindow)
		{
			return sizeWindow;
		}
		
	};
	
	
	Ref<WindowInstance> Window::createWindowInstance(const WindowInstanceParam& param)
	{
		return _iOS_Window::create(param);
	}
}

@implementation _slib_iOS_Window

@end

UIView* _iOS_Window_findFirstResponder(UIView* root)
{
	if (root.isFirstResponder) {
		return root;
	}
	for (UIView* subView in root.subviews) {
		UIView* v = _iOS_Window_findFirstResponder(subView);
		if (v != nil) {
			return v;
		}
	}
	return nil;
}

@implementation _slib_iOS_Window_RootViewController

- (BOOL)prefersStatusBarHidden
{
	return NO;
}

- (void)viewWillTransitionToSize:(CGSize)_size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
	[super viewWillTransitionToSize:_size withTransitionCoordinator:coordinator];
	UIWindow* _window = self.view.window;
	if (_window != nil && [_window isKindOfClass:[_slib_iOS_Window class]]) {
		_slib_iOS_Window* window = (_slib_iOS_Window*)_window;
		slib::Ref<slib::_iOS_Window> w = window->m_window;
		if (w.isNotNull()) {
			CGFloat f = slib::UIPlatform::getGlobalScaleFactor();
			CGRect r = window.frame;
			r.size = _size;
			window.frame = r;
			w->onResize((sl_ui_pos)(_size.width * f), (sl_ui_pos)(_size.height * f));
		}
	}
}

- (BOOL)shouldAutorotate
{
	return YES;
}

-(UIView*)findFirstResponderText
{
	UIView* view = _iOS_Window_findFirstResponder(self.view);
	if (view != nil) {
		if ([view isKindOfClass:[UITextField class]] || [view isKindOfClass:[UITextView class]]) {
			return view;
		}
	}
	return nil;
}

UIScrollView* _g_ui_keyboard_scrollview = nil;
CGRect _g_ui_keyboard_scrollview_original_frame;

-(void)restoreKeyboardScrollView
{
	if (_g_ui_keyboard_scrollview != nil) {
		_g_ui_keyboard_scrollview.frame = _g_ui_keyboard_scrollview_original_frame;
		_g_ui_keyboard_scrollview = nil;
	}
}

-(void)keyboardWillShow : (NSNotification*)aNotification
{
	UIView* view = [self findFirstResponderText];
	if (view == nil) {
		[self restoreKeyboardScrollView];
		return;
	}
	
	UIScrollView* scroll = nil;
	UIView* parent = view;
	while (parent != nil) {
		if ([parent isKindOfClass:[UIScrollView class]]) {
			scroll = (UIScrollView*)parent;
			break;
		}
		parent = parent.superview;
	}

	if (scroll != _g_ui_keyboard_scrollview) {
		[self restoreKeyboardScrollView];
	}
	
	NSDictionary* info = [aNotification userInfo];
	CGSize kbSize = [[info objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;
	CGRect rcScreen = [[UIScreen mainScreen] bounds];
	CGRect rcTextLocal = view.bounds;
	CGRect rcTextScreen = [view convertRect:rcTextLocal toView:nil];
	CGFloat yText = rcTextScreen.origin.y + rcTextScreen.size.height + rcScreen.size.height / 100;
	
	if (scroll != nil) {
		CGRect rcScrollLocal = scroll.bounds;
		if (_g_ui_keyboard_scrollview == scroll) {
			rcScrollLocal.size = _g_ui_keyboard_scrollview_original_frame.size;
		}
		CGRect rcScrollScreen = [scroll convertRect:rcScrollLocal toView:nil];
		CGFloat yScroll = rcScrollScreen.origin.y + rcScrollScreen.size.height;
		if (yScroll > rcScreen.size.height - kbSize.height) {
			CGRect rc = scroll.frame;
			if (_g_ui_keyboard_scrollview == scroll) {
				rc = _g_ui_keyboard_scrollview_original_frame;
			}
			_g_ui_keyboard_scrollview_original_frame = rc;
			rc.size.height -= yScroll - (rcScreen.size.height - kbSize.height);
			scroll.frame = rc;
			_g_ui_keyboard_scrollview = scroll;
		}
	}
	if (yText > rcScreen.size.height - kbSize.height) {
		CGFloat offset = rcScreen.size.height - kbSize.height - yText;
		if (scroll != nil) {
			if (!([scroll isKindOfClass:[UITextView class]])) {
				CGPoint pos = scroll.contentOffset;
				pos.y -= offset;
				scroll.contentOffset = pos;
			}
		} else {
			CGAffineTransform transform = CGAffineTransformTranslate(CGAffineTransformIdentity, 0, offset);
			[UIView animateWithDuration:0.3 animations:^(void) {
				self.view.transform = transform;
			}];
		}
	}
}

-(void)keyboardWillHide {
	[UIView animateWithDuration:0.3 animations:^(void) {
		self.view.transform = CGAffineTransformIdentity;
	}];
	[self restoreKeyboardScrollView];
}


- (void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillShow:)
												 name:UIKeyboardWillShowNotification
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillHide)
												 name:UIKeyboardWillHideNotification
											   object:nil];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:UIKeyboardWillShowNotification
												  object:nil];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:UIKeyboardWillHideNotification
												  object:nil];
}

@end


namespace slib
{
	Ref<WindowInstance> UIPlatform::createWindowInstance(UIView* window)
	{
		Ref<WindowInstance> ret = UIPlatform::_getWindowInstance((__bridge void*)window);
		if (ret.isNotNull()) {
			return ret;
		}
		ret = _iOS_Window::create(window);
		if (ret.isNotNull()) {
			UIPlatform::_registerWindowInstance((__bridge void*)window, ret.get());
		}
		return ret;
	}
	
	Ref<WindowInstance> UIPlatform::getWindowInstance(UIView* window)
	{
		return UIPlatform::_getWindowInstance((__bridge void*)window);
	}
	
	void UIPlatform::removeWindowInstance(UIView* window)
	{
		UIPlatform::_removeWindowInstance((__bridge void*)window);
	}
	
	UIView* UIPlatform::getWindowHandle(WindowInstance* instance)
	{
		_iOS_Window* window = (_iOS_Window*)instance;
		if (window) {
			return window->m_window;
		} else {
			return nil;
		}
	}
	
}

#endif
