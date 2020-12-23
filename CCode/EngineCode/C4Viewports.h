 

#ifndef C4Viewports_h
#define C4Viewports_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Interface.h"
#include "C4Cameras.h"


namespace C4
{
	enum : WidgetType
	{
		kWidgetOrthoViewport	= 'OVPT',
		kWidgetFrustumViewport	= 'FVPT',
		kWidgetWorldViewport	= 'WRLD'
	};


	//# \class	ViewportWidget		The base class for interface widgets that display 3D viewports.
	//
	//# The $ViewportWidget$ class is the base class for interface widgets that display 3D viewports.
	//
	//# \def	class ViewportWidget : public RenderableWidget
	//
	//# \ctor	ViewportWidget(ViewportType type, const Vector2D& size);
	//
	//# The $ViewportWidget$ constructor has protected access. The $ViewportWidget$ class can only
	//# exist as the base class for a more specific type of viewport.
	//
	//# \param	type	The type of the viewport.
	//# \param	size	The size of the viewport, in pixels.
	//
	//# \desc
	//# The $ViewportWidget$ class is the base class for interface widgets that display a viewport
	//# inside of which a 3D scene can be rendered. The type of camera used in the viewport, either
	//# an orthographic camera or a frustum camera, is determined by the subclass of $ViewportWidget$
	//# that is ultimately created.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@OrthoViewportWidget@$
	//# \also	$@FrustumViewportWidget@$


	//# \function	ViewportWidget::GetViewportCamera		Returns the camera used by a viewport widget.
	//
	//# \proto	Camera *GetViewportCamera(void);
	//# \proto	const Camera *GetViewportCamera(void) const;
	//
	//# \desc
	//# The $GetViewportCamera$ function returns the camera node through which the 3D scene is rendered
	//# inside a viewport widget. If the viewport is an $@OrthoViewportWidget@$, then the returned pointer can
	//# be cast to a pointer to an $@WorldMgr/OrthoCamera@$ node. If the viewport is a $@FrustumViewportWidget@$,
	//# then the returned pointer can be cast to a pointer to a $@WorldMgr/FrustumCamera@$ node.
	//
	//# \also	$@WorldMgr/Camera@$
	//# \also	$@WorldMgr/OrthoCamera@$
	//# \also	$@WorldMgr/FrustumCamera@$


	//# \function	ViewportWidget::SetMouseEventProc		Sets the mouse event handler for a viewport widget.
	//
	//# \proto	void SetMouseEventProc(MouseEventProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the mouse event handler function.
	//# \param	cookie	A user-defined pointer that is passed to the handler function.
	//
	//# \desc
	//# The $SetMouseEventProc$ function installs a handler function that is called when a mouse event occurs inside
	//# a viewport widget. The $proc$ parameter should point to a function having the following prototype.
	//
	//# \code	typedef void MouseEventProc(const MouseEventData *, ViewportWidget *, void *);
	//
	//# The first parameter passed to the handler function is a pointer to the mouse event data, and the second
	//# parameter is a pointer to the viewport inside which a mouse event occurred. The last parameter receives
	//# the pointer specified by the $cookie$ parameter.
	//#
	//# The current mouse event handler is called when the viewport widget's $@Widget::HandleMouseEvent@$ function
	//# is called by the Interface Manager.
	//#
	//# If the $proc$ parameter is $nullptr$, then any currently installed mouse event handler is removed.
	//#
	//# In between mouse down and mouse up events, the track task handler function is called once per frame if
	//# such a handler has been installed using the $@ViewportWidget::SetTrackTaskProc@$ function.
	//
	//# \also	$@ViewportWidget::SetTrackTaskProc@$
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetOverlayProc@$
	//# \also	$@Utilities/MouseEventData@$


	//# \function	ViewportWidget::SetTrackTaskProc		Sets the track task handler for a viewport widget.
	//
	//# \proto	void SetTrackTaskProc(TrackTaskProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the track task handler function.
	//# \param	cookie	A user-defined pointer that is passed to the handler function. 
	//
	//# \desc
	//# The $SetTrackTaskProc$ function installs a handler function that is called once per frame after a 
	//# mouse down event has occurred inside a viewport widget and before a mouse up event occurs. The $proc$
	//# parameter should point to a function having the following prototype. 
	//
	//# \code	typedef void TrackTaskProc(const Point3D&, ViewportWidget *, void *);
	// 
	//# The first parameter passed to the handler function is the mouse position in local viewport coordinates,
	//# and the second parameter is a pointer to the viewport inside which a mouse event occurred. The last 
	//# parameter receives the pointer specified by the $cookie$ parameter. 
	//#
	//# The current track task handler is called when the viewport widget's $@Widget::TrackTask@$ function is
	//# called by the Interface Manager.
	//# 
	//# If the $proc$ parameter is $nullptr$, then any currently installed track task handler is removed.
	//
	//# \also	$@ViewportWidget::SetMouseEventProc@$
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetOverlayProc@$


	//# \function	ViewportWidget::SetRenderProc		Sets the rendering function for a viewport widget.
	//
	//# \proto	void SetRenderProc(RenderProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the rendering function.
	//# \param	cookie	A user-defined pointer that is passed to the rendering function.
	//
	//# \desc
	//# The $SetRenderProc$ function installs a rendering function that is called once per frame to render
	//# the contents of a viewport widget except in the case that a previous rendered image has been cached
	//# in a texture map and remains valid.
	//#
	//# The $proc$ parameter should point to a function having the following prototype.
	//
	//# \code	typedef void RenderProc(List<Renderable> *, ViewportWidget *, void *);
	//
	//# The first parameter passed to the rendering function is a pointer to a list to which renderable
	//# objects can be added, and the second parameter is a pointer to the viewport that is being rendered.
	//# The last parameter receives the pointer specified by the $cookie$ parameter.
	//#
	//# Right before the rendering function is called, the viewport's camera is established as the current
	//# rendering camera. If the camera is configured to clear the viewport, then the viewport is cleared
	//# immediately before the rendering function gains control. The rendering function may cause calls to the
	//# $@GraphicsMgr/GraphicsMgr::DrawRenderList@$ function to occur, or it may add renderable objects to the
	//# render list passed to it. If the render list contains any objects upon return from the rendering function,
	//# then they are immediately rendered with the $DrawRenderList$ function and removed from the list.
	//#
	//# If the $proc$ parameter is $nullptr$, then any currently installed rendering function is removed,
	//# but items can still be rendered in the viewport through the overlay rendering function if one has
	//# been installed using the $@ViewportWidget::SetOverlayProc@$ function.
	//
	//# \also	$@ViewportWidget::SetOverlayProc@$
	//# \also	$@ViewportWidget::SetMouseEventProc@$
	//# \also	$@ViewportWidget::SetTrackTaskProc@$
	//# \also	$@GraphicsMgr/Renderable@$


	//# \function	ViewportWidget::SetOverlayProc		Sets the overlay rendering function for a viewport widget.
	//
	//# \proto	void SetOverlayProc(RenderProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the overlay rendering function.
	//# \param	cookie	A user-defined pointer that is passed to the overlay rendering function.
	//
	//# \desc
	//# The $SetOverlayProc$ function installs a rendering function that is called once per frame to render
	//# an overlay image for a viewport widget. If there is one installed, the overlay rendering function is
	//# always called, even in the case that an image previously rendered by the primary rendering function
	//# (installed with the $@ViewportWidget::SetRenderProc@$ function) has been cached in a texture map
	//# and remains valid.
	//#
	//# The $proc$ parameter should point to a function having the following prototype.
	//
	//# \code	typedef void RenderProc(List<Renderable> *, ViewportWidget *, void *);
	//
	//# The first parameter passed to the overlay rendering function is a pointer to a list to which renderable
	//# objects can be added, and the second parameter is a pointer to the viewport that is being rendered.
	//# The last parameter receives the pointer specified by the $cookie$ parameter.
	//#
	//# The overlay rendering function is called immediately after either (a) the primary rendering function
	//# has been called, or (b) the cached texture map image for the viewport has been rendered. In both cases,
	//# the viewport's camera has already been established as the current rendering camera. The overlay rendering
	//# function may cause calls to the $@GraphicsMgr/GraphicsMgr::DrawRenderList@$ function to occur, or it may
	//# add renderable objects to the render list passed to it. If the render list contains any objects upon return
	//# from the overlay rendering function, then they are immediately rendered with the $DrawRenderList$ function
	//# and removed from the list.
	//#
	//# If the $proc$ parameter is $nullptr$, then any currently installed overlay rendering function is removed.
	//
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetMouseEventProc@$
	//# \also	$@ViewportWidget::SetTrackTaskProc@$
	//# \also	$@GraphicsMgr/Renderable@$


	//# \function	ViewportWidget::AllocateTexture		Allocates a texture map used to cache the image rendered in a viewport widget.
	//
	//# \proto	void AllocateTexture(void);
	//
	//# \desc
	//# The $AllocateTexture$ function allocates an internal texture map that is used to cache the image last rendered
	//# into a viewport by the primary rendering function installed with the $@ViewportWidget::SetRenderProc@$ function.
	//# If the process of rendering the contents of the viewport is expensive, this caching mechanism improves performance
	//# whenever the contents of the viewport are not changing. Once an image has been cached in the texture map, the primary
	//# rendering function is no longer called until the texture map is invalidated using the $@ViewportWidget::InvalidateTexture@$
	//# function. Instead, the image in the texture map is drawn by rendering a single quad that covers the viewport.
	//#
	//# If an overlay rendering function has been installed with the $@ViewportWidget::SetOverlayProc@$ function, then it
	//# continues to be called once per frame regardless of whether the primary viewport image is cached in a texture map.
	//# This allows lower-cost dynamic objects to be rendered on top of a high-cost static scene.
	//#
	//# The cached texture map can be removed by calling the $@ViewportWidget::DeallocateTexture@$ function, and this would
	//# once again cause the primary rendering function to be called once per frame.
	//
	//# \also	$@ViewportWidget::DeallocateTexture@$
	//# \also	$@ViewportWidget::InvalidateTexture@$
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetOverlayProc@$


	//# \function	ViewportWidget::DeallocateTexture	Deallocates the cached texture map for a viewport widget.
	//
	//# \proto	void DeallocateTexture(void);
	//
	//# \desc
	//# The $DeallocateTexture$ function deallocates the internal texture map that is used to cache the image last rendered
	//# into a viewport. After this function is called, the primary rendering function installed with the
	//# $@ViewportWidget::SetRenderProc@$ function will be called once per frame to render the contents of the viewport.
	//
	//# \also	$@ViewportWidget::AllocateTexture@$
	//# \also	$@ViewportWidget::InvalidateTexture@$
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetOverlayProc@$


	//# \function	ViewportWidget::InvalidateTexture	Invalidates the cached texture map for a viewport widget.
	//
	//# \proto	void InvalidateTexture(void);
	//
	//# \desc
	//# The $InvalidateTexture$ function invalidates the internal texture map containing the cached image last rendered
	//# into a viewport. This causes the primary rendering function installed with the $@ViewportWidget::SetRenderProc@$
	//# function to be called the next time the viewport widget itself is rendered in order to update the contents of
	//# the viewport. The new image will then be copied into the texture map and used as the viewport contents until
	//# the texture map is again invalidated. The $InvalidateTexture$ function should be called any time that something
	//# changes in such a way that the viewport image needs to be re-rendered.
	//
	//# \also	$@ViewportWidget::AllocateTexture@$
	//# \also	$@ViewportWidget::DeallocateTexture@$
	//# \also	$@ViewportWidget::SetRenderProc@$
	//# \also	$@ViewportWidget::SetOverlayProc@$


	class ViewportWidget : public RenderableWidget
	{
		public:

			typedef void MouseEventProc(const MouseEventData *, ViewportWidget *, void *);
			typedef void TrackTaskProc(const Point3D&, ViewportWidget *, void *);
			typedef void RenderProc(List<Renderable> *, ViewportWidget *, void *);

		private:

			struct ViewportVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			int32						viewportIndex;
			Camera						*viewportCamera;

			MouseEventProc				*mouseEventProc;
			void						*mouseEventCookie;

			TrackTaskProc				*trackTaskProc;
			void						*trackTaskCookie;

			RenderProc					*renderProc;
			void						*renderCookie;

			RenderProc					*overlayProc;
			void						*overlayCookie;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseTextureAttribute		textureAttribute;

			bool						textureValidFlag;
			TextureHeader				textureHeader;

			ViewportWidget(const ViewportWidget& viewportWidget) = delete;

			void Initialize(void);

		protected:

			ViewportWidget(WidgetType type, Camera *camera);
			ViewportWidget(WidgetType type, Camera *camera, const Vector2D& size);
			ViewportWidget(const ViewportWidget& viewportWidget, Camera *camera);

			virtual void SetGraphicsCamera(bool clear) = 0;

		public:

			C4API ~ViewportWidget();

			int32 GetViewportIndex(void) const
			{
				return (viewportIndex);
			}

			void SetViewportIndex(int32 index)
			{
				viewportIndex = index;
			}

			Camera *GetViewportCamera(void)
			{
				return (viewportCamera);
			}

			const Camera *GetViewportCamera(void) const
			{
				return (viewportCamera);
			}

			void SetMouseEventProc(MouseEventProc *proc, void *cookie = nullptr)
			{
				mouseEventProc = proc;
				mouseEventCookie = cookie;
			}

			void SetTrackTaskProc(TrackTaskProc *proc, void *cookie = nullptr)
			{
				trackTaskProc = proc;
				trackTaskCookie = cookie;
			}

			void SetRenderProc(RenderProc *proc, void *cookie = nullptr)
			{
				renderProc = proc;
				renderCookie = cookie;
			}

			void SetOverlayProc(RenderProc *proc, void *cookie = nullptr)
			{
				overlayProc = proc;
				overlayCookie = cookie;
			}

			void InvalidateTexture(void)
			{
				textureValidFlag = false;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetWidgetSize(const Vector2D& size) override;
			C4API void Preprocess(void) override;
			C4API void Build(void) override;

			C4API void AllocateTexture(void);
			C4API void DeallocateTexture(void);

			C4API void Render(List<Renderable> *renderList) override;

			C4API void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			C4API void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition) override;
	};


	//# \class	OrthoViewportWidget		The interface widget that displays an orthographic 3D viewport.
	//
	//# The $OrthoViewportWidget$ class represents an interface widget that displays an orthographic 3D viewport.
	//
	//# \def	class OrthoViewportWidget final : public ViewportWidget
	//
	//# \ctor	OrthoViewportWidget(const Vector2D& size, const Vector2D& scale);
	//
	//# \param	size		The size of the viewport, in pixels.
	//# \param	scale		The orthographic camera scale.
	//
	//# \desc
	//# The $OrthoViewportWidget$ class displays a viewport with an orthographic camera.
	//
	//# \base	ViewportWidget		An $OrthoViewportWidget$ is a specific type of viewport.
	//
	//# \also	$@FrustumViewportWidget@$


	class OrthoViewportWidget final : public ViewportWidget
	{
		friend class WidgetReg<OrthoViewportWidget>;

		private:

			OrthoCamera		orthoCamera;
			Vector2D		orthoScale;

			C4API Widget *Replicate(void) const override;

			void SetGraphicsCamera(bool clear) override;

		protected:

			C4API OrthoViewportWidget(WidgetType type = kWidgetOrthoViewport);
			C4API OrthoViewportWidget(WidgetType type, const Vector2D& size, const Vector2D& scale);
			C4API OrthoViewportWidget(const OrthoViewportWidget& orthoViewportWidget);

		public:

			C4API OrthoViewportWidget(const Vector2D& size, const Vector2D& scale);
			C4API ~OrthoViewportWidget();

			OrthoCamera *GetViewportCamera(void)
			{
				return (&orthoCamera);
			}

			const OrthoCamera *GetViewportCamera(void) const
			{
				return (&orthoCamera);
			}

			const Vector2D& GetOrthoScale(void) const
			{
				return (orthoScale);
			}

			void SetOrthoScale(const Vector2D& scale)
			{
				orthoScale = scale;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetZoomScale(const Vector2D& scale, const Point3D& position, bool integer = false);
	};


	//# \class	FrustumViewportWidget	The interface widget that displays a frustum 3D viewport.
	//
	//# The $FrustumViewportWidget$ class represents an interface widget that displays a frustum 3D viewport.
	//
	//# \def	class FrustumViewportWidget : public ViewportWidget
	//
	//# \ctor	FrustumViewportWidget(const Vector2D& size, float focalLength);
	//
	//# \param	size			The size of the viewport, in pixels.
	//# \param	focalLength		The focal length of the frustum camera.
	//
	//# \desc
	//# The $FrustumViewportWidget$ class displays a viewport with a frustum camera.
	//
	//# \base	ViewportWidget		A $FrustumViewportWidget$ is a specific type of viewport.
	//
	//# \also	$@OrthoViewportWidget@$


	class FrustumViewportWidget : public ViewportWidget
	{
		friend class WidgetReg<FrustumViewportWidget>;

		private:

			FrustumCamera	frustumCamera;

			float			cameraAzimuth;
			float			cameraAltitude;

			C4API Widget *Replicate(void) const override;

			C4API void SetGraphicsCamera(bool clear) override;

		protected:

			C4API FrustumViewportWidget(WidgetType type = kWidgetFrustumViewport);
			C4API FrustumViewportWidget(WidgetType type, const Vector2D& size, float focalLength);
			C4API FrustumViewportWidget(const FrustumViewportWidget& frustumViewportWidget);

		public:

			C4API FrustumViewportWidget(const Vector2D& size, float focalLength);
			C4API ~FrustumViewportWidget();

			FrustumCamera *GetViewportCamera(void)
			{
				return (&frustumCamera);
			}

			const FrustumCamera *GetViewportCamera(void) const
			{
				return (&frustumCamera);
			}

			float GetCameraAzimuth(void) const
			{
				return (cameraAzimuth);
			}

			float GetCameraAltitude(void) const
			{
				return (cameraAltitude);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetCameraTransform(float azm, float alt, const Point3D& position);
			C4API void SetCameraPosition(const Point3D& position);
	};


	//# \class	WorldViewportWidget		The interface widget that renders a world in a 3D viewport.
	//
	//# The $WorldViewportWidget$ class represents an interface widget that renders a world in a 3D viewport.
	//
	//# \def	class WorldViewportWidget final : public FrustumViewportWidget
	//
	//# \ctor	WorldViewportWidget(const Vector2D& size, float focalLength);
	//
	//# \param	size			The size of the viewport, in pixels.
	//# \param	focalLength		The focal length of the frustum camera.
	//
	//# \desc
	//# The $WorldViewportWidget$ class renders a world into a viewport with a frustum camera.
	//
	//# \base	FrustumViewportWidget		A $WorldViewportWidget$ is a specific type of frustum viewport.


	class WorldViewportWidget final : public FrustumViewportWidget
	{
		friend class WidgetReg<WorldViewportWidget>;

		private:

			World			*viewportWorld;

			Point3D			cameraTarget;
			float			cameraDistance;
			float			maxCameraDistance;

			bool			trackFlag;
			Point3D			previousPosition;

			WorldViewportWidget();

			Widget *Replicate(void) const override;

			static void ViewportHandleMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void ViewportRender(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			C4API WorldViewportWidget(const Vector2D& size, float focalLength);
			C4API ~WorldViewportWidget();

			World *GetViewportWorld(void) const
			{
				return (viewportWorld);
			}

			void Preprocess(void) override;

			C4API void EnableCameraOrbit(const Point3D& target, float distance);
			C4API void DisableCameraOrbit(void);

			C4API void SetCameraAngles(float azm, float alt);

			C4API void LoadWorld(const char *name);
			C4API void UnloadWorld();
	};


	class Grid : public Renderable
	{
		private:

			struct GridVertex
			{
				Point2D		position;
				Color4C		color;
			};

			unsigned_int32		gridFlags;

			float				gridLineSpacing;
			int32				majorLineInterval;

			Point2D				boundingBoxMin;
			Point2D				boundingBoxMax;

			Color4C				minorLineColor;
			Color4C				majorLineColor;
			Color4C				axisLineColor;
			Color4C				boundingBoxColor;

			GridVertex			*gridStorage;
			int32				gridStorageCount;

			VertexBuffer		vertexBuffer;

		public:

			enum
			{
				kGridShowBoundingBox	= 1 << 0
			};

			C4API Grid();
			C4API ~Grid();

			unsigned_int32 GetGridFlags(void) const
			{
				return (gridFlags);
			}

			void SetGridFlags(unsigned_int32 flags)
			{
				gridFlags = flags;
			}

			float GetGridLineSpacing(void) const
			{
				return (gridLineSpacing);
			}

			void SetGridLineSpacing(float spacing)
			{
				gridLineSpacing = spacing;
			}

			int32 GetMajorLineInterval(void) const
			{
				return (majorLineInterval);
			}

			void SetMajorLineInterval(int32 interval)
			{
				majorLineInterval = interval;
			}

			void SetBoundingBox(const Vector2D& min, const Vector2D& max)
			{
				boundingBoxMin = min;
				boundingBoxMax = max;
			}

			void SetMinorLineColor(const ColorRGB& color)
			{
				minorLineColor.Set((int32) (color.red * 255.0F), (int32) (color.green * 255.0F), (int32) (color.blue * 255.0F), 0xFF);
			}

			void SetMajorLineColor(const ColorRGB& color)
			{
				majorLineColor.Set((int32) (color.red * 255.0F), (int32) (color.green * 255.0F), (int32) (color.blue * 255.0F), 0xFF);
			}

			void SetAxisLineColor(const ColorRGB& color)
			{
				axisLineColor.Set((int32) (color.red * 255.0F), (int32) (color.green * 255.0F), (int32) (color.blue * 255.0F), 0xFF);
			}

			void SetBoundingBoxColor(const ColorRGB& color)
			{
				boundingBoxColor.Set((int32) (color.red * 255.0F), (int32) (color.green * 255.0F), (int32) (color.blue * 255.0F), 0xFF);
			}

			C4API void Build(const Point2D& min, const Point2D& max, float scale);
	};


	class DragRect : public Renderable
	{
		private:

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			DiffuseAttribute		diffuseColor;

		public:

			C4API DragRect(const ColorRGBA& color);
			C4API ~DragRect();

			C4API void Build(const Point2D& p1, const Point2D& p2, float scale);
			C4API void Build(const Point2D& p1, const Point2D& p2, const Vector3D& dx, const Vector3D& dy, float scale);
	};
}


#endif

// ZYUQURM
