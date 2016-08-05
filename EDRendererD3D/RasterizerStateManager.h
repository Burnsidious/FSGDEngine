#pragma once



namespace EDRendererD3D
{
	class RasterizerStateManager
	{
	public:
		enum RasterStates{RS_Default = 0, RS_LINE, RS_CCW, RS_NOCULL, RS_COUNT};

		~RasterizerStateManager(void);
		
        static RasterizerStateManager &GetReference();

        static void DeleteInstance();

		bool ApplyState(RasterStates state);

		RasterStates GetCurrentState() { return currentState; }

	private:
		RasterizerStateManager(void);
        RasterizerStateManager(const RasterizerStateManager &) {}
        RasterizerStateManager &operator=(const RasterizerStateManager &) { return *this; }

		/// the one and only instance.
        static RasterizerStateManager *instancePtr;

		CComPtr<ID3D11RasterizerState> rasterStates[RS_COUNT];
		UINT stencilRefs[RS_COUNT];
		RasterStates currentState;

		void CreateStates();
	};
}