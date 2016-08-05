#pragma once


namespace EDRendererD3D
{
	class DepthStencilStateManager
	{
	public:
		enum DSStates{DSS_Default = 0, DSS_LessEqual, DSS_NoDepth, 
			DSS_DeferredLight1, DSS_DeferredLight2, DSS_LightOutsideFinal, DSS_LightInsideFinal, 
			DSS_COUNT};

		~DepthStencilStateManager(void);
		
		/// Gets a reference to the one and only IndexBuffer instance.
        static DepthStencilStateManager &GetReference();

        /// Deletes the instance of the IndexBuffer.
        static void DeleteInstance();

		bool ApplyState(DSStates state);
		DSStates GetCurrentState() { return currentState; }
	private:
		DepthStencilStateManager(void);
        DepthStencilStateManager(const DepthStencilStateManager &) {}
        DepthStencilStateManager &operator=(const DepthStencilStateManager &) { return *this; }

		/// the one and only instance.
        static DepthStencilStateManager *instancePtr;

		CComPtr<ID3D11DepthStencilState > dsStates[DSS_COUNT];
		UINT stencilRefs[DSS_COUNT];

		DSStates currentState;
		void CreateStates();
	};
}