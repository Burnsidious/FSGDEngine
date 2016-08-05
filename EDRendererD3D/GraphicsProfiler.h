#pragma once



namespace EDRendererD3D
{
	class GraphicsProfiler
	{
	typedef int (WINAPI *BeginEventSigniture) (DWORD col, LPCWSTR wszName);
	typedef int (WINAPI *EndEventSigniture) ();
	typedef void (WINAPI *SetMarkerSigniture) ( DWORD col, LPCWSTR wszName );
	typedef void (WINAPI *SetRegionSigniture) ( DWORD col, LPCWSTR wszName );
	typedef BOOL (WINAPI *QueryRepeatFrameSigniture) ( void );
	typedef void (WINAPI *SetOptionsSigniture) ( DWORD dwOptions );
	typedef DWORD (WINAPI *GetStatusSigniture) ( void );

	public:
		~GraphicsProfiler(void);

		/// Gets a reference to the one and only IndexBuffer instance.
        static GraphicsProfiler &GetReference();

        /// Deletes the instance of the IndexBuffer.
        static void DeleteInstance();

		BeginEventSigniture BeginEvent;
		EndEventSigniture EndEvent;
		SetMarkerSigniture SetMarker;
		SetRegionSigniture SetRegion;
		QueryRepeatFrameSigniture QueryRepeatFrame;
		SetOptionsSigniture SetOptions;
		GetStatusSigniture GetStatus;
	private:
		GraphicsProfiler(void);
		GraphicsProfiler(const GraphicsProfiler &) {}
		GraphicsProfiler &operator=(const GraphicsProfiler &) { return *this; }

		bool Initialize();
		/// the one and only instance.
		static GraphicsProfiler *instancePtr;

		HINSTANCE dx9DLL;
	};

}